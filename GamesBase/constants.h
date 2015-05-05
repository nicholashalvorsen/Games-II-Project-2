#ifndef Constants_H
#define Constants_H

#include <string>

#define Vector3 D3DXVECTOR3
#define Vector4 D3DXVECTOR4
#define Matrix D3DXMATRIX
#define Identity D3DXMatrixIdentity
#define Translate D3DXMatrixTranslation
#define RotateX D3DXMatrixRotationX
#define RotateY D3DXMatrixRotationY
#define RotateZ D3DXMatrixRotationZ
#define Scale D3DXMatrixScaling
#define ToRadian D3DXToRadian
#define ToDegree D3DXToDegree

const wchar_t WAVE_BANK[]  = L"audio\\Win\\MyWBank.xwb";
const wchar_t SOUND_BANK[] = L"audio\\Win\\MySBank.xsb";

const int GAME_WIDTH = 20;
const int GAME_DEPTH = 40;
const int GAME_BEHIND_DEPTH = 10;
const int NUM_SCENERY = 2;
const int NUM_STARS = 300;
const int NUM_CLIFFS = 30;
const float CLIFF_HEIGHT = 300;
const float CLIFF_WIDTH = 3.9;

const float GRAVITY = - 8;
const float Y_VELOCITY_LIMIT = -7;
// not used currently
const float PLAYER_ROTATE_SPEED = 3;
const float PLAYER_THRUST = 20;
const float PLAYER_BOUNCE_FORCE = 8;
const float PLAYER_X_SPEED = 5;
const float PLAYER_X_ACCEL = 150;
const float PLAYER_GLIDE_DROP = -1.2;
const float PLAYER_DIVE_SPEED = -10;

const int NUM_PILLARS = 15;
const float PILLAR_HEIGHT_START = 1.5;
const float PILLAR_SPEED = -3;
const float PILLAR_SIZE = 4;

const int NUM_DIAMONDS = NUM_PILLARS / 5;
const float DIAMOND_HEIGHT_START = 0.4;
const float DIAMOND_SPEED = -3;
const float DIAMOND_SIZE = 0.75;

const int NUM_CLOUDS = 15;
const float CLOUD_SPEED = -5;
const float CLOUD_X_SPEED = 1;
const float CLOUD_SIZE = 3;

const int NUM_PLANETS = 15;
const float PLANET_SPEED = -6;
const float PLANET_X_SPEED = 1;
const float PLANET_SIZE = 2.5;

const int NUM_ROCKS = 10;
const float ROCK_SPEED = -2;
const float ROCK_X_SPEED = 1;
const float ROCK_SIZE = 3;
const int ROCKS_WIDTH = GAME_WIDTH * 3;

const int NUM_BUBBLES = 400;

const int SEA_SIZE = 150;
const float WATER_RISE_SPEED = 0;

const float CAMERA_MOVE_SPEED = 20;

const float LAYER_HEIGHT[4] = {0, 20, 60, -20};
const std::wstring LAYER_NAMES[4] = {L"S H I P W R E C K", L"S K Y L I N E", L"S P A C E", L"U N D E R W A T E R"};

const int MENU_OFFSET = 125;

const float FADE_TEXT_DURATION = 2;
const float FADE_TEXT_FADE_SPEED = 1;

enum menuStates { PLAY = 1, PLAY_EASY, SOUNDS
};

enum gameStates {
	MENU, LEVEL1, LEVEL2, GAME_OVER
};

#endif