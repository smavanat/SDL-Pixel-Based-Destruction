////https://lazyfoo.net/tutorials/SDL/05_optimized_surface_loading_and_soft_stretching/index.php
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
//SDL_Surface* gStretchedSurface = NULL;
//
//SDL_Surface* loadSurface(std::string path) {
//	//The final optimised image
//	SDL_Surface* optimisedSurface = NULL;
//
//	//Load image at specified path
//	SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
//
//	if (loadedSurface == NULL) {
//		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
//	}
//
//	else {
//		//Convert surface to screen format
//		optimisedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
//		if (optimisedSurface == NULL) {
//			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
//		}
//
//		//Get rid of old loaded surface
//		SDL_FreeSurface(loadedSurface);
//	}
//
//	return optimisedSurface;
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
//	gStretchedSurface = loadSurface("assets/stretch.bmp");
//
//	if (gStretchedSurface == NULL) {
//		printf("Failed to load default image!\n");
//		success = false;
//	}
//
//	return success;
//}
//
//void close() {
//	//Deallocate surface
//	SDL_FreeSurface(gStretchedSurface);
//	gStretchedSurface = NULL;
//
//	//Destroy Window
//	SDL_DestroyWindow(gWindow);
//	gWindow = NULL;
//
//	//Quit SDL subsystem
//	SDL_Quit();
//}
//
//int main(int argc, char* args[])
//{
//	//Start up SDL and create window
//	if (!init())
//	{
//		printf("Failed to initialize!\n");
//	}
//	else
//	{
//		//Load media
//		if (!loadMedia())
//		{
//			printf("Failed to load media!\n");
//		}
//		else
//		{
//			//Main loop flag
//			bool quit = false;
//
//			//Event handler
//			SDL_Event e;
//
//			//While application is running
//			while (!quit)
//			{
//				//Handle events on queue
//				while (SDL_PollEvent(&e) != 0)
//				{
//					//User requests quit
//					if (e.type == SDL_QUIT)
//					{
//						quit = true;
//					}
//				}
//
//				//Apply the image stretched
//				SDL_Rect stretchRect;
//				stretchRect.x = 0;
//				stretchRect.y = 0;
//				stretchRect.w = SCREEN_WIDTH;
//				stretchRect.h = SCREEN_HEIGHT;
//				SDL_BlitScaled(gStretchedSurface, NULL, gScreenSurface, &stretchRect);
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
