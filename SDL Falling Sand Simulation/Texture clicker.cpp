#include<SDL.h>
#include<stdio.h>
#include<SDL_image.h>
#include<iostream>
#include <vector>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture class. This will contain our initial red square, that we will then modify and break up.
class Texture {
	public:
		Texture(int x, int y);

		~Texture();

		bool loadFromFile(std::string path);

		bool loadPixelsFromFile(std::string path);

		bool loadPixelsFromFile();

		bool loadFromPixels();

		void free();

		void setOrigin(int x, int y);

		void render();

		int getWidth();
		int getHeight();
		int getOriginX();
		int getOriginY();

		Uint32* getPixels32();
		Uint32 getPitch32();
		Uint32 mapRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	private:
		SDL_Texture* texture;

		SDL_Surface* oldSurface;

		SDL_Surface* surfacePixels;

		int width;
		int height;
		int originX;
		int originY;
};

//Some global variables
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
Texture testTexture = Texture(240, 190);

int scale = 10;

bool init();
bool loadMedia();

Texture::Texture(int x, int y) {
	texture = NULL;
	oldSurface = NULL;
	width = 0;
	height = 0;
	setOrigin(x, y);
}

Texture::~Texture() {
	free();
}

bool Texture::loadFromFile(std::string path) {
	if (!loadPixelsFromFile(path)) {
		printf("Failed to load pixels for %s!\n", path.c_str());
	}
	else {
		if (!loadFromPixels())
		{
			printf("Failed to texture from pixels from %s!\n", path.c_str());
		}
	}

	return texture != NULL;
}

bool Texture::loadPixelsFromFile(std::string path) {
	free();

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else {
		surfacePixels = SDL_ConvertSurfaceFormat(loadedSurface, SDL_GetWindowPixelFormat(gWindow), 0);
		if (surfacePixels == NULL) {
			printf("Unable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError());
		}
		else {
			width = surfacePixels->w;
			height = surfacePixels->h;
		}
		SDL_FreeSurface(loadedSurface);
	}
	return surfacePixels != NULL;
}

bool Texture::loadFromPixels() {
	if (surfacePixels == NULL) {
		printf("No pixels loaded!");
	}
	else {
		texture = SDL_CreateTextureFromSurface(gRenderer, surfacePixels);
		if (texture == NULL)
		{
			printf("Unable to create texture from loaded pixels! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			width = surfacePixels->w;
			height = surfacePixels->h;
		}

		//Get rid of old loaded surface
		//SDL_FreeSurface(surfacePixels);
		//surfacePixels = NULL;
	}
	return texture != NULL;
}

Uint32* Texture::getPixels32() {
	Uint32* pixels = NULL;

	if (surfacePixels != NULL) {
		pixels = static_cast<Uint32*>(surfacePixels->pixels);
	}

	return pixels;
}

Uint32 Texture::getPitch32() {
	Uint32 pitch = 0;

	if (surfacePixels != NULL)
	{
		pitch = surfacePixels->pitch / 4;
	}

	return pitch;
}

Uint32 Texture::mapRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	Uint32 pixel = 0;

	if (surfacePixels != NULL)
	{
		pixel = SDL_MapRGBA(surfacePixels->format, r, g, b, a);
	}

	return pixel;
}

void Texture::free() {
	if (texture != NULL)
	{
		SDL_DestroyTexture(texture);
		texture = NULL;
		width = 0;
		height = 0;
	}

	if (surfacePixels != NULL) {
		SDL_FreeSurface(surfacePixels);
		surfacePixels = NULL;
	}
}

void Texture::setOrigin(int x, int y) {
	originX = x;
	originY = y;
}

void Texture::render() {
	SDL_Rect renderQuad = { originX, originY, width, height };
	SDL_RenderCopy(gRenderer, texture, NULL, &renderQuad);
}

int Texture::getWidth() {
	return width;
}

int Texture::getHeight() {
	return height;
}

int Texture::getOriginX() {
	return originX;
}

int Texture::getOriginY() {
	return originY;
}

void erase(int x, int y) {
	
	x -= testTexture.getOriginX();
	y -= testTexture.getOriginY();

	Uint32* pixels = testTexture.getPixels32();
	int pixelCount = testTexture.getPitch32() * testTexture.getHeight();

	Uint32 transparent = testTexture.mapRGBA(0xFF, 0xFF, 0xFF, 0x00);

	if (scale > 0) {
		for (int w = 0; w < scale * 2; w++)
		{
			for (int h = 0; h < scale * 2; h++)
			{
				int dx = scale - w; // horizontal offset
				int dy = scale - h; // vertical offset
				if ((dx * dx + dy * dy) <= (scale * scale)&&(x+dx<testTexture.getWidth()) && (x+dx > -1) && (y + dy < testTexture.getHeight()) && (y + dy > -1))
				{
					pixels[(y + dy) * testTexture.getWidth() + (x + dx)] = transparent;
				}
			}
		}
	}
	else {
		pixels[y * testTexture.getWidth() + x] = transparent;
	}

	testTexture.loadFromPixels();
}

bool isAtEdge(int pixelPosition, int arrayWidth, int arrayLength) {
	if (pixelPosition < arrayWidth || pixelPosition % arrayWidth == 0 ||
		pixelPosition % arrayWidth == arrayWidth - 1 || pixelPosition >= arrayLength - arrayWidth) {
		return true;
	}
	return false;
}

bool isAtCorner(int pixelPosition, int arrayWidth, int arrayLength) {
	if (pixelPosition == 0 || pixelPosition == arrayWidth - 1 ||
		pixelPosition == arrayLength - arrayWidth || pixelPosition == arrayLength - 1) {
		return true;
	}
	return false;
}

bool isAtTopEdge(int pixelPosition, int arrayWidth) {
	if (pixelPosition < arrayWidth) {
		return true;
	}
	return false;
}
bool isAtBottomEdge(int pixelPosition, int arrayWidth, int arrayLength) {
	if (pixelPosition >= arrayLength - arrayWidth) {
		return true;
	}
	return false;
}
bool isAtLeftEdge(int pixelPosition, int arrayWidth) {
	if (pixelPosition % arrayWidth == 0) {
		return true;
	}
	return false;
}
bool isAtRightEdge(int pixelPosition, int arrayWidth) {
	if (pixelPosition % arrayWidth == arrayWidth - 1) {
		return true;
	}
	return false;
}

int* getNeighbours(int pixelPosition, int arrayWidth) {
	int* neighbourArr = new int[8];
	int index = 0;
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if (j == 0 && i == 0) {
				continue;
			}
			else {
				neighbourArr[index] = pixelPosition + (i * arrayWidth) + j;
				index++;
			}
		}
	}
	return neighbourArr;
}

//I hate this method so much
bool findColoursOfNeighbours(int pixelPosition, int arrayWidth, int arrayLength, Uint32* bufferArray) {
	bool nextToBlank = false;
	Uint32 noPixelColour = testTexture.mapRGBA(0xFF, 0xFF, 0xFF, 0x00);
	//There is probably a more efficient way of doing this. Will find later
	if (isAtEdge(pixelPosition, arrayWidth, arrayLength)) {
		if (isAtCorner(pixelPosition, arrayWidth, arrayLength)) {
			if (isAtTopEdge(pixelPosition, arrayWidth) && isAtLeftEdge(pixelPosition, arrayWidth)) {
				for (int i = 0; i < 2; i++) {
					for (int j = 0; j < 2; j++) {
						if (bufferArray[pixelPosition + (i * arrayWidth) + j] == noPixelColour) {
							nextToBlank = true;
							break;
						}
					}
					if (nextToBlank) {
						break;
					}
				}
			}
			if (isAtTopEdge(pixelPosition, arrayWidth) && isAtRightEdge(pixelPosition, arrayWidth)) {
				for (int i = 0; i < 2; i++) {
					for (int j = -1; j < 1; j++) {
						if (bufferArray[pixelPosition + (i * arrayWidth) + j] == noPixelColour) {
							nextToBlank = true;
							break;
						}
					}
					if (nextToBlank) {
						break;
					}
				}
			}
			if (isAtBottomEdge(pixelPosition, arrayWidth, arrayLength) && isAtLeftEdge(pixelPosition, arrayWidth)) {
				for (int i = -1; i < 1; i++) {
					for (int j = 0; j < 2; j++) {
						if (bufferArray[pixelPosition + (i * arrayWidth) + j] == noPixelColour) {
							nextToBlank = true;
							break;
						}
					}
					if (nextToBlank) {
						break;
					}
				}
			}
			if (isAtBottomEdge(pixelPosition, arrayWidth, arrayLength) && isAtLeftEdge(pixelPosition, arrayWidth)) {
				for (int i = -1; i < 1; i++) {
					for (int j = 0; j < 2; j++) {
						if (bufferArray[pixelPosition + (i * arrayWidth) + j] == noPixelColour) {
							nextToBlank = true;
							break;
						}
					}
					if (nextToBlank) {
						break;
					}
				}
			}
		}
		else {
			//Need to create these boolean functions to determine which edge the pixel is at
			if (isAtTopEdge(pixelPosition, arrayWidth)) {
				for (int i = 0; i < 2; i++) {
					for (int j = -1; j < 2; j++) {
						if (bufferArray[pixelPosition + (i * arrayWidth) + j] == noPixelColour) {
							nextToBlank = true;
							break;
						}
					}
					if (nextToBlank) {
						break;
					}
				}
			}
			if (isAtRightEdge(pixelPosition, arrayWidth)) {
				for (int i = -1; i < 2; i++) {
					for (int j = -1; j < 1; j++) {
						if (bufferArray[pixelPosition + (i * arrayWidth) + j] == noPixelColour) {
							nextToBlank = true;
							break;
						}
					}
					if (nextToBlank) {
						break;
					}
				}
			}
			if (isAtBottomEdge(pixelPosition, arrayWidth, arrayLength)) {
				for (int i = -1; i < 1; i++) {
					for (int j = -1; j < 2; j++) {
						if (bufferArray[pixelPosition + (i * arrayWidth) + j] == noPixelColour) {
							nextToBlank = true;
							break;
						}
					}
					if (nextToBlank) {
						break;
					}
				}
			}
			if (isAtLeftEdge(pixelPosition, arrayWidth)) {
				for (int i = -1; i < 2; i++) {
					for (int j = 0; j < 2; j++) {
						if (bufferArray[pixelPosition + (i * arrayWidth) + j] == noPixelColour) {
							nextToBlank = true;
							break;
						}
					}
					if (nextToBlank) {
						break;
					}
				}
			}
		}
	}
	else {
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				//noPixelColour is just some pseudocode variable for the value of the pixels that are 
				//counted as erased. Change it to something else in the actual code
				//Also I don't know if this for loop actually works. Test with an int array in a seperate
				//program or something
				if (bufferArray[pixelPosition + (i * arrayWidth) + j] == noPixelColour) {
					nextToBlank = true;
					break;
				}
			}
			if (nextToBlank) {
				break;
			}
		}
	}
	return nextToBlank;
}

void contourFinder() {
	Uint32* pixels = testTexture.getPixels32();
	Uint32 noPixelColour = testTexture.mapRGBA(0xFF, 0xFF, 0xFF, 0x00);
	Uint32 contourColour = testTexture.mapRGBA(0xFF, 0x00, 0xFF, 0xFF);
	//Uint32 contourColour = testTexture.mapRGBA(255, 192, 203, 0x00);
	for (int i = 0; i < testTexture.getWidth() * testTexture.getHeight(); i++) {
		//printf("%i\n",i);
		if (pixels[i] == noPixelColour || findColoursOfNeighbours(i, testTexture.getWidth(), testTexture.getWidth() * testTexture.getHeight(), pixels) == false) {
			continue;
		}
		else {
			//printf("The contourFinderWorks\n");
			pixels[i] = contourColour;
		}
	}
	testTexture.loadFromPixels();
}

void directionTraveller(Uint32 direction, int arrayLength, Uint32* pixels, int tracker[], Uint32 noPixelColour) {
	int* neighbourArr = getNeighbours(direction, testTexture.getWidth());
	std::vector<Uint32> possiblePursuit;
	for (int j = 0; j < 8; j++) {
		if (pixels[neighbourArr[j]] != noPixelColour) {
			if (tracker[neighbourArr[j]] == 1 && findColoursOfNeighbours(neighbourArr[j], testTexture.getWidth(), arrayLength, pixels)) {
				possiblePursuit.push_back(neighbourArr[j]);
			}
		}
	}
	if (!possiblePursuit.empty() && !isAtEdge(direction, testTexture.getWidth(), arrayLength)) {
		direction = possiblePursuit[0];
	}
}

void splitTexture() {
	Uint32* pixels = testTexture.getPixels32();
	Uint32 noPixelColour = testTexture.mapRGBA(0xFF, 0xFF, 0xFF, 0x00);
	int arrayLength = testTexture.getWidth() * testTexture.getHeight();
	int* visitedTracker = new int[arrayLength];
	Uint32 directions[2];
	memset(visitedTracker, 0, arrayLength * sizeof(int));

	for (int i = 0; i < arrayLength; i++) {
		//printf("%i\n",i);
		if (visitedTracker[i] == 1 || pixels[i] == noPixelColour || findColoursOfNeighbours(i, testTexture.getWidth(), arrayLength, pixels) == false) {
			visitedTracker[i] = 1;
			continue;
		}
		else {
			if (visitedTracker[i] == 1) {
				printf("How have we already visited this place?");
			}
			visitedTracker[i] = 1;
			//printf("The contourFinderWorks\n");
			if (!isAtEdge(i, testTexture.getWidth(), arrayLength)) {
				//printf("We're not at an edge at %i\n", i);
				//pixels[i] = testTexture.mapRGBA(255, 165, 0, 1);
				int* neighbourArr = getNeighbours(i, testTexture.getWidth());
				std::vector<Uint32> possiblePursuits;
				bool isSurroundedByErased = true;
				for (int j = 0; j < 8; j++) {
					if (pixels[neighbourArr[j]] != noPixelColour) {
						isSurroundedByErased = false;
						if (visitedTracker[neighbourArr[j]] == 0 && findColoursOfNeighbours(neighbourArr[j], testTexture.getWidth(), arrayLength, pixels)) {
							possiblePursuits.push_back(neighbourArr[j]);
						}
					}
				}
				printf("Vector Size: %i\n", possiblePursuits.size());
				if (isSurroundedByErased) {
					printf("We have a single pixel islet\n");
				}
				if (possiblePursuits.size() > 1) {
					directions[0] = possiblePursuits[0];
					directions[1] = possiblePursuits[1];

					//This shit doesn't cause infinite loops anymore
					//But it still loops too much
					//Why?
					//That's now fixed. Now there's just an out of bounds error on a vector and idk why. 
					//It may be because I'm constanly erasing memory from directions. Fixed
					while (directions[0] == directions[1]) {
						for (Uint32& direction : directions) {
							if (direction != -1) {
								visitedTracker[direction] = 1;
								pixels[direction] = testTexture.mapRGBA(255, 165, 0, 1);//This line isn't working
								//printf("%i\n", direction);
								int* neighbourArr = getNeighbours(direction, testTexture.getWidth());
								//std::vector<Uint32> possiblePursuit;
								for (int j = 0; j < 8; j++) {
									//printf("%i\n", neighbourArr[i]);
									if (pixels[neighbourArr[j]] != noPixelColour) {
										if (visitedTracker[neighbourArr[j]] == 0 && findColoursOfNeighbours(neighbourArr[j], testTexture.getWidth(), arrayLength, pixels)) {
											//possiblePursuit.push_back(neighbourArr[j]);
											direction = neighbourArr[j];
										}
									}
								}
								if (isAtEdge(direction, testTexture.getWidth(), arrayLength)) {
									direction = -1;
								}
								delete[] neighbourArr;
							}
							//printf("Visited Tracker direction status: %i\n",visitedTracker[direction]);
							
							//printf("%i\n", direction);
						}
						if (directions[0] == directions[1]/*directions.empty()*/) {
							printf("We reached an edge\n");
							break;
						}
					}
					printf("Infinite loop broken!!!!!!\n");
				}
			}
			else {
				//printf("We have an edge case at %i\n", i);
				pixels[i] = testTexture.mapRGBA(0xFF, 0x00, 0xFF, 0xFF);
			}
		}
	}

	/*for (int i = 0; i < arrayLength; i++) {
		if (i % testTexture.getWidth() == testTexture.getWidth() - 1) {
			printf("%i\n", visitedTracker[i]);
		}
		else {
			printf("%i", visitedTracker[i]);
		}
	}*/

	testTexture.loadFromPixels();
	delete[] visitedTracker;
}

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

		if (!testTexture.loadFromPixels())
		{
			printf("Unable to load Foo' texture from surface!\n");
		}
	}

	return success;
}

void close()
{
	//Free loaded images
	testTexture.free();

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
						if (e.button.button == SDL_BUTTON_LEFT)
							//contourFinder();
							splitTexture();
							leftMouseButtonDown = false;
						if (e.button.button == SDL_BUTTON_RIGHT)
							rightMouseButtonDown = false;
						break;
					case SDL_MOUSEBUTTONDOWN:
						if (e.button.button == SDL_BUTTON_LEFT)
							leftMouseButtonDown = true;
						if (e.button.button == SDL_BUTTON_RIGHT) {
							rightMouseButtonDown = true;
							SDL_GetMouseState(&x, &y);
						}
					case SDL_MOUSEMOTION:
						if (leftMouseButtonDown &&  e.motion.x >= 0 && e.motion.x < 640 && e.motion.y < 480 && e.motion.y >= 0) {
							erase(e.motion.x, e.motion.y);
						}
						//Dragging functionality
						else if (rightMouseButtonDown && e.motion.x >= 0 && e.motion.x < 640 && e.motion.y < 480 && e.motion.y >= 0) {
							if (e.motion.x >= testTexture.getOriginX() && e.motion.x < testTexture.getOriginX() + testTexture.getWidth() && e.motion.y < testTexture.getOriginY() + testTexture.getHeight() && e.motion.y >= testTexture.getOriginY()) {
								int newX = testTexture.getOriginX() + e.motion.xrel;
								int newY = testTexture.getOriginY() + e.motion.yrel;
								testTexture.setOrigin(newX, newY);
							}
						}
						break;
					}
				}
				//This is where all the functionality in the main loop will go.
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				testTexture.render();

				SDL_RenderPresent(gRenderer);
			}
		}
	}
	close();
	return 0;
}