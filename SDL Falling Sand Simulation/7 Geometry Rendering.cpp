////https://lazyfoo.net/tutorials/SDL/08_geometry_rendering/index.php
//
//#include <stdio.h>
//#include <SDL.h>
//#include <string.h>
//#include <SDL_image.h>
//
////Screen dimension constants
//const int SCREEN_WIDTH = 640;
//const int SCREEN_HEIGHT = 480;
//
////Starts up SDL and creates window
//bool init();
//
////Loads media
//bool loadMedia();
//
////Frees media and shuts down SDL
//void close();
//
////Render Window
//SDL_Window* gWindow = NULL;
//
////Window Renderer
//SDL_Renderer* gRenderer = NULL;
//
//bool init() {
//	//Initialisation flag
//	bool success = true;
//
//	//Initialise SDL
//	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//		printf("SDL could not initialise! SDL_Error: %s\n", SDL_GetError());
//		success = false;
//	}
//	else {
//		//Create Window:
//		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
//
//		if (gWindow == NULL) {
//			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
//			success = false;
//		}
//		else {
//			//Create a renderer for the window
//			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
//			if (gRenderer == NULL) {
//				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
//				success = false;
//			}
//			else {
//				//Initialise Renderer colour
//				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
//
//				//Initialise PNG loading
//				int imgFlags = IMG_INIT_PNG;
//				if (!(IMG_Init(imgFlags) & imgFlags)) {
//					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
//					success = false;
//				}
//			}
//		}
//	}
//	return success;
//}
//
//bool loadMedia() {
//	//Loading success flag
//	bool success = true;
//	
//	//Nothing to load
//	return success;
//}
//
//void close() {
//	//Destroy Window
//	SDL_DestroyRenderer(gRenderer);
//	SDL_DestroyWindow(gWindow);
//	gRenderer = NULL;
//	gWindow = NULL;
//
//	//Quit SDL subsystem
//	SDL_Quit();
//}
//
//int main(int argc, char* args[]) {
//	//Start up SDL and create window
//	if (!init()) {
//		printf("Failed to initialise!\n");
//	}
//	else {
//		//Load Media
//		if (!loadMedia()) {
//			printf("Failed to load media!\n");
//		}
//		else {
//
//			//Main loop flag
//			bool quit = false;
//
//			//Event Handler
//			SDL_Event e;
//
//
//			//While application is running
//			while (!quit) {
//				//Handle events on queue
//				while (SDL_PollEvent(&e) != 0) {
//
//					//User requests quit
//					if (e.type == SDL_QUIT) {
//						quit = true;
//					}
//				}
//				
//				//Clear Screen
//				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
//				SDL_RenderClear(gRenderer);
//
//				//Render red filled quad
//				SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
//				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
//				SDL_RenderFillRect(gRenderer, &fillRect);
//
//				//Render green outlined quad.
//				SDL_Rect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
//				SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
//				SDL_RenderDrawRect(gRenderer, &outlineRect);
//
//				//Draw pale blue horizontal line
//				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
//				SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);
//
//				//Draw a vertical line of yellow dots
//				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0x00, 0xFF);
//				for (int i = 0; i < SCREEN_HEIGHT; i+=4) {
//					SDL_RenderDrawPoint(gRenderer, SCREEN_WIDTH / 2, i);
//				}
//
//				//Update screen
//				SDL_RenderPresent(gRenderer);
//			}
//		}
//	}
//
//	//Free resources and close SDL
//	close();
//
//	return 0;
//}