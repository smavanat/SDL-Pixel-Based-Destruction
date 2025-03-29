#include "Entity.h"

void Entity::SetTexture(Texture* texture) {
	eTexture = texture;
}

b2BodyId Entity::CreatePolygonCollider(b2WorldId id) {
	std::vector<int> tempPoints = marchingSquares(eTexture);

	std::vector<int> temprdpPoints;
	//Position at size()-2 is where 0 is stored, which is what we want. This will give us the 
	//straight line that we want.
	temprdpPoints.push_back(tempPoints[tempPoints.size() - 2]);
	rdp(0, tempPoints.size() - 1, 3, eTexture->getWidth(), tempPoints, temprdpPoints);
	temprdpPoints.push_back(tempPoints[tempPoints.size() - 2]);
	eCollider = createTexturePolygon(temprdpPoints, eTexture->getWidth(), eTexture->getOrigin().x, eTexture->getOrigin().y, eTexture->getAngle(), id, eTexture->getCentre());
	return eCollider;
}

b2BodyId Entity::CreateBoxCollider(b2WorldId id) {
	b2BodyDef tempBodyDef = b2DefaultBodyDef();
	tempBodyDef.position = { (eTexture->getOrigin().x + (eTexture->getWidth() / 2)) * pixelsToMetres, (eTexture->getOrigin().y + (eTexture->getHeight() / 2)) * pixelsToMetres };
	tempBodyDef.type = b2_dynamicBody;
	eCollider = b2CreateBody(id, &tempBodyDef);
	//b2Polygon tempBox = b2MakeOffsetBox((eTexture->getWidth() / 2) * pixelsToMetres, (eTexture->getHeight() / 2) * pixelsToMetres, { (eTexture->getWidth() / 2) * pixelsToMetres, (testTexture.getHeight() / 2) * pixelsToMetres }, 0);
	b2Polygon tempBox = b2MakeBox((eTexture->getWidth()/2) * pixelsToMetres, (eTexture->getHeight()/2) * pixelsToMetres);
	b2ShapeDef tempShapeDef = b2DefaultShapeDef();
	b2CreatePolygonShape(eCollider, &tempShapeDef, &tempBox);
	return eCollider;
}

void Entity::Update() {
	position = newVector2(b2Body_GetPosition(eCollider).x * metresToPixels, b2Body_GetPosition(eCollider).y*metresToPixels);
	angle = b2Rot_GetAngle(b2Body_GetRotation(eCollider)) / DEGREES_TO_RADIANS;
	eTexture->setOrigin(position.x - (eTexture->getWidth() / 2), position.y - (eTexture->getHeight() / 2));
	eTexture->setAngle(angle);
}

void Entity::Render(SDL_Renderer* gRenderer) {
	eTexture->render(gRenderer, NULL, this->angle, NULL, SDL_FLIP_NONE);
}