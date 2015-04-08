#ifndef Constants_H
#define Constants_H

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
const int GAME_DEPTH = 25;
const int GAME_BEHIND_DEPTH = 10;
const int NUM_SCENERY = 2;
const int NUM_CLIFFS = 30;
const float CLIFF_HEIGHT = 10;
const float CLIFF_WIDTH = 3.9;

const float GRAVITY = - 8;
const float VELOCITY_LIMIT = 10;
// not used currently
const float PLAYER_ROTATE_SPEED = 3;
const float PLAYER_THRUST = 20;
const float PLAYER_BOUNCE_FORCE = 8;
const float PLAYER_X_SPEED = 5;
const float PLAYER_X_ACCEL = 150;
const float PLAYER_GLIDE_DROP = -1;
const float PLAYER_DIVE_SPEED = -10;

const int NUM_PILLARS = 20;
const float PILLAR_HEIGHT_START = 3;
const float PILLAR_SPEED = -3;

const int NUM_CLOUDS = 20;
const float CLOUD_SPEED = -5;
const float CLOUD_HEIGHT_START = 14;
const float CLOUD_SIZE = 3;

const float WATER_RISE_SPEED = 0;

const float CAMERA_MOVE_SPEED = 16;


const int MENU_OFFSET = 125;

enum menuStates { NEW_GAME = 1, SFX, CHEATS, FEELING_LUCKY,
};

enum gameStates {
	MENU, LEVEL1, LEVEL2, GAME_OVER
};

#endif