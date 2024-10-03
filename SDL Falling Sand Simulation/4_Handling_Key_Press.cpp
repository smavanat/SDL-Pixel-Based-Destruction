////https://lazyfoo.net/tutorials/SDL/04_key_presses/index.php
//
////Using SDL and standard IO
//#include <SDL.h>
//#include <stdio.h>
//#include <string>;
//
////Screen dimension constants
//const int SCREEN_WIDTH = 640;
//const int SCREEN_HEIGHT = 480;
//
////Key Press surface constants
//enum KeyPressSurfaces {
//	KEY_PRESS_SURFACE_DEFAULT,
//	KEY_PRESS_SURFACE_UP,
//	KEY_PRESS_SURFACE_DOWN,
//	KEY_PRESS_SURFACE_LEFT,
//	KEY_PRESS_SURFACE_RIGHT,
//	KEY_PRESS_SURFACE_TOTAL
//};
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
////Loads individual image
//SDL_Surface* loadSurface(std::string path);
//
////Render window
//SDL_Window* gWindow = NULL;
//
////Surface contained by the window
//SDL_Surface* gScreenSurface = NULL;
//
////Images that correspond to a keypress
//SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
//
////Current displayed image
//SDL_Surface* gCurrentSurface = NULL;
//
//SDL_Surface* loadSurface(std::string path) {
//	//Load image at specified path
//	SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
//	if (loadedSurface == NULL) {
//		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
//	}
//
//	return loadedSurface;
//}
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
//		gWindow = SDL_CreateWindow("HANDLING KEY PRESSES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
//		if (gWindow == NULL) {
//			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
//			success = false;
//		}
//		else {
//			//Get Window Surface
//			gScreenSurface = SDL_GetWindowSurface(gWindow);
//		}
//	}
//	return success;
//}
//
//bool loadMedia() {
//	//Loading success flag
//	bool success = true;
//
//	//Load default surface
//	gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("assets/press.bmp");
//
//	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL) {
//		printf("Failed to load default image!\n");
//		success = false;
//	}
//
//	//Load up surface
//	gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface("assets/up.bmp");
//
//	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] == NULL) {
//		printf("Failed to load up image!\n");
//		success = false;
//	}
//
//	//Load down surface
//	gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface("assets/down.bmp");
//
//	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL) {
//		printf("Failed to load down image!\n");
//		success = false;
//	}
//
//	//Load left surface
//	gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface("assets/left.bmp");
//
//	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL) {
//		printf("Failed to load left image!\n");
//		success = false;
//	}
//
//	//Load right surface
//	gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface("assets/right.bmp");
//
//	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] == NULL) {
//		printf("Failed to load right image!\n");
//		success = false;
//	}
//
//	return success;
//}
//
//void close() {
//	//Deallocate surface
//	SDL_FreeSurface(gCurrentSurface);
//	gCurrentSurface = NULL;
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
//			//Main loop flag
//			bool quit = false;
//
//			//Event Handler
//			SDL_Event e;
//
//			//Set default current surface
//			gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
//
//			//While application is running
//			while (!quit) {
//				while (SDL_PollEvent(&e) != 0) {
//					//User requests quit
//					if (e.type == SDL_QUIT) {
//						quit = true;
//					}
//					//User presses a key
//					else if (e.type == SDL_KEYDOWN) {
//
//						//Select surface based on key press
//						switch (e.key.keysym.sym) {
//
//							case SDLK_UP:
//								gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
//								break;
//
//							case SDLK_DOWN:
//								gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
//								break;
//
//							case SDLK_LEFT:
//								gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
//								break;
//
//							case SDLK_RIGHT:
//								gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
//								break;
//
//							default:
//								gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
//								break;
//						}
//					}
//				}
//
//				//Apply the image
//				SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
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