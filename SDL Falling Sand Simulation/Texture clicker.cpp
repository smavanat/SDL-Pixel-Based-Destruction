#include<SDL.h>
#include<stdio.h>
#include<SDL_image.h>
#include<iostream>
#include "Texture.hpp"
#include "Outline.hpp"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

////Some global variables
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
Texture testTexture = Texture(240, 190);

//For holding our textures. Needs to be a pointer vector because otherwise will get error because the texture objects 
//are being copied over to a new location and the originals deleted.
//This means that the old pointers for surfacePixels etc. are being invalidated, hence the memory write errors.
std::vector<Texture*> textures;

int scale = 5;

bool init();
bool loadMedia();

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}
	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load Foo' texture
	if (!testTexture.loadPixelsFromFile("assets/foo.png"))
	{
		printf("Failed to load Foo' texture!\n");
		success = false;
	}
	else {

		if (!testTexture.loadFromPixels(gRenderer))
		{
			printf("Unable to load Foo' texture from surface!\n");
		}
	}

	return success;
}

void close()
{
	//Free loaded images
	for (int i = 0; i < textures.size(); i++) {
		textures[i]->free();
	}

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[]) {

	if (!init()) {
		printf("Failed to initialize!\n");
	}
	else {
		if (!loadMedia()) {
			printf("Failed to load media!\n");
		}
		else {
			bool quit = false;
			bool leftMouseButtonDown = false;
			bool rightMouseButtonDown = false;
			Texture* dragTexture = NULL;
			textures.push_back(&testTexture);//This works fine now.
			std::vector<std::vector<int>> testingPoints;
			bool getOutline = false;
			bool getSimplifiedOutline = false;
			//Testing for box2d:

			SDL_Event e;
			while (!quit) {
				int x, y;
				while (SDL_PollEvent(&e)) {
					switch (e.type) {
					case SDL_QUIT:
						quit = true;
						break;
					//Reseting bool values
					case SDL_MOUSEBUTTONUP:
						if (e.button.button == SDL_BUTTON_LEFT) {
							//contourFinder();
							std::vector<Texture*> texturesToRemove;
							std::vector<Texture*> texturesToAdd;

							for (Texture* t : textures) {
								if (t->isAltered()) {
									for (Texture* texture : splitTextureAtEdge(t, gRenderer)) {
										texturesToAdd.push_back(texture);
									}
									t->resetSplittingFlag();
									texturesToRemove.push_back(t);
								}
							}

							for (Texture* t : texturesToAdd) {
								textures.push_back(t);
							}
							texturesToAdd.clear();

							for (Texture* t : texturesToRemove) {
								textures.erase(find(textures.begin(), textures.end(), t));
								t->free();
							}
							texturesToRemove.clear();
							/*testingPoints = marchingSquares(textures[0]);*/

							printf("Number of Textures: %i\n", textures.size());
							leftMouseButtonDown = false;
						}
						if (e.button.button == SDL_BUTTON_RIGHT) {
							rightMouseButtonDown = false;
							dragTexture = NULL;
						}
						break;
					case SDL_MOUSEBUTTONDOWN:
						if (e.button.button == SDL_BUTTON_LEFT) {
							leftMouseButtonDown = true;
							getOutline = false;
							getSimplifiedOutline = false;
							for (Texture* t : textures) {
								if (e.motion.x >= t->getOriginX() && e.motion.x < t->getOriginX() + t->getWidth()
									&& e.motion.y < t->getOriginY() + t->getHeight() && e.motion.y >= t->getOriginY()) {
									erasePixels(t, gRenderer, scale, e.motion.x, e.motion.y);
								}
							}
						}
						if (e.button.button == SDL_BUTTON_RIGHT && e.motion.x >= 0 && e.motion.x < 640 && e.motion.y < 480 && e.motion.y >= 0) {
							rightMouseButtonDown = true;
							SDL_GetMouseState(&x, &y);
							for (Texture* t : textures) {
								if (e.motion.x >= t->getOriginX() && e.motion.x < t->getOriginX() + t->getWidth()
									&& e.motion.y < t->getOriginY() + t->getHeight() && e.motion.y >= t->getOriginY() 
									&& !t->clickedOnTransparent(e.motion.x, e.motion.y)) {
									dragTexture = t;
									break; 
								}
							}
						}
						break;
					case SDL_MOUSEMOTION:
						if (leftMouseButtonDown && e.motion.x >= 0 && e.motion.x < 640 && e.motion.y < 480 && e.motion.y >= 0) {
							for (Texture* t : textures) {
								if (e.motion.x >= t->getOriginX() && e.motion.x < t->getOriginX() + t->getWidth()
									&& e.motion.y < t->getOriginY() + t->getHeight() && e.motion.y >= t->getOriginY()) {
									erasePixels(t, gRenderer, scale, e.motion.x, e.motion.y);
								}
							}
						}
						//Dragging functionality
						if (dragTexture != NULL) {
							int newX = dragTexture->getOriginX() + e.motion.xrel;
							int newY = dragTexture->getOriginY() + e.motion.yrel;
							dragTexture->setOrigin(newX, newY);
						}
						break;
					case SDL_KEYDOWN:
						if (e.key.keysym.sym == SDLK_o) {
							testingPoints.clear();
							for (int i = 0; i < textures.size(); i++) {
								std::vector<int> tempPoints = marchingSquares(textures[i]);
								testingPoints.push_back(tempPoints);
							}
							getOutline = true;
							//printf("Testing Distance: %f",lineDist(13, 4, 20, 5));
						}
						if (e.key.keysym.sym == SDLK_s) {
							getSimplifiedOutline = true;
						}
						break;
					}
				}
				//This is where all the functionality in the main loop will go.
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				for (Texture* t : textures) {
					t->render(gRenderer);
				}
				if (getOutline && ! getSimplifiedOutline) {
					for (int i = 0; i < testingPoints.size(); i++) {
						//testingPoints = marchingSquares(textures[i]);
						for (int j = 0; j < testingPoints[i].size() - 1; j++) {
							//printf("%i", testingPoints[j]);
							//Marching squares
							SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
							SDL_RenderDrawLine(gRenderer, textures[i]->getOriginX() + (testingPoints[i][j] % textures[i]->getWidth()),
								textures[i]->getOriginY() + (int)(floor(testingPoints[i][j] / textures[i]->getWidth())),
								textures[i]->getOriginX() + (testingPoints[i][j + 1] % textures[i]->getWidth()),
								textures[i]->getOriginY() + (int)(floor(testingPoints[i][j + 1] / textures[i]->getWidth())));
						}
					}
				}
				//rdp
				if (getSimplifiedOutline) {
					//printf("New Frame\n");
					for (int i = 0; i < testingPoints.size(); i++) {
						std::vector<int> rdpPoints;
						//Position at size()-2 is where 0 is stored, which is what we want. This will give us the straight line
						//that we want.
						rdpPoints.push_back(testingPoints[i][testingPoints[i].size() - 2]);
						rdp(0, testingPoints[i].size() - 1, 5, textures[i]->getWidth(), testingPoints[i], rdpPoints);
						rdpPoints.push_back(testingPoints[i][testingPoints[i].size() - 2]);
						printf("rdpPoints size: %i\n", (int)rdpPoints.size());
						for (int j = 0; j < rdpPoints.size() - 1; j++) {
							SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
							SDL_RenderDrawLine(gRenderer, textures[i]->getOriginX() + (rdpPoints[j] % textures[i]->getWidth()),
								textures[i]->getOriginY() + (int)(floor(rdpPoints[j] / textures[i]->getWidth())),
								textures[i]->getOriginX() + (rdpPoints[j + 1] % textures[i]->getWidth()),
								textures[i]->getOriginY() + (int)(floor(rdpPoints[j + 1] / textures[i]->getWidth())));
						}
						printf("Moving onto a new Texture\n");
					}
				}
				SDL_RenderPresent(gRenderer);
			}
		}
	}
	close();
	return 0;
}