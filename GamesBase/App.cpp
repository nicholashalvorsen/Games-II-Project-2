#include "d3dApp.h"
#include "Line.h"
#include "Box.h"
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
#include "AnimationState.h"
#include "Light.h"

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

private:
	void buildFX();
	void buildVertexLayouts();

	Audio *audio;
	//to zoom the camera in/out
	float zoom;
	//to limit boosting
	float thrust_timer;

	//Lighting
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectScalarVariable* mfxLightType;
	D3DXVECTOR3 mEyePos;

	Light mLight;

	//Geometry
	Line line;
	Box box;
	ComplexGeometry bouncerBox;
	Box pillarBox;
	Quad quad;
	Pyramid pyramid;
	Triangle triangle;
	Waves waves;

	//Models
	//ComplexGeometry wing;
	ComplexGeometry sceneryGeometry[NUM_SCENERY];
	ComplexGeometry cliffsGeometry;
	Object scenery[NUM_SCENERY];
	Object cliffs[NUM_CLIFFS];

	//Objects
	Axis axis;
	Player player;
	Object wavesObject;

	Object beginningPlatform;
	Object pillars[NUM_PILLARS];
	Object clouds[NUM_CLOUDS];

	RenderInfo ri;
	ID3D10Effect* mFX;
	ID3D10InputLayout* mVertexLayout;

	bool atCloudHeight;
	float cameraYBoost;
	float cameraZBoost;

	float mTheta;
	float mPhi;
	float mx, my, mz;

	std::uniform_real_distribution<float> randomScaleDistribution;
	std::mt19937 generator;
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
}

App::~App() {
	if( md3dDevice ) md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
}

void App::initApp() {
	D3DApp::initApp();
	buildFX();
	buildVertexLayouts();

	audio = new Audio();
	audio->initialize();
	
	// temp, I don't feel like listening to this 100 times 
	//audio->playCue("music");

	srand(time(0));
	zoom = 1.0f;
	thrust_timer = 0.0f;
	//Geometry
	line.init(md3dDevice, WHITE);
	box.init(md3dDevice, WHITE);
	pillarBox.init(md3dDevice, WHITE);
	quad.init(md3dDevice, WHITE);
	pyramid.init(md3dDevice, WHITE);
	triangle.init(md3dDevice, WHITE);
	waves.init(md3dDevice, 257, 257, 0.5f, 0.03f, 3.25f, 0.0f);

	//Complex Geometry
	// ---------------------------ANIMATION TEST
	bouncerBox.init(&box);
	AnimationState* bbani = new AnimationState();
	bbani->addAnimation(Vector3(0, 1, 0), Vector3(0, 0, 0), Vector3(1, 1, 1), Vector3(0, 3, 0), Vector3(0, 0, 0), Vector3(1, 1, 1));
	bbani->addAnimation(Vector3(0, 3, 0), Vector3(0, 0, 0), Vector3(1, 1, 1), Vector3(0, 1, 0), Vector3(0, 0, 0), Vector3(1, 1, 1));
	bouncerBox.addChild(&box, Vector3(0, 1, 0), Vector3(0, 0, 0), Vector3(1, 1, 1), bbani, Vector4(1, 0, 0, 1));

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
	ship.addChild(&quad, Vector3(0, 1, 0), Vector3(0, 0, 0), Vector3(SHIP_WIDTH, 1, SHIP_LENGTH), shipAni, shipColor);
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
	ship.addChild(&quad, Vector3(SHIP_WIDTH / 4, 1, SHIP_LENGTH), Vector3(0, ToRadian(-90), ToRadian(180)), Vector3(SHIP_FRONT_LENGTH / 2, 1, SHIP_WIDTH / 2), shipAni, shipColor);
	// forward long thing
	ship.addChild(&box, Vector3(SHIP_WIDTH / 2.0f, .745, SHIP_LENGTH + SHIP_FRONT_LENGTH / 1.5), Vector3(0, ToRadian(-90), 0), Vector3(SHIP_FRONT_LENGTH * 1.5, .5, .5), shipAni, shipColor2);
	// rear
	ship.addChild(&box, Vector3(SHIP_WIDTH / 2, .7, .249), Vector3(0, ToRadian(-90), ToRadian(180)), Vector3(.5, 1.7, SHIP_WIDTH * .9), shipAni, shipColor2);
	// mast
	ship.addChild(&box, Vector3(SHIP_WIDTH / 2, MAST_HEIGHT / 2 + 1, SHIP_LENGTH / 2), Vector3(0, 0, 0), Vector3(.25, MAST_HEIGHT, .25), shipAni, shipColor3);
	// sail
	sailAni->addAnimation(Vector3(0, 0, 0), Vector3(ToRadian(10), 0, 0), Vector3(1, 1, 1), Vector3(1, 0, 0), Vector3(ToRadian(-10), 0, 0), Vector3(1, 1, 1));
	sailAni->addAnimation(Vector3(1, 0, 0), Vector3(ToRadian(-10), 0, 0), Vector3(1, 1, 1), Vector3(0, 0, 0), Vector3(ToRadian(10), 0, 0), Vector3(1, 1, 1));
	ship.addChild(&quad, Vector3(SHIP_WIDTH / 2 - SAIL_WIDTH / 2, MAST_HEIGHT * .8 + 1, SHIP_LENGTH / 2 + .25), Vector3(ToRadian(90), 0, 0), Vector3(SAIL_WIDTH, 0, SAIL_HEIGHT), sailAni, Vector4(1, 1, 1, 1));

	sceneryGeometry[0] = ship;
	sceneryGeometry[1] = ship;

	cliffsGeometry.init(&line);
	Vector4 cliffsColor(.4f, .4f, .4f, 1);
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
	player.init(&box, Vector3(0, 0, 0));
	player.setColor(0.5f, 0.9f, 0.4f, 1.0f);
	player.setRotation(Vector3(0, -90 * M_PI / 180, 0));
	player.setScale(Vector3(0.5, 0.5, 0.5));
	wavesObject.init(&waves, Vector3(0, -.5, 0));
	wavesObject.setColor(9.0f / 255.0f, 72.0f / 255.0f, 105.0f / 255.0f, 1);
	wavesObject.setVelocity(Vector3(0, WATER_RISE_SPEED, 0));

	for (int i = 0; i < NUM_PILLARS; i++)
	{
		pillars[i].init(&pillarBox, Vector3(0, -100, 1.0f * (GAME_DEPTH + GAME_BEHIND_DEPTH) / NUM_PILLARS*i));
		pillars[i].setScale(Vector3(1.5, rand() % 2 + PILLAR_HEIGHT_START * 2, 1.5));
		pillars[i].setVelocity(Vector3(0, 0, PILLAR_SPEED));
		pillars[i].setColor(1, 0, .9, 1);
	}
		 
	beginningPlatform.init(&pillarBox, Vector3(0, .5, GAME_DEPTH * .75));
	beginningPlatform.setScale(Vector3(10, PILLAR_HEIGHT_START, GAME_DEPTH * 1.5));
	beginningPlatform.setVelocity(Vector3(0, 0, PILLAR_SPEED));
	beginningPlatform.setColor(1, 1, .9, 1);

	for (int i = 0; i < NUM_CLOUDS; i++)
	{
		clouds[i].init(&pillarBox, Vector3(0, -100, 1.0f * (GAME_DEPTH + GAME_BEHIND_DEPTH) / NUM_CLOUDS*i));
		clouds[i].setScale(Vector3(CLOUD_SIZE, 1, CLOUD_SIZE));
		clouds[i].setVelocity(Vector3(0, 0, CLOUD_SPEED));
		clouds[i].setColor(.8, .8, .8, 1);
	}

	for (int i = 0; i < NUM_SCENERY; i++)
	{
		int lr = rand() % 2;
		int xpos;

		if (lr == 1)
			xpos = -GAME_WIDTH / 2 - 4 - rand() % 5;
		else
			xpos =  GAME_WIDTH / 2 + 4 + rand() % 5;

		scenery[i].init(&sceneryGeometry[i], Vector3(xpos, 1, 1.0f * (GAME_DEPTH + GAME_BEHIND_DEPTH) / NUM_SCENERY*i));
		scenery[i].setVelocity(Vector3(0, 0, -1));
	}

	for (int i = 0; i < NUM_CLIFFS; i++)
	{
		cliffs[i].init(&cliffsGeometry, Vector3(CLIFF_WIDTH * i - NUM_CLIFFS * CLIFF_WIDTH / 2, CLIFF_HEIGHT / 2, 40));
	}

	// create initial disturbance of waves
	// the waves are set to never dampen so this will create waves that last forever
	for (int x = 0; x < 25; x++)
	{

		DWORD i = 5 + rand() % 250;
		DWORD j = 5 + rand() % 250;

		float r = RandF(2.0f, 2.2f);

		waves.disturb(i, j, r);
	}

	//Light
	mLight.ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 0.2f);
	mLight.diffuse = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.1f);
	mLight.specular = D3DXCOLOR(0.7f, 0.7f, 0.7f, 0.7f);
	mLight.att.x    = 1.0f;
	mLight.att.y    = 0.0f;
	mLight.att.z    = 0.0f;
	mLight.spotPow  = 64.0f;
	mLight.range    = 10000.0f;
	mLight.pos = Vector3(0, 15.0f, 0);
	mLight.dir = Vector3(0.0f, -1.0f, 0.0f);

	atCloudHeight = false;
	cameraYBoost = 0;
	cameraZBoost = 0;
}

void App::onResize() {
	D3DApp::onResize();
	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&ri.mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void App::updateScene(float dt) {
	D3DApp::updateScene(dt);

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
	if (GetAsyncKeyState('R')) player.resetPos();
	//Thrust with a timer of a 1:2 thrust to cooldown ratio and a 2 second starting thrust bank.
	if (GetAsyncKeyState(VK_SPACE)){ 
		
		if(thrust_timer < 1.0f){
			thrust_timer+=dt;
            player.thrustUp(dt);
		}
	
	}else if(thrust_timer > 0){
       	  thrust_timer -= 0.5*dt;
	}
	if (GetAsyncKeyState(VK_LEFT)) player.accelLeft(dt);
	if (GetAsyncKeyState(VK_RIGHT)) player.accelRight(dt);
	if (!GetAsyncKeyState(VK_LEFT) && !GetAsyncKeyState(VK_RIGHT)) player.decelX(dt);
	if (GetAsyncKeyState(VK_UP)) player.setGliding(true);
	else
		player.setGliding(false);
	if (GetAsyncKeyState(VK_DOWN)) player.setDiving(true);
	else
		player.setDiving(false);
	if (GetAsyncKeyState('B')) {
		player.setVelocity(Vector3(0, 20, 0));
		atCloudHeight = true;
	}

	Vector3 oldPlayerPosition = player.getPosition();

	player.update(dt);
	waves.update(dt);
	wavesObject.update(dt);

	if (player.getPosition().y < CLOUD_HEIGHT_START - 1 && player.getVelocity().y < 0)
		atCloudHeight = false;

	if (atCloudHeight)
	{
		if (cameraYBoost < CLOUD_HEIGHT_START)
			cameraYBoost += CAMERA_MOVE_SPEED * dt;	

		if (cameraZBoost < 10)
			cameraZBoost += CAMERA_MOVE_SPEED * dt;
	}
	if (!atCloudHeight)
	{
		if (cameraYBoost > 0)
			cameraYBoost -= CAMERA_MOVE_SPEED * dt;

		if (cameraZBoost > 0)
			cameraZBoost -= CAMERA_MOVE_SPEED * dt;

	}

	beginningPlatform.update(dt);
	if (beginningPlatform.getPosition().y < .8)
		beginningPlatform.setVelocity(Vector3(beginningPlatform.getVelocity().x, 1, beginningPlatform.getVelocity().z));
	else
		beginningPlatform.setVelocity(Vector3(beginningPlatform.getVelocity().x, 0, beginningPlatform.getVelocity().z));

	for (int i = 0; i < NUM_PILLARS; i++)
	{
		if (pillars[i].getPosition().y < 0)
			pillars[i].setVelocity(Vector3(pillars[i].getVelocity().x, 1, pillars[i].getVelocity().z));
		else
			pillars[i].setVelocity(Vector3(pillars[i].getVelocity().x, 0, pillars[i].getVelocity().z));

		if (pillars[i].getPosition().z < -2)
			pillars[i].setVelocity(Vector3(pillars[i].getVelocity().x, -1, pillars[i].getVelocity().z));

		pillars[i].update(dt);

		if (pillars[i].getPosition().z < -GAME_BEHIND_DEPTH)
		{
			int x = rand() % GAME_WIDTH - GAME_WIDTH / 2;
			pillars[i].setPosition(Vector3(x, 0 - PILLAR_HEIGHT_START, GAME_DEPTH));
			pillars[i].setScale(Vector3(pillars[i].getScale().x * 0.9f, pillars[i].getScale().y *1.15f, pillars[i].getScale().z *.9f));
		}
	}

	for (int i = 0; i < NUM_CLOUDS; i++)
	{
		clouds[i].update(dt);

		if (clouds[i].getPosition().z < -GAME_BEHIND_DEPTH)
		{
			int x = rand() % GAME_WIDTH - GAME_WIDTH / 2;
			clouds[i].setPosition(Vector3(x, CLOUD_HEIGHT_START, GAME_DEPTH));
		}
	}

	for (int i = 0; i < NUM_SCENERY; i++)
	{
		scenery[i].update(dt);
		if (scenery[i].getPosition().z < -20)
		{
			int lr = rand() % 2;
			int xpos;

			if (lr = 1)
				xpos = -10 - rand() % 3;
			else
				xpos = 10 + rand() % 3;

			scenery[i].setPosition(Vector3(xpos, 1, GAME_DEPTH * 2));
			//Scale boxes to get smaller every time they are re-positioned.
			
		}
	}

	for (int i = 0; i < NUM_CLIFFS; i++)
		cliffs[i].update(dt);

    /* bottom collision, temp */ 
	if (player.getPosition().y - player.getScale().y < wavesObject.getPosition().y - 1)
	{
		player.setVelocity(Vector3(player.getVelocity().x, PLAYER_BOUNCE_FORCE, player.getVelocity().z));

		player.setPosition(oldPlayerPosition);
	}

	// collision
	for (int i = 0; i < NUM_PILLARS; i++)
	{
		if (player.collided(&pillars[i]))
		{
			player.setPosition(oldPlayerPosition + Vector3(0, 0.1, 0));
			player.setVelocity(Vector3(player.getVelocity().x, PLAYER_BOUNCE_FORCE, player.getVelocity().z));
		}
	}

	for (int i = 0; i < NUM_CLOUDS; i++)
	{
		if (player.collided(&clouds[i]))
		{
			player.setPosition(oldPlayerPosition + Vector3(0, 0.1, 0));
			player.setVelocity(Vector3(player.getVelocity().x, PLAYER_BOUNCE_FORCE, player.getVelocity().z));
		}
	}



	if (player.collided(&beginningPlatform))
	{
		player.setPosition(oldPlayerPosition + Vector3(0, 0.1, 0));
		player.setVelocity(Vector3(player.getVelocity().x, PLAYER_BOUNCE_FORCE, player.getVelocity().z));

	}

	/* don't let the player go too fast */
	if (abs(player.getVelocity().x) > VELOCITY_LIMIT)
		player.setVelocity(Vector3(player.getVelocity().x * .99, player.getVelocity().y, player.getVelocity().z));

	if (abs(player.getVelocity().y) > VELOCITY_LIMIT)
		player.setVelocity(Vector3(player.getVelocity().x, player.getVelocity().y * .99, player.getVelocity().z));

	if (abs(player.getVelocity().y) > VELOCITY_LIMIT)
		player.setVelocity(Vector3(player.getVelocity().x, player.getVelocity().y, player.getVelocity().z * .99));


	// Update angles based on input to orbit camera around box.
	if(GetAsyncKeyState('A') & 0x8000)	mTheta -= 2.0f*dt;
	if(GetAsyncKeyState('D') & 0x8000)	mTheta += 2.0f*dt;
	if(GetAsyncKeyState('W') & 0x8000)	mPhi -= 2.0f*dt;
	if(GetAsyncKeyState('S') & 0x8000)	mPhi += 2.0f*dt;

	// Restrict the angle mPhi.
	if( mPhi < 0.1f )	mPhi = 0.1f;
	if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

	// Convert Spherical to Cartesian coordinates: mPhi measured from +y
	// and mTheta measured counterclockwise from -z.
	float d = 4;
	mEyePos.x =  5.0f*sinf(mPhi)*sinf(mTheta) * d;
	mEyePos.z = player.getPosition().z - 10 + cameraZBoost;
	mEyePos.y =  player.getPosition().y + 2 + cameraYBoost;

	// Build the view matrix.
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	
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

	D3DXVECTOR3 target(player.getPosition() + Vector3(0.0f, 0.0f+cameraYBoost*.7, 0.0f+cameraZBoost*.7));
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&ri.mView, &mEyePos, &target, &up);
}

void App::drawScene() {
	D3DApp::drawScene();
	mClearColor = D3DXCOLOR(107.0f / 255.0f, 123.0f / 255.0f, 164.0f / 255.0f, 1.0f);

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

	//Draw Axis
	axis.draw(&ri);

	//Draw objects
	
	player.draw(&ri);
	wavesObject.draw(&ri);
	
	for (int i = 0; i < NUM_PILLARS; i++)
		pillars[i].draw(&ri);

	for (int i = 0; i < NUM_CLOUDS; i++)
		clouds[i].draw(&ri);

	beginningPlatform.draw(&ri);

	for (int i = 0; i < NUM_SCENERY; i++)
		scenery[i].draw(&ri);
		
	for (int i = 0; i < NUM_CLIFFS; i++)
		cliffs[i].draw(&ri);

	//Draw text to screen
	std::wostringstream outs;
	outs.precision(3);
	outs << "Controls:\n"
		<< "Move: Left/Right\n"
		<< "Glide: Up\n"
		<< "Dive: Down\n"
		<< "Thrust: Space\n"
		<< "Ascend (debug): B\n";
	mFrameStats.clear();
	mFrameStats.append(outs.str());

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, WHITE);
	mSwapChain->Present(0, 0);
}

void App::buildFX() {
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
	#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
	#endif
 
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"shader.fx", 0, 0, "fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
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
 