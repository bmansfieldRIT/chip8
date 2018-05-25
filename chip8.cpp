/*
*   chip8.cpp
*   An emulator implementing the Chip8 Virtual Machine
*   By Brian Mansfield
*/

#include "chip.h"

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

void chip8::initialize(){

}

void chip8::emulateCycle(){
    // fetch opcode
    auto opcode = memory[pc] << 8 | memory[pc + 1];

    // decode opcode and execute opcodes
    switch(opcode & 0xF000){

        case 0x000:
            switch(opcode & 0x000F){
                // 0x00E0: clear screen
                case 0x0000:
                    break;
                // 0x00EE: return from subroutine
                case 0x000E:
                    break;
                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
        // 2NNN: call the subroutine at address NNN
        case 0x2000:
            // place the program counter on the stack
            stack[sp] = pc;
            ++sp;
            // set program counter to the new address
            pc = opcode & 0x0FFF;
        // ANNN: sets I to address NNN
        case 0xA000:
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0xB000:
            break;
    default:
        printf("Unknown opcode: 0x%X\n", opcode);
    }

    // update timers
    if (delay_timer > 0)
        --delay_timer;
    if (sound_timer > 0){
        if (sound_timer == 1)
            printf("BEEP!");
        --sound_timer;
    }
}

chip8 myChip8

int main(){
    // Set up the render systema nd register input callbacks
    setupGraphics();
    setupInput();

    // Initialize the chip* system and load the game into memory
    myChip8.initialize();
    myChip8.loadGame("pong");

    // emulation loop
    while(true){
        // emulate one cycle
        myChip8.emulateCycle();

        // if the draw flag is set, update the screen
        // only two opcode should set this - 0x00E0 (clear), 0xDXYN (draw)
        if (myChip8.drawFlag)
            drawGraphics();

        // store key press state (press and release)
        myChip8.setKeys();
    }
}
