package main

import (
	"log"
	"os"
)

func main() {
	l1, err := os.ReadFile("../../perfaware/part1/listing_0037_single_register_mov.asm")
	if err != nil {
		log.Println("Failed to read the first listing")
		return
	}

	l2, err := os.ReadFile("../../perfaware/part1/listing_0038_many_register_mov.asm")
	if err != nil {
		log.Println("Failed to read the second listing")
		return
	}

	log.Println(string(l1))
	log.Println(string(l2))
}
