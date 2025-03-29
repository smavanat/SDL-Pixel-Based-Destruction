#pragma once
#include "box2d/base.h"
#include "box2d/box2d.h"
#include "Texture.hpp"
#include "Outline.hpp"
//This class solely exists to manage Texture-Collider interactions. In future I may expand it so it becomes more
//generalised, but for now I just need it to manage these two things.
class Entity
{
	Texture* eTexture = NULL;
	b2BodyId eCollider;
	/*double originalTextureAngle = 0;
	double originalColliderAngle = 0;*/
	double angle; //In degrees
	Vector2 position;

	public:
		void SetTexture(Texture* texture);

		b2BodyId CreatePolygonCollider(b2WorldId id);

		b2BodyId CreateBoxCollider(b2WorldId id);

		void Update();

		void Render(SDL_Renderer* gRenderer);
};

