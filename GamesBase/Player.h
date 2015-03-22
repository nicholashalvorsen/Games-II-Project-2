//=======================================================================================
// player.h by Nick Halvorsen (C) 2015 All Rights Reserved.
//=======================================================================================

#ifndef Player_H
#define Player_H

#include "d3dUtil.h"
#include "Object.h"

class Player : public Object
{
public:
	Player();

	void draw(RenderInfo* ri);
	void update(float dt);
	void thrustUp(float dt);
	void thrust(float dt);
	void rotateRight(float dt);
	void rotateLeft(float dt);
	void resetPos();

	float rotationAngle;
};


#endif // Player_H