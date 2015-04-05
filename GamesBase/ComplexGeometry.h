#ifndef COMPLEX_GEOMETRY_H
#define COMPLEX_GEOMETRY_H

#include "constants.h"
#include "Geometry.h"
#include "AnimationState.h"
#include <vector>

struct Child {
	Geometry* g;
	Vector3 rotate, translate, scale;
	Matrix m;
	Vector4 c;
	bool hasColor;
	AnimationState* animation;
};

class ComplexGeometry: public Geometry {
public:
	ComplexGeometry(): base(0) {}

	void init(Geometry* base) {
		this->base = base;
	}

	void addChild(Geometry* g, Vector3 translate, Vector3 rotate, Vector3 scale, AnimationState* ani, Vector4 color) {
		Child c;
		c.g = g;
		c.animation = ani;
		c.translate = translate;
		c.rotate = rotate;
		c.scale = scale;

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

	void addChild(Geometry* g, Vector3 translate, Vector3 rotate, Vector3 scale, AnimationState* ani) {
		Child c;
		c.g = g;
		c.animation = ani;

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

	void update(int animation, float time, float dt) {
		if (time == 0) return;
		for (int i = 0; i < children.size(); i++) {
			Child c = children[i];

			Animation ani = c.animation->getAnimation(animation);
			c.rotate.x += (ani.Erotate.x - ani.Srotate.x) / time * dt;
			c.rotate.y += (ani.Erotate.y - ani.Srotate.y) / time * dt;
			c.rotate.z += (ani.Erotate.z - ani.Srotate.z) / time * dt;
			c.translate.x -= (ani.Etranslate.x - ani.Stranslate.x) / time * dt;
			c.translate.y -= (ani.Etranslate.y - ani.Stranslate.y) / time * dt;
			c.translate.z -= (ani.Etranslate.z - ani.Stranslate.z) / time * dt;
			c.scale.x += (ani.Escale.x - ani.Sscale.x) / time * dt;
			c.scale.y += (ani.Escale.y - ani.Sscale.y) / time * dt;
			c.scale.z += (ani.Escale.z - ani.Sscale.z) / time * dt;
			
			Matrix rotXM, rotYM, rotZM, transM, scaleM;
			RotateX(&rotXM, c.rotate.x);
			RotateY(&rotYM, c.rotate.y);
			RotateZ(&rotZM, c.rotate.z); 
			Translate(&transM, c.translate.x, c.translate.y, c.translate.z);
			Scale(&scaleM, c.scale.x, c.scale.y, c.scale.z);

			c.m = scaleM * rotXM * rotYM * rotZM * transM;

			children[i] = c;
			children[i].g->update(animation, time, dt);
		}
	}

	void setInitialState(int animation) {
		for (int i = 0; i < children.size(); i++) {
			Child c = children[i];
			Animation ani = c.animation->getAnimation(animation);
			c.rotate = ani.Srotate;
			c.translate = ani.Stranslate;
			c.scale = ani.Sscale;

			Matrix rotXM, rotYM, rotZM, transM, scaleM;
			RotateX(&rotXM, c.rotate.x);
			RotateY(&rotYM, c.rotate.y);
			RotateZ(&rotZM, c.rotate.z); 
			Translate(&transM, c.translate.x, c.translate.y, c.translate.z);
			Scale(&scaleM, c.scale.x, c.scale.y, c.scale.z);

			c.m = scaleM * rotXM * rotYM * rotZM * transM;

			children[i] = c;
			children[i].g->setInitialState(animation);
		}
	}
private:
	Geometry* base;
	std::vector<Child> children;
};
#endif