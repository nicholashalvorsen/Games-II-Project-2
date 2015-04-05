#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "constants.h"
#include "RenderInfo.h"

class Geometry {
public:
	virtual void draw(RenderInfo* ri, Matrix world, Vector4 color) = 0;
	virtual float getRadius() = 0;
	virtual void update(int animation, float time, float dt) {}
	virtual void setInitialState(int animation) {}
};
#endif