#!/usr/bin/env bash

set -euo pipefail

if [ $# -lt 1 ]; then
	echo "Usage: $0 <path-to-binary>"
	exit 1
fi

BINARY_PATH="$1"

if [ ! -f "$BINARY_PATH" ]; then
	echo "Error: File not found: $BINARY_PATH"
	exit 1
fi

TMPDIR=$(mktemp -d)
trap 'rm -rf $TMPDIR' EXIT

echo "=== Disassembling $BINARY_PATH ==="
./build/8086 "$BINARY_PATH" >"$TMPDIR/disassembled.asm"

echo "=== Re-assembling ==="
nasm -f bin -o "$TMPDIR/reassembled" "$TMPDIR/disassembled.asm"

echo "=== Diffing original vs re-assembled ==="
if diff -q "$BINARY_PATH" "$TMPDIR/reassembled" >/dev/null; then
	echo "SUCCESS: Binary matches original!"
else
	echo "FAILURE: Binary differs from original"
	echo ""
	echo "--- Hex dump comparison ---"
	echo "Original:"
	xxd "$BINARY_PATH"
	echo ""
	echo "Re-assembled:"
	xxd "$TMPDIR/reassembled"
	exit 1
fi
