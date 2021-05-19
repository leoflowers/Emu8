#include <cstdlib>
#include <random>
#include <chrono>
#include <stdlib.h>

#include "font.h"
#include "platform.h"


#define START_ADDRESS 0x200


class Chip8{
    public:
        uint8_t registers[16];
        uint8_t memory[4096];
        uint16_t index;
        uint16_t pc;
        uint16_t stack[16];
        uint8_t sp;
        uint8_t delayTimer;
        uint8_t soundTimer;
        uint8_t keypad[16];
        uint32_t video[64 * 32];
        uint8_t opcode;
        
	    std::default_random_engine randGen;
        std::uniform_int_distribution<uint8_t> randByte;

        // Constructor
        Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()){};

        // Methods
        void LoadROM(char const* filename);
	    void Cycle();

        // CPU Ops
        void OP_00E0(){
            memset(video, 0, sizeof(video));
        }
        void OP_00EE(){
            sp--;
            pc = stack[sp];
        }
        void OP_1nnn(struct Chip8 *self){
            unsigned int address = opcode & 0x0FFFu;
            pc = address;
        }
        void OP_2NNN(){
            unsigned int address = opcode & 0x0FFFu;

            stack[sp] = pc;
            sp++;
            pc = address;
        }
        void OP_3xkk(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int byte = opcode & 0x00FFu;

            if(registers[Vx] == byte){
                pc += 2;
            }
        }  
        void OP_4xkk(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int byte = opcode & 0x00FFu;

            if(registers[Vx] != byte){
                pc += 2;
            }
        }
        void OP_5xy0(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;

            if(registers[Vx] == registers[Vy])
                pc += 2;
        }       
        void OP_6xkk(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int byte = (opcode & 0x00FFu);

            registers[Vx] = byte;
        }     
        void OP_7xkk(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int byte = (opcode & 0x00FFu);

            registers[Vx] += byte;
        }
        void OP_8xy0(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;

            registers[Vx] = registers[Vy];
        }   
        void OP_8xy1(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;

            registers[Vx] |= registers[Vy];
        }             
        void OP_8xy2(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;

            registers[Vx] &= registers[Vy];
        }
        void OP_8xy3(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;

            registers[Vx] ^= registers[Vy];
        }
        void OP_8xy4(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;
            unsigned int sum =  registers[Vx] + registers[Vy];

            if(sum > 255U)
                registers[0xF] = 1;
            inlse
                registers[0xF] = 0;

            registers[Vx] = sum & 0xFFu;
        }   
        void OP_8xy5(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;
            unsigned int diff =  registers[Vx] - registers[Vy];

            if(registers[Vx] > registers[Vy])
                registers[0xF] = 1;
            else
                registers[0xF] = 0;

            registers[Vx] = diff;
        }   
        void OP_8xy6(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;

            registers[0xF] = registers[Vx] & 0x1u;

            registers[Vx] >>= 1u;
        }
        void OP_8xy7(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;

            if(registers[Vy] > registers[Vx])
                registers[0xF] = 1;
            else
                registers[0xF] = 0;

            registers[Vx] = registers[Vy] - registers[Vx];
        }        
        void OP_8xyE(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;

            registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

            registers[Vx] <<= 1u;
        }        
        void OP_9xy0(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;

            if(registers[Vx] != registers[Vy])
                pc += 2;
        }   
        void OP_Annn(){
            unsigned int address = opcode & 0x0FFFu;
            index = address;
        }   
        void OP_Bnnn(){
            unsigned int address = opcode & 0x0FFFu;
            index = address + registers[0];
        }   
        void OP_Cxkk(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int bytes = opcode & 0x00FFu;

            registers[Vx] = randByte(randGen) & bytes;
        }    
        void OP_Dxyn(){
            unsigned int Vx = (opcode & 0x0F00) >> 8u;
            unsigned int Vy = (opcode & 0x00F0) >> 4u;
            unsigned int spriteHeight = (opcode & 0x000F);

            unsigned int xPos = registers[Vx] % VIDEO_WIDTH;
            unsigned int yPos = registers[Vy] % VIDEO_HEIGHT;

            registers[0xF] = 0;

            for(unsigned int row = 0; row < sprite_height; i++){
                unsigned int spriteByte = memory[index + row];

                for(unsigned int spriteBit = 0; spriteBit < 8; spriteBit++){
                    unsigned int spritePixel = spriteByte & (0x80 >> spriteBit);
                    unsigned int* screenPixel = &video[((yPos + row) * VIDEO_WIDTH) + (xPos + sprtieBit)];

                    if(spritePixel){
                        if(*screenPixel == 0xFFFFFFFF)
                        registers[0xF] = 1;
                    }

                    *screenPixel ^= 0xFFFFFFFF;
                }
            }
        }                           
        void OP_Ex9E(){
            unsigned int Vx = (opcode & 0x0F00) >> 8u;

            if(keypad[registers[Vx]])
            pc += 2;
        }
        void OP_ExA1(){
            unsigned int Vx = (opcode & 0x0F00) >> 8u;
            
            if(!keypad[registers[Vx]])
            pc += 2;
        }
        void OP_Fx07(){
            unsigned int Vx = (opcode & 0x0F00) >> 8u;

            registers[Vx] = delayTimer;
        }
        void OP_Fx0A(){
            unsigned int Vx = (opcode & 0x0F00) >> 8u;
                
            for(int i = 0; i < 16; i++){
                if(keypad[i]){
                    registers[Vx] = i;
                    break;
                }
            }
            pc -= 2;
        }
        void OP_Fx15(){
            unsigned int Vx = (opcode & 0x0F00) >> 8u;

            delayTimer = registers[Vx];
        }
        void OP_Fx18(){
            unsigned int Vx = (opcode & 0x0F00) >> 8u;

            soundTimer = registers[Vx];
        }
        void OP_Fx1E(){
            unsigned int Vx = (opcode & 0x0F00) >> 8u;

            index += registers[Vx];
        }
        void OP_Fx29(){
            unsigned int Vx = (opcode & 0x0F00) >> 8u;
            unsigned int val = registers[Vx];

            index = FONTSSET_START_ADDRESS + (5*val);
        }
        void OP_Fx33(){
            unsigned int Vx = (opcode & 0x0F00) >> 8u;
            unsigned int val = registers[Vx];

            registers[index + 2] = val % 10;
            val /= 10;

            registers[index + 1] = val % 10;
            val /= 10;

            registers[index] = val % 10;
        }
        void OP_Fx55(){
            unsigned int Vx = (opcode & 0x0F00) >> 8u;

            for(unsigned int i = 0; i <= Vx; i++)
            memory[index + i] = registers[i];
        }
        void OP_Fx65(){
            unsigned int Vx = (opcode & 0x0F00) >> 8u;

            for(unsigned int i = 0; i <= Vx; i++)
                registers[i] = memory[index + i];
        }
        void Table0(){
            ((*this).*(table0[opcode & 0x000F]))();
        }
        void Table8(){
            ((*this).*(table8[opcode & 0x000F]))();
        }
        void TableE(){
            ((*this).*(tableE[opcode & 0x000F]))();
        }
        void TableF(){
            ((*this).*(tableF[opcode & 0x000F]))();
        }
        void OP_NULL(){}

        typedef void (Chip8::*Chip8Func)();
        Chip8Func table[0xF + 1]{&Chip8::OP_NULL};
        Chip8Func table0[0xE + 1]{&Chip8::OP_NULL};
        Chip8Func table8[0xE + 1]{&Chip8::OP_NULL};
        Chip8Func tableE[0xE + 1]{&Chip8::OP_NULL};
        Chip8Func tableF[0xE + 1]{&Chip8::OP_NULL};
};