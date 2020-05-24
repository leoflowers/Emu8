#include <stdio.h>
#include <fstream>
#include <sys/stat.h>
#include <stdlib.h>
#include <string>
#include <random>
#include <cstdint>
#include <chrono>

const unsigned int FONTSSET_START_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSSET_SIZE = 80;



unsigned int fontset[FONTSSET_SIZE] ={
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

        Chip8(){

            pc = START_ADDRESS;

            for(int i = 0; i < FONTSSET_SIZE; i++)
                memory[FONTSSET_SIZE + i] = fontset[i];
        }


        Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()){
            randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
        }


        void Chip8::LoadROM(char const* filename);


        void Chip8::OP_00E0(){
            memset(video, 0, sizeof(video));
        }


        void Chip8::OP_00EE(){
            sp--;
            pc = stack[sp];
        }

        void Chip8::OP_1nnn(struct Chip8 *self){
            unsigned int address = opcode & 0x0FFFu;
            pc = address;
        }


        void Chip8::OP_2NNN(){
            unsigned int address = opcode & 0x0FFFu;

            stack[sp] = pc;
            sp++;
            pc = address;
        }

        void Chip8::OP_3xkk(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int byte = opcode & 0x00FFu;

            if(registers[Vx] == byte){
                pc += 2;
            }
        }  
        
        void Chip8::OP_4xkk(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int byte = opcode & 0x00FFu;

            if(registers[Vx] != byte){
                pc += 2;
            }
        }

        void Chip8::OP_5xy0(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;

            if(registers[Vx] == registers[Vy])
                pc += 2;
        }       


        void Chip8::OP_6xkk(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int byte = (opcode & 0x00FFu);

            registers[Vx] = byte;
        }     


        void Chip8::OP_7xkk(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int byte = (opcode & 0x00FFu);

            registers[Vx] += byte;
        }

        void Chip8::OP_8xy0(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;

            registers[Vx] = registers[Vy];
        }   


        void Chip8::OP_8xy1(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;

            registers[Vx] |= registers[Vy];
        }             


        void Chip8::OP_8xy2(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;

            registers[Vx] &= registers[Vy];
        }


        void Chip8::OP_8xy3(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;

            registers[Vx] ^= registers[Vy];
        }


        void Chip8::OP_8xy4(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;
            unsigned int sum =  registers[Vx] + registers[Vy];

            if(sum > 255U)
                registers[0xF] = 1;
            else
                registers[0xF] = 0;

            registers[Vx] = sum & 0xFFu;
        }   


        void Chip8::OP_8xy5(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;
            unsigned int diff =  registers[Vx] - registers[Vy];

            if(registers[Vx] > registers[Vy])
                registers[0xF] = 1;
            else
                registers[0xF] = 0;

            registers[Vx] = diff;
        }   

        void Chip8::OP_8xy6(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;

            registers[0xF] = registers[Vx] & 0x1u;

            registers[Vx] >>= 1u;
        }


        void Chip8::OP_8xy7(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;

            if(registers[Vy] > registers[Vx])
                registers[0xF] = 1;
            else
                registers[0xF] = 0;

            registers[Vx] = registers[Vy] - registers[Vx];
        }        


        void Chip8::OP_8xyE(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;

            registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

            registers[Vx] <<= 1u;
        }        


        void Chip8::OP_9xy0(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int Vy = (opcode & 0x00F0u) >> 4u;

            if(registers[Vx] != registers[Vy])
                pc += 2;
        }   


        void Chip8::OP_Annn(){
            unsigned int address = opcode & 0x0FFFu;
            index = address;
        }   


        void Chip8::OP_Bnnn(){
            unsigned int address = opcode & 0x0FFFu;
            index = address + registers[0];
        }   


        void Chip8::OP_Cxkk(){
            unsigned int Vx = (opcode & 0x0F00u) >> 8u;
            unsigned int bytes = opcode & 0x00FFu;

            registers[Vx] = randByte(randGen) & bytes;
        }    


        void OP_Dxyn(struct Chip8 *self){
            // can pick up from here
        }                           
};











void Chip8::LoadROM(char const* filename){
    // opens rom
    std::ifstream rom;
    rom.open(filename);

    // reads rom into memory
    while(rom.is_open()){
        // get size/length of rom
        rom.seekg(0, rom.end);
        int size = rom.tellg();
        rom.close();

        // creates buffer or appropriate size
        char* buf = new char[size];

        // reads rom into memory from intermediate buffer    
        rom.read(buf, size);
        for(int i = 0; i < size; i++)
            memory[i + START_ADDRESS] = buf[i];


        delete[] buf;
    }
}




int main() {



    return 0;
}

/*
struct Chip8* new_state(){
    struct Chip8* chip = malloc(sizeof(struct Chip8));

    // initialize proper starting address
    chip->pc = START_ADDRESS;

    // load fonts into memory
    for(unsigned int i = 0; i < FONTSSET_SIZE; ++i)
        chip->memory[FONTSSET_START_ADDRESS + i] = fontset[i];

    return chip;
}
*/