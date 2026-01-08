package main

import (
	"fmt"
	"os"
	"strings"
)

// https://codeberg.org/bolt/8086-Users-Manual/src/branch/main/INTEL_The-8086-Family-Users-Manual.pdf
// See page 262

func decodeRegister(b byte, w byte) string {
	switch b {
	case 0b000:
		if w == 0b00 {
			return "al"
		} else {
			return "ax"
		}
	case 0b001:
		if w == 0b00 {
			return "cl"
		} else {
			return "cx"
		}
	case 0b010:
		if w == 0b00 {
			return "dl"
		} else {
			return "dx"
		}
	case 0b011:
		if w == 0b00 {
			return "bl"
		} else {
			return "bx"
		}
	case 0b100:
		if w == 0b00 {
			return "ah"
		} else {
			return "sp"
		}
	case 0b101:
		if w == 0b00 {
			return "ch"
		} else {
			return "bp"
		}
	case 0b110:
		if w == 0b00 {
			return "dh"
		} else {
			return "si"
		}
	case 0b111:
		if w == 0b00 {
			return "bh"
		} else {
			return "di"
		}
	default:
		panic("Unable to decode register (this shouldn't happen)")
	}
}

func decodeInstructions(path string) {
	binary, err := os.ReadFile(path)
	if err != nil {
		fmt.Printf("Failed to read source: '%s'\n", path)
		return
	}

	// Get the name of the file being decoded
	split_path := strings.Split(path, "/")
	filename := split_path[len(split_path)-1]

	// We will write the decoded instructions line by line to the result buffer
	var result strings.Builder
	// result.WriteString("bits 16")
	result.WriteString(fmt.Sprintf("; Decoded instructions for %s\nbits 16", filename))

	var i = 0
	for i < len(binary) {
		// All instructions being decoded are mov instructions, which are two bytes long
		byte1 := binary[i]
		byte2 := binary[i+1]

		// Increment the counter by two to move to the next mov instruction
		i += 2

		// The instruction encoding
		// Since we are only decoding mov operations, this is always 0b100010
		_ = (byte1 >> 2) & 0b00111111

		d := byte1 & 0b00000010
		w := byte1 & 0b00000001

		// The mod field, which tells us what kind of move this is
		// Always 0b11 in this case because we only care about register -> register
		_ = (byte2 >> 6) & 0b00000011

		// The registers involved in the move operation
		reg := (byte2 >> 3) & 0b00000111 // the reg field
		rm := byte2 & 0b00000111         // the r/m field

		var source string
		var dest string
		switch d {
		case 0b00:
			source = decodeRegister(reg, w)
			dest = decodeRegister(rm, w)
		case 0b01:
			source = decodeRegister(rm, w)
			dest = decodeRegister(reg, w)
		}

		// Append a newline and write the decoded instruction to the result buffer
		result.WriteString(fmt.Sprintf("\nmov %s, %s", dest, source))
	}

	fmt.Println(result.String())
}

func main() {
	args := os.Args[1:]
	if len(args) < 1 {
		fmt.Println("Missing required path argument")
		return
	}

	path := os.Args[1] // path to the binary that will be decoded
	decodeInstructions(path)
}
