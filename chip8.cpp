/*
*   chip8.cpp
*   An emulator implementing the Chip8 Virtual Machine
*   By Brian Mansfield
*/

#include <stdio.h>
#include <string>
#include <random>
#include "Chip8.h"

const int SCREEN_WIDTH  = 64;
const int SCREEN_HEIGHT = 32;

/*
* systems memory map:
* 0x000-0x1FF - Chip8 interpreter, contains font set in emu
* 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
* 0x200-0XFFF - Program ROM and work RAM
*/

unsigned char last_key;

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

chip8::chip8()
{
	// empty
}

chip8::~chip8()
{
	// empty
}

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

void resetKeyStates(){
    for (auto& k : key)
        k = 0;
    last_key = 0;
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
    resetKeyStates();

    drawFlag = true;
}

// given opcode 0xXNNN, sets the program counter to address NNN
void setPCToAddr(unsigned short opcode){
    pc = opcode & 0x0FFF;
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
                    clearDisplay();
                    drawFlag = true;
                    pc += 2;
                    break;
                // 0x00EE: return from subroutine
                case 0x000E:
                    --sp;
                    pc = stack[pc];
                    break;
                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
            break;
        }
        // 1NNN: Jump to address NNN
        case 0x1000:{
            setPCtoAddr(opcode);
        }
        // 2NNN: call the subroutine at address NNN
        case 0x2000:{
            // place the program counter on the stack
            stack[sp] = pc;
            ++sp;
            setPCtoAddr(opcode);
        }
        // 3XNN: skip next instruction if VX == NN
        // usually next instr is jump to skip a code block
        case 0x3000:{
            if (V[opcode & 0x0F00 >> 8] == (opcode & 0x00FF))
                pc += 4;
                break;
            pc += 2;
            break;
        }
        // 4XNN: skip next instruction if VX != NN
        // usually next instr is jump to skip a code block
        case 0x4000:{
            if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                pc += 4;
                break;
            pc += 2;
            break;
        }
        // 5XY0: skips next instruction if VX == VY
        case 0x5000:{
            if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
                pc += 4;
                break;
            pc += 2;
            break;
        }
        // 6XNN: set VX to NN
        case 0x6000:{
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            pc += 2;
            break;
        }
        // 7XNN: Add NN to VX (Carry flag not changed)
        case 0x7000:{
            V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            pc += 2;
            break;
        }
        // 8XY0: Set VX to value of VY
        case 0x8000:{
            switch (opcode & 0x000F){
                // 8XY0: Set VX to value of VY
                case 0x0000:{
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                }
                // 8XY1: Set VX to VX | VY
                case 0x0001:{
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                }
                // 8XY2: Set VX to VX & VY
                case 0x0002:{
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                }
                // 8XY3: Set VX to VX ^ VY
                case 0x0003:{
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                }
                // 8XY4: Set VX to VX + VY. VF is set 1 to indicate a carry.
                case 0x0004:{

                    if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                        V[0xF] = 1; // carry
                    else
                        V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                }
                // 8XY5: Set VX to VX - VY. VF is set 0 to indicate a borrow.
                case 0x0005:{

                    if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
                        V[0xF] = 0; // borrow
                    else
                        V[0xF] = 1;
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                }
                // 0x8XY6: VX = Vy >> 1. VF set to rightmost VY bit before shift.
                case 0x0006:{
                    V[15] = (V[(opcode & 0x00F0) >> 4] & 0x0001);
                    V[(opcode & 0x0F00) >> 8] = V[(opcode 0x00F0) >> 4] >> 1;
                    pc += 2;
                    break;
                }
                // 8XY7: Vx = VY - VX. VF is set 0 to indicate a borrow.
                case 0x0007:{
                    if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
                        V[0xF] = 0; // borrow
                    else
                        V[0xF] = 1;
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                }
                // 8XYE: VX = Vy = VY << 1. VF set to leftmost bit of VY before shift.
                case 0x000E:{
                    V[0xFF] = V[(opcode & 0x00F0) >> 4] & 0x1000;
                    V[(opcode & 0x00F0) >> 4] <<= 1;
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                }
            }
        }
        // 9XY0: skip next instr if VX != VY
        // Usually next instruction is a jump to skip a code block
        case 0x9000:{
            if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
                pc += 4;
            else
                pc += 2;
            break;
        }
        // ANNN: sets I to address NNN
        case 0xA000:{
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        }
        // BNNN: PC = V0 + NNN. jumps to address NNN + V0.
        case 0xB000:{
            pc = V[0x0] + (opcode & 0x0FFF);
            pc += 2;
            break;
        }
        // CXNN: VX = rand(0, 255) & NN.
        case 0xC000:{
            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_real_distibution<double> dist (0.0, 255.0)
            V[(opcode 0x0F00) >> 8] = dist(mt) & (opcode & 0x00FF);
            pc += 2;
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
        case 0xE000:{
            switch (opcode & 0x00FF){
                // EX9E: skip next instr if key stored in VX is pressed
                // usually next instruction is jump to skip a code block
                case 0x009E:{
                    if (key[V[(opcode & 0x0F00) >> 8]] != 0)
                        pc += 4;
                    else
                        pc += 2;
                    break;
                }
                // EXA1: skip next instr if key stored in VX is NOT pressed
                // usually next instruction is jump to skip a code block
                case 0x00A1:{
                    if (key[V[(opcode & 0x0F00) >> 8]] == 0)
                        pc += 4;
                    else
                        pc += 2;
                    break;
                }
            }
        }
        case 0xF000:{
            switch (opcode & 0x00FF){
                // FX07: set VX to the value of the delay timer
                case 0x0007:{
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                    break;
                }
                // FX0A: await key press, then store key in VX
                // BLOCKING OPERATION! all instr halted until next key event!
                case 0x000A:{
                    bool keyPress = false;

					for(int i = 0; i < 16; ++i)
					{
						if(key[i] != 0)
						{
							V[(opcode & 0x0F00) >> 8] = i;
							keyPress = true;
						}
					}

					// If we didn't received a keypress, skip this cycle and try again.
					if(!keyPress)
						return;

					pc += 2;
                    break;
                }
                // FX15: set delay timer to VX
                case 0x0015:{
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                }
                // FX18: set sound timer to VX
                case 0x0018:{
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                }
                // FX1E: add VX to I
                case 0x001E:{
                    if(I + V[(opcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
						V[0xF] = 1;
					else
						V[0xF] = 0;
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                }
                // FX29: set I to the location of the sprite for character in VX
                // Characters 0-F are represented by 4x5 font
                case 0x0029:{
                    I = V[(opcode & 0x0F00) >> 8] * 0x5;
					pc += 2;
                    break;
                }
                // FX33: store binary-coded decimal representation of VX at
                // memory address I, I + 1, I + 2 (hundreds, tens, ones digits resp.)
                case 0x0033:{
                    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                    break;
                }
                // FX55: stores [V0 - VX] in memory starting at addr I
                case 0x0055:{
                    for (auto i = 0x0; i <= ((opcode & 0x0F00) >> 8); ++i, ++I)
                        memory[I] = V[i];
                    pc+= 2;
                    break;
                }
                // FX65: loads [V0 - VX] from memory starting at addr I
                case 0x0065:{
                    for (auto i = 0x0; i <= ((opcode & 0x0F00) >> 8); ++i, ++I)
                        V[i] = memory[I];
                    pc+= 2;
                    break;
                }
            }
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

void chip8::debugRender() {
	// Draw
	for(int y = 0; y < 32; ++y)
	{
		for(int x = 0; x < 64; ++x)
		{
			if(gfx[(y*64) + x] == 0)
				printf("O");
			else
				printf(" ");
		}
		printf("\n");
	}
	printf("\n");
}

bool chip8::loadApplication(const char * filename) {
	initialize();
	printf("Loading: %s\n", filename);

	// Open file
	FILE * pFile = fopen(filename, "rb");
	if (pFile == NULL)
	{
		fputs ("File error", stderr);
		return false;
	}

	// Check file size
	fseek(pFile , 0 , SEEK_END);
	long lSize = ftell(pFile);
	rewind(pFile);
	printf("Filesize: %d\n", (int)lSize);

	// Allocate memory to contain the whole file
	char * buffer = (char*)malloc(sizeof(char) * lSize);
	if (buffer == NULL)
	{
		fputs ("Memory error", stderr);
		return false;
	}

	// Copy the file into the buffer
	size_t result = fread (buffer, 1, lSize, pFile);
	if (result != lSize)
	{
		fputs("Reading error",stderr);
		return false;
	}

	// Copy buffer to Chip8 memory
	if((4096-512) > lSize)
	{
		for(int i = 0; i < lSize; ++i)
			memory[i + 512] = buffer[i];
	}
	else
		printf("Error: ROM too big for memory");

	// Close file, free buffer
	fclose(pFile);
	free(buffer);

	return true;
}
