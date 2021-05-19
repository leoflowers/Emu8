#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class Platform{
    private:
	    SDL_Window* window;
	    SDL_Renderer* renderer;
	    SDL_Texture* texture;

    public:
		Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
        ~Platform();
		
		void Update(void const* buffer, int pitch);
		bool Platform::ProcessInput(unsigned int* keys);
};