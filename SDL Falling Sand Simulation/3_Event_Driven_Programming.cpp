////https://lazyfoo.net/tutorials/SDL/03_event_driven_programming/index.php
//
////Using SDL and standard IO
//#include <SDL.h>
//#include <stdio.h>
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
////The Surface contained by the window. 
//SDL_Surface* gScreenSurface = NULL;
//
////The image we will load to show on the screen
//SDL_Surface* gHelloWorld = NULL;
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
//		//Create Window
//		gWindow = SDL_CreateWindow("SDL TUTORIAL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
//		if (gWindow == NULL) {
//			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
//			success = false;
//		}
//		else {
//			//Get Window surface
//			gScreenSurface = SDL_GetWindowSurface(gWindow);
//		}
//	}
//
//	return success;
//}
//
//bool loadMedia() {
//	//Loading success flag
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
//
//void close() {
//	//Deallocate surface
//	SDL_FreeSurface(gHelloWorld);
//	gHelloWorld = NULL;
//
//	//Destroy Window
//	SDL_DestroyWindow(gWindow);
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
//				//Apply the image
//				SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
//
//				//Update the surface
//				SDL_UpdateWindowSurface(gWindow);
//			}
//		}
//	}
//
//	//Free resources and close SDL
//	close();
//
//	return 0;
//}