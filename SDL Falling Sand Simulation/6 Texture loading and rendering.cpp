////https://lazyfoo.net/tutorials/SDL/07_texture_loading_and_rendering/index.php
//
////Include SDL and standard IO
//#include <SDL.h>
//#include <stdio.h>
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
////Frees up media and shuts down SDL
//void close();
//
////Loads individual image as a texture
//SDL_Texture* loadTexture(std::string path);
//
////Render window
//SDL_Window* gWindow = NULL;
//
////Window Renderer
//SDL_Renderer* gRenderer =  NULL;
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
//	gTexture = loadTexture("assets/texture.png");
//	if (gTexture == NULL) {
//		printf("Failed to load texture image!\n");
//		success = false;
//	}
//
//	return success;
//}
//
//void close() {
//	//Free loaded image
//	SDL_DestroyTexture(gTexture);
//	gTexture = NULL;
//
//	//Destroy Window
//	SDL_DestroyRenderer(gRenderer);
//	SDL_DestroyWindow(gWindow);
//	gRenderer = NULL;
//	gWindow = NULL;
//
//	//Quit SDL subsystems
//	IMG_Quit();
//	SDL_Quit();
//}
//
//int main(int argc, char* args[]) {
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
//			while (!quit) {
//				
//				//Handle events on queue
//				while (SDL_PollEvent(&e) != 0) {
//					
//					//User requests quit
//					if (e.type == SDL_QUIT) {
//						quit = true;
//					}
//				}
//
//				//Clear screen
//				SDL_RenderClear(gRenderer);
//
//				//Render texture to screen
//				SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
//
//				//Update screen
//				SDL_RenderPresent(gRenderer);
//			}
//			
//		}
//	}
//
//	//Free resources and close SDL
//	close();
//
//	return 0;
//}