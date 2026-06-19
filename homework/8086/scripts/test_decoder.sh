#!/usr/bin/env bash
set -euo pipefail

if [ $# -lt 1 ]; then
	echo "Usage: $0 <path-to-binary>"
	exit 1
fi

BINARY_PATH="$1"
TMP_DIR="tmp"
ASM_FILE="$TMP_DIR/disasm.asm"
REASSEMBLED="$TMP_DIR/reassembled"

# Ensure tmp directory exists and is ignored by git
mkdir -p "$TMP_DIR"
if [ ! -f "$TMP_DIR/.gitignore" ]; then
	echo "*" >"$TMP_DIR/.gitignore"
fi

# Build the disassembler if needed
if [ ! -f "./8086" ] || [ "main.c" -nt "./8086" ]; then
	make
fi

# 1. Run the disassembler
"./8086" "$BINARY_PATH" >"$ASM_FILE"

# 2. Reassemble the output with NASM
nasm -f bin -o "$REASSEMBLED" "$ASM_FILE"

# 3. Diff the result with the original binary
if diff -q "$BINARY_PATH" "$REASSEMBLED" >/dev/null; then
	echo "Round-trip test passed!"
else
	echo "Round-trip test FAILED: reassembled binary does not match original."
	echo "Disassembly output: $ASM_FILE"
	echo "Reassembled binary:  $REASSEMBLED"
	exit 1
fi
