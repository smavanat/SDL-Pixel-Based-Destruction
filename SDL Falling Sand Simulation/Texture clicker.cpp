#include<SDL.h>
#include<stdio.h>
#include<SDL_image.h>
#include<iostream>
#include<box2d/box2d.h>
#include<box2d/base.h>
#include<PolyPartition/polypartition.h>
#include "Texture.hpp"
#include "Outline.hpp"

//TODO: Figure out why collider oversimplification is occuring
//		Figure out how to account for rotation
//		Figure out why there's an odd movement of textures when new colliders are formed.
//		Figure out why the vector subscript has a seizure. Now only when do 6-8 colliders

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const float metresToPixels = 20.0f;
const float pixelsToMetres = 1.0f / metresToPixels;

////Some global variables
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
Texture testTexture = Texture(192, 10);
b2WorldDef worldDef;
b2WorldId worldId;

//For holding our textures. Needs to be a pointer vector because otherwise will get error because the texture objects 
//are being copied over to a new location and the originals deleted.
//This means that the old pointers for surfacePixels etc. are being invalidated, hence the memory write errors.
std::vector<Texture*> textures;

std::vector<b2BodyId> colliders;	
std::vector<b2BodyId> statics;

int scale = 5;

bool init();
bool loadMedia();

b2Vec2* getVec2Array(std::vector<int> rdpPoints, int arrayWidth, int x, int y) {
	b2Vec2* points = new b2Vec2[rdpPoints.size()];
	for (int i = 0; i < rdpPoints.size(); i++) {
		int* temp = convertIndexToCoords(rdpPoints[i], arrayWidth);
		points[i] = { (temp[0]) * pixelsToMetres, (temp[1]) * pixelsToMetres };
	}
	//For Testing:
	printf("Coords:\n");
	for (int i = 0; i < rdpPoints.size(); i++) {
		printf("%f, %f\n", points[i].x*metresToPixels, points[i].y*metresToPixels);
	}
	return points;
}

b2Vec2* convertToVec2(TPPLPoint* polyPoints, int numPoints) {
	b2Vec2* points = new b2Vec2[numPoints];
	for (int i = 0; i < numPoints; i++) {
		points[i].x = polyPoints[i].x;
		points[i].y = polyPoints[i].y;
		//printf("X: %f, Y: %f ", points[i].x, points[i].y);
	}
	//printf("\n");
	return points;
}

//Necessary to check which colliders to remove from vector since c++ does not generate default comparators for structs.
bool operator ==(const b2BodyId& lhs, const b2BodyId& rhs) {
	if (lhs.index1 == rhs.index1 && lhs.revision == rhs.revision && lhs.world0 == rhs.world0) return true;
	else return false;
}

//This doesn't always work. Maybe true pure triangulation? Or at least try and find the cause. The
//program sometimes oversimplifies the collider outline.
void createTexturePolygon(std::vector<int> rdpPoints, int arrayWidth, int x, int y) {
	b2Vec2* points = getVec2Array(rdpPoints, arrayWidth, x, y);

	b2BodyDef testbodyDef = b2DefaultBodyDef();
	testbodyDef.type = b2_dynamicBody;
	testbodyDef.position = { static_cast<float>(x)* pixelsToMetres, static_cast<float>(y)*pixelsToMetres };
	b2BodyId testId = b2CreateBody(worldId, &testbodyDef);
	//This isn't working because the polygons have more than 8 vertices, which causes b2ComputeHull to fail.
	//Will have to implement a polypartition algorithm to fix for polygons with vertices > 8.
	//Then attach all polygons to the same body.
	//Looking at the algorithms outlined here: https://github.com/ivanfratric/polypartition
	//I think the hertel-mehlhorn algorithm seems like the best bet to fix this.
	if (rdpPoints.size() > 8) {
		TPPLPoly *poly = new TPPLPoly();
		poly->Init(rdpPoints.size());
		TPPLPolyList polyList;

		for (int i = 0; i < rdpPoints.size(); i++) {
			(*poly)[i].x = points[i].x;
			(*poly)[i].y = points[i].y;
		}

		//Need to set it to be oriented Counter-Clockwise otherwise the triangulation algorithm fails.
		poly->SetOrientation(TPPL_ORIENTATION_CCW);
		TPPLPartition test = TPPLPartition();
		//int result = test.ConvexPartition_HM(poly, &polyList);	
		int result = test.ConvexPartition_OPT(poly, &polyList);
		printf("Result: %i, Size: %i, ", result, polyList.size());
		std::cout << "Valid: " << poly->Valid() << "\n";

		/*printf("Printing Polylist:\n");
		for (TPPLPolyList::iterator it = polyList.begin(); it != polyList.end(); ++it) {
			TPPLPoint* temp = it->GetPoints();
			for (int i = 0; i < it->GetNumPoints(); i++) {
				printf("X: %f, Y: %f ", temp[i].x, temp[i].y);
			}
			printf("\n");
		}
		printf("Finished Printing\n");*/

		for (TPPLPolyList::iterator it = polyList.begin(); it != polyList.end(); ++it) {
			//For some reason the list feeds degenerate inputs to the hull on the last input. Idk why. 
			//This means the last polygon isn't being generated. Why the fuck not.
			//Ok so for some reason the last polygon in every list has three vertices, two of which are the fucking same.
			//Why?? How knows. But that's why b2ComputeHull won't work, because there isn't any hull to compute.
			//Need to check that this is true in the polylist and not just the one returned from the function.
			//FIXED: Used ConvexPartition_OPT instead of ConvexPartition_HM. I think the ear clipping was what fucked it 
			//up originally.
			b2Hull hull = b2ComputeHull(convertToVec2(it->GetPoints(), it->GetNumPoints()), it->GetNumPoints());
			if (hull.count == 0) {
				printf("Something odd has occured when generating a hull from a polyList\n");
			}
			else {
				b2Polygon testagon = b2MakePolygon(&hull, 0.0f);
				b2ShapeDef testshapeDef = b2DefaultShapeDef();
				testshapeDef.friction = 0.3f;
				b2CreatePolygonShape(testId, &testshapeDef, &testagon);
			}
		}
		printf("Number of shapes on the body: %i\n", b2Body_GetShapeCount(testId));
	}
	else {
		b2Hull hull = b2ComputeHull(points, rdpPoints.size());
		if (hull.count == 0) {
			printf("Something odd Has occured when generating a hull from a polygon\n");
		}
		else {
			b2Polygon testagon = b2MakePolygon(&hull, 0.0f);
			b2ShapeDef testshapeDef = b2DefaultShapeDef();
			testshapeDef.friction = 0.3f;
			b2CreatePolygonShape(testId, &testshapeDef, &testagon);
		}
	}
	colliders.push_back(testId);
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
		worldDef = b2DefaultWorldDef();
		worldDef.gravity = { 0.0f, 0.0f };
		worldId = b2CreateWorld(&worldDef);
		//This isn't putting the ground body in the correct position for some reason.
		b2BodyDef groundBodyDef = b2DefaultBodyDef();
		groundBodyDef.position = { 320.0f*pixelsToMetres, 460.0f * pixelsToMetres };
		b2BodyId groundId = b2CreateBody(worldId, &groundBodyDef);
		b2Polygon groundBox = b2MakeBox(320.0f*pixelsToMetres, 5.0f*pixelsToMetres);
		b2ShapeDef groundShapeDef = b2DefaultShapeDef();
		b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);
		statics.push_back(groundId);
		//So that there is some sort of default collider to go along with a default texture.
		b2BodyDef tempBodyDef = b2DefaultBodyDef();
		tempBodyDef.position = { 192.0f * pixelsToMetres, 10.0f * pixelsToMetres };
		b2BodyId tempId = b2CreateBody(worldId, &tempBodyDef);
		colliders.push_back(tempId);
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
	b2DestroyWorld(worldId);
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
			//Texture* dragTexture = NULL;
			int dragIndex = -1;
			textures.push_back(&testTexture);//This works fine now.
			std::vector<std::vector<int>> testingPoints;
			std::vector<std::vector<int>> rdpPoints;
			bool getOutline = false;
			bool getSimplifiedOutline = false;
			bool getColliderOutlines = false;

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
							std::vector<Texture*> texturesToRemove;
							std::vector<Texture*> texturesToAdd;
							std::vector<b2BodyId> collidersToRemove;
							std::vector<std::vector<int>> testingPointsToRemove;
							std::vector<std::vector<int>> rdpPointsToRemove;

							for(int i = 0; i < textures.size(); i++) {
								if (textures[i]->isAltered()) {
									for (Texture* texture : splitTextureAtEdge(textures[i], gRenderer)) {
										texturesToAdd.push_back(texture);
									}
									textures[i]->resetSplittingFlag();
									texturesToRemove.push_back(textures[i]);
									collidersToRemove.push_back(colliders[i]);
									testingPointsToRemove.push_back(testingPoints[i]);
									rdpPointsToRemove.push_back(rdpPoints[i]);
								}
							}

							for (Texture* t : texturesToAdd) {
								textures.push_back(t);

								std::vector<int> tempPoints = marchingSquares(t);
								testingPoints.push_back(tempPoints);

								std::vector<int> temprdpPoints;
								//Position at size()-2 is where 0 is stored, which is what we want. This will give us the 
								//straight line that we want.
								temprdpPoints.push_back(tempPoints[tempPoints.size() - 2]);
								rdp(0, tempPoints.size() - 1, 5, t->getWidth(), tempPoints, temprdpPoints);
								temprdpPoints.push_back(tempPoints[tempPoints.size() - 2]);
								rdpPoints.push_back(temprdpPoints);
								createTexturePolygon(temprdpPoints, t->getWidth(), t->getOriginX(), t->getOriginY());
							}
							texturesToAdd.clear();

							for (Texture* t : texturesToRemove) {
								textures.erase(find(textures.begin(), textures.end(), t));
								t->free();
							}
							texturesToRemove.clear();

							for (std::vector<int> i : testingPointsToRemove) {
								testingPoints.erase(find(testingPoints.begin(), testingPoints.end(), i));
							}
							testingPointsToRemove.clear();

							for (std::vector<int> i : rdpPointsToRemove) {
								rdpPoints.erase(find(testingPoints.begin(), testingPoints.end(), i));
							}
							rdpPointsToRemove.clear();

							//Need to re-write the below for blocks so I don't keep iterating over all the textures.
							//Probably need to stick them in the inner for loop at the top of this block
							//I so cba
							//testingPoints.clear();
							//for (int i = 0; i < textures.size(); i++) {
							//	std::vector<int> tempPoints = marchingSquares(textures[i]);
							//	testingPoints.push_back(tempPoints);
							//}

							//rdpPoints.clear();
							//for (int i = 0; i < textures.size(); i++) {
							//	printf("Texture No: %i", i);
							//	std::vector<int> tempPoints;
							//	//Position at size()-2 is where 0 is stored, which is what we want. This will give us the 
							//	//straight line that we want.
							//	tempPoints.push_back(testingPoints[i][testingPoints[i].size() - 2]);
							//	rdp(0, testingPoints[i].size() - 1, 5, textures[i]->getWidth(), testingPoints[i], tempPoints);
							//	tempPoints.push_back(testingPoints[i][testingPoints[i].size() - 2]);
							//	rdpPoints.push_back(tempPoints);
							//	createTexturePolygon(tempPoints, textures[i]->getWidth(), textures[i]->getOriginX(), textures[i]->getOriginY());
							//}

							for (b2BodyId id : collidersToRemove) {
								colliders.erase(find(colliders.begin(), colliders.end(), id));
								b2DestroyBody(id);
								//printf("Something is happening");
							}
							collidersToRemove.clear();

							printf("Number of Textures: %i\n", textures.size());
							printf("Number of Colliders: %i\n", colliders.size());
							leftMouseButtonDown = false;
						}
						if (e.button.button == SDL_BUTTON_RIGHT) {
							rightMouseButtonDown = false;
							//dragTexture = NULL;
							dragIndex = -1;
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
							//for (Texture* t : textures) {
							for(int i = 0; i < textures.size(); i++) {
								if (e.motion.x >= textures[i]->getOriginX() && e.motion.x < textures[i]->getOriginX() + textures[i]->getWidth()
									&& e.motion.y < textures[i]->getOriginY() + textures[i]->getHeight() && e.motion.y >= textures[i]->getOriginY()
									&& !textures[i]->clickedOnTransparent(e.motion.x, e.motion.y)) {
									//dragTexture = t;
									dragIndex = i;
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
						if (dragIndex != -1) {
							int newX = textures[dragIndex]->getOriginX() + e.motion.xrel;
							int newY = textures[dragIndex]->getOriginY() + e.motion.yrel;
							textures[dragIndex]->setOrigin(newX, newY);
							//b2Body_SetTransform(colliders[dragIndex], { newX*pixelsToMetres, newY*pixelsToMetres }, b2Body_GetRotation(colliders[dragIndex]));
						}
						break;
					case SDL_KEYDOWN:
						if (e.key.keysym.sym == SDLK_o) {
							getOutline = !getOutline;
						}
						if (e.key.keysym.sym == SDLK_s) {
							getSimplifiedOutline = !getSimplifiedOutline;
						}
						if (e.key.keysym.sym == SDLK_c) {
							/*b2Vec2 comparator = b2Vec2();
							int lastVertex = 7;

							for (int i = 0; i < colliders.size(); i++) {
								printf("Shape No: %i\n", i);
								int shapeCount = b2Body_GetShapeCount(colliders[i]);
								printf("Shape Count: %i\n", shapeCount);
								b2Vec2 colliderPosition = b2Body_GetPosition(colliders[i]);
								b2ShapeId* colliderShapes = new b2ShapeId[shapeCount];
								b2Body_GetShapes(colliders[i], colliderShapes, shapeCount);
								for (int j = 0; j < shapeCount; j++) {
									b2Vec2* colliderVertices = b2Shape_GetPolygon(colliderShapes[j]).vertices;
									for (int k = 7; k > -1; k--) {
										if (colliderVertices[k] != comparator) {
											lastVertex = k;
											break;
										}
									}
									for (int k = 0; k < lastVertex+1; k++) {
										printf("PosX: %f, PosY, %f\n", ((colliderVertices[k].x)* metresToPixels), ((colliderVertices[k].y)* metresToPixels));
									}
								}
							}*/
							getColliderOutlines = !getColliderOutlines;
						}
						break;
					}
				}
				//This is where all the functionality in the main loop will go.
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				for (Texture* t : textures) {
					t->render(gRenderer, NULL, 0.0, NULL, SDL_FLIP_NONE);
				}
				//Marching squares
				if (getOutline) {
					for (int i = 0; i < testingPoints.size(); i++) {
						for (int j = 0; j < testingPoints[i].size() - 1; j++) {
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
					for (int i = 0; i < rdpPoints.size(); i++) {
						for (int j = 0; j < rdpPoints[i].size() - 1; j++) {
							SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
							SDL_RenderDrawLine(gRenderer, textures[i]->getOriginX() + (rdpPoints[i][j] % textures[i]->getWidth()),
								textures[i]->getOriginY() + (int)(floor(rdpPoints[i][j] / textures[i]->getWidth())),
								textures[i]->getOriginX() + (rdpPoints[i][j + 1] % textures[i]->getWidth()),
								textures[i]->getOriginY() + (int)(floor(rdpPoints[i][j + 1] / textures[i]->getWidth())));
						}
					}
				}
				//Colliders
				if (getColliderOutlines) {
					SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
					b2ShapeId* shapes = new b2ShapeId[1];
					b2Body_GetShapes(statics[0], shapes, 1);
					//printf("PosX: %f, PosY: %f", b2Body_GetPosition(statics[0]).x*metresToPixels, b2Body_GetPosition(statics[0]).y*metresToPixels);
					b2Vec2 position = b2Body_GetPosition(statics[0]);
					b2Vec2* vertices = b2Shape_GetPolygon(shapes[0]).vertices;
					for (int i = 0; i < 4; i++) {
						SDL_RenderDrawLine(gRenderer, ((vertices[i].x + position.x)* metresToPixels), ((vertices[i].y + position.y)* metresToPixels),
													((vertices[(i + 1) > 3 ? 0 : (i + 1)].x + position.x)* metresToPixels), ((vertices[(i + 1) > 3 ? 0 : (i + 1)].y + position.y)* metresToPixels));
					}

					//The two variables below are for a really cursed way of getting the collider outlines to draw correctly
					//Since the vertices vector is always of size 8, in order to find the last actual uninitialised vertex
					//I create the uninitalised "comparator" variable, run a for loop to find the first index equal to it
					//and then store the index - 1 in last vertex
					b2Vec2 comparator = b2Vec2();
					int lastVertex = 7;
					for (int i = 0; i < colliders.size(); i++) {
						int shapeCount = b2Body_GetShapeCount(colliders[i]);
						b2Vec2 colliderPosition = b2Body_GetPosition(colliders[i]);
						b2ShapeId* colliderShapes = new b2ShapeId[shapeCount];
						b2Body_GetShapes(colliders[i], colliderShapes, shapeCount);
						for (int j = 0; j < shapeCount; j++) {
							b2Vec2* colliderVertices = b2Shape_GetPolygon(colliderShapes[j]).vertices;
							//To find the last actually initialsed vertex.
							for (int k = 7; k > -1; k--) {
								if (colliderVertices[k] != comparator) {
									lastVertex = k;
									break;
								}
							}
							for (int k = 0; k < lastVertex+1; k++) {
								SDL_RenderDrawLine(gRenderer, ((colliderVertices[k].x + colliderPosition.x)* metresToPixels), ((colliderVertices[k].y + colliderPosition.y)* metresToPixels),
									((colliderVertices[(k + 1) > lastVertex ? 0 : (k + 1)].x + colliderPosition.x)* metresToPixels), ((colliderVertices[(k + 1) > lastVertex ? 0 : (k + 1)].y + colliderPosition.y)* metresToPixels));
							}
						}
					}
				}
				//Updating graphics
				SDL_RenderPresent(gRenderer);

				//Updating physics
				b2World_Step(worldId, 1.0f / 60.0f, 4);
				for (int i = 0; i < colliders.size(); i++) {
					//printf("No: %i: ", i);
					b2Vec2 position = b2Body_GetPosition(colliders[i]);
					b2Rot rotation = b2Body_GetRotation(colliders[i]);
					//printf("Pos: %4.2f %4.2f Angle: %4.2f", position.x*metresToPixels, position.y* metresToPixels, b2Rot_GetAngle(rotation));
					textures[i]->setOrigin(position.x * metresToPixels, position.y * metresToPixels);
				}
			}
		}
	}
	close();
	return 0;
}