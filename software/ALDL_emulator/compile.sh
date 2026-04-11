#! /bin/bash

clear

echo "Cleaning out directory..."
rm -rf out
mkdir out

echo "Compiling..."
avr-gcc -Wall -c emulator.c -o out/emulator.o -mmcu=atmega328p -D F_CPU=16000000UL -O2

avr-gcc -mmcu=atmega328p -o out/emulator.elf out/emulator.o

avr-size -C -x --mcu=atmega328p out/emulator.elf

echo "Writing output files..."
avr-objcopy out/emulator.elf -O ihex 	out/emulator.hex
avr-objcopy out/emulator.elf -O binary 	out/emulator.bin

echo "Done!"