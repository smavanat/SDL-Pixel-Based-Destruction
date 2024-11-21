#include<SDL.h>
#include<stdio.h>
#include<SDL_image.h>
#include<iostream>
#include <vector>
#include <queue>

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
				if ((dx * dx + dy * dy) < (scale * scale)&&(x+dx<testTexture.getWidth()) && (x+dx > -1) && (y + dy < testTexture.getHeight()) && (y + dy > -1))
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

int* getNeighbours(int pixelPosition, int arrayWidth, int arrayLength) {
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

	//If we are at an edge, set the direction associated with that edge to -1, as there can be no neighbour there
	if (isAtTopEdge(pixelPosition, arrayWidth)) {
		neighbourArr[0] = -1;
		neighbourArr[1] = -1;
		neighbourArr[2] = -1;
	}
	if (isAtRightEdge(pixelPosition, arrayWidth)) {
		neighbourArr[2] = -1;
		neighbourArr[4] = -1;
		neighbourArr[7] = -1;
	}
	if (isAtBottomEdge(pixelPosition, arrayWidth, arrayLength)) {
		neighbourArr[5] = -1;
		neighbourArr[6] = -1;
		neighbourArr[7] = -1;
	}
	if (isAtLeftEdge(pixelPosition, arrayWidth)) {
		neighbourArr[0] = -1;
		neighbourArr[3] = -1;
		neighbourArr[5] = -1;
	}
	return neighbourArr;
}
int* getCardinalNeighbours(int pixelPosition, int arrayWidth, int arrayLength) {
	int* neighbourArr = new int[4];
	//Sets the values for each of the directions in the order top, right, bottom, left
	neighbourArr[0] = pixelPosition - arrayWidth;
	neighbourArr[1] = pixelPosition + 1;
	neighbourArr[2] = pixelPosition + arrayWidth;
	neighbourArr[3] = pixelPosition - 1;
	//If we are at an edge, set the direction associated with that edge to -1, as there can be no neighbour there
	if (isAtTopEdge(pixelPosition, arrayWidth)) {
		neighbourArr[0] = -1;
	}
	if (isAtRightEdge(pixelPosition, arrayWidth)) {
		neighbourArr[1] = -1;
	}
	if (isAtBottomEdge(pixelPosition, arrayWidth, arrayLength)) {
		neighbourArr[2] = -1;
	}
	if (isAtLeftEdge(pixelPosition, arrayWidth)) {
		neighbourArr[3] = -1;
	}
	return neighbourArr;
}

//Does a search if next to any transparent pixels when at left edge
bool findColourOfNeighbourAtLeftEdge(int pixelPosition, int arrayWidth, int arrayLength, Uint32* bufferArray) {
	bool nextToBlank = false;
	Uint32 noPixelColour = testTexture.mapRGBA(0xFF, 0xFF, 0xFF, 0x00);
	if (isAtTopEdge(pixelPosition, arrayWidth)) {
		if (bufferArray[pixelPosition + 1] == noPixelColour || bufferArray[pixelPosition + arrayWidth] == noPixelColour) {
			nextToBlank = true;
		}
	}
	else if (isAtBottomEdge(pixelPosition, arrayWidth, arrayLength)) {
		if (bufferArray[pixelPosition + 1] == noPixelColour || bufferArray[pixelPosition - arrayWidth] == noPixelColour) {
			nextToBlank = true;
		}
	}
	else {
		if (bufferArray[pixelPosition + 1] == noPixelColour || bufferArray[pixelPosition - arrayWidth] == noPixelColour || bufferArray[pixelPosition + arrayWidth] == noPixelColour) {
			nextToBlank = true;
		}
	}
	return nextToBlank;
}

//Does a search if next to any transparent pixels when at left edge
bool findColourOfNeighbourAtRightEdge(int pixelPosition, int arrayWidth, int arrayLength, Uint32* bufferArray) {
	bool nextToBlank = false;
	Uint32 noPixelColour = testTexture.mapRGBA(0xFF, 0xFF, 0xFF, 0x00);
	if (isAtTopEdge(pixelPosition, arrayWidth)) {
		if (bufferArray[pixelPosition - 1] == noPixelColour || bufferArray[pixelPosition + arrayWidth] == noPixelColour) {
			nextToBlank = true;
		}
	}
	else if (isAtBottomEdge(pixelPosition, arrayWidth, arrayLength)) {
		if (bufferArray[pixelPosition - 1] == noPixelColour || bufferArray[pixelPosition - arrayWidth] == noPixelColour) {
			nextToBlank = true;
		}
	}
	else {
		if (bufferArray[pixelPosition - 1] == noPixelColour || bufferArray[pixelPosition - arrayWidth] == noPixelColour || bufferArray[pixelPosition + arrayWidth] == noPixelColour) {
			nextToBlank = true;
		}
	}
	return nextToBlank;
}

//Does a search if next to any transparent pixels when at Top edge
bool findColourOfNeighbourAtTopEdge(int pixelPosition, int arrayWidth, int arrayLength, Uint32* bufferArray) {
	bool nextToBlank = false;
	Uint32 noPixelColour = testTexture.mapRGBA(0xFF, 0xFF, 0xFF, 0x00);
	if (isAtLeftEdge(pixelPosition, arrayWidth)) {
		if (bufferArray[pixelPosition + 1] == noPixelColour || bufferArray[pixelPosition + arrayWidth] == noPixelColour) {
			nextToBlank = true;
		}
	}
	else if (isAtRightEdge(pixelPosition, arrayWidth)) {
		if (bufferArray[pixelPosition - 1] == noPixelColour || bufferArray[pixelPosition + arrayWidth] == noPixelColour) {
			nextToBlank = true;
		}
	}
	else {
		if (bufferArray[pixelPosition + 1] == noPixelColour || bufferArray[pixelPosition - 1] == noPixelColour || bufferArray[pixelPosition + arrayWidth] == noPixelColour) {
			nextToBlank = true;
		}
	}
	return nextToBlank;
}

//Does a search if next to any transparent pixels when at Bottom edge
bool findColourOfNeighbourAtBottomEdge(int pixelPosition, int arrayWidth, int arrayLength, Uint32* bufferArray) {
	bool nextToBlank = false;
	Uint32 noPixelColour = testTexture.mapRGBA(0xFF, 0xFF, 0xFF, 0x00);
	if (isAtLeftEdge(pixelPosition, arrayWidth)) {
		if (bufferArray[pixelPosition + 1] == noPixelColour || bufferArray[pixelPosition - arrayWidth] == noPixelColour) {
			nextToBlank = true;
		}
	}
	else if (isAtRightEdge(pixelPosition, arrayWidth)) {
		if (bufferArray[pixelPosition - 1] == noPixelColour || bufferArray[pixelPosition - arrayWidth] == noPixelColour) {
			nextToBlank = true;
		}
	}
	else {
		if (bufferArray[pixelPosition + 1] == noPixelColour || bufferArray[pixelPosition - 1] == noPixelColour || bufferArray[pixelPosition - arrayWidth] == noPixelColour) {
			nextToBlank = true;
		}
	}
	return nextToBlank;
}

bool findColoursOfNeighbours(int pixelPosition, int arrayWidth, int arrayLength, Uint32* bufferArray) {
	bool nextToBlank = false;
	Uint32 noPixelColour = testTexture.mapRGBA(0xFF, 0xFF, 0xFF, 0x00);
	int* neighbourArr = getNeighbours(pixelPosition, arrayWidth, arrayLength);
	for (int i = 0; i < 8; i++) {
		if (neighbourArr[i] != -1 && bufferArray[neighbourArr[i]] == noPixelColour) {
			nextToBlank = true;
		}
	}
	return nextToBlank;
}

//Returns if next to blank pixel. Only for those pixels that are not an edge.
bool findColoursOfNotEdgeNeighbours(int pixelPosition, int arrayWidth, int arrayLength, Uint32* bufferArray) {
	bool nextToBlank = false;
	//Transparent pixel colour.
	Uint32 noPixelColour = testTexture.mapRGBA(0xFF, 0xFF, 0xFF, 0x00);

	for (int i = -1; i < 2; i++) {
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
	return nextToBlank;
}

//Highlights the countours of erased pixels.
//Do this, and then use flood-filled algorithm to make new textures? and erase them from the old one 
//each time, and then eventually delete the initial texture.
//Could possibly work. Each new texture would initially be the same size as the old texture,
//And then we would find the highest (by counting how many pairs of contour vertices there are) and 
//widest point (by counting the max difference between two contour vertices).
//Create an array of hieght*width, and copy everything into that, filling in the blanks with whitespace.
//I think the initial copy would just be the index, and once we had sorted everything out, then we would copy
//over the actual colour data. That would allow us to effectively account for the relative positions of the 
//pixels to each other.
void contourFinder(Uint32* pixels, Uint32 noPixelColour, int*visitedTracker) {
	//Uint32 contourColour = testTexture.mapRGBA(0xFF, 0x00, 0xFF, 0xFF);
	for (int i = 0; i < testTexture.getWidth() * testTexture.getHeight(); i++) {
		if (pixels[i] == noPixelColour || !findColoursOfNeighbours(i, testTexture.getWidth(), testTexture.getWidth() * testTexture.getHeight(), pixels)) {
			continue;
		}
		else {
			//printf("The contourFinderWorks\n");
			//pixels[i] = contourColour;
			visitedTracker[i] = 2;
		}
	}
}


//void directionTraveller(int index, int arrayLength, Uint32* pixels, int* visitedArray) {
//	//int arrayLength = testTexture.getWidth() * testTexture.getHeight();
//	//This is the transparent pixel colour
//	Uint32 noPixelColour = testTexture.mapRGBA(0xFF, 0xFF, 0xFF, 0x00);
//	//Colour for outlines. A debugging tool
//	Uint32 contourColour = testTexture.mapRGBA(0xFF, 0x00, 0xFF, 0xFF);
//	int currentIndex = index;
//
//	do {
//		//Essentially, I think the neighbour we want to get is the one which is returned in getNeighbours, 
//		//but whose getCardinalNeighbours are not all coloured.
//		//functions both put them with blank neighbours.
//		//In other words, use getNeighbours to find all of the possible neighbours of the current one,
//		//but use getCardinalNeighbours in findColoursOfNeighbours to get the next one.
//		//Uhh no? Apparently the reverse works? Why?????
//		printf("%i\n", currentIndex);
//		pixels[currentIndex] = contourColour;
//		visitedArray[currentIndex] = 1;
//		int oldIndex = currentIndex;
//		int* neighbourArr = getCardinalNeighbours(currentIndex, testTexture.getWidth(), arrayLength);
//		for (int i = 0; i < 4; i++) {
//			if (neighbourArr[i] == -1 || pixels[neighbourArr[i]] == noPixelColour || visitedArray[neighbourArr[i]] == 1) {
//				continue;
//			}
//			else {
//				int* temp = getNeighbours(neighbourArr[i], testTexture.getWidth(), arrayLength);
//				for (int j = 0; j < 8; j++) {
//					if (pixels[temp[j]] == noPixelColour) {
//						currentIndex = neighbourArr[i];
//						break;
//					}
//				}
//				if (currentIndex != oldIndex) {
//					break;
//				}
//			}
//		}
//	} while (visitedArray[currentIndex] != 1);
//
//	//Strange bug. This only works for one of the two sides of the erased part of the texture.
//	do {
//		visitedArray[currentIndex] = 1;
//		pixels[currentIndex] = contourColour;
//		int* temp = getCardinalNeighbours(currentIndex, testTexture.getWidth(), arrayLength);
//		
//		printf("Doing something\n");
//		for (int i = 0; i < 4; i++) {
//			printf("temp[%i] is %i\n", i, temp[i]);
//			if (temp[i] == -1) {
//				continue;
//			}
//			else if (isAtEdge(temp[i], testTexture.getWidth(), arrayLength) && pixels[temp[i]] != noPixelColour) {
//				printf("We found one\n");
//				currentIndex = temp[i];
//			}
//		}
//	} while (visitedArray[currentIndex] != 1);
//}

//Crappy augmented flood-fill algorithm implementation taken from here: https://www.geeksforgeeks.org/flood-fill-algorithm/
std::vector<int> bfs(int index, int arrayWidth, int arrayLength, Uint32* pixels, Uint32 noPixelColour, int* visitedTracker) {
	std::vector<int> indexes;
	std::queue<int> q;

	indexes.push_back(index);
	q.push(index);
	//Need to use the visited tracker otherwise the program doesn't know if we have visited a pixel or not
	//so it keeps looping infinitely
	visitedTracker[index] = 1;

	while (!q.empty()) {
		int currentIndex = q.front();
		q.pop();
		if (!isAtTopEdge(currentIndex, arrayWidth) && pixels[currentIndex - arrayWidth] != noPixelColour && visitedTracker[currentIndex - arrayWidth] == 0) {
			indexes.push_back(currentIndex - arrayWidth);
			q.push(currentIndex - arrayWidth);
			visitedTracker[currentIndex - arrayWidth] = 1;
		}
		if (!isAtLeftEdge(currentIndex, arrayWidth) && pixels[currentIndex - 1] != noPixelColour && visitedTracker[currentIndex - 1] == 0) {
			indexes.push_back(currentIndex - 1);
			q.push(currentIndex - 1);
			visitedTracker[currentIndex - 1] = 1;
		}
		if (!isAtBottomEdge(currentIndex, arrayWidth, arrayLength) && pixels[currentIndex + arrayWidth] != noPixelColour && visitedTracker[currentIndex + arrayWidth] == 0) {
			indexes.push_back(currentIndex + arrayWidth);
			q.push(currentIndex + arrayWidth);
			visitedTracker[currentIndex + arrayWidth] = 1;
		}
		if (!isAtRightEdge(currentIndex, arrayWidth) && pixels[currentIndex + 1] != noPixelColour && visitedTracker[currentIndex + 1] == 0) {
			indexes.push_back(currentIndex + 1);
			q.push(currentIndex + 1);
			visitedTracker[currentIndex + 1] = 1;
		}
	}

	std::sort(indexes.begin(), indexes.end());

	return indexes;
}

Uint32* constructNewPixelBuffer(std::vector<int> indexes, int*visitedTracker, Uint32 noPixelColour, int arrayWidth) {
	Uint32* newPixelBuffer;
	int width = 0;
	int height = 1;

	int startLinePos = indexes[0];
	int endLinePos = 0;
	
	for (int i = 0; i < indexes.size()-1; i++) {
		if (i != 0) {
			if (floor(indexes[i+1]/arrayWidth) != floor(indexes[i-1]/arrayWidth)) {
				height += floor(indexes[i + 1] / arrayWidth) - floor(indexes[i - 1] / arrayWidth);
			}
			//if the pixel ahead of the current one is on the same row but the one behind is on a different row we have a startrow
			if ((floor(indexes[i + 1] / arrayWidth) == floor(indexes[i] / arrayWidth)) && (floor(indexes[i - 1] / arrayWidth) < floor(indexes[i] / arrayWidth))) {
				startLinePos = indexes[i];
			}
			//if the pixel behind the current one is on the same row but the one ahead is on a new row, we have an endrow
			if ((floor(indexes[i - 1] / arrayWidth) == floor(indexes[i] / arrayWidth)) && (floor(indexes[i + 1] / arrayWidth) > floor(indexes[i] / arrayWidth))) {
				endLinePos = indexes[i];
				//Calculate width only once we have a new endrow.
				if (width < endLinePos - startLinePos) {
					width = endLinePos - startLinePos;
				}
			}
		}
	}
	height = height / 2; //Because height is updated at the start of a row and at the end of a row.
	width = width + 1; //No way to test the width, so I pray this is the correct value. Otheriwise I'm fucked.
	//Rough fix. Think this should work because it wasn't getting height properly if it was on the bottom row.
	if (isAtBottomEdge(indexes[indexes.size() - 1], arrayWidth, testTexture.getHeight() * testTexture.getWidth())) {
		height +=1;
	}

	int actualHeight = floor(indexes[indexes.size() - 1] / arrayWidth);

	printf("Actual Height:%i\n", actualHeight);
	printf("Height: %i\n", height);
	printf("Width: %i\n", width);
	printf("%i\n", indexes[indexes.size() - 1]);
	newPixelBuffer = new Uint32[width * height];
	memset(newPixelBuffer, noPixelColour, width * height * sizeof(Uint32));

	return newPixelBuffer;
}

void splitTextureAtEdge() {
	//Get the texture pixels
	Uint32* pixels = testTexture.getPixels32();
	//This is the transparent pixel colour
	Uint32 noPixelColour = testTexture.mapRGBA(0xFF, 0xFF, 0xFF, 0x00);
	//Colour for outlines. A debugging tool
	Uint32 contourColour = testTexture.mapRGBA(0xFF, 0x00, 0xFF, 0xFF);
	//A placement int that gets the length of the pixel 1D array
	int arrayLength = testTexture.getWidth() * testTexture.getHeight();
	//A bitmap that remembers if we visited a pixel before or not.
	int* visitedTracker = new int[arrayLength];
	//An array that stores the two directions that you move in.
	Uint32 directions[2];
	//Initialising visitedTracker to all 0.
	memset(visitedTracker, 0, arrayLength * sizeof(int));
	std::vector<int> possibleStarts;
	
	//contourFinder(pixels, noPixelColour, visitedTracker);

	//Top edge
	for (int i = 0; i < testTexture.getWidth(); i++) {
		if (pixels[i] != noPixelColour) {
			possibleStarts = bfs(i, testTexture.getWidth(), arrayLength, pixels, noPixelColour, visitedTracker);
			break;
		}
	}
	printf("bfs size: %i\n", possibleStarts.size());
	/*for (int i = 0; i < possibleStarts.size(); i++) {
		printf("%i\n", possibleStarts[i]);
	}*/
	constructNewPixelBuffer(possibleStarts, visitedTracker, noPixelColour, testTexture.getWidth());
	printf("Texture width: %i\n", testTexture.getWidth());
	printf("Pixel Buffer Size:%i\n", arrayLength);
	//printf("Edge count: %i\n", possibleStarts.size());
	testTexture.loadFromPixels();
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
							//splitTexture();
							splitTextureAtEdge();
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