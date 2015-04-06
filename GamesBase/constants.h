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
const int GAME_DEPTH = 20;
const int GAME_BEHIND_DEPTH = 5;
const int NUM_SCENERY = 2;

const float GRAVITY = - 8;
const float VELOCITY_LIMIT = 10;
// not used currently
const float PLAYER_ROTATE_SPEED = 3;
const float PLAYER_THRUST = 20;
const float PLAYER_BOUNCE_FORCE = 8;
const float PLAYER_X_SPEED = 5;
const float PLAYER_X_ACCEL = 100;
const float PLAYER_GLIDE_DROP = -1;
const float PLAYER_DIVE_SPEED = -10;

const int NUM_PILLARS = 20;
const float PILLAR_HEIGHT_START = 3;
const float PILLAR_SPEED = -3;

const float WATER_RISE_SPEED = 0.05;


#endif