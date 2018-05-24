/*
*   chip8.cpp
*   An emulator implementing the Chip8 Virtual Machine
*   By Brian Mansfield
*/

// stores the current opcode
// opcodes are two bytes, so we use a short
unsigned short opcode;

// 4k memory
unsigned char memory[4046];

// holds the CPU registers
// 15 8-bit general purpose registers, 16th is for carry flag
unsigned char V[16];

// index register 0x00 to 0xFFF
unsigned short I;
// program counter 0x000 to 0xFFF
unsigned short pc;

/*
* systems memory map:
* 0x000-0x1FF - Chip8 interpreter, contains font set in emu
* 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
* 0x200-0XFFF - Program ROM and work RAM
*/

// screen has 64 x 32 = 2048 pixels
// pixel state is 1 or 0
unsigned char gfx[64 * 32];

// countdown timers
// when set above 0, they will count down at 60Hz
unsigned char delay_timer;
// buzzer sounds when sound timer reaches 0
unsigned char sound_timer;

// 16 level of stack, hold the current location before a jump is performed
unsigned short stack[16];
unsigned short sp;

// HEX based keypad 0x0 - 0xF
unsigned char key[16];
