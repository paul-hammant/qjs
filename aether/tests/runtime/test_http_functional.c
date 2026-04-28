// HTTP Server Functional Tests
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../std/net/aether_http_server.h"

#define TEST_PASS(name) printf("✓ %s\n", name); return 0
#define TEST_FAIL(name, msg) printf("✗ %s: %s\n", name, msg); return 1

int test_request_parsing() {
    const char* raw = "GET /api/test HTTP/1.1\r\nHost: localhost\r\n\r\n";
    HttpRequest* req = aether_http_parse_request(raw);
    
    if (!req) TEST_FAIL("Request Parsing", "Failed to parse");
    if (strcmp(req->method, "GET") != 0) TEST_FAIL("Request Parsing", "Wrong method");
    if (strcmp(req->path, "/api/test") != 0) TEST_FAIL("Request Parsing", "Wrong path");
    
    const char* host = aether_http_get_header(req, "Host");
    if (!host || strcmp(host, "localhost") != 0) TEST_FAIL("Request Parsing", "Missing/wrong Host header");
    
    aether_http_request_free(req);
    TEST_PASS("Request Parsing");
}

int test_query_params() {
    const char* raw = "GET /search?q=test&limit=10 HTTP/1.1\r\n\r\n";
    HttpRequest* req = aether_http_parse_request(raw);
    
    if (!req) TEST_FAIL("Query Params", "Failed to parse");
    
    const char* q = aether_http_get_query_param(req, "q");
    const char* limit = aether_http_get_query_param(req, "limit");
    
    if (!q || strcmp(q, "test") != 0) TEST_FAIL("Query Params", "Wrong q param");
    if (!limit || strcmp(limit, "10") != 0) TEST_FAIL("Query Params", "Wrong limit param");
    
    aether_http_request_free(req);
    TEST_PASS("Query Params");
}

int test_response_building() {
    HttpServerResponse* res = aether_http_response_create();
    if (!res) TEST_FAIL("Response Building", "Failed to create");
    
    aether_http_response_set_status(res, 404);
    aether_http_response_set_header(res, "Content-Type", "text/plain");
    aether_http_response_set_body(res, "Not Found");
    
    const char* serialized = aether_http_response_serialize(res);
    if (!serialized) TEST_FAIL("Response Building", "Failed to serialize");
    if (!strstr(serialized, "404")) TEST_FAIL("Response Building", "Missing status");
    if (!strstr(serialized, "Not Found")) TEST_FAIL("Response Building", "Missing body");
    
    aether_http_server_response_free(res);
    TEST_PASS("Response Building");
}

int test_json_response() {
    HttpServerResponse* res = aether_http_response_create();
    aether_http_response_json(res, "{\"test\":true}");
    
    const char* serialized = aether_http_response_serialize(res);
    if (!strstr(serialized, "application/json")) TEST_FAIL("JSON Response", "Missing Content-Type");
    if (!strstr(serialized, "{\"test\":true}")) TEST_FAIL("JSON Response", "Missing body");
    
    aether_http_server_response_free(res);
    TEST_PASS("JSON Response");
}

int test_route_matching() {
    HttpRequest req = {0};
    req.param_count = 0;
    
    // Exact match
    if (!aether_http_route_matches("/api/test", "/api/test", &req)) 
        TEST_FAIL("Route Matching", "Exact match failed");
    
    // Param extraction
    req.param_count = 0;
    if (!aether_http_route_matches("/users/:id", "/users/123", &req))
        TEST_FAIL("Route Matching", "Param pattern failed");
    if (req.param_count != 1) TEST_FAIL("Route Matching", "Param not extracted");
    
    const char* id = aether_http_get_path_param(&req, "id");
    if (!id || strcmp(id, "123") != 0) TEST_FAIL("Route Matching", "Wrong param value");
    
    // Wildcard
    req.param_count = 0;
    if (!aether_http_route_matches("/static/*", "/static/css/main.css", &req))
        TEST_FAIL("Route Matching", "Wildcard failed");
    
    TEST_PASS("Route Matching");
}

int test_server_lifecycle() {
    HttpServer* server = aether_http_server_create(8080);
    if (!server) TEST_FAIL("Server Lifecycle", "Failed to create");
    if (server->port != 8080) TEST_FAIL("Server Lifecycle", "Wrong port");
    
    aether_http_server_free(server);
    TEST_PASS("Server Lifecycle");
}

int test_status_codes() {
    const char* ok = aether_http_status_text(200);
    const char* not_found = aether_http_status_text(404);
    const char* error = aether_http_status_text(500);
    
    if (strcmp(ok, "OK") != 0) TEST_FAIL("Status Codes", "Wrong 200 text");
    if (strcmp(not_found, "Not Found") != 0) TEST_FAIL("Status Codes", "Wrong 404 text");
    if (strcmp(error, "Internal Server Error") != 0) TEST_FAIL("Status Codes", "Wrong 500 text");
    
    TEST_PASS("Status Codes");
}

int main() {
    printf("\n=== HTTP Server Tests ===\n\n");
    
    int failures = 0;
    failures += test_request_parsing();
    failures += test_query_params();
    failures += test_response_building();
    failures += test_json_response();
    failures += test_route_matching();
    failures += test_server_lifecycle();
    failures += test_status_codes();
    
    printf("\n========================\n");
    if (failures == 0) {
        printf("ALL %d TESTS PASSED ✓\n", 7);
    } else {
        printf("%d/%d TESTS FAILED ✗\n", failures, 7);
    }
    printf("========================\n\n");
    
    return failures;
}
