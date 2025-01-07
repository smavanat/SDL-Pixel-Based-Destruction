#pragma once
#include<SDL.h>
#include<stdio.h>
#include<SDL_image.h>
#include<iostream>
//Texture class. This contains functionality for creating textures that are loaded from files or from dynamically
//created pixel buffers. It has no control over manipulating its own pixels. This is handled externally.
class Texture {
	public:
		Texture();

		Texture(int x, int y);

		Texture(int x, int y, int w, int h, Uint32* pixels, SDL_Renderer* gRenderer);

		~Texture();

		bool loadFromFile(std::string path, SDL_Renderer* gRenderer);

		bool loadPixelsFromFile(std::string path);

		bool loadFromPixels(SDL_Renderer* gRenderer);

		bool isAltered();

		bool clickedOnTransparent(int x, int y);

		void free();

		void setOrigin(int x, int y);

		void render(SDL_Renderer* gRenderer);

		void markAsAltered();

		void resetSplittingFlag();

		int getWidth();
		int getHeight();
		int getOriginX();
		int getOriginY();

		Uint32* getPixels32();
		Uint32 getPitch32();
		Uint32 mapRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
		SDL_PixelFormat* getPixelFormat();

	private:
		SDL_Texture* texture;

		SDL_Surface* surfacePixels;

		int width;
		int height;
		int originX;
		int originY;

		bool needsSplitting;
};

