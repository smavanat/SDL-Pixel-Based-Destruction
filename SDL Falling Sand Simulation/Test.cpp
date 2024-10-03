//Online tutorial for referencing: https://gigi.nullneuron.net/gigilabs/sdl2-pixel-drawing/

//#include<SDL.h>
//#include <iostream>
//
//int main(int argc, char** argv) {
//	bool leftMouseButtonDown = false;
//	bool quit = false;
//	SDL_Event e;
//
//	SDL_Init(SDL_INIT_VIDEO);
//
//	SDL_Window* gWindow = SDL_CreateWindow("SDL2 Pixel Drawing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
//	SDL_Renderer* gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
//	SDL_Texture* texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 640, 480);
//	Uint32* pixels = new Uint32[640 * 480];
//	memset(pixels, 255, 640 * 480 * sizeof(Uint32));
//
//	while (!quit) {
//		SDL_UpdateTexture(texture, NULL, pixels, 640 * sizeof(Uint32));
//		SDL_WaitEvent(&e);
//		switch (e.type) {
//			case SDL_QUIT:
//				quit = true;
//				break;
//			case SDL_MOUSEBUTTONUP:
//				if (e.button.button == SDL_BUTTON_LEFT)
//					leftMouseButtonDown = false;
//				break;
//			case SDL_MOUSEBUTTONDOWN:
//				if (e.button.button == SDL_BUTTON_LEFT)
//					leftMouseButtonDown = true;
//			case SDL_MOUSEMOTION:
//				if (leftMouseButtonDown) {
//					int mouseX = e.motion.x;
//					int mouseY = e.motion.y;
//					pixels[mouseY * 640 * mouseX] = 0;
//				}
//				break;
//		}
//
//		SDL_RenderClear(gRenderer);
//		SDL_RenderCopy(gRenderer, texture, NULL, NULL);
//		SDL_RenderPresent(gRenderer);
//	}
//	SDL_DestroyWindow(gWindow);
//	SDL_Quit();
//
//	delete[] pixels;
//	SDL_DestroyTexture(texture);
//	SDL_DestroyRenderer(gRenderer);
//
//	return 0;
//}