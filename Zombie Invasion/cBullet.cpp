/*
=================
cBullet.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cBullet.h"

/*
=================================================================
Defualt Constructor
=================================================================
*/
cBullet::cBullet() : cSprite()
{
	this->bulletVelocity = { 0.0, 0.0 };
}
/*
=================================================================
Update the sprite position
=================================================================
*/

void cBullet::update(double deltaTime)
{

	FPoint direction = { 0.0, 0.0 };
	direction.X = (sin((this->getSpriteRotAngle())*PI / 180));
	direction.Y = -(cos((this->getSpriteRotAngle())*PI / 180));

	direction.X *= this->getSpriteTranslation().x;
	direction.Y *= this->getSpriteTranslation().y;

	this->bulletVelocity.x = this->bulletVelocity.x + direction.X;
	this->bulletVelocity.y = this->bulletVelocity.y + direction.Y;

	SDL_Rect currentSpritePos = this->getSpritePos();
	currentSpritePos.x += this->bulletVelocity.x * 0.3;
	currentSpritePos.y += this->bulletVelocity.y *0.3;

	this->setSpritePos({ currentSpritePos.x, currentSpritePos.y });

	cout << "Bullet position - x: " << this->getSpritePos().x << " y: " << this->getSpritePos().y << " deltaTime: " << deltaTime << endl;

	this->setBoundingRect(this->getSpritePos());

	if (currentSpritePos.x < 0 || currentSpritePos.x > 1024)
	{
		this->setActive(false);
	}

	if (currentSpritePos.y < 0 || currentSpritePos.y > 768)
	{
		this->setActive(false);
	}


}
/*
=================================================================
  Sets the velocity for the Bullet
=================================================================
*/
void cBullet::setBulletVelocity(SDL_Point bulletVel)
{
	this->bulletVelocity = bulletVel;
}
/*
=================================================================
  Gets the Bullet velocity
=================================================================
*/
SDL_Point cBullet::getBulletVelocity()
{
	return this->bulletVelocity;
}
