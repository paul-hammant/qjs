#!/bin/sh
# Test that selective imports correctly reject non-imported symbols.
# This test expects compilation to FAIL for non-imported symbols.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"

pass=0
fail=0

# Test 1: calling non-imported function should fail
cat > /tmp/ae_sel_import_bad1.ae << 'EOF'
import std.math (sqrt)
main() {
    // pow is NOT in import list — should fail
    y = math.pow(2.0, 3.0)
    println(y)
}
EOF
if AETHER_HOME="" "$ROOT/build/ae" check /tmp/ae_sel_import_bad1.ae 2>/dev/null; then
    echo "  [FAIL] non-imported function should be rejected"
    fail=$((fail + 1))
else
    echo "  [PASS] non-imported function correctly rejected"
    pass=$((pass + 1))
fi

# Test 2: imported function should succeed
cat > /tmp/ae_sel_import_good.ae << 'EOF'
import std.math (sqrt)
main() {
    x = math.sqrt(9.0)
    println(x)
}
EOF
if AETHER_HOME="" "$ROOT/build/ae" check /tmp/ae_sel_import_good.ae 2>/dev/null; then
    echo "  [PASS] imported function accepted"
    pass=$((pass + 1))
else
    echo "  [FAIL] imported function should be accepted"
    fail=$((fail + 1))
fi

# Test 3: full import (no selection) should allow all functions
cat > /tmp/ae_sel_import_full.ae << 'EOF'
import std.math
main() {
    x = math.sqrt(9.0)
    y = math.pow(2.0, 3.0)
    println(x)
    println(y)
}
EOF
if AETHER_HOME="" "$ROOT/build/ae" check /tmp/ae_sel_import_full.ae 2>/dev/null; then
    echo "  [PASS] full import allows all functions"
    pass=$((pass + 1))
else
    echo "  [FAIL] full import should allow all functions"
    fail=$((fail + 1))
fi

# Cleanup
rm -f /tmp/ae_sel_import_bad1.ae /tmp/ae_sel_import_good.ae /tmp/ae_sel_import_full.ae

echo ""
echo "Selective import tests: $pass passed, $fail failed"
if [ "$fail" -gt 0 ]; then exit 1; fi
