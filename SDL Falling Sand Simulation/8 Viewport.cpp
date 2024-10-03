////https://lazyfoo.net/tutorials/SDL/09_the_viewport/index.php
//
//#include <stdio.h>
//#include <SDL.h>
//#include <string>
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
////Loads texture
//SDL_Texture* loadTexture(std::string path);
//
////Render Window
//SDL_Window* gWindow = NULL;
//
////Window Renderer
//SDL_Renderer* gRenderer = NULL;
//
////Current displayed texture
//SDL_Texture* gTexture = NULL;
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
//SDL_Texture* loadTexture(std::string path) {
//	//The final texture
//	SDL_Texture* newTexture = NULL;
//
//	//Load image at specified path
//	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
//	if (loadedSurface == NULL) {
//		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
//	}
//	else {
//		//Create texture from surface pixels
//		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
//		if (newTexture == NULL) {
//			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
//		}
//
//		//Get rid of old loaded surface
//		SDL_FreeSurface(loadedSurface);
//	}
//
//	return newTexture;
//}
//
//bool loadMedia() {
//	//Loading success flag
//	bool success = true;
//
//	//Load PNG texture
//	gTexture = loadTexture("assets/viewport.png");
//	if (gTexture == NULL) {
//		printf("Failed to load texture image\n");
//		success = false;
//	}
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
//				//Top left corner viewport
//				SDL_Rect topLeftViewport;
//				topLeftViewport.x = 0;
//				topLeftViewport.y = 0;
//				topLeftViewport.w = SCREEN_WIDTH / 2;
//				topLeftViewport.h = SCREEN_HEIGHT / 2;
//				SDL_RenderSetViewport(gRenderer, &topLeftViewport);
//
//				//Render texture to screen
//				SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
//
//				//Top right viewport
//				SDL_Rect topRightViewport;
//				topRightViewport.x = SCREEN_WIDTH / 2;
//				topRightViewport.y = 0;
//				topRightViewport.w = SCREEN_WIDTH / 2;
//				topRightViewport.h = SCREEN_HEIGHT / 2;
//				SDL_RenderSetViewport(gRenderer, &topRightViewport);
//
//				//Render texture to screen
//				SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
//
//				//Bottom viewport
//				SDL_Rect bottomViewport;
//				bottomViewport.x = 0;
//				bottomViewport.y = SCREEN_HEIGHT / 2;
//				bottomViewport.w = SCREEN_WIDTH;
//				bottomViewport.h = SCREEN_HEIGHT / 2;
//				SDL_RenderSetViewport(gRenderer, &bottomViewport);
//
//				//Render texture to screen
//				SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
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