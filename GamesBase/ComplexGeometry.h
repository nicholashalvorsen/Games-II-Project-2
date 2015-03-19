#ifndef COMPLEX_GEOMETRY_H
#define COMPLEX_GEOMETRY_H

#include "constants.h"
#include "Geometry.h"
#include <vector>

struct Child {
	Geometry* g;
	Matrix m;
	Vector4 c;
	bool hasColor;
};

class ComplexGeometry: public Geometry {
public:
	ComplexGeometry(): base(0) {}

	void init(Geometry* base) {
		this->base = base;
	}

	void addChild(Geometry* g, Vector3 translate, Vector3 rotate, Vector3 scale, Vector4 color) {
		Child c;
		c.g = g;

		Matrix rotXM, rotYM, rotZM, transM, scaleM;
		RotateX(&rotXM, rotate.x);
		RotateY(&rotYM, rotate.y);
		RotateZ(&rotZM, rotate.z); 
		Translate(&transM, translate.x, translate.y, translate.z);
		Scale(&scaleM, scale.x, scale.y, scale.z);

		c.m = scaleM * rotXM * rotYM * rotZM * transM;
		c.c = color;
		c.hasColor = true;

		children.push_back(c);
	}

	void addChild(Geometry* g, Vector3 translate, Vector3 rotate, Vector3 scale) {
		Child c;
		c.g = g;

		Matrix rotXM, rotYM, rotZM, transM, scaleM;
		RotateX(&rotXM, rotate.x);
		RotateY(&rotYM, rotate.y);
		RotateZ(&rotZM, rotate.z); 
		Translate(&transM, translate.x, translate.y, translate.z);
		Scale(&scaleM, scale.x, scale.y, scale.z);

		c.m = scaleM * rotXM * rotYM * rotZM * transM;
		c.hasColor = false;

		children.push_back(c);
	}

	void draw(RenderInfo* ri, Matrix world, Vector4 color) {
		base->draw(ri, world, color);
		for (Child c : children) {
			Matrix nw = c.m * world;
			if (c.hasColor)
				c.g->draw(ri, nw, c.c);
			else
				c.g->draw(ri, nw, color);
		}
	}

	float getRadius() { return 1; }
private:
	Geometry* base;
	std::vector<Child> children;
};
#endif