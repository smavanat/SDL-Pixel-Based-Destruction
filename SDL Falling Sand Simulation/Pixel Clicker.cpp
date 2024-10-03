//Basic implementation of creation of pixel on click just using screen surface without a texture. 
//Next step is to transfer this to a texture to make this permanent.

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init();

//Loads media
//bool loadMedia();

//Frees media and shuts down SDL
void close();

//Render Window
SDL_Window* gWindow = NULL;

//The Surface contained by the window. 
SDL_Surface* gScreenSurface = NULL;

SDL_Renderer* gRenderer = NULL;

bool init() {
	//Initialisation flag
	bool success = true;

	//Initialise SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialise! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//Create Window
		gWindow = SDL_CreateWindow("SDL TUTORIAL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else {
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
		}
	}

	return success;
}

//bool loadMedia() {
//	Loading success flag
//	bool success = true;
//
//	//Load splash image
//	gHelloWorld = SDL_LoadBMP("assets/hello_world.bmp");
//	if (gHelloWorld == NULL) {
//		printf("Unable to load image %s! SDL_Error: %s\n", "SDL Falling Sand Simulation/Resource Files/hello_world.bmp", SDL_GetError);
//		success = false;
//	}
//
//	return success;
//}

void close() {
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;

	//Destroy Window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystem
	SDL_Quit();
}

int main(int argc, char* args[]) {
	//Start up SDL and create window
	if (!init()) {
		printf("Failed to initialise!\n");
	}
	else {
		//Load Media
		//if (!loadMedia()) {
		//	printf("Failed to load media!\n");
		//}
		//else {

			//Main loop flag
			bool quit = false;

			bool leftMouseButtonDown = false;

			//Event Handler
			SDL_Event e;

			int x = 0;
			int y = 0;

			//While application is running
			while (!quit) {

				SDL_RenderSetScale(gRenderer, 5, 5);
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
				SDL_RenderDrawPoint(gRenderer, 0, 0);

				SDL_WaitEvent(&e);
				//Handle events on queue
				switch (e.type) {
				case (SDL_QUIT):
					quit = true;
					break;
				case(SDL_MOUSEBUTTONDOWN):
					leftMouseButtonDown = true;
					SDL_GetMouseState(&x, &y);
					printf("%i, %i ", x, y);
					printf("true");
					SDL_RenderDrawPoint(gRenderer, x / 5, y / 5);
				case(SDL_MOUSEBUTTONUP):
					leftMouseButtonDown = false;
				}	
				
				//Apply the image

				if (leftMouseButtonDown == true) {
					printf("true");
					SDL_RenderDrawPoint(gRenderer, x/5, y/5);
				}

				
				//Update screen
				SDL_RenderPresent(gRenderer);
			}
		//}
	}

	//Free resources and close SDL
	close();

	return 0;
}