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
	Object scenery[NUM_SCENERY];

	//Objects
	Axis axis;
	Player player;
	Object wavesObject;

	Object beginningPlatform;
	Object pillars[NUM_PILLARS];

	RenderInfo ri;
	ID3D10Effect* mFX;
	ID3D10InputLayout* mVertexLayout;

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

App::App(HINSTANCE hInstance):D3DApp(hInstance), mFX(0), mVertexLayout(0), randomScaleDistribution(0.25f, 2.25f) {
	ri.mTech = 0;
	ri.mfxWVPVar = 0;
	D3DXMatrixIdentity(&ri.mView);
	D3DXMatrixIdentity(&ri.mProj);

	mTheta = 0;
	mPhi = M_PI*0.40f;
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
	audio->playCue("music");

	srand(time(0));

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
	// rearccccccccccccccccccccc
	ship.addChild(&box, Vector3(SHIP_WIDTH / 2, .7, .249), Vector3(0, ToRadian(-90), ToRadian(180)), Vector3(.5, 1.7, SHIP_WIDTH * .9), shipAni, shipColor2);
	// mast
	ship.addChild(&box, Vector3(SHIP_WIDTH / 2, MAST_HEIGHT / 2 + 1, SHIP_LENGTH / 2), Vector3(0, 0, 0), Vector3(.25, MAST_HEIGHT, .25), shipAni, shipColor3);
	// sail
	sailAni->addAnimation(Vector3(0, 0, 0), Vector3(ToRadian(10), 0, 0), Vector3(1, 1, 1), Vector3(1, 0, 0), Vector3(ToRadian(-10), 0, 0), Vector3(1, 1, 1));
	sailAni->addAnimation(Vector3(1, 0, 0), Vector3(ToRadian(-10), 0, 0), Vector3(1, 1, 1), Vector3(0, 0, 0), Vector3(ToRadian(10), 0, 0), Vector3(1, 1, 1));
	ship.addChild(&quad, Vector3(SHIP_WIDTH / 2 - SAIL_WIDTH / 2, MAST_HEIGHT * .8 + 1, SHIP_LENGTH / 2 + .25), Vector3(ToRadian(90), 0, 0), Vector3(SAIL_WIDTH, 0, SAIL_HEIGHT), sailAni, Vector4(1, 1, 1, 1));
	
	sceneryGeometry[0] = ship;
	sceneryGeometry[1] = ship;

	//Objects
	axis.init(&line);
	player.init(&bouncerBox, Vector3(0, 0, 0));
	player.setColor(0.5f, 0.9f, 0.4f, 1.0f);
	player.setRotation(Vector3(0, -90 * M_PI / 180, 0));
	player.setScale(Vector3(0.5, 0.5, 0.5));
	wavesObject.init(&waves, Vector3(0, 0, 0));
	wavesObject.setColor(9.0f / 255.0f, 72.0f / 255.0f, 105.0f / 255.0f, 1);
	wavesObject.setVelocity(Vector3(0, WATER_RISE_SPEED, 0));

	for (int i = 0; i < NUM_PILLARS; i++)
	{
		pillars[i].init(&pillarBox, Vector3(0, -100, 1.0f * (GAME_DEPTH + GAME_BEHIND_DEPTH) / NUM_PILLARS*i));
		pillars[i].setScale(Vector3(1.5, rand() % 2 + PILLAR_HEIGHT_START * 2, 1.5));
		pillars[i].setVelocity(Vector3(0, 0, PILLAR_SPEED));
		pillars[i].setColor(1, 1, .9, 1);
	}

	beginningPlatform.init(&pillarBox, Vector3(0, 0, GAME_DEPTH * .75));
	beginningPlatform.setScale(Vector3(10, PILLAR_HEIGHT_START, GAME_DEPTH * 1.5));
	beginningPlatform.setVelocity(Vector3(0, 0, PILLAR_SPEED));
	beginningPlatform.setColor(1, 1, .9, 1);

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

	// create initial disturbance of waves
	// the waves are set to never dampen so this will create waves that last forever
	for (int x = 0; x < 25; x++)
	{

		DWORD i = 5 + rand() % 250;
		DWORD j = 5 + rand() % 250;

		float r = RandF(2.0f, 2.2f);

		waves.disturb(i, j, r);
	}

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
	if (GetAsyncKeyState(VK_SPACE)) player.thrustUp(dt);
	if (GetAsyncKeyState(VK_LEFT)) player.accelLeft(dt);
	if (GetAsyncKeyState(VK_RIGHT)) player.accelRight(dt);
	if (!GetAsyncKeyState(VK_LEFT) && !GetAsyncKeyState(VK_RIGHT)) player.decelX(dt);
	if (GetAsyncKeyState(VK_UP)) player.setGliding(true);
	else
		player.setGliding(false);
	if (GetAsyncKeyState(VK_DOWN)) player.setDiving(true);
	else
		player.setDiving(false);

	Vector3 oldPlayerPosition = player.getPosition();

	player.update(dt);
	waves.update(dt);
	wavesObject.update(dt);

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

		if (pillars[i].getPosition().z < -10)
		{
			int x = rand() % GAME_WIDTH - GAME_WIDTH / 2;
			pillars[i].setPosition(Vector3(x, 0 - PILLAR_HEIGHT_START, GAME_DEPTH));
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
		}
	}

    /* bottom collision, temp */
	if (player.getPosition().y - player.getScale().y < wavesObject.getPosition().y - 1)
	{
		player.setVelocity(Vector3(player.getVelocity().x, PLAYER_BOUNCE_FORCE, player.getVelocity().z));

		player.setPosition(oldPlayerPosition);
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
	float x =  5.0f*sinf(mPhi)*sinf(mTheta) * d;
	float z = -5.0f*sinf(mPhi)*cosf(mTheta) * d;
	float y =  5.0f*cosf(mPhi) * d;

	// Build the view matrix.
	D3DXVECTOR3 pos(x, y, z);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&ri.mView, &pos, &target, &up);
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

	//Draw axis
	axis.draw(&ri);

	//Draw objects

	player.draw(&ri);
	wavesObject.draw(&ri);
	
	for (int i = 0; i < NUM_PILLARS; i++)
		pillars[i].draw(&ri);

	beginningPlatform.draw(&ri);

	for (int i = 0; i < NUM_SCENERY; i++)
		scenery[i].draw(&ri);

	//Draw text to screen
	std::wostringstream outs;
	outs.precision(3);
	outs << "Controls:\n"
		<< "Move: Left/Right\n"
		<< "Glide: Up\n" 
		<< "Dive: Down\n"
		<< "Thrust: Space\n";
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

	ri.mTech = mFX->GetTechniqueByName("ColorTech");
	
	ri.mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
	ri.mfxColorVar = mFX->GetVariableByName("colorOverride")->AsVector();
}

void App::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    ri.mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 2, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &mVertexLayout));
}
 