#include <stdio.h>
#include <fstream>
#include <sys/stat.h>
#include <string>
#include <random>
#include <cstdint>


#include "../include/platform.h"


const unsigned int START_ADDRESS = 0x200;



int main(int argc, char* argv[]){
    if(argc != 4){
		std::cerr << "Not enough arguments, try again" << std::endl;
		std::exit(EXIT_FAILURE);
    }


    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    Platform platform("CHIP-8 Emulator", VIDEO_WIDTH*videoScale, VIDEO_HEIGHT*videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);


	char const* rom = argv[3];
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
