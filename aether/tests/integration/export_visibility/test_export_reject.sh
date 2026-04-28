#!/bin/sh
# Test that non-exported symbols are correctly rejected by the compiler.
# This test expects compilation to FAIL.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"

pass=0
fail=0

# Test: calling non-exported function should fail
cat > /tmp/ae_export_test_bad1.ae << 'EOF'
import mathlib
main() {
    println(mathlib.internal_multiply(2, 3))
}
EOF
cd "$SCRIPT_DIR"
if AETHER_HOME="" "$ROOT/build/ae" build /tmp/ae_export_test_bad1.ae -o /tmp/ae_export_bad1 2>/dev/null; then
    echo "  [FAIL] non-exported function should be rejected"
    fail=$((fail + 1))
else
    echo "  [PASS] non-exported function correctly rejected"
    pass=$((pass + 1))
fi

# Test: accessing non-exported constant should fail
cat > /tmp/ae_export_test_bad2.ae << 'EOF'
import mathlib
main() {
    println(mathlib.SECRET)
}
EOF
if AETHER_HOME="" "$ROOT/build/ae" build /tmp/ae_export_test_bad2.ae -o /tmp/ae_export_bad2 2>/dev/null; then
    echo "  [FAIL] non-exported constant should be rejected"
    fail=$((fail + 1))
else
    echo "  [PASS] non-exported constant correctly rejected"
    pass=$((pass + 1))
fi

# Cleanup
rm -f /tmp/ae_export_test_bad1.ae /tmp/ae_export_test_bad2.ae /tmp/ae_export_bad1 /tmp/ae_export_bad2

echo ""
echo "Export rejection tests: $pass passed, $fail failed"
if [ "$fail" -gt 0 ]; then exit 1; fi
