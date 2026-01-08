#!/bin/bash

set -e

TMP_CPP=".compiler_check.cpp"
TMP_OUT=".compiler_check.out"

cleanup() {
    rm -f "$TMP_CPP" "$TMP_OUT"
}

trap cleanup EXIT

cat > "$TMP_CPP" <<'EOF'
#include <iostream>
int main() {
    std::cout << "compiler-check-ok\n";
    return 0;
}
EOF

if [ -n "${CXX}" ] && command -v "${CXX}" > /dev/null 2>&1; then
    COMPILER="${CXX}"
elif command -v g++ > /dev/null 2>&1; then
    COMPILER="g++"
elif command -v clang++ > /dev/null 2>&1; then
    COMPILER="clang++"
elif command -v c++ > /dev/null 2>&1; then
    COMPILER="c++"
else
    echo "[FAIL] No C++ compiler found (g++, clang++, c++)."
    exit 1
fi

echo "[INFO] Using compiler: ${COMPILER}"
"${COMPILER}" -std=c++17 "$TMP_CPP" -o "$TMP_OUT" > /dev/null 2>&1
"${TMP_OUT}" > /dev/null 2>&1

echo "[OK] C++17 compiler works"
