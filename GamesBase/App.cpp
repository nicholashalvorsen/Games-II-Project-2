﻿#include "d3dApp.h"
#include "Line.h"
#include "Box.h"
#include "Diamond.h"
#include "Quad.h"
#include "Object.h"
#include "Axis.h"
#include "Pyramid.h"
#include "RenderInfo.h"
#include "ComplexGeometry.h"
#include "Triangle.h"
#include "audio.h"
#include "Player.h"
#include "Waves.h"
#include "menu.h"
#include "AnimationState.h"
#include "Light.h"
#include "Trampoline.h"
#include "wing.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctime>
#include <random>
#include <sstream>

class App : public D3DApp {
public:
	App(HINSTANCE hInstance);
	~App();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 
	void setEasyMode();
	void setUpGame(bool menu);

private:
	void buildFX();
	void buildVertexLayouts();
	void updateGameState(float dt);
	void fadeText(std::wstring msg);

	Audio *audio;
	//to zoom the camera in/out
	float zoom;
	//to limit boosting
	float thrust_timer;

	//Lighting
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectScalarVariable* mfxLightType;
	ID3D10EffectVariable* mfxPlayerPos;
	D3DXVECTOR3 mEyePos;

	Light mLight;
	Light pointlights[8];
	//Geometry
	Line line;
	ComplexGeometry box;
	ComplexGeometry bouncerBox;
	ComplexGeometry pillarBox;
	Quad quad;
	Pyramid pyramid;
	Diamond diamond;
	Triangle triangle;
	Waves waves;
	Wing wing;
	Trampoline testTramp;
	Object trampObject;

	//Models
	//ComplexGeometry wing;
	ComplexGeometry sceneryGeometry[NUM_SCENERY];
	ComplexGeometry cliffsGeometry;
	Object scenery[NUM_SCENERY];
	Object cliffs[NUM_CLIFFS];
	Object simpleCliff;
    Object simpleLeftCliff;
    Object simpleRightCliff;
	Object planets[NUM_PLANETS];
	Object stars[NUM_STARS];
	Object rocks[NUM_ROCKS];

	//Objects
	Axis axis;
	Player player;
	Object wavesObject;
	
	Object diamonds[NUM_PILLARS/5];

	std::pair<Object, Object> pWings;
	Object beginningPlatform;
	Object pillars[NUM_PILLARS];
	Object clouds[NUM_CLOUDS];

	RenderInfo ri;
	ID3D10Effect* mFX;
	ID3D10InputLayout* mVertexLayout;

	int atLayer;
	float cameraYBoost;
	float cameraZBoost;

	float mTheta;
	float mPhi;
	float mx, my, mz;

	float diff;

	Menu* mainMenu;
	bool activeMenu;
	bool easyMode;
	bool underwaterShader;

	std::uniform_real_distribution<float> randomScaleDistribution;
	std::mt19937 generator;

	int gameState;
	float elapsedTime;

	bool fadeTextActive;
	std::wstring fadeTextMessage;
	float fadeTextOpacity;
	float fadeTextCurrentDuration;

	bool gameWon;
	bool bPressedLastFrame;
	bool mPressedLastFrame;
	bool rPressedLastFrame;
	bool muted; 
	bool submarine;

	int points;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
/*	// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
*/
	App theApp(hInstance);
	theApp.initApp();
	return theApp.run();
}

App::App(HINSTANCE hInstance):D3DApp(hInstance), mFX(0), mVertexLayout(0), randomScaleDistribution(0.25f, 2.25f), mEyePos(0.0f, 0.0f, 0.0f) {
	ri.mTech = 0;
	ri.mfxWVPVar = 0;
	D3DXMatrixIdentity(&ri.mView);
	D3DXMatrixIdentity(&ri.mProj);

	mTheta = 0;
	mPhi = M_PI*0.33f;
	mx = 0;
	my = 0;
	mz = 0;
	points = 0;
}

App::~App() {
	if( md3dDevice ) md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
}

void App::initApp() {
	diff = 0.0f;
	D3DApp::initApp();
	buildFX();
	buildVertexLayouts();

	muted = false;
	audio = new Audio();
	audio->initialize();
	
	testTramp.init(md3dDevice, RED);
	trampObject.init(&testTramp, D3DXVECTOR3(0, -100, 0));
	trampObject.update(0.0f);

	//audio_timer = 0;
	srand(time(0));
	zoom = 1.0f;
	//Geometry
	line.init(md3dDevice, WHITE);
	//box.init(md3dDevice, WHITE);
	quad.init(md3dDevice, WHITE);
	pyramid.init(md3dDevice, WHITE);
	diamond.init(md3dDevice, WHITE);
	triangle.init(md3dDevice, WHITE);
	wing.init(md3dDevice, GREEN);
	waves.init(md3dDevice, SEA_SIZE + 7, SEA_SIZE + 7, 0.5f, 0.03f, 3.25f, 0.0f);

	//Complex Geometry
	// ---------------------------ANIMATION TEST
	box.init(&quad);
	box.addChild(&quad, Vector3(0, 0, -0.5f), Vector3(ToRadian(90), 0, 0), Vector3(1, 1, 1), 0);
	box.addChild(&quad, Vector3(0, 0, +0.5f), Vector3(ToRadian(-90), 0, 0), Vector3(1, 1, 1), 0);
	box.addChild(&quad, Vector3(-0.5f, 0, 0), Vector3(0, 0, ToRadian(-90)), Vector3(1, 1, 1), 0);
	box.addChild(&quad, Vector3(+0.5f, 0, 0), Vector3(0, 0, ToRadian(90)), Vector3(1, 1, 1), 0);
	box.addChild(&quad, Vector3(0, -0.5f, 0), Vector3(0, ToRadian(180), 0), Vector3(1, 1, 1), 0);
	box.addChild(&quad, Vector3(0, 0.5f, 0), Vector3(0, 0, 0), Vector3(1, 1, 1), 0);

	bouncerBox.init(&box);
	AnimationState* bbani = new AnimationState();
	bbani->addAnimation(Vector3(0, 1, 0), Vector3(0, 0, 0), Vector3(1, 1, 1), Vector3(0, 3, 0), Vector3(0, 0, 0), Vector3(1, 1, 1));
	bbani->addAnimation(Vector3(0, 3, 0), Vector3(0, 0, 0), Vector3(1, 1, 1), Vector3(0, 1, 0), Vector3(0, 0, 0), Vector3(1, 1, 1));
	bouncerBox.addChild(&box, Vector3(0, 1, 0), Vector3(0, 0, 0), Vector3(1, 1, 1), bbani, Vector4(1, 0, 0, 1));
	bouncerBox.setRadius(1.1);

	pillarBox.init(&box);
	pillarBox.addChild(&box, Vector3(0, 0.55f, 0), Vector3(0, 0, 0), Vector3(1.2f, 0.1f, 1.2f), 0);
	pillarBox.setRadius(1.2);

	ComplexGeometry ship;
	ship.init(&line);
	AnimationState* shipAni = new AnimationState();
	AnimationState* sailAni = new AnimationState();
	Vector4 shipColor(.5, .3, .2, 1);
	Vector4 shipColor2(.5, .2, .1, 1);
	Vector4 shipColor3(.6, .4, .3, 1);
	const float SHIP_WIDTH = 5;
	const float SHIP_LENGTH = 10;
	const float SHIP_FRONT_LENGTH = SHIP_LENGTH / 2;
	const float MAST_HEIGHT = 10;
	const float SAIL_WIDTH = SHIP_WIDTH * 1.8;
	const float SAIL_HEIGHT = SHIP_WIDTH * 1.2;
	// deck
	ship.addChild(&quad, Vector3(SHIP_WIDTH / 2, 1, SHIP_LENGTH / 2), Vector3(0, 0, 0), Vector3(SHIP_WIDTH, 1, SHIP_LENGTH), shipAni, shipColor);
	// bottom left
	ship.addChild(&box, Vector3(0, .8, SHIP_LENGTH / 2), Vector3(ToRadian(90+20), ToRadian(-90), 0), Vector3(SHIP_LENGTH, 1, 2), shipAni, shipColor2);
	ship.addChild(&box, Vector3(.55, -.25, SHIP_LENGTH / 2), Vector3(ToRadian(90+40), ToRadian(-90), 0), Vector3(SHIP_LENGTH, 1, 1), shipAni, shipColor2);
	ship.addChild(&box, Vector3(1.1, -.6, SHIP_LENGTH / 2), Vector3(ToRadian(90+80), ToRadian(-90), 0), Vector3(SHIP_LENGTH, 1, 1), shipAni, shipColor2);
	// bottom right
	ship.addChild(&box, Vector3(SHIP_WIDTH - 0, .8, SHIP_LENGTH / 2), Vector3(ToRadian(90-20), ToRadian(-90), 0), Vector3(SHIP_LENGTH, 1, 2), shipAni, shipColor2);
	ship.addChild(&box, Vector3(SHIP_WIDTH - .55, -.25, SHIP_LENGTH / 2), Vector3(ToRadian(90-40), ToRadian(-90), 0), Vector3(SHIP_LENGTH, 1, 1), shipAni, shipColor2);
	ship.addChild(&box, Vector3(SHIP_WIDTH - 1.1, -.6, SHIP_LENGTH / 2), Vector3(ToRadian(90-80), ToRadian(-90), 0), Vector3(SHIP_LENGTH, 1, 1), shipAni, shipColor2);
	// bottom center
	ship.addChild(&box, Vector3(SHIP_WIDTH / 2.0f, -.2, SHIP_LENGTH / 2), Vector3(0, ToRadian(-90), 0), Vector3(SHIP_LENGTH, 2, 2), shipAni, shipColor2);
	// forward deck left
	ship.addChild(&triangle, Vector3(SHIP_WIDTH / 4, 1, SHIP_LENGTH), Vector3(0, ToRadian(-90), 0), Vector3(SHIP_FRONT_LENGTH / 2, 1, SHIP_WIDTH / 4), shipAni, shipColor);
	ship.addChild(&triangle, Vector3(2 * SHIP_WIDTH / 4, 1, SHIP_LENGTH + SHIP_FRONT_LENGTH / 2), Vector3(0, ToRadian(-90), 0), Vector3(SHIP_FRONT_LENGTH / 4, 1, SHIP_WIDTH / 4), shipAni, shipColor);
	// forward deck right
	ship.addChild(&triangle, Vector3(SHIP_WIDTH - SHIP_WIDTH / 4, 1, SHIP_LENGTH), Vector3(0, ToRadian(-90), ToRadian(180)), Vector3(SHIP_FRONT_LENGTH / 2, 1, SHIP_WIDTH / 4), shipAni, shipColor);
	ship.addChild(&triangle, Vector3(SHIP_WIDTH - (2 * SHIP_WIDTH / 4), 1, SHIP_LENGTH + SHIP_FRONT_LENGTH / 2), Vector3(0, ToRadian(-90), ToRadian(180)), Vector3(SHIP_FRONT_LENGTH / 4, 1, SHIP_WIDTH / 4), shipAni, shipColor);
	// forward deck center
	ship.addChild(&quad, Vector3(SHIP_WIDTH / 2, 1, SHIP_LENGTH + SHIP_FRONT_LENGTH / 4), Vector3(0, ToRadian(-90), ToRadian(180)), Vector3(SHIP_FRONT_LENGTH / 2, 1, SHIP_WIDTH / 2), shipAni, shipColor);
	// forward long thing
	ship.addChild(&box, Vector3(SHIP_WIDTH / 2.0f, .745, SHIP_LENGTH + SHIP_FRONT_LENGTH / 1.5), Vector3(0, ToRadian(-90), 0), Vector3(SHIP_FRONT_LENGTH * 1.5, .5, .5), shipAni, shipColor2);
	// rear
	ship.addChild(&box, Vector3(SHIP_WIDTH / 2, .7, .249), Vector3(0, ToRadian(-90), ToRadian(180)), Vector3(.5, 1.7, SHIP_WIDTH * .9), shipAni, shipColor2);
	// mast
	ship.addChild(&box, Vector3(SHIP_WIDTH / 2, MAST_HEIGHT / 2 + 1, SHIP_LENGTH / 2), Vector3(0, 0, 0), Vector3(.25, MAST_HEIGHT, .25), shipAni, shipColor3);
	// sail
	sailAni->addAnimation(Vector3(0, 0, 0), Vector3(ToRadian(10), 0, 0), Vector3(1, 1, 1), Vector3(1, 0, 0), Vector3(ToRadian(-10), 0, 0), Vector3(1, 1, 1));
	sailAni->addAnimation(Vector3(1, 0, 0), Vector3(ToRadian(-10), 0, 0), Vector3(1, 1, 1), Vector3(0, 0, 0), Vector3(ToRadian(10), 0, 0), Vector3(1, 1, 1));
	ship.addChild(&quad, Vector3(SHIP_WIDTH / 2, MAST_HEIGHT * .8 + 1, SHIP_LENGTH / 2 + .25), Vector3(ToRadian(90), 0, 0), Vector3(SAIL_WIDTH, 0, SAIL_HEIGHT), sailAni, Vector4(1, 1, 1, 1));
	sceneryGeometry[0] = ship;
	sceneryGeometry[1] = ship;

	cliffsGeometry.init(&line);
	Vector4 cliffsColor(.3f, .3f, .3f, 1);
	Vector4 cliffsColor2(0.0f, 92.0f / 255.0f, 9.0f / 255.0f, 1);
	AnimationState* cliffsAni = new AnimationState();
	cliffsGeometry.addChild(&box, Vector3(0, 0, 0), Vector3(ToRadian(90), 0, ToRadian(90)), Vector3(CLIFF_HEIGHT, 1, 1), cliffsAni, cliffsColor);
	cliffsGeometry.addChild(&box, Vector3(.7, 0, .2), Vector3(ToRadian(60), 0, ToRadian(90)), Vector3(CLIFF_HEIGHT, 1, 1), cliffsAni, cliffsColor);
	cliffsGeometry.addChild(&box, Vector3(.95, 0, .4), Vector3(ToRadian(120), 0, ToRadian(90)), Vector3(CLIFF_HEIGHT, 1, 1), cliffsAni, cliffsColor);
	cliffsGeometry.addChild(&box, Vector3(1.4, 0, .3), Vector3(ToRadian(100), 0, ToRadian(90)), Vector3(CLIFF_HEIGHT, 1, 1), cliffsAni, cliffsColor);
	cliffsGeometry.addChild(&box, Vector3(2.2, 0, .2), Vector3(ToRadian(95), 0, ToRadian(90)), Vector3(CLIFF_HEIGHT, 1, 1), cliffsAni, cliffsColor);
	cliffsGeometry.addChild(&box, Vector3(2.9, 0, .2), Vector3(ToRadian(80), 0, ToRadian(90)), Vector3(CLIFF_HEIGHT, 1, 1), cliffsAni, cliffsColor);
	cliffsGeometry.addChild(&box, Vector3(0, CLIFF_HEIGHT / 2 + .25/2, 0), Vector3(ToRadian(90), 0, ToRadian(90)), Vector3(.25, .25, CLIFF_WIDTH), cliffsAni, cliffsColor2);

	//Objects
	axis.init(&line);
	player.init(&box, Vector3(0, 3, 0));
	//player.setColor(0.5f, 0.9f, 0.4f, 1.0f);
	player.setColor(0, 0, 0, 1);
	player.setRotation(Vector3(0, -90 * M_PI / 180, 0));
	player.setScale(Vector3(0.5, 0.5, 0.5));
	wavesObject.init(&waves, Vector3(0, -.5, SEA_SIZE / 8));
	wavesObject.setColor(9.0f / 255.0f, 72.0f / 255.0f, 105.0f / 255.0f, 1);
	wavesObject.setVelocity(Vector3(0, WATER_RISE_SPEED, 0));

	for (int i = 0; i < NUM_DIAMONDS; i++)
	{
		diamonds[i].init(&diamond, Vector3(0, -100, 1.0f * (GAME_DEPTH + GAME_BEHIND_DEPTH) / NUM_DIAMONDS*i));
		diamonds[i].setScale(Vector3(DIAMOND_SIZE, rand() % 2 + DIAMOND_HEIGHT_START * 2, DIAMOND_SIZE));
		diamonds[i].setVelocity(Vector3(0, 0, DIAMOND_SPEED));
		diamonds[i].setColor(1,1,0, 0.5);
	}


	pWings.first.init(&wing, player.getPosition());
	pWings.second.init(&wing, player.getPosition());
	pWings.second.setRotation(Vector3(0, 0, PI));
	pWings.first.setScale(Vector3(0.4, 0.4, 0.4));
	pWings.second.setScale(Vector3(0.4, 0.4, 0.4));
	

	for (int i = 0; i < NUM_PILLARS; i++)
	{
		pillars[i].init(&pillarBox, Vector3(0, -100, 1.0f * (GAME_DEPTH + GAME_BEHIND_DEPTH) / NUM_PILLARS*i));
		pillars[i].setScale(Vector3(PILLAR_SIZE, rand() % 2 + PILLAR_HEIGHT_START * 2, PILLAR_SIZE));
		pillars[i].setVelocity(Vector3(0, 0, PILLAR_SPEED));
		pillars[i].setColor(181.0f / 255.0f, 152.0f / 255.0f, 108.0f / 255.0f, 1);
	}
	trampObject.init(&testTramp, Vector3(0, LAYER_HEIGHT[0]-100, 1.0f * (GAME_DEPTH + GAME_BEHIND_DEPTH) / NUM_PILLARS*3));
    trampObject.setScale(Vector3(1,1,1));
	trampObject.setColor(234.0f/255.0f, 228.0f/255.0f, 71.0f/255.0f, 1);
    trampObject.setVelocity(Vector3(0, 0, PILLAR_SPEED));
    trampObject.setInActive();
    trampObject.update(0.0f);

	/*
	trampObject.init(&testTramp, Vector3(0, LAYER_HEIGHT[0]+5, 1.0f * (GAME_DEPTH + GAME_BEHIND_DEPTH) / NUM_PILLARS*3));
    trampObject.setScale(Vector3(1,1,1));
    trampObject.setVelocity(Vector3(0, 0, PILLAR_SPEED));
    trampObject.setInActive();
    trampObject.update(0.0f);*/

		 
	beginningPlatform.init(&box, Vector3(0, .5, GAME_DEPTH * .4));
	beginningPlatform.setScale(Vector3(10, PILLAR_HEIGHT_START, GAME_DEPTH * 1.5));
	beginningPlatform.setVelocity(Vector3(0, 0, PILLAR_SPEED));
	beginningPlatform.setColor(191.0f / 255.0f, 162.0f / 255.0f, 118.0f / 255.0f, 1);

	for (int i = 0; i < NUM_CLOUDS; i++)
	{
		clouds[i].init(&pillarBox, Vector3(0, -100, 1.0f * (GAME_DEPTH + GAME_BEHIND_DEPTH) / NUM_CLOUDS*i));
		clouds[i].setScale(Vector3(CLOUD_SIZE, 1, CLOUD_SIZE));
		clouds[i].setVelocity(Vector3(CLOUD_X_SPEED, 0, CLOUD_SPEED));
		clouds[i].setColor(.8, .8, .8, 1);
	}

	for (int i = 0; i < NUM_PLANETS; i++)
	{
		planets[i].init(&pillarBox, Vector3(0, -100, 1.0f * (GAME_DEPTH + GAME_BEHIND_DEPTH) / NUM_PLANETS*i));
		planets[i].setScale(Vector3(PLANET_SIZE, 1, PLANET_SIZE));
		planets[i].setVelocity(Vector3(PLANET_X_SPEED, 0, PLANET_SPEED));
		planets[i].setColor(1, 1, 1, 1);
	}

	for (int i = 0; i < NUM_ROCKS; i++)
	{
		rocks[i].init(&pillarBox, Vector3(0, LAYER_HEIGHT[3], 1.0f * (GAME_DEPTH + GAME_BEHIND_DEPTH) / NUM_ROCKS*i));
		rocks[i].setScale(Vector3(ROCK_SIZE, 1, ROCK_SIZE));
		rocks[i].setVelocity(Vector3(ROCK_X_SPEED, 0, ROCK_SPEED));
		rocks[i].setColor(.4, .4, .4, 1);
	}

	for (int i = 0; i < NUM_SCENERY; i++)
	{
		int lr = rand() % 2;
		int xpos;

		if (lr == 1)
			xpos = -GAME_WIDTH / 2 - 7 - rand() % 5;
		else
			xpos =  GAME_WIDTH / 2 + 7 + rand() % 5;

		scenery[i].init(&sceneryGeometry[i], Vector3(xpos, 1, 1.0f * (GAME_DEPTH + GAME_BEHIND_DEPTH) / NUM_SCENERY*i));
		scenery[i].setVelocity(Vector3(0, 0, -1));
	}

	for (int i = 0; i < NUM_CLIFFS; i++)
	{
		cliffs[i].init(&cliffsGeometry, Vector3(CLIFF_WIDTH * i - NUM_CLIFFS * CLIFF_WIDTH / 2, CLIFF_HEIGHT / 2, 40));
	}

	simpleCliff.init(&box, Vector3(0, CLIFF_HEIGHT / 2, 53));
    simpleCliff.setScale(Vector3(70, CLIFF_HEIGHT, 2));
    simpleCliff.setColor(cliffsColor.x, cliffsColor.y, cliffsColor.z, 1);
    
    simpleLeftCliff.init(&box, Vector3(-36, CLIFF_HEIGHT / 2, 20));
    simpleLeftCliff.setScale(Vector3(2, CLIFF_HEIGHT, 68));
    simpleLeftCliff.setColor(cliffsColor.x, cliffsColor.y, cliffsColor.z, 1);

    simpleRightCliff.init(&box, Vector3(36, CLIFF_HEIGHT / 2, 20));
    simpleRightCliff.setScale(Vector3(2, CLIFF_HEIGHT, 68));
    simpleRightCliff.setColor(cliffsColor.x, cliffsColor.y, cliffsColor.z, 1);


	for (int i = 0; i < NUM_STARS; i++)
	{
		float theta = (float)rand() / (float)RAND_MAX * 3.14;
		float phi = (float)rand() / (float)RAND_MAX * 3.14;
		float radius = 80;
		if (rand() % 2 == 0)
			theta *= -1;
		if (rand() % 2 == 0)
			phi *= -1;
		float x= radius * cos(theta) * sin(phi);
		float y = radius * sin(theta) * sin(phi);
		float z = radius * cos(phi);
		z = fabs(z);
		stars[i].init(&box, Vector3(x, y, z));
		stars[i].setColor(1, 1, 1, 1);
		stars[i].setScale(Vector3(.1, .1, .1));
	}

	// create initial disturbance of waves
	// the waves are set to never dampen so this will create waves that last forever
	for (int x = 0; x < SEA_SIZE / 20; x++)
	{

		DWORD i = SEA_SIZE - 10;
		DWORD j = 5 + rand() % SEA_SIZE;

		float r = RandF(1.5f, 2.0f);

		waves.disturb(i, j, r);
	}

	activeMenu = false;
	mainMenu = new Menu();
	
	mainMenu->initialize(md3dDevice, NULL);

	//mainMenu->setMenuHeading("Dunstan's Big Bad Bounce-Around");
	mainMenu->setMenuHeading("");

	std::vector<std::string> menuItems;
	menuItems.push_back("Play");	// Menu 1
	menuItems.push_back("Play (easy)");	// Menu 2
	mainMenu->setMenuItems(menuItems);

	//Light
	mLight.ambient = D3DXCOLOR(0.6f, 0.6f, 0.6f, 0.6f);
	mLight.diffuse = D3DXCOLOR(0.4f, 0.4f, 0.4f, 0.4f);
	mLight.specular = D3DXCOLOR(0.6f, 0.6f, 0.6f, 0.6f);
	mLight.att.x    = 1.0f;
	mLight.att.y    = 0.0f;
	mLight.att.z    = 0.0f;
	mLight.spotPow  = 64.0f;
	mLight.range    = 1000.0f;
	mLight.pos = Vector3(0.0f, 15.0f, 1.0f);
	mLight.dir = Vector3(0.0f, -1.0f, 3.0f);
	
	/*for(int i = 0; i < 8; i++){
		pointlights[i].ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 0.2f);
		pointlights[i].diffuse = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.1f);
		pointlights[i].specular = D3DXCOLOR(0.7f, 0.7f, 0.7f, 0.7f);
		pointlights[i].att.x    = 1.0f;
		pointlights[i].att.y    = 0.0f;
		pointlights[i].att.z    = 0.0f;
		pointlights[i].spotPow  = 64.0f;
		pointlights[i].range    = 10000.0f;
		pointlights[i].pos = Vector3(15.0f * i, 0, 0);
		pointlights[i].dir = Vector3(0.0f, 1.0f, 0.0f);
	}*/

	setUpGame(true);
}

void App::onResize() {
	D3DApp::onResize();
	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&ri.mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void App::updateScene(float dt) {
	D3DApp::updateScene(dt);
	updateGameState(dt);
	if(elapsedTime >= 00.0f) {
        trampObject.setActive();
    } else {
        trampObject.setInActive();
    }

	switch(gameState) {
	case MENU:
		if (GetAsyncKeyState('M'))
			mPressedLastFrame = true;
		else
		{
			if (mPressedLastFrame)
			{
				muted = !muted;
				if (muted == true)
				{
					audio->stopCue("music");
					audio->stopCue("musicLayer2");
					audio->stopCue("musicLayer3");
					audio->stopCue("waterstream");
					audio->stopCue("winnermusic");
				}
				else
				{
					if (!gameWon)
					{
						if (atLayer < 2)
							audio->playCue("music");
						else if (atLayer == 2)
							audio->playCue("musiclayer2");
						else if (atLayer == 3)
							audio->playCue("musiclayer3");
					}
					else
					{
						audio->playCue("winnermusic");
					}
				}
			}

			mPressedLastFrame = false;
		}
		mainMenu->update();
		break;
	case GAME_OVER:
	case LEVEL1:
		{

		// Every quarter second, generate a random wave.
		/*static float t_base = 0.0f;
		if ((mTimer.getGameTime() - t_base) >= 0.25f)
		{
			t_base += 0.25f;

			DWORD i = 5 + rand() % 250;
			DWORD j = 5 + rand() % 250;

			float r = RandF(1.0f, 2.0f);

			waves.disturb(i, j, r);
		}*/

		if (GetAsyncKeyState(VK_ESCAPE)) exit(0);
		
		if (GetAsyncKeyState('P')) submarine = true;

		if (GetAsyncKeyState('M'))
			mPressedLastFrame = true;
		else
		{
			if (mPressedLastFrame)
			{
				muted = !muted;
				if (muted)
				{
					audio->stopCue("music");
					audio->stopCue("musicLayer2");
					audio->stopCue("musicLayer3");
					audio->stopCue("waterstream");
				}
				else
				{
					if (atLayer < 2)
						audio->playCue("music");
					else if (atLayer == 2)
						audio->playCue("musiclayer2");
					else if (atLayer == 3)
						audio->playCue("musiclayer3");
				}
			}

			mPressedLastFrame = false;
		}
		if (GetAsyncKeyState('R'))
			rPressedLastFrame = true;
		else
		{
			if (rPressedLastFrame)
			{
				setUpGame(false);
			}

			rPressedLastFrame = false;
		}
		if(gameState != GAME_OVER && !gameWon) {
			//Thrust with a timer of a 1:2 thrust to cooldown ratio and a 2 second starting thrust bank.
			if (GetAsyncKeyState(VK_SPACE)){ 
		
				if(thrust_timer < 1.0f){
					thrust_timer+=dt;
					player.thrustUp(dt);
				}
	
			}else if(thrust_timer > 0){
       			  thrust_timer -= 0.5*dt;

			}
			if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A')){
				player.accelLeft(dt);
				pWings.first.setRotation(Vector3(0 , 0, PI/6));
				pWings.second.setRotation(Vector3(0 , 0, PI+PI/6));
			}
			else if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D')){
				pWings.first.setRotation(Vector3(0 , 0, -PI/6));
				pWings.second.setRotation(Vector3(0 , 0, PI-PI/6));
				player.accelRight(dt);
			}
			else{
				pWings.first.setRotation(Vector3(0 , 0, 0));
				pWings.second.setRotation(Vector3(0 , 0, PI));
			}
			if (!GetAsyncKeyState(VK_LEFT) && !GetAsyncKeyState(VK_RIGHT) && !GetAsyncKeyState('A') && !GetAsyncKeyState('D')) player.decelX(dt);
			if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W'))
			{
				if (player.gliding == false && !muted)
					audio->playCue("glide");
				player.setGliding(true);
			}
			else
			{
				audio->stopCue("glide");
				player.setGliding(false);
			}
			if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S'))
			{
				if (player.diving == false && !muted)
					audio->playCue("dive");

				player.setDiving(true);
			}
			else
			{
				audio->stopCue("dive");
				player.setDiving(false);
			}

			bool hitTramp = false;

			Vector3 pos1 = player.getPosition();
			Vector3 scale1 = player.getScale();
			Vector3 pos2 = trampObject.getPosition();
			Vector3 scale2 = trampObject.getScale() * 2.5;
			if (pos1.x + scale1.x / 2 > pos2.x - scale2.x &&
				pos1.x - scale1.x / 2 < pos2.x + scale2.x &&
				pos1.y + scale1.y / 2 > pos2.y - scale2.y / 2 &&
				pos1.y - scale1.y / 2 < pos2.y + scale2.y / 2 &&
				pos1.z - scale1.z / 2 > pos2.z - scale2.z &&
				pos1.z + scale1.z / 2 < pos2.z + scale2.z)
				hitTramp = true;
				
			if ((!GetAsyncKeyState('B') && bPressedLastFrame) || player.collided(&trampObject) || hitTramp) {
				if (!muted)
					audio->playCue("boing");
				elapsedTime = 0;

				if (atLayer == 0)
				{
					fadeText(LAYER_NAMES[1]);
					int x = rand() % GAME_WIDTH - GAME_WIDTH / 2;
                    trampObject.setInActive();
                    trampObject.setPosition(Vector3(x, LAYER_HEIGHT[1] + 2, GAME_DEPTH));

					player.setVelocity(Vector3(0, 18, 0));
					atLayer = 1;
				} else
				if (atLayer == 1)
				{
					fadeText(LAYER_NAMES[2]);
					int x = rand() % GAME_WIDTH - GAME_WIDTH / 2;
                    trampObject.setInActive();
                    trampObject.setPosition(Vector3(x, LAYER_HEIGHT[2] + 2, GAME_DEPTH));

					player.setVelocity(Vector3(0, 27, 0));
					atLayer = 2;
					if (!muted)
					{
						audio->stopCue("music");
						audio->playCue("musiclayer2");
					}
				} else
					if (atLayer == 2) {
					submarine = true;
					trampObject.setInActive();
					int x = rand() % GAME_WIDTH - GAME_WIDTH / 2;
					trampObject.setPosition(Vector3(x, LAYER_HEIGHT[3] + 2, GAME_DEPTH));
					for (int x = 0; x < 20; x++)
					{
						DWORD i = 15;
						DWORD j = 5 + rand() % SEA_SIZE;

						float r = RandF(2.0f, 2.2f);

						waves.disturb(i, j, r);
					}
					if (!muted)
						audio->playCue("waterstream");

					player.setVelocity(Vector3(player.getVelocity().x, PLAYER_BOUNCE_FORCE, player.getVelocity().z));
                } else
					if (atLayer == 3 && player.collided(&trampObject) || hitTramp) {
					gameWon = true;

					if (!muted)
					{
						audio->stopCue("waterstream");
						audio->stopCue("music");
						audio->stopCue("musiclayer2");
						audio->stopCue("musiclayer3");
						audio->playCue("winnermusic");
					}
				}

			}

				if (GetAsyncKeyState('B'))
					bPressedLastFrame = true;
				else
					bPressedLastFrame = false;
		}

		Vector3 oldPlayerPosition = player.getPosition();
		if(gameState != GAME_OVER) {
			player.update(dt);
		}
		if (gameWon)
			player.setVelocity(Vector3(0, 20, 0));

		pWings.first.setPosition(player.getPosition()+Vector3(0.0f, 0.0f, 1.0f));
		pWings.second.setPosition(player.getPosition()+Vector3(0.0f, 0.0f, 1.0f));
	
		pWings.first.update(dt);
		pWings.second.update(dt);

		waves.update(dt);
		wavesObject.update(dt);
        trampObject.update(dt);

		if (atLayer == 0 && player.getPosition().y < LAYER_HEIGHT[atLayer] - 2 && submarine)
		{
			if (!muted)
			{
				audio->stopCue("music");
				audio->stopCue("waterstream");
				audio->playCue("musiclayer3");
				audio->playCue("splash");
				audio->playCue("whoosh");
			}

			atLayer = 3;
			underwaterShader = true;
			buildFX();
			fadeText(LAYER_NAMES[atLayer]);
		}

		if (player.getPosition().y < LAYER_HEIGHT[atLayer] - 2 && player.getVelocity().y < 0 && atLayer != 0 && atLayer != 3)
		{
			underwaterShader = false;
			buildFX();
			atLayer--;

			if (!muted)
			{
				audio->playCue("whoosh");
			}

			fadeText(LAYER_NAMES[atLayer]);

			if (atLayer == 1 && !muted)
			{
				audio->playCue("music");
				audio->stopCue("musiclayer2");
			}
		}

		if (cameraYBoost < LAYER_HEIGHT[atLayer] + 4)
			cameraYBoost += CAMERA_MOVE_SPEED * dt;	

		//if (cameraZBoost < 10 && atLayer >= 1)
		//	cameraZBoost += CAMERA_MOVE_SPEED * dt;

		if (cameraYBoost > LAYER_HEIGHT[atLayer] + 4)
			cameraYBoost -= CAMERA_MOVE_SPEED * dt;

		//if (cameraZBoost > 0 && atLayer < 1)
		//	cameraZBoost -= CAMERA_MOVE_SPEED * dt;
			

		beginningPlatform.update(dt);
		if (beginningPlatform.getPosition().y < .8)
			beginningPlatform.setVelocity(Vector3(beginningPlatform.getVelocity().x, 1, beginningPlatform.getVelocity().z));
		else
			beginningPlatform.setVelocity(Vector3(beginningPlatform.getVelocity().x, 0, beginningPlatform.getVelocity().z));

		for (int i = 0; i < NUM_DIAMONDS; i++)
		{
			diamond.increaseRotation(1);

			//diamonds[i].setRotation(Vector3(0, diamond.getRotation() * M_PI / 180, 0));//Spinning effect diamond.getRotation() * M_PI / 180

			diamonds[i].setRotation(Vector3(0, diamond.getRotation() * M_PI / 180*dt, 0));//Spinning effect diamond.getRotation() * M_PI / 180

			
			if ((atLayer == 0 && diamonds[i].getPosition().y < LAYER_HEIGHT[0] + 4) || 
				(atLayer == 1 && diamonds[i].getPosition().y < LAYER_HEIGHT[1] + 4) ||
				(atLayer == 2 && diamonds[i].getPosition().y < LAYER_HEIGHT[2] + 34))
				diamonds[i].setVelocity(Vector3(diamonds[i].getVelocity().x, 100, diamonds[i].getVelocity().z));
			else
				{
					diamonds[i].setVelocity(Vector3(diamonds[i].getVelocity().x, 0, diamonds[i].getVelocity().z));
				}

			if (diamonds[i].getPosition().z < -2)
				diamonds[i].setVelocity(Vector3(diamonds[i].getVelocity().x, -1, diamonds[i].getVelocity().z));

			diamonds[i].update(dt);

			if (diamonds[i].getPosition().z < -GAME_BEHIND_DEPTH)
			{
				int x = rand() % GAME_WIDTH - GAME_WIDTH / 2;
				diamonds[i].setPosition(Vector3(x, 0 - PILLAR_HEIGHT_START, GAME_DEPTH));
			}
		}

		for (int i = 0; i < NUM_PILLARS; i++)
		{
			if (pillars[i].getPosition().y < 0)
				pillars[i].setVelocity(Vector3(pillars[i].getVelocity().x, 1, pillars[i].getVelocity().z));
			else
				pillars[i].setVelocity(Vector3(pillars[i].getVelocity().x, 0, pillars[i].getVelocity().z));

			if (pillars[i].getPosition().z < -2)
				pillars[i].setVelocity(Vector3(pillars[i].getVelocity().x, -1, pillars[i].getVelocity().z));

			pillars[i].update(dt);

			if (pillars[i].getPosition().z < -GAME_BEHIND_DEPTH && !submarine)
			{
				int x = rand() % GAME_WIDTH - GAME_WIDTH / 2;
				pillars[i].setPosition(Vector3(x, 0 - PILLAR_HEIGHT_START, GAME_DEPTH));
			}
		}

		if (trampObject.getPosition().z < -GAME_BEHIND_DEPTH) {
			int x = rand() % GAME_WIDTH - GAME_WIDTH / 2;
			trampObject.setPosition(Vector3(x, LAYER_HEIGHT[atLayer] + 2, GAME_DEPTH));
		}

		for (int i = 0; i < NUM_CLOUDS; i++)
		{
			clouds[i].update(dt);

			if (clouds[i].getPosition().z < -GAME_BEHIND_DEPTH && !submarine)
			{
				int x = rand() % GAME_WIDTH - GAME_WIDTH / 2;
				clouds[i].setPosition(Vector3(x - CLOUD_X_SPEED * 5, LAYER_HEIGHT[1], GAME_DEPTH));
			}
		}

		for (int i = 0; i < NUM_PLANETS; i++)
		{
			planets[i].update(dt);

			if (planets[i].getPosition().z < -GAME_BEHIND_DEPTH && !submarine)
			{
				int r = rand() % 2;

				int x = rand() % GAME_WIDTH - GAME_WIDTH / 2;

				if (r == 0)
				{
					planets[i].setPosition(Vector3(x - PLANET_X_SPEED * 5, LAYER_HEIGHT[2], GAME_DEPTH));
					planets[i].setVelocity(Vector3(PLANET_X_SPEED, 0, planets[i].getVelocity().z));
				}
				else
				{
					planets[i].setPosition(Vector3(x + PLANET_X_SPEED * 5, LAYER_HEIGHT[2], GAME_DEPTH));
					planets[i].setVelocity(Vector3(-PLANET_X_SPEED, 0, planets[i].getVelocity().z));
				}
			}
		}

		for (int i = 0; i < NUM_ROCKS; i++)
		{
			rocks[i].update(dt);
			rocks[i].setRotation(rocks[i].getRotation() + Vector3(0, 0.1 * i/2 * dt, 0));

			if (rocks[i].getPosition().z < -GAME_BEHIND_DEPTH)
			{
				int r = rand() % 2;

				int x = rand() % ROCKS_WIDTH - ROCKS_WIDTH / 2;

				if (r == 0)
				{
					rocks[i].setPosition(Vector3(x - ROCK_X_SPEED * 5, LAYER_HEIGHT[3], GAME_DEPTH));
					rocks[i].setVelocity(Vector3(ROCK_X_SPEED, 0, rocks[i].getVelocity().z));
				}
				else
				{
					rocks[i].setPosition(Vector3(x + ROCK_X_SPEED * 5, LAYER_HEIGHT[3], GAME_DEPTH));
					rocks[i].setVelocity(Vector3(-ROCK_X_SPEED, 0, rocks[i].getVelocity().z));
				}


			}
		}

		for (int i = 0; i < NUM_SCENERY; i++)
		{
			scenery[i].update(dt);
			if (scenery[i].getPosition().z < -20)
			{
				int lr = rand() % 2;
				int xpos;

				if (lr == 1)
					xpos = -GAME_WIDTH / 2 - 7 - rand() % 5;
				else
					xpos =  GAME_WIDTH / 2 + 7 + rand() % 5;

				scenery[i].setPosition(Vector3(xpos, 1, GAME_DEPTH * 1.1));
			}
			if (scenery[i].getPosition().y < 0)
				scenery[i].setVelocity(Vector3(scenery[i].getVelocity().x, 1, scenery[i].getVelocity().z));
			else
				scenery[i].setVelocity(Vector3(scenery[i].getVelocity().x, 0, scenery[i].getVelocity().z));
		}

		//for (int i = 0; i < NUM_CLIFFS; i++)
			//cliffs[i].update(dt);

		simpleCliff.update(dt);
        simpleLeftCliff.update(dt);
        simpleRightCliff.update(dt);


		for (int i = 0; i < NUM_STARS; i++)
		{
			stars[i].update(dt);
		}

		/* bottom collision, temp */ 
		//if (player.getPosition().y - player.getScale().y < wavesObject.getPosition().y - 1)
		//{
		//	player.setVelocity(Vector3(player.getVelocity().x, PLAYER_BOUNCE_FORCE, player.getVelocity().z));

		//	player.setPosition(oldPlayerPosition);
		//}

		// collision
		for (int i = 0; i < NUM_DIAMONDS; i++)
			{
				if (player.collided(&diamonds[i]) && diamonds[i].getActiveState())
				{
					diamonds[i].setInActive();
					points += 200;
					//audio->playCue("splash");
				}
			}

		bool playerBounced = false;

		if (atLayer == 0)
			for (int i = 0; i < NUM_SCENERY; i++)
				if (player.collided(&scenery[i]))
					if (abs((oldPlayerPosition.y - player.getScale().y / 2) + .1 > pillars[i].getPosition().y + pillars[i].getScale().y / 2))
						playerBounced = true;

		if (atLayer == 0)
			for (int i = 0; i < NUM_PILLARS; i++)
				if (player.collided(&pillars[i]))
					if (abs((oldPlayerPosition.y - player.getScale().y / 2) + .1 > pillars[i].getPosition().y + pillars[i].getScale().y / 2))
						playerBounced = true;

		if (atLayer == 1)
			for (int i = 0; i < NUM_CLOUDS; i++)
				if (player.collided(&clouds[i]))
					playerBounced = true;

		if (atLayer == 2)
			for (int i = 0; i < NUM_PLANETS; i++)
				if (player.collided(&planets[i]))
					playerBounced = true;

		if (atLayer == 3)
			for (int i = 0; i < NUM_ROCKS; i++)
				if (player.collided(&rocks[i]))
					playerBounced = true;

		if (player.collided(&beginningPlatform))
			playerBounced = true;

		if (playerBounced)
		{
			player.setPosition(oldPlayerPosition + Vector3(0, 0.1, 0));
			player.setVelocity(Vector3(player.getVelocity().x, PLAYER_BOUNCE_FORCE, player.getVelocity().z));
			points += 10 * (atLayer + 1);
			if (!muted)
			{
				int randAudio = rand() % 2;
				if (randAudio == 0)
					audio->playCue("bounce");
				else
					audio->playCue("bounce2");
			}
		}

		/* don't let the player go too fast */

		if (player.getVelocity().y < Y_VELOCITY_LIMIT && !player.diving && atLayer != 3 || (player.getVelocity().y < Y_VELOCITY_LIMIT / 3 && !player.diving && atLayer == 3))
			player.setVelocity(Vector3(player.getVelocity().x, player.getVelocity().y * .99, player.getVelocity().z));
			

		// Update angles based on input to orbit camera around box.
		//if(GetAsyncKeyState('A') & 0x8000)	mTheta -= 2.0f*dt;
		//if(GetAsyncKeyState('D') & 0x8000)	mTheta += 2.0f*dt;
		//if(GetAsyncKeyState('W') & 0x8000)	mPhi -= 2.0f*dt;
		//if(GetAsyncKeyState('S') & 0x8000)	mPhi += 2.0f*dt;

		// Restrict the angle mPhi.
		if( mPhi < 0.1f )	mPhi = 0.1f;
		if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

		// Convert Spherical to Cartesian coordinates: mPhi measured from +y
		// and mTheta measured counterclockwise from -z.
		float d = 4;
		mEyePos.x =  5.0f*sinf(mPhi)*sinf(mTheta) * d;
		mEyePos.z = -5.0f*sinf(mPhi)*cosf(mTheta) * d + cameraZBoost;
		mEyePos.y = 5.0f*cosf(mPhi) * d + cameraYBoost;
		//mEyePos.z = player.getPosition().z - 10 + cameraZBoost;
		//mEyePos.y =  player.getPosition().y + 2 + cameraYBoost;

	
		// Build the view matrix.
		/*D3DXVECTOR3 pos(x, y, z);
		//zoom out when thrusting
		if (player.getVelocity().y > 0){
			zoom += 0.02 * player.getVelocity().y * dt;
			pos = pos * zoom;
		}
		else if(zoom > 1){
			zoom += 0.02 * player.getVelocity().y * dt;
			pos = pos * zoom;
		}
		else{
			zoom = 1.0f;
		}*/
		//D3DXVECTOR3 target(player.getPosition());


		//D3DXVECTOR3 target(Vector3(0.0f, 0.0f + cameraYBoost*.7, 0.0f + cameraZBoost*atLayer));
		D3DXVECTOR3 up(0, 1, 0);

		mEyePos.x = player.getPosition().x;
		mEyePos.y = cameraYBoost + 5;
		mEyePos.z = player.getPosition().z - 8;

		D3DXVECTOR3 target;
		target.x = player.getPosition().x;
		target.y = cameraYBoost - 5;
		target.z = player.getPosition().z + 10;

		if (fadeTextActive)
		{
			fadeTextCurrentDuration += dt;
			if (fadeTextCurrentDuration < FADE_TEXT_FADE_SPEED)
				fadeTextOpacity = fadeTextCurrentDuration / FADE_TEXT_FADE_SPEED;
			if (fadeTextCurrentDuration > FADE_TEXT_FADE_SPEED + FADE_TEXT_DURATION)
				fadeTextOpacity = 1 - (fadeTextCurrentDuration - FADE_TEXT_DURATION - FADE_TEXT_FADE_SPEED) / FADE_TEXT_FADE_SPEED;
			if (fadeTextCurrentDuration > 2*FADE_TEXT_FADE_SPEED + FADE_TEXT_DURATION)
				fadeTextActive = false;
		}
	
		diff = dt;
		D3DXMatrixLookAtLH(&ri.mView, &mEyePos, &target, &up);
		break;
		}
	case LEVEL2:
		{
		break;
		}
	}
}

void App::drawScene() {
	switch(gameState) {
	case MENU:
		mainMenu->displayMenu(diff);
		break;
	case GAME_OVER:
	case LEVEL1:
	case LEVEL2:
		{
		D3DApp::drawScene();
		mClearColor = D3DXCOLOR(.419f, .482f, .64f, 1.0f);
		if (atLayer == 2)
			mClearColor = D3DXCOLOR(.15, .15, .15, 1);

		// Restore default states, input layout and primitive topology 
		// because mFont->DrawText changes them.  Note that we can 
		// restore the default states by passing null.
		md3dDevice->OMSetDepthStencilState(0, 0);
		float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
		md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
		md3dDevice->IASetInputLayout(mVertexLayout);

		//Set Lighting
		mfxEyePosVar->SetRawValue(&mEyePos, 0, sizeof(D3DXVECTOR3));
		mfxLightVar->SetRawValue(&mLight, 0, sizeof(Light));
		mfxLightType->SetInt(0);
		mfxPlayerPos->SetRawValue(&player.getPosition(), 0, sizeof(D3DXVECTOR3));

		//Draw Axis
		//axis.draw(&ri);

		 

		//Draw objects
		pWings.first.draw(&ri);
		pWings.second.draw(&ri);
		player.draw(&ri);

		for (int i = 0; i < NUM_DIAMONDS; i++)
			diamonds[i].draw(&ri);

		if (player.getPosition().y < LAYER_HEIGHT[2])
		{
			for (int i = 0; i < NUM_PILLARS; i++)
				pillars[i].draw(&ri);
				
			wavesObject.draw(&ri);

			simpleCliff.draw(&ri);
			simpleLeftCliff.draw(&ri);
			simpleRightCliff.draw(&ri);
		}

		if (atLayer > 0 && atLayer < 3)
		{
			for (int i = 0; i < NUM_CLOUDS; i++)
				clouds[i].draw(&ri);
		}

		if (atLayer == 2)
		{
			for (int i = 0; i < NUM_PLANETS; i++)
				planets[i].draw(&ri);
		}

		if (atLayer == 3)
		{
			for (int i = 0; i < NUM_ROCKS; i++)
				rocks[i].draw(&ri);
		}

		beginningPlatform.draw(&ri);

		//for (int i = 0; i < NUM_SCENERY; i++)
		//	scenery[i].draw(&ri);
		
		//for (int i = 0; i < NUM_CLIFFS; i++)
			//cliffs[i].draw(&ri);


		trampObject.draw(&ri);

		if (atLayer == 2)
			for (int i = 0; i < NUM_STARS; i++)
				stars[i].draw(&ri);

		std::wostringstream po;
		std::wstring pt;
		po << "Points: " << points;
		pt.clear();
		pt.append(po.str());
		RECT Rp = {580, 5, 0, 0};
		mFont->DrawText(0, pt.c_str(), -1, &Rp, DT_NOCLIP, WHITE);
			break;
		}
	}

	//Draw text to screen
	if (gameState == GAME_OVER) {
		RECT rect;
		int width;
		int height;
		if(GetWindowRect(mhMainWnd, &rect))
		{
		  width = rect.right - rect.left;
		  height = rect.bottom - rect.top;
		}
		RECT R2 = {0, 100, width, height / 4};
		std::string gameOverString = "G A M E   O V E R";
		mFont2->DrawTextA(0, gameOverString.c_str(), -1, &R2, DT_CENTER, D3DXCOLOR(1, 1, 1, 1));
		RECT R3 = { 0, 400, width, 3 * height / 4 };
		gameOverString = "(R) to restart";
		mFont->DrawTextA(0, gameOverString.c_str(), -1, &R3, DT_CENTER, D3DXCOLOR(1, 1, 1, 1));
	} else if (gameState == MENU) {
		RECT rect;
		int width;
		int height;
		if(GetWindowRect(mhMainWnd, &rect))
		{
		  width = rect.right - rect.left;
		  height = rect.bottom - rect.top;
		}
		RECT R2 = {0, 100, width, height / 2};
		std::string gameOverString = "D U N S T A N ' S   B I G   B A D \n B O U N C E   A R O U N D";
		mFont2->DrawTextA(0, gameOverString.c_str(), -1, &R2, DT_CENTER, D3DXCOLOR(1, 1, 1, 1));

		RECT R3 = {300, 200, width, height / 2};
		gameOverString = "Controls:\nMove: Left/Right or A/D\nGlide: Up or W\nDive: Down or S\n\nMute sounds: M";
		mFont->DrawTextA(0, gameOverString.c_str(), -1, &R3, DT_CENTER, D3DXCOLOR(1, 1, 1, 1));
	} else if (gameWon) {
		RECT rect;
		int width;
		int height;
		if (GetWindowRect(mhMainWnd, &rect))
		{
			width = rect.right - rect.left;
			height = rect.bottom - rect.top;
		}
		RECT R2 = { 0, 100, width, height / 4 };
		std::string gameOverString = "Y O U   W I N !";
		mFont2->DrawTextA(0, gameOverString.c_str(), -1, &R2, DT_CENTER, D3DXCOLOR(1, 1, 1, 1));
		}
	else if (fadeTextActive)
	{
		RECT rect;
		int width;
		int height;
		if(GetWindowRect(mhMainWnd, &rect))
		{
		  width = rect.right - rect.left;
		  height = rect.bottom - rect.top;
		}
		RECT R2 = {0, 100, width, height / 4};
		
		mFont2->DrawText(0, fadeTextMessage.c_str(), -1, &R2, DT_CENTER, D3DXCOLOR(1, 1, 1, fadeTextOpacity));
	}

	if (easyMode)
	{
		RECT rect;
		int width;
		int height;
		if(GetWindowRect(mhMainWnd, &rect))
		{
		  width = rect.right - rect.left;
		  height = rect.bottom - rect.top;
		}
		
		RECT rect2 = {height, 5, width, height/4};
		std::wstring easyMessage = L"Easy mode";
		mFont->DrawText(0, easyMessage.c_str(), -1, &rect2, DT_CENTER, D3DXCOLOR(1, 1, 1, .5));


	}
	
	
	RECT R1 = {200, 5, 0, 0};
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R1, DT_NOCLIP, D3DXCOLOR(1, 1, 1, .4));
	
	/*std::wostringstream outs;
	std::wstring debugText;
	outs.precision(3);
	outs << "Controls:\n"
		<< "Move: Left/Right\n"
		<< "Glide: Up\n"
		<< "Dive: Down\n"
		<< "Thrust: Space\n"
		<< "Ascend (debug): B\n"
		<< "Menu State: " << mainMenu->getMenuState()
		<< "\nLayer: " << atLayer;
		
	debugText.clear();
	debugText.append(outs.str());
	
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, debugText.c_str(), -1, &R, DT_NOCLIP, WHITE);*/
	mSwapChain->Present(0, 0);
}

void App::updateGameState(float dt) {
	elapsedTime += dt;
	switch(gameState) {
	case MENU:
		if(mainMenu->getMenuState() == PLAY) {
			gameState = LEVEL1;
			elapsedTime = 0;
		}
		if(mainMenu->getMenuState() == PLAY_EASY) {
			gameState = LEVEL1;
			elapsedTime = 0;
			setEasyMode();
		}

		break;
	case LEVEL1:
	case LEVEL2:
		if(player.getPosition().y + player.getScale().y / 2 < wavesObject.getPosition().y - 1) {
			if (!submarine)
			{
				if (!muted)
				{
					audio->playCue("splash");
					audio->playCue("death");
				}
				gameState = GAME_OVER;
				player.setPosition(Vector3(player.getPosition().x, -1000, player.getPosition().z));
				player.update(dt);
			}
		}
		if (player.getPosition().y + player.getScale().y / 2 < LAYER_HEIGHT[3] - 5)
		{
			if (!muted)
				audio->playCue("death");

			gameState = GAME_OVER;
		}
		break;
	}
}

void App::buildFX() {
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
	#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
	#endif
 
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;

	if(underwaterShader == true)
	{
		hr = D3DX10CreateEffectFromFile(L"underwaterShader.fx", 0, 0, "fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	}
	else
	{
		hr = D3DX10CreateEffectFromFile(L"shader.fx", 0, 0, "fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	}

	if(FAILED(hr)) {
		if( compilationErrors ) {
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	ri.mTech = mFX->GetTechniqueByName("Tech");
	
	ri.mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
	ri.mfxWorldVar  = mFX->GetVariableByName("gWorld")->AsMatrix();
	ri.mfxColorVar = mFX->GetVariableByName("colorOverride")->AsVector();
	mfxEyePosVar = mFX->GetVariableByName("gEyePosW");
	mfxLightVar  = mFX->GetVariableByName("gLight");
	mfxLightType = mFX->GetVariableByName("gLightType")->AsScalar();
	mfxPlayerPos = mFX->GetVariableByName("playerPos");
}

void App::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    ri.mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 4, PassDesc.pIAInputSignature,PassDesc.IAInputSignatureSize, &mVertexLayout));
}

void App::fadeText(std::wstring msg)
{
	fadeTextActive = true;
	fadeTextMessage = msg;
	fadeTextCurrentDuration = 0;
	fadeTextOpacity = 0;
}

void App::setEasyMode()
{
	easyMode = true;

	beginningPlatform.setVelocity(beginningPlatform.getVelocity() + Vector3(0, 0, 2));

	for (int i = 0; i < NUM_PILLARS; i++)
	{
		pillars[i].setScale(pillars[i].getScale() + Vector3(2, 0, 2));
		pillars[i].setVelocity(pillars[i].getVelocity() + Vector3(0, 0, 2));
	}

	for (int i = 0; i < NUM_CLOUDS; i++)
	{
		clouds[i].setScale(clouds[i].getScale() + Vector3(2, 0, 2));
		clouds[i].setVelocity(clouds[i].getVelocity() + Vector3(0, 0, 2));
	}

	for (int i = 0; i < NUM_PLANETS; i++)
	{
		planets[i].setScale(planets[i].getScale() + Vector3(2, 0, 2));
		planets[i].setVelocity(planets[i].getVelocity() + Vector3(0, 0, 2));
	}


}


void App::setUpGame(bool menu)
{
	if (menu)
		gameState = MENU;
	else
		gameState = LEVEL1;
	easyMode = false;
	gameWon = false;
	bPressedLastFrame = false;
	mPressedLastFrame = false;
	rPressedLastFrame = false;
	submarine = false;
	underwaterShader = false;
	atLayer = 0;
	cameraYBoost = 0;
	cameraZBoost = 0;
	thrust_timer = 0.0f;
	if (!muted)
	{
		audio->stopCue("music");
		audio->stopCue("musiclayer2");
		audio->stopCue("musiclayer3");
		audio->stopCue("winnermusic");
		audio->stopCue("waterstream");
		audio->playCue("music");
	}

	
	player.setPosition(Vector3(0, 3, 0));
	player.setVelocity(Vector3(0, 0, 0));
	trampObject.setPosition(Vector3(trampObject.getPosition().x, -10000, trampObject.getPosition().z));

	for (int i = 0; i < NUM_PILLARS; i++)
		pillars[i].setPosition(Vector3(0, -100, 1.0f * (GAME_DEPTH + GAME_BEHIND_DEPTH) / NUM_PILLARS*i));

	beginningPlatform.setPosition(Vector3(0, .5, GAME_DEPTH * .4));

	points = 0;
	atLayer = 0;
	underwaterShader = false;
	waves.reset(SEA_SIZE + 7, SEA_SIZE + 7, 0.5f);
	fadeText(LAYER_NAMES[0]);
}