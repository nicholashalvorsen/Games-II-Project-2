#ifndef AXIS_H
#define AXIS_H

#include "constants.h"
#include "Object.h"
#include "Line.h"
#include "RenderInfo.h"

class Axis {
public:
	void init(Line* line) {
		x.init(line, Vector3(0, 0, 0));
		x.setScale(Vector3(5, 5, 5));
		x.setColor(1, 0, 0, 1);
		x.update(0);
		y.init(line, Vector3(0, 0, 0));
		y.setRotation(Vector3(0, 0, (float) ToRadian(90)));
		y.setScale(Vector3(5, 5, 5));
		y.setColor(0, 1, 0, 1);
		y.update(0);
		z.init(line, Vector3(0, 0, 0));
		z.setRotation(Vector3(0, (float) ToRadian(-90), 0));
		z.setScale(Vector3(5, 5, 5));
		z.setColor(0, 1, 1, 1);
		z.update(0);
	}

	void draw(RenderInfo* ri) {
		x.draw(ri);
		y.draw(ri);
		z.draw(ri);
	}

private:
	Object x, y, z;
};
#endif