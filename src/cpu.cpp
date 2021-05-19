#include "../include/cpu.h"

Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()); {
            randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
            pc = START_ADDRESS;

            for(int i = 0; i < FONTSSET_SIZE; i++)
                memory[FONTSSET_SIZE + i] = fontset[i];

            table[0x0] = &Chip8::Table0;
            table[0x1] = &Chip8::OP_1nnn;
            table[0x2] = &Chip8::OP_2nnn;
            table[0x3] = &Chip8::OP_3xkk;
            table[0x4] = &Chip8::OP_4xkk;
            table[0x5] = &Chip8::OP_5xy0;
            table[0x6] = &Chip8::OP_6xkk;
            table[0x7] = &Chip8::OP_7xkk;
            table[0x8] = Table8;
            table[0x9] = &Chip8::OP_9xy0;
            table[0xA] = &Chip8::OP_Annn;
            table[0xB] = &Chip8::OP_Bnnn;
            table[0xC] = &Chip8::OP_Cxkk;
            table[0xD] = &Chip8::OP_Dxyn;
            table[0xE] = &Chip8::TableE;
            table[0xF] = &Chip8::Tablef;

            table0[0x0] = &Chip8::OP_00E0;
            table0[0xE] = &Chip8::OP_00EE;
            
            table8[0x0] = &Chip8::OP_8xy0;
            table8[0x1] = &Chip8::OP_8xy1;
            table8[0x2] = &Chip8::OP_8xy2; 
            table8[0x3] = &Chip8::OP_8xy3;
            table8[0x4] = &Chip8::OP_8xy4;
            table8[0x5] = &Chip8::OP_8xy5;
            table8[0x6] = &Chip8::OP_8xy6;
            table8[0x7] = &Chip8::OP_8xy7;
            table8[0xE] = &Chip8::OP_8xyE;

            tableE[0x1] = &Chip8::OP_ExA1;
            tableE[0xE] = &Chip8::OP_Ex9E;

            tableF[0x07] = &Chip8::OP_Fx07;
            tableF[0x0A] = &Chip8::OP_Fx0A;
            tableF[0x15] = &Chip8::OP_Fx15;
            tableF[0x18] = &Chip8::OP_Fx18;
            tableF[0x1E] = &Chip8::OP_Fx1E;
            tableF[0x29] = &Chip8::OP_Fx29;
            tableF[0x33] = &Chip8::OP_Fx33;
            tableF[0x55] = &Chip8::OP_Fx55;
            tableF[0x65] = &Chip8::OP_Fx65;
}


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
    

void Chip8::Cycle(){
    // fetching opcode
    opcode = (memory[pc] >> 8u) | memory[pc + 1];

    // increment pc
    pc += 2;

    // decode and execute
    ((*this).*(table[(opcode & 0xF000) >> 12u]))();

    // decrement delay timer if set
    if (delayTimer > 0)
	    delayTimer--;

    // decrement sound timer if set
    if (soundTime > 0)
	    soundTimer--;
}