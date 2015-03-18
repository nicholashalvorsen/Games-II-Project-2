//=======================================================================================
// player.h by Nick Halvorsen (C) 2015 All Rights Reserved.
//=======================================================================================

#include "Player.h"

Player::Player() {
	rotationAngle = 0;
	GameObject();
}

void Player::draw() {

	GameObject::draw();
}

void Player::update(float dt) {
	velocity += Vector3(0, GRAVITY * .5 * dt, 0);
	GameObject::update(dt);
}


void Player::thrust(float dt) {
	velocity += Vector3(cos(rotationAngle - 2 * PI / 4) * dt * GRAVITY, sin(rotationAngle - 2 * PI / 4) * dt * GRAVITY, 0);
}

void Player::resetPos() {
	position = Vector3(0, 0, 0);
	velocity = Vector3(0, 0, 0);
}

void Player::thrustUp(float dt) {
	if (velocity.y < 0)
		velocity += Vector3(0, GRAVITY * -3 * dt, 0);
	else
		velocity += Vector3(0, GRAVITY * -2 * dt, 0);
}

void Player::rotateRight(float dt) {
	rotationAngle -= PLAYER_ROTATE_SPEED * dt;
	rotateZ(rotationAngle, rotationAngle, 10);
}


void Player::rotateLeft(float dt) {
	rotationAngle += PLAYER_ROTATE_SPEED * dt;
	rotateZ(rotationAngle, rotationAngle, 10);

}