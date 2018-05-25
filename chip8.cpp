/*
*   chip8.cpp
*   An emulator implementing the Chip8 Virtual Machine
*   By Brian Mansfield
*/

#include <stdio.h>
#include <string>
#include "Chip8.h"

const int SCREEN_WIDTH  = 64;
const int SCREEN_HEIGHT = 32;

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
unsigned char gfx[SCREEN_WIDTH * SCREEN_HEIGHT];

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

// chip8 font set. Each num or char is 4 px wide and 5 px high
unsigned char chip8_fontset[80] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void clearDisplay(){
    for (auto& p : gfx)
        p = 0;
}

void clearStack(){
    for (auto& l : stack)
        l = 0;
}

void clearRegisters(){
    for (auto& r : V)
        r = 0;
}

void clearMemory(){
    for (auto& m : memory)
        m = 0;
}

 void loadFontset(){
     for (int i = 0; i < 80; ++i)
         memory[i] = chip8_fontset[i];
 }

 void resetTimers(){
     delay_timer = 0;
     sound_timer = 0;
 }

void Chip8::initialize(){
    // program counter starts at 0x200
    pc = 0x200;
    // reset current opcode
    opcode = 0;
    // reset index register
    I = 0;
    // reset stack pointer
    sp = 0;

    clearDisplay();
    clearRegisters();
    clearMemory();
    loadFontset();
    resetTimers();
}

void Chip8::loadGame(std::string name){
    // load game into memory
}

void Chip8::setKeys(){
    // initialize keyboard array
}

void setupGraphics(){
    // initialize GLUT, etc.
}

void setupInput(){
    // initialize keyboard mappings, input, etc.
}

void drawGraphics(){
    // use GLUT to draw pixel on the screen, etc.
}

void Chip8::emulateCycle(){
    // fetch opcode
    auto opcode = memory[pc] << 8 | memory[pc + 1];

    // decode opcode and execute opcodes
    switch(opcode & 0xF000){

        case 0x000:{
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
            break;
        }
        // 2NNN: call the subroutine at address NNN
        case 0x2000:{
            // place the program counter on the stack
            stack[sp] = pc;
            ++sp;
            // set program counter to the new address
            pc = opcode & 0x0FFF;
        }
        // ANNN: sets I to address NNN
        case 0xA000:{
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        }
        case 0xB000:{
            break;
        }
        // DXYN: draw a pixel at coords VX, VY, that is N px high
        case 0xD000:{
            // get VX
            auto x = V[(opcode & 0x0F00) >> 8];
            // get VY
            auto y = V[(opcode & 0x00F0) >> 4];
            auto height = opcode & 0x000F;
            unsigned short pixel;

            // carry flag gets set to 1 if a collision occurs
            V[0xF] = 0;
            for (auto yline = 0; yline < height; yline++){
                // sprite bitcodes will be at mem locations (I - I+height)
                pixel = memory[I + yline];
                for (auto xline = 0; xline < 8; xline++){
                    // test if pixel is 1 (otherwise, do nothing)
                    if ((pixel & (0x80 >> xline)) != 0){
                        // test if display pixel is set to 1
                        if (gfx[(x + xline + ((y + yline) * SCREEN_WIDTH))] == 1)
                            V[0xF] = 1;
                        // per spec, XOR the bit in memory with 1
                        // we can hardcode 1 because we won't reach here if 0
                        gfx[(x + xline + ((y + yline) * SCREEN_WIDTH))] ^= 1;
                    }
                }
            }
            drawFlag = true;
            pc += 2;

            break;
        }
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

Chip8 chip8;

int main(){

    // Set up the render systema nd register input callbacks
    setupGraphics();
    setupInput();

    // Initialize the chip* system and load the game into memory
    chip8.initialize();
    chip8.loadGame("pong");

    // emulation loop
    while(true){
        // emulate one cycle
        chip8.emulateCycle();

        // if the draw flag is set, update the screen
        // only two opcode should set this - 0x00E0 (clear), 0xDXYN (draw)
        if (chip8.drawFlag)
            drawGraphics();

        // store key press state (press and release)
        chip8.setKeys();
    }
}
