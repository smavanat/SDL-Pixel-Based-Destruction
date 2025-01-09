#include<SDL.h>
#include<stdio.h>
#include<SDL_image.h>
#include<iostream>
#include<vector>
#include<queue>
#include<algorithm>
#include "Texture.hpp"
#include "Outline.hpp"

#pragma region splitTexture
void erasePixels(Texture* texture, SDL_Renderer* gRenderer, int scale, int x, int y) {

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

	texture->loadFromPixels(gRenderer); //This is the bit that is causing all the bugs. Why??
	texture->markAsAltered();
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

//bool findColoursOfNeighbours(int pixelPosition, int arrayWidth, int arrayLength, Uint32* bufferArray, Texture* texture) {
//	bool nextToBlank = false;
//	Uint32 noPixelColour = texture->mapRGBA(0xFF, 0xFF, 0xFF, 0x00);
//	int* neighbourArr = getNeighbours(pixelPosition, arrayWidth, arrayLength);
//	for (int i = 0; i < 8; i++) {
//		if (neighbourArr[i] != -1 && bufferArray[neighbourArr[i]] == noPixelColour) {
//			nextToBlank = true;
//		}
//	}
//	return nextToBlank;
//}

////Gets the outline of a texture;
//std::vector<int> contourFinder(Uint32* pixels, Uint32 noPixelColour) {
//	//Uint32 contourColour = testTexture.mapRGBA(0xFF, 0x00, 0xFF, 0xFF);
//	std::vector<int> outlinePixels;
//	for (int i = 0; i < testTexture.getWidth() * testTexture.getHeight(); i++) {
//		if (pixels[i] == noPixelColour || !findColoursOfNeighbours(i, testTexture.getWidth(), testTexture.getWidth() * testTexture.getHeight(), pixels)) {
//			continue;
//		}
//		else {
//			//pixels[i] = contourColour;
//			outlinePixels.push_back(i);
//		}
//	}
//	return outlinePixels;
//}

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

Texture* constructNewPixelBuffer(std::vector<int> indexes, int* visitedTracker, Uint32* pixels, Uint32 noPixelColour, int arrayWidth, Texture* texture, SDL_Renderer* gRenderer) {
	Uint32* newPixelBuffer;
	Texture* newTexture;
	int width = 0;
	int height = (int)(indexes.back() / arrayWidth) - (int)(indexes.front() / arrayWidth) + 1;

	int startLinePos = indexes[0] % arrayWidth;
	int endLinePos = indexes[0] % arrayWidth;

	for (int i = 1; i < indexes.size() - 1; i++) {
		//THE SMALLEST startLinePos AND BIGGEST endLinePos DO NOT HAVE TO BE ON THE SAME ROW
		//if the pixel ahead of the current one is on the same row but the one behind is on a different row 
		//we have a startrow. But we only want to update the value if it's % is smaller than the current one
		//as this indicates it is further to the left.
		if (startLinePos > indexes[i] % arrayWidth) {
			startLinePos = indexes[i] % arrayWidth;
		}
		//If the pixel behind the current one is on the same row but the one ahead is on a new row, 
		//we have an endrow. But we only want to update the value if its % is bigger than the current one 
		//as this indicates it is further to the right.
		if (endLinePos < indexes[i] % arrayWidth) {
			endLinePos = indexes[i] % arrayWidth;
		}
	}
	width = endLinePos - startLinePos;
	width = width + 1; //No way to test the width, so I pray this is the correct value. Otheriwise I'm fucked. I am indeed fucked. No longer

	int actualHeight = floor(indexes[indexes.size() - 1] / arrayWidth) + 1;

	printf("Height: %i\n", height);
	printf("Width: %i\n", width);
	//Essentially, in order for marching squares to work, there has to be a one-pixel wide colourless perimeter around
	//the texture. This is so that there is an actual "border" for marching squares to trace around, and I think this is 
	//the least code and computationally expenseive method of implementing this, as the other way would be to have
	//marching squares "imagine" such a border, which would require a lot of checking of assumptions and pixel positions
	//ie, more code and work.
	//To implement this, we need to increase height and width by 2 (1 pixel for each side). We can increase height 
	//temporarily when we call it, but we need to increase width permanently, as otherwise it gets messed up in 
	//multiplication calls.
	width += 2;
	//Creating the pixel buffer for the new texture
	newPixelBuffer = new Uint32[(width) * (height + 2)];
	//The memset here is actually making all the pixels have an alpha of 255 for some reason, even though noPixelColour has an alpha of 0.
	//memset(newPixelBuffer, noPixelColour, width * height * sizeof(Uint32));//Filling it with transparent pixels
	//Using a for loop instead of memset fixes the alpha problem here.
	for (int i = 0; i < (width) * (height + 2); i++) {
		newPixelBuffer[i] = noPixelColour;
	}

	//Now need to figure out a way of getting the actual indecies to become relative indecies so I can stick them 
	//into this new array. This would be good if I had an origin point, but I don't. Perhaps a way of getting one would be
	//to get the smallest startLinePos by % with arrayWidth, and then have that be "0" on the x direction.
	//This is an incredibly retarded solution, and luckily, I'm already doing half of the work for this to get the 
	//max width, so less retarded than initially thought.
	//y does not need to be obtained in such a retarded fashion, because we have the width value to determine how long 
	//each row should be (assuming the vain hope that my method of obtaining the width was correct).
	//No way of testing this until we start working with multiple split textures.
	//There is actually no need for an origin position, we just get the offset by using startLinePos, because an offset
	//is only actually needed when slicing vertically, not horizontally. Using an origin messes everything up.

	int currentHeight = 1; //Since the first row will be the blank pixel perimeter.
	for (int i = 0; i < indexes.size(); i++) {
		if (i != 0 && (floor(indexes[i] / arrayWidth) > floor(indexes[i - 1] / arrayWidth))) {
			currentHeight += floor(indexes[i] / arrayWidth) - floor(indexes[i - 1] / arrayWidth);
		}
		//Add 1 here as an offset to the LHS perimeter. The RHS and BHS perimeters will be automatically accounted for
		//as the code will never reach them, so no need to worry about that.
		newPixelBuffer[(currentHeight * (width)) + ((indexes[i] % arrayWidth) - startLinePos) + 1] = pixels[indexes[i]];
	}

	//Edge case of 1x1 textures. Need to do newPixelBuffer[5], since the fifth pixel would be the middle one in a 3x3 grid.
	if (indexes.size() == 1) {
		newPixelBuffer[5] = pixels[indexes[0]];
	}

	//This works. Still need to fix the alpha issue. Used pointers otherwise this doesn't work.
	int originX = texture->getOriginX() + (startLinePos)-1; //-1 for transparent pixel border, so that the texture is not offset by one because of the invisible perimeter.
	int originY = texture->getOriginY() + ((int)floor(indexes[0] / arrayWidth)) - 1;

	//Set this as a pointer as otherwise this variable will be destroyed once this method finishes.
	newTexture = new Texture(originX, originY, width, height, newPixelBuffer, gRenderer);

	cleanup(pixels, noPixelColour, indexes);
	return newTexture;

	//delete[] newPixelBuffer; //This needs to be commented out since we are actively using newPixelBuffer to create our texture.
}

std::vector<Texture*> splitTextureAtEdge(Texture* texture, SDL_Renderer* gRenderer) {
	if (!texture || !texture->isAltered()) return {};

	//Get the texture pixels
	Uint32* pixels = texture->getPixels32(); //This has the correct alpha values for the pixels (checked)
	//This is the transparent pixel colour
	Uint32 noPixelColour = texture->mapRGBA(0xFF, 0xFF, 0xFF, 0x00);
	//A placement int that gets the length of the pixel 1D array
	int arrayLength = texture->getWidth() * texture->getHeight();
	//A bitmap that remembers if we visited a pixel before or not.
	int* visitedTracker = new int[arrayLength];
	//Initialising visitedTracker to all 0.
	memset(visitedTracker, 0, arrayLength * sizeof(int));
	//Pixel buffer vector
	std::vector<int> possibleStarts;
	//Vector for all the new textures that are being formed. This method will return them
	std::vector<Texture*> newTextures;

	printf("Texture Width: %i\n", texture->getWidth());
	printf("Texture Height: %i\n", texture->getHeight());

	//For loop to get all the split texture parts.
	for (int i = 0; i < arrayLength; i++) {
		if (pixels[i] != noPixelColour) {
			possibleStarts = bfs(i, texture->getWidth(), arrayLength, pixels, noPixelColour, visitedTracker);
			printf("bfs size: %i\n", possibleStarts.size());
			if (!possibleStarts.empty()) {
				newTextures.push_back(constructNewPixelBuffer(possibleStarts, visitedTracker, pixels, noPixelColour, texture->getWidth(), texture, gRenderer));
			}
		}
	}

	delete[] visitedTracker;
	return newTextures;
}
#pragma endregion

#pragma region marchingSquares 
	//Marching squares: First we need to get the starting pixel. This is just done by iterating over the array until 
	//					we find a non-transparent pixel
	//					Then we need to find the square value of it and the four pixels surrounding it
	//					Then based on that square value (and in the special saddle cases also on the previous square value)
	//					we choose a new direction to move the "analysis" and add the currently analysed pixel to a vector;
	//					Good source code and ideas from here: https://emanueleferonato.com/2013/03/01/using-marching-squares-algorithm-to-trace-the-contour-of-an-image/
	//					And here: https://barradeau.com/blog/?p=391

	int getStartingPixel(Uint32* pixels, Uint32 noPixelColour, int arrayLength) {
		for (int i = 0; i < arrayLength; i++) {
			if (pixels[i] != noPixelColour) {
				return i;
			}
		}
		return -1;
	}

	//ChatGPT version: (This is actually far less retarded than my original code and actually quite nice. Doesn't stop the fact that the code overall still doesn't work though)
	int getCurrentSquare(int startIndex, int textureWidth, int textureLength, const uint32_t* pixels, uint32_t noPixelColour) {
		int result = 0;

		// Calculate row and column of startIndex
		int row = startIndex / textureWidth;
		int col = startIndex % textureWidth;

		// Top-left pixel
		if (pixels[startIndex] != noPixelColour) result += 1;

		// Top-right pixel
		if (pixels[startIndex + 1] != noPixelColour) result += 2;

		// Bottom-left pixel
		if (pixels[startIndex + textureWidth] != noPixelColour) result += 4;

		// Bottom-right pixel
		if (pixels[startIndex + textureWidth + 1] != noPixelColour) result += 8;

		return result;
	}

	//ChatGPT version. Idk looks odd.
	//Maybe the issue is that we shouldn't be considering the 15th case at all. so if we're at an edge, even the left or top edge,
	//we actually pretend like there is an invisible, one pixel long perimeter around the texture, so that we never form the 
	//15th case. So then at the top edge we would get 12, unless at the TRH corner, where we would get 4, or at the TLH corner,
	//where we would get 8. And then for the left hand side we would get 10, unless in the BLH corner, where we would get 2
	//(TLH corner is discussed in previous sentence). And then we keep Bottom and Right as is since we already pretend they have
	//a perimeter anyway. Maybe this fixes???????? But would case fucked out of bounds errors. ORRR. We don't imagine the pixel
	//perimeter, we actually add it in for real. That way we don't have to pretend. But that could be aids to do. 
	std::vector<int> marchingSquares(Texture* texture) {
		uint32_t* pixels = texture->getPixels32();
		int width = texture->getWidth();
		int length = texture->getHeight() * width;
		int totalPixels = width * length;
		uint32_t noPixelColour = texture->mapRGBA(0xFF, 0xFF, 0xFF, 0x00);

		std::vector<int> contourPoints;
		int startPoint = getStartingPixel(pixels, noPixelColour, totalPixels);
		printf("Starting Point: %d\n", startPoint);
		if (startPoint == -1) return contourPoints;
		//If the texture is filled on the LHS, we will end up with 15 as our first currentSquare. 
		//To avoid this, we simply offset startPoint one to the left, to get 12 as our currentSquare, 
		//and then marching squares handles the rest.
		if (getCurrentSquare(startPoint, width, length, pixels, noPixelColour) == 15) {
			startPoint -= 1;
		}

		printf("Starting Point: %d\n", startPoint);
		printf("Width: %i\n", texture->getWidth());

		int stepX = 0, stepY = 0;
		int prevX = 0, prevY = 0;
		int currentPoint = startPoint;
		bool closedLoop = false;

		while (!closedLoop) {
			int currentSquare = getCurrentSquare(currentPoint, width, length, pixels, noPixelColour);
			//printf("Current Square: %d\n", currentSquare);

			// Movement lookup based on currentSquare value
			switch (currentSquare) {
			case 1: case 13:
				stepX = 0; stepY = -1;
				break;
			case 8: case 10: case 11:
				stepX = 0; stepY = 1;
				break;
			case 4: case 12: case 14:
				stepX = -1; stepY = 0;
				break;
			case 2: case 3: case 7:
				stepX = 1; stepY = 0;
				break;
			case 5:
				stepX = 0; stepY = -1;
				break;
			case 6:
				stepX = (prevY == -1) ? -1 : 1;
				stepY = 0;
				break;
			case 9:
				stepX = 0;
				stepY = (prevX == 1) ? -1 : 1;
				break;
			default:
				printf("Unhandled or empty square encountered at index: %d\n", currentPoint);
				return contourPoints;
			}

			currentPoint += stepY * width + stepX;
			//printf("Current Point: %i\n", currentPoint);

			// Boundary checks. Should not happen but here just in case.
			if (currentPoint < 0 || currentPoint >= totalPixels) {
				printf("Out-of-bounds detected at index: %d\n", currentPoint);
				return contourPoints;
			}

			contourPoints.push_back(currentPoint);
			prevX = stepX;
			prevY = stepY;

			if (currentPoint == startPoint) closedLoop = true;
		}

		return contourPoints;
	}
#pragma endregion

#pragma region rdp
	//Code source: https://editor.p5js.org/codingtrain/sketches/SQjSugKn6
	int* convertIndexToCoords(int index, int arrayWidth) {
		return new int[2] { index% arrayWidth, (int)floor(index / arrayWidth) };
	}

	float lineDist(int point, int startPoint, int endPoint, int arrayWidth) {
		int* pointCoords = convertIndexToCoords(point, arrayWidth);
		int* startPointCoords = convertIndexToCoords(startPoint, arrayWidth);
		int* endPointCoords = convertIndexToCoords(endPoint, arrayWidth);

		//The source for this very cursed single line of code can be found here : https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
		float distance = abs(((endPointCoords[1] - startPointCoords[1]) * pointCoords[0]) -
			((endPointCoords[0] - startPointCoords[0]) * pointCoords[1]) +
			(endPointCoords[0] * startPointCoords[1]) - (endPointCoords[1] * startPointCoords[0])) /
			sqrt(pow((endPointCoords[1] - startPointCoords[1]), 2) + pow((endPointCoords[0] - startPointCoords[0]), 2));
		return distance;
	}

	int findFurthest(std::vector<int> allPoints, int a, int b, int epsilon, int arrayWidth) {
		float recordDistance = -1;
		int furthestIndex = -1;
		int start = allPoints[a];
		int end = allPoints[b];
		for (int i = a + 1; i < b; i++) {
			float d = lineDist(allPoints[i], start, end, arrayWidth);
			if (d > recordDistance) {
				recordDistance = d;
				furthestIndex = i;
			}
		}
		if (recordDistance > epsilon) return furthestIndex;
		else return -1;
	}

	//This method would be used for lines that do not join up
	void rdp(int startIndex, int endIndex, int epsilon, int arrayWidth, std::vector<int> allPoints, std::vector<int>& rdpPoints) {
		int nextIndex = findFurthest(allPoints, startIndex, endIndex, epsilon, arrayWidth);
		if (nextIndex > 0) {
			//printf("Next Index: %i\n", nextIndex);
			if (startIndex != nextIndex) {
				rdp(startIndex, nextIndex, epsilon, arrayWidth, allPoints, rdpPoints);
			}
			rdpPoints.push_back(allPoints[nextIndex]); //I don't think this line is working. I think the rdp vector still just has the start and end points and that is it.
			if (endIndex != nextIndex) {
				rdp(nextIndex, endIndex, epsilon, arrayWidth, allPoints, rdpPoints);
			}
		}
	}
#pragma endregion