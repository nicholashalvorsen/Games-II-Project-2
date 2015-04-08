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

const float GAME_WIDTH = 10;
const float GAME_BOTTOM = -10;
const float GAME_TOP = 8;

const float GRAVITY = - 5;
const float VELOCITY_LIMIT = 10;
const float PLAYER_ROTATE_SPEED = 3;
const float PLAYER_THRUST = 10;

const int MENU_OFFSET = 125;

enum menuStates { GAME_MODE = 1, SFX, CHEATS, FEELING_LUCKY,
					MODE_1_PLAYER = 11, MODE_2_PLAYER,
					SFX_ON = 21, SFX_OFF,
					CHEATS_ON = 31, CHEATS_OFF
};

#endif