//=======================================================================================
// player.h by Nick Halvorsen (C) 2015 All Rights Reserved.
//=======================================================================================

#include "Player.h"
#include "constants.h"

Player::Player() {
	Object();
	rotationAngle = 0;
}

void Player::draw(RenderInfo* ri) {

	Object::draw(ri);
}

void Player::update(float dt) {
	setVelocity(getVelocity() + Vector3(0, GRAVITY * dt, 0));
	setVelocity(Vector3(getVelocity().x * .9999, getVelocity().y, getVelocity().z));
	Object::update(dt);
}


void Player::thrust(float dt) {
	setVelocity(getVelocity() + Vector3(cos(rotationAngle - 2 * PI / 4) * dt * -1 * PLAYER_THRUST, sin(rotationAngle - 2 * PI / 4) * dt * -1 * PLAYER_THRUST, 0));
}

void Player::resetPos() {
	setPosition(Vector3(0, 0, 0));
	setVelocity(Vector3(0, 0, 0));
}

void Player::thrustUp(float dt) {
	if (getVelocity().y < 0)
		setVelocity(getVelocity() + Vector3(0, GRAVITY * -3 * dt, 0));
	else
		setVelocity(getVelocity() + Vector3(0, GRAVITY * -2 * dt, 0));
}

void Player::rotateRight(float dt) {
	rotationAngle -= PLAYER_ROTATE_SPEED * dt;
	setRotation(getRotation() - Vector3(0, 0, PLAYER_ROTATE_SPEED * dt));
}


void Player::rotateLeft(float dt) {
	rotationAngle += PLAYER_ROTATE_SPEED * dt;
	setRotation(getRotation() + Vector3(0, 0, PLAYER_ROTATE_SPEED * dt));
}