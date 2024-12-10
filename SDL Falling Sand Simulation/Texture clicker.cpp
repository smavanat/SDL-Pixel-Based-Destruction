#include<SDL.h>
#include<stdio.h>
#include<SDL_image.h>
#include<iostream>
#include <vector>
#include <queue>

//TODO: Make the new images appear where they are supposed to (figure out relative origin shenanigans). Done
//		Make the program be able to figure out which texture needs to be erased from. Done
//		Work out why splitTextureAtEdge isn't working when I try to generalise it. 
//		Work out why the alpha isn't working 
//		Make textures be able to be moved properly without breaking the program.

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture class. This will contain our initial red square, that we will then modify and break up.
class Texture {
	public:
		Texture();

		Texture(int x, int y);

		Texture(int x, int y, int w, int h, Uint32* pixels);

		~Texture();

		bool loadFromFile(std::string path);

		bool loadPixelsFromFile(std::string path);

		bool loadFromPixels();

		bool isAltered();

		void free();

		void setOrigin(int x, int y);

		void render();

		void markAsAltered();

		void resetSplittingFlag();

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

		bool needsSplitting;
};

//Some global variables
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
Texture testTexture = Texture(240, 190);

//For holding our textures. Needs to be a pointer vector because otherwise will get error because the texture objects 
//are being copied over to a new location and the originals deleted.
//This means that the old pointers for surfacePixels etc. are being invalidated, hence the memory write errors.
std::vector<Texture*> textures;
std::vector<Texture*> toBeFreed;

int scale = 10;

bool init();
bool loadMedia();

Texture::Texture(){
	texture = NULL;
	oldSurface = NULL;
	width = 0;
	height = 0;
	needsSplitting = false;
}

//Use when loading from file
Texture::Texture(int x, int y) {
	texture = NULL;
	oldSurface = NULL;
	width = 0;
	height = 0;
	setOrigin(x, y);
	needsSplitting = false;
}

//Use when using pixel buffer.
Texture::Texture(int x, int y, int w, int h, Uint32* pixels) {
	texture = NULL;
	oldSurface = NULL;
	width = 0;
	height = 0;
	setOrigin(x, y);
	needsSplitting = false;
	surfacePixels = SDL_CreateRGBSurfaceFrom(pixels, w, h, 32, w*4, 0, 0, 0, 0);//pitch is the texture width * pixelsize in bytes
	surfacePixels = SDL_ConvertSurfaceFormat(surfacePixels, SDL_PIXELFORMAT_ARGB8888, 0);
	loadFromPixels(); //Otherwise the texture does not exist.
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
		surfacePixels = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_ARGB8888, 0);
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
		//SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
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

bool Texture::isAltered() {
	return needsSplitting;
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

void Texture::markAsAltered() {
	needsSplitting = true;
}

void Texture::resetSplittingFlag() {
	needsSplitting = false;
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

//Old erase function. Mostly here as a reference rather than actually used.
void erasePixels(int x, int y) {
	
	x -= testTexture.getOriginX();
	y -= testTexture.getOriginY();

	Uint32* pixels = testTexture.getPixels32();

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
	testTexture.markAsAltered();
}

void erasePixels(Texture* texture, int x, int y) {

	x -= texture->getOriginX();
	y -= texture->getOriginY();

	Uint32* pixels = texture->getPixels32();

	Uint32 transparent = texture->mapRGBA(0xFF, 0xFF, 0xFF, 0x00);

	if (scale > 0) {
		for (int w = 0; w < scale * 2; w++)
		{
			for (int h = 0; h < scale * 2; h++)
			{
				int dx = scale - w; // horizontal offset
				int dy = scale - h; // vertical offset
				if ((dx * dx + dy * dy) < (scale * scale) && (x + dx < texture->getWidth()) && (x + dx > -1) && (y + dy < texture->getHeight()) && (y + dy > -1))
				{
					pixels[(y + dy) * texture->getWidth() + (x + dx)] = transparent;
				}
			}
		}
	}
	else {
		pixels[y * texture->getWidth() + x] = transparent;
	}

	texture->loadFromPixels(); //This is the bit that is causing all the bugs. Why??
	texture->markAsAltered();
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

void contourFinder(Uint32* pixels, Uint32 noPixelColour, int*visitedTracker) {
	//Uint32 contourColour = testTexture.mapRGBA(0xFF, 0x00, 0xFF, 0xFF);
	for (int i = 0; i < testTexture.getWidth() * testTexture.getHeight(); i++) {
		if (pixels[i] == noPixelColour || !findColoursOfNeighbours(i, testTexture.getWidth(), testTexture.getWidth() * testTexture.getHeight(), pixels)) {
			continue;
		}
		else {
			//pixels[i] = contourColour;
			visitedTracker[i] = 2;
		}
	}
}

void cleanup(Uint32* pixels, Uint32 noPixelColour, std::vector<int> indexes) {
	for (int i = 0; i < indexes.size(); i++) {
		pixels[indexes[i]] = noPixelColour;
	}
}

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

void constructNewPixelBuffer(std::vector<int> indexes, int*visitedTracker, Uint32*pixels, Uint32 noPixelColour, int arrayWidth, Texture* texture) {
	Uint32* newPixelBuffer;
	int width = 0;
	//Need to do this +1 bs otherwise the program has a hissy fit for some reason. I substract it later.
	int height = (int)(indexes.back() / arrayWidth) - (int)(indexes.front() / arrayWidth)+1; 

	int startLinePos = indexes[0] % arrayWidth;
	int endLinePos = 0;
	
	for (int i = 0; i < indexes.size()-1; i++) {
		if (i != 0) {
			//THE SMALLEST startLinePos AND BIGGEST endLinePos DO NOT HAVE TO BE ON THE SAME ROW
			//if the pixel ahead of the current one is on the same row but the one behind is on a different row 
			//we have a startrow. But we only want to update the value if it's % is smaller than the current one
			//as this indicates it is further to the left.
			if ((floor(indexes[i + 1] / arrayWidth) == floor(indexes[i] / arrayWidth)) 
				&& (floor(indexes[i - 1] / arrayWidth) < floor(indexes[i] / arrayWidth))
				&& (startLinePos > indexes[i] % arrayWidth)) {
				startLinePos = indexes[i] % arrayWidth;
			}
			//If the pixel behind the current one is on the same row but the one ahead is on a new row, 
			//we have an endrow. But we only want to update the value if its % is bigger than the current one 
			//as this indicates it is further to the right.
			if ((floor(indexes[i - 1] / arrayWidth) == floor(indexes[i] / arrayWidth)) 
				&& (floor(indexes[i + 1] / arrayWidth) > floor(indexes[i] / arrayWidth))
				&& (endLinePos % arrayWidth < indexes[i] % arrayWidth)) {
				endLinePos = indexes[i] % arrayWidth;
			}
		}
	}
	//height = height / 2; //Because height is updated at the start of a row and at the end of a row.
	width = endLinePos - startLinePos;
	width = width + 1; //No way to test the width, so I pray this is the correct value. Otheriwise I'm fucked. I am indeed fucked. No longer
	height -= 1;

	int actualHeight = floor(indexes[indexes.size() - 1] / arrayWidth);

	printf("Actual Height:%i\n", actualHeight);
	printf("Height: %i\n", height);
	printf("Width: %i\n", width);
	printf("Size of pixel buffer: %i\n", indexes[indexes.size() - 1]);

	//Creating the pixel buffer for the new texture
	newPixelBuffer = new Uint32[width * height];
	memset(newPixelBuffer, noPixelColour, width * height * sizeof(Uint32));//Filling it with transparent pixels

	//Now need to figure out a way of getting the actual indecies to become relative indecies so I can stick them 
	//into this new array. This would be good if I had an origin point, but I don't. Perhaps a way of getting one would be
	//to get the smallest startLinePos by % with arrayWidth, and then have that be "0" on the x direction.
	//This is an incredibly retarded solution, and luckily, I'm already doing half of the work for this to get the 
	//max width, so less retarded than initially thought.
	//y does not need to be obtained in such a retarded fashion, because we have the width value to determine how long 
	//each row should be (assuming the vain hope that my method of obtaining the width was correct).
	//No way of testing this until we start working with multiple split textures.
	int origin = ((int)floor(indexes[0] / arrayWidth)) + (startLinePos % arrayWidth);

	//While loop with index for the height? Maybe.
	int currentHeight = 0;
	//while (currentHeight < height) {
		//Will this work? Because surely this inner for loop will run to completion before the next while check. 
		//But maybe it won't matter and the while loop isn't necessary.
		//I dont think it is. I've commented it out for now, may re-use later.
	for (int i = 0; i < indexes.size(); i++) {
		if (i != 0 && (floor(indexes[i] / arrayWidth) > floor(indexes[i - 1] / arrayWidth))) {
			currentHeight += floor(indexes[i] / arrayWidth) - floor(indexes[i - 1] / arrayWidth);
		}
		newPixelBuffer[(currentHeight * width) + ((indexes[i] % arrayWidth) - origin)] = pixels[indexes[i]];
	}
	//}

	//This works. Still need to fix the alpha issue. Used pointers otherwise this doesn't work.
	int originX = texture->getOriginX() + (startLinePos % arrayWidth);
	int originY = texture->getOriginY() + ((int)floor(indexes[0] / arrayWidth));

	//Need to get the origin of the new texture in the screen. But for now lets just set it to (0,0) so that we can 
	//do some testing. Set this as a pointer as otherwise this variable will be destroyed once this method finishes.
	Texture* newTexture = new Texture(originX, originY, width, height, newPixelBuffer);
	textures.push_back(newTexture);

	cleanup(pixels, noPixelColour, indexes);

	//delete[] newPixelBuffer;
}

//Fixed by adding a flag in the texture class to see if it is being erased. Now there is a new problem where textures
//are being removed from the vector for no reason. But that should hopefully be easier to fix.
//The bug is that splitTextureAtEdge is not working on any texture after the first has been split. This is what is causing
//the allocation error. The new textures are fucked for some reason.
void splitTextureAtEdge(Texture* texture) {
	if (!texture || !texture->isAltered()) return;

	//Get the texture pixels
	Uint32* pixels = texture->getPixels32();
	//This is the transparent pixel colour
	Uint32 noPixelColour = texture->mapRGBA(0xFF, 0xFF, 0xFF, 0x00);
	//printf("Expected Transparent Colour: %u\n", noPixelColour);
	//A placement int that gets the length of the pixel 1D array
	int arrayLength = texture->getWidth() * testTexture.getHeight();
	//A bitmap that remembers if we visited a pixel before or not.
	int* visitedTracker = new int[arrayLength];
	//Initialising visitedTracker to all 0.
	memset(visitedTracker, 0, arrayLength * sizeof(int));
	std::vector<int> possibleStarts;

	/*bool hasTransparentRegions = false;
	for (int i = 0; i < arrayLength; ++i) {
		if (pixels[i] == noPixelColour) {
			hasTransparentRegions = true;
			break;
		}
	}
	if (!hasTransparentRegions) return;*/

	printf("Texture Width: %i\n", texture->getWidth());
	printf("Texture Height: %i\n", texture->getHeight());

	//For loop to get all the split texture parts.
	for (int i = 0; i < texture->getWidth(); i++) {
		if (pixels[i] != noPixelColour) {
			possibleStarts = bfs(i, texture->getWidth(), arrayLength, pixels, noPixelColour, visitedTracker);
			if (!possibleStarts.empty()) {
				constructNewPixelBuffer(possibleStarts, visitedTracker, pixels, noPixelColour, texture->getWidth(), texture);
			}
		}
	}

	//toBeFreed.push_back(texture);

	printf("bfs size: %i\n", possibleStarts.size());
	printf("Texture width: %i\n", texture->getWidth());
	printf("Pixel Buffer Size:%i\n", arrayLength);

	//delete[] visitedTracker;
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
			textures.push_back(&testTexture);//This works fine now.

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

							for (Texture* t : textures) {
								if (t->isAltered()) {
									splitTextureAtEdge(t);
									t->resetSplittingFlag();
									texturesToRemove.push_back(t);
								}
							}
							for (Texture* t : texturesToRemove) {
								textures.erase(find(textures.begin(), textures.end(), t));
								t->free();
							}

							printf("Number of Textures: %i\n", textures.size());
							leftMouseButtonDown = false;
						}
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
							for (Texture* t : textures) {
								if (e.motion.x >= t->getOriginX() && e.motion.x < t->getOriginX() + t->getWidth()
									&& e.motion.y < t->getOriginY() + t->getHeight() && e.motion.y >= t->getOriginY()) {
									erasePixels(t, e.motion.x, e.motion.y);
								}
							}
						}
						//Dragging functionality
						//The textures disappear now?????
						else if (rightMouseButtonDown && e.motion.x >= 0 && e.motion.x < 640 && e.motion.y < 480 && e.motion.y >= 0) {
							for (Texture* t : textures) {
								if (e.motion.x >= t->getOriginX() && e.motion.x < t->getOriginX() + t->getWidth() 
									&& e.motion.y < t->getOriginY() + t->getHeight() && e.motion.y >= t->getOriginY()) {
									int newX = t->getOriginX() + e.motion.xrel;
									int newY = t->getOriginY() + e.motion.yrel;
									t->setOrigin(newX, newY);
								}
							}
							/*if (e.motion.x >= testTexture.getOriginX() && e.motion.x < testTexture.getOriginX() + testTexture.getWidth()
								&& e.motion.y < testTexture.getOriginY() + testTexture.getHeight() && e.motion.y >= testTexture.getOriginY()) {
								int newX = testTexture.getOriginX() + e.motion.xrel;
								int newY = testTexture.getOriginY() + e.motion.yrel;
								testTexture.setOrigin(newX, newY);
							}*/
						}
						break;
					}
				}
				//This is where all the functionality in the main loop will go.
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				for (Texture* t : textures) {
					t->render();
					//printf("OriginX: %i, OriginY; %i", t->getOriginX(), t->getOriginY());
				}

				SDL_RenderPresent(gRenderer);
			}
		}
	}
	close();
	return 0;
}