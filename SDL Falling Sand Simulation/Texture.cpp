#include "Texture.hpp"
#include<SDL.h>
#include<stdio.h>
#include<SDL_image.h>

Texture::Texture() {
	texture = NULL;
	width = 0;
	height = 0;
	needsSplitting = false;
}
	
//Use when loading from file
Texture::Texture(int x, int y) {
	texture = NULL;
	width = 0;
	height = 0;
	setOrigin(x, y);
	needsSplitting = false;
}
	
//Use when using pixel buffer.
Texture::Texture(int x, int y, int w, int h, Uint32* pixels, SDL_Renderer* gRenderer) {
	texture = NULL;
	width = 0;
	height = 0;
	setOrigin(x, y);
	needsSplitting = false;
	surfacePixels = SDL_CreateRGBSurfaceWithFormatFrom(pixels, w, h, 32, w * 4, SDL_PIXELFORMAT_ARGB8888);//pitch is the texture width * pixelsize in bytes
	SDL_SetSurfaceBlendMode(surfacePixels, SDL_BLENDMODE_BLEND);
	loadFromPixels(gRenderer); //Otherwise the texture does not exist.
}
	
Texture::~Texture() {
	free();
}
	
bool Texture::loadFromFile(std::string path, SDL_Renderer* gRenderer) {
	if (!loadPixelsFromFile(path)) {
		printf("Failed to load pixels for %s!\n", path.c_str());
	}
	else {
		if (!loadFromPixels(gRenderer))
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
	
bool Texture::loadFromPixels(SDL_Renderer* gRenderer) {
	if (surfacePixels == NULL) {
		printf("No pixels loaded!");
	}
	else {
		texture = SDL_CreateTextureFromSurface(gRenderer, surfacePixels);
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		if (texture == NULL)
		{
			printf("Unable to create texture from loaded pixels! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			width = surfacePixels->w;
			height = surfacePixels->h;
		}
	
	}
	return texture != NULL;
}
	
bool Texture::isAltered() {
	return needsSplitting;
}
	
bool Texture::clickedOnTransparent(int x, int y) {
	x -= getOriginX();
	y -= getOriginY();
	Uint8 red, green, blue, alpha;
	SDL_GetRGBA(getPixels32()[(y * getWidth()) + x], surfacePixels->format, &red, &green, &blue, &alpha);
	if (alpha == 0) return true;
	else return false;
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
	
void Texture::render(SDL_Renderer* gRenderer) {
	SDL_Rect renderQuad = { originX, originY, width, height };
	SDL_RenderCopy(gRenderer, texture, NULL, &renderQuad);
}

//When need a rotateable texture
void Texture::render(SDL_Renderer* gRenderer, SDL_Rect* clip, double angle, SDL_Point* centre, SDL_RendererFlip flip) {
	SDL_Rect renderQuad = { originX, originY, width, height };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx(gRenderer, texture, clip, &renderQuad, angle, centre, flip);
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
	
SDL_PixelFormat* Texture::getPixelFormat() {
	return surfacePixels->format;
}