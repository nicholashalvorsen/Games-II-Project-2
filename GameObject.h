
#ifndef GameObject_H
#define GameObject_H

//#include "d3dUtil.h"
#include "d3dUtil.h"

#include "Box.h"
#include "constants.h"

class GameObject
{
public:

	GameObject();
	~GameObject();

	void init(Box *b, float r, Vector3 pos, Vector3 vel, float speed, float scale);
	void draw();
	void update(float dt);

	void setPosition (Vector3 pos) {position = pos;}
	Vector3 getPosition() {return position;}
	void setVelocity (Vector3 vel) {velocity = vel;}
	Vector3 getVelocity() {return velocity;}
	void setSpeed(float s) {speed = s;}
	float getSpeed() {return speed;}
	void setRadius(float r) {radius = r; radiusSquared = (scale*r)*(scale*r);}
	float getRadiusSquare() {return radiusSquared;}
	float getRadius() {return radius;}
	Matrix getWorldMatrix() {return world;}
	void setScale(float s) {scale = s; radiusSquared = (s*radius)*(s*radius);}
	float getScale() {return scale;}
	void setActive() {active = true;}
	void setInActive() {active = false;}
	bool getActiveState() {return active;}
	void setMTech(ID3D10EffectTechnique* m){ mTech = m;}
	bool collided(GameObject *gameObject);

	void setPositionX(float x) { setPosition(Vector3(x, position.y, position.z)); }
	void setPositionY(float y) { setPosition(Vector3(position.x, y, position.z)); }
	void setPositionZ(float z) { setPosition(Vector3(position.x, position.y, z)); }

	void rotateX(float destination, float returnLocation, float speed);
	void rotateY(float destination, float returnLocation, float speed);
	void rotateZ(float destination, float returnLocation, float speed);

	ID3D10EffectTechnique* getmTech(){ return mTech; }

private:

protected:
	Vector3 position;
	Vector3 velocity;
	float speed;
	float radius;
	float radiusSquared;
	float scale;	
	Box *box; 
	
	bool active;
	Matrix world;
	
	ID3D10EffectTechnique* mTech;

	int   dirX;
	float curX;
	float speedX;
	float destinationX;
	float returnX;
	int   dirY;
	float curY;
	float speedY;
	float destinationY;
	float returnY;
	int   dirZ;
	float curZ;
	float speedZ;
	float destinationZ;
	float returnZ;
};


#endif
