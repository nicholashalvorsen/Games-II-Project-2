#ifndef OBJECT_H
#define OBJECT_H

#include "constants.h"
#include "d3dUtil.h"
#include "Geometry.h"
#include "RenderInfo.h"

class Object {
public:
	Object() {Identity(&world);}
	~Object() { geometry = NULL;}
	
	void init(Geometry *g, Vector3 pos) {
		geometry = g;
		position = pos;
		scale = Vector3(1, 1, 1);
		radiusSquared = g->getRadius() * g->getRadius(); 
		rotation = Vector3(0, 0, 0);
		velocity = Vector3(0, 0, 0);
		color = Vector4(0, 0, 0, 1.0f);
		active = true;
		newAnimation = false;
		animation = 0;
		animationLength = 0;
	}

	void update(float dt) {
		position += velocity * dt;

		if (newAnimation) {
			newAnimation = false;
			geometry->setInitialState(animation);
		}
		geometry->update(animation, animationLength, dt);

		Matrix rotXM, rotYM, rotZM, transM, scaleM;
		RotateX(&rotXM, rotation.x);
		RotateY(&rotYM, rotation.y);
		RotateZ(&rotZM, rotation.z); 
		Translate(&transM, position.x, position.y, position.z);
		Scale(&scaleM, scale.x, scale.y, scale.z);
		
		world = scaleM * rotXM * rotYM * rotZM * transM;
	}

	void draw(RenderInfo* ri) {
		if (!active) return;
		geometry->draw(ri, world, color);
	}

	bool collided(Object *object) {
		// collision using radius
		/*
		Vector3 diff = position - object->getPosition();
		float length = D3DXVec3LengthSq(&diff);
		float radii = radiusSquared + object->radiusSquared;
		if (length <= radii)
			return true;
		return false;
		*/

		// box collision
		if (getPosition().x + getScale().x / 2 > object->getPosition().x - object->getScale().x / 2 &&
			getPosition().x - getScale().x / 2 < object->getPosition().x + object->getScale().x / 2 &&
			getPosition().y - getScale().y / 2 > object->getPosition().y - object->getScale().y / 2 &&
			getPosition().y + getScale().y / 2 < object->getPosition().y + object->getScale().y / 2 &&
			getPosition().z - getScale().z / 2 > object->getPosition().z - object->getScale().z / 2 &&
			getPosition().z + getScale().z / 2 < object->getPosition().z + object->getScale().z / 2)
			return true;
		else
			return false;
	}

	void setPosition (Vector3 pos) { position = pos; }
	Vector3 getPosition() { return position; }
	void setScale(Vector3 s) {
		scale = s;
		float m = s.x;
		if (m < s.y) m = s.y;
		if (m < s.z) m = s.z;
		radiusSquared = (geometry->getRadius() * m) * (geometry->getRadius() * m);
	}
	Vector3 getScale() { return scale; }
	void setActive() { active = true; }
	void setInActive() { active = false; }
	bool getActiveState() { return active; }
	Vector3 getRotation() { return rotation; }
	void setRotation(Vector3 rot) { rotation = rot; }

	void setVelocity (Vector3 vel) { velocity = vel; }
	Vector3 getVelocity() { return velocity; }

	void setColor(float r, float g, float b, float a) {
		color.x = r;
		color.y = g;
		color.z = b;
		color.w = a;
	}

	void setAnimation(int animation, float animationLength) {
		newAnimation = true;
		this->animation = animation;
		this->animationLength = animationLength;
	}
protected:
	Geometry* geometry;
private:
	Vector3 position;
	Vector3 scale;
	Vector3 rotation;
	Vector4 color;

	Vector3 velocity;
	float radiusSquared;

	bool newAnimation;
	int animation;
	float animationLength;

	bool active;

	Matrix world;
};

#endif