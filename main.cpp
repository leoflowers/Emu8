#include <stdio.h>
#include <fstream>
#include <sys/stat.h>
#include <stdlib.h>
#include <string>
#include <random>
#include <cstdint>
#include <chrono>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>


const unsigned int FONTSSET_START_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSSET_SIZE = 80;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;


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


class Platform{
    private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;

    public:
	Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight){
	    SDL_Init(SDL_INIT_VIDEO);
	    
	    window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
	}


	~Platform(){
	    SDL_DestroyTexture(texture);
	    SDL_DestroyRenderer(renderer);
	    SDL_DestroyWindow(window);
	    SDL_Quit();
	}

	
	void Update(void const* buffer, int pitch){
	    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
	    SDL_RenderClear(renderer);
	    SDL_RenderCopy(renderer, texture, nullptr, nullptr); 
	    SDL_RenderPresent(renderer);
	}


	bool ProcessInput(unsigned int* keys){
	    bool quit = false;
	    SDL_Event event;

	    while(SDL_PollEvent(&event)){
		switch(event.type){
		    case SDL_QUIT:
			quit = true;
			break;
		    case SDL_KEYDOWN:
			switch(event.key.keysym.sym){
			    case SDLK_ESCAPE:
				quit = true;
				break;
			    case SDLK_x:
				keys[0] = 1;
				break;
			    case SDLK_1:
				keys[1] = 1;
				break;
			    case SDLK_2:
				keys[2] = 1;
				break;
			    case SDLK_3:
				keys[3] = 1;
				break;
			    case SDLK_q:
				keys[4] = 1;
				break;
			    case SDLK_w:
				keys[5] = 1;
				break;
			    case SDLK_e:
				keys[6] = 1;
				break;
			    case SDLK_a:
				keys[7] = 1;
				break;
			    case SDLK_s:
				keys[8] = 1;
				break;
			   case SDLK_d:
				keys[9] = 1;
				break;
			   case SDLK_z:
				keys[0xA] = 1;
				break;
			   case SDLK_c:
				keys[0xB] = 1;
				break;
			   case SDLK_4:
				keys[0xC] = 1;
				break;
			   case SDLK_r:
				keys[0xD] = 1;
				break;
			   case SDLK_f:
				keys[0xE] = 1;
				break;
			   case SDLK_v:
				keys[0xF] = 1;
				break;
			}
			break;
		    case SDL_KEYUP:
			switch(event.key.keysym.sym){
			    case SDLK_ESCAPE:
				quit = true;
				break;
			    case SDLK_x:
				keys[0] = 1;
				break;
			    case SDLK_1:
				keys[1] = 1;
				break;
			    case SDLK_2:
				keys[2] = 1;
				break;
			    case SDLK_3:
				keys[3] = 1;
				break;
			    case SDLK_q:
				keys[4] = 1;
				break;
			    case SDLK_w:
				keys[5] = 1;
				break;
			    case SDLK_e:
				keys[6] = 1;
				break;
			    case SDLK_a:
				keys[7] = 1;
				break;
			    case SDLK_s:
				keys[8] = 1;
				break;
			   case SDLK_d:
				keys[9] = 1;
				break;
			   case SDLK_z:
				keys[0xA] = 1;
				break;
			   case SDLK_c:
				keys[0xB] = 1;
				break;
			   case SDLK_4:
				keys[0xC] = 1;
				break;
			   case SDLK_r:
				keys[0xD] = 1;
				break;
			   case SDLK_f:
				keys[0xE] = 1;
				break;
			   case SDLK_v:
				keys[0xF] = 1;
				break;
			}
			break;
		}
	    }
	    return quit;
	}
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

        
	/*Chip8(){

          //  pc = START_ADDRESS;

            //for(int i = 0; i < FONTSSET_SIZE; i++)
            //    memory[FONTSSET_SIZE + i] = fontset[i];}
	}*/


        Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()){
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


        void LoadROM(char const* filename);
	void Cycle();


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


int main(int argc, char__ argv){
    if(argc != 4){
	std::cerr << "Not enough arguments, try again" << std::endl;
	std::exit(EXIT_FAILURE);
    }


    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const* rom = argv[3];

    Platform platform("CHIP-8 Emulator", VIDEO_WIDTH*videoScale, VIDEO_HEIGHT*videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);


    Chip8 chip8; 
    chip8.LoadROM(rom);


    int videoPitch = sizeof(chip8.video[0])*VIDEO_WIDTH;


    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;


    while(!quit){
	quit = platform.ProcessInput(chip8.keypad);

	auto currentTime = std::chrono::high_resolution_clock::now();
	float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();


	if (dt > cycleDelay){
	    lastCycleTime = currentTime;
	    chip8.Cycle();
	    platform.Update(chip8.video, videoPitch);
	}
    }


    return 0;
}
