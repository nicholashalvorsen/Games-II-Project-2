//=======================================================================================
// player.h by Nick Halvorsen (C) 2015 All Rights Reserved.
//=======================================================================================

#include "Player.h"
#include "constants.h"

Player::Player() {
	rotationAngle = 0;
	animationTimer = 0;
	gliding = false;
}

void Player::draw(RenderInfo* ri) {

	Object::draw(ri);
}

void Player::update(float dt) {

	float playerGravity = GRAVITY;
	float playerGlide = PLAYER_GLIDE_DROP;

	if (getPosition().y < 0)
	{
		playerGlide = PLAYER_GLIDE_DROP * .65;
		playerGravity = GRAVITY * .65;
	}

	setVelocity(getVelocity() + Vector3(0, playerGravity * dt, 0));
	setVelocity(Vector3(getVelocity().x * .99992, getVelocity().y, getVelocity().z));

	if (gliding && getVelocity().y < playerGlide)
		setVelocity(getVelocity() + Vector3(0, playerGravity * -1.5 * dt, 0));
	
	if (diving && getVelocity().y < PLAYER_BOUNCE_FORCE * .5)
		setVelocity(getVelocity() + Vector3(0, PLAYER_DIVE_SPEED * dt, 0));

	Object::update(dt);

	animationTimer += dt;
	if (animationTimer < 6) {
		geometry->update(1, 5.0f, dt);
	} else if (animationTimer < 12) {
		geometry->update(0, 5.0f, dt);
	} else animationTimer = 0;
}

// not used currently
void Player::thrust(float dt) {
	setVelocity(getVelocity() + Vector3(cos(rotationAngle - 2 * PI / 4) * dt * -1 * PLAYER_THRUST, sin(rotationAngle - 2 * PI / 4) * dt * -1 * PLAYER_THRUST, 0));
}

void Player::resetPos() {
	setPosition(Vector3(0, 0, 0));
	setVelocity(Vector3(0, 0, 0));
}

void Player::thrustUp(float dt) {
	if (getVelocity().y < 0)
		setVelocity(getVelocity() + Vector3(0, PLAYER_THRUST * dt, 0));
	else
		setVelocity(getVelocity() + Vector3(0, PLAYER_THRUST * .6666 * dt, 0));
}

/// not used currently
void Player::rotateRight(float dt) {
	rotationAngle -= PLAYER_ROTATE_SPEED * dt;
	setRotation(getRotation() - Vector3(0, 0, PLAYER_ROTATE_SPEED * dt));
}

// not used currently
void Player::rotateLeft(float dt) {
	rotationAngle += PLAYER_ROTATE_SPEED * dt;
	setRotation(getRotation() + Vector3(0, 0, PLAYER_ROTATE_SPEED * dt));
}

void Player::accelLeft(float dt) {
	setVelocity(Vector3(getVelocity().x - PLAYER_X_ACCEL * dt, getVelocity().y, getVelocity().z));
	if (getVelocity().x < -PLAYER_X_SPEED)
		setVelocity(Vector3(-PLAYER_X_SPEED, getVelocity().y, getVelocity().z));
}

void Player::accelRight(float dt) {
	setVelocity(Vector3(getVelocity().x + PLAYER_X_ACCEL * dt, getVelocity().y, getVelocity().z));
	if (getVelocity().x > PLAYER_X_SPEED)
		setVelocity(Vector3(PLAYER_X_SPEED, getVelocity().y, getVelocity().z));
}

void Player::decelX(float dt) {
	setVelocity(Vector3(getVelocity().x * .99 * dt, getVelocity().y, getVelocity().z));
	
	if (getVelocity().x < .1)
		setVelocity(Vector3(0, getVelocity().y, getVelocity().z));
}

void Player::setGliding(bool g){
	gliding = g;
}

void Player::setDiving(bool d){
	diving = d;
}

// not used currently
void Player::airBounce(){
	if (getVelocity().y < PLAYER_BOUNCE_FORCE * .7)
		setVelocity(Vector3(getVelocity().x, PLAYER_BOUNCE_FORCE, getVelocity().z));
}