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

	Audio audio;

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

	//Objects
	Axis axis;
	Player player;
	Object wavesObject;

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

App::App(HINSTANCE hInstance):D3DApp(hInstance), mFX(0), mVertexLayout(0), randomScaleDistribution(0.25f, 2.25f), mEyePos(0.0f, 0.0f, 0.0f) {
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
	audio.initialize();

	//Geometry
	line.init(md3dDevice, WHITE);
	box.init(md3dDevice, WHITE);
	pillarBox.init(md3dDevice, WHITE);
	quad.init(md3dDevice, WHITE);
	pyramid.init(md3dDevice, WHITE);
	triangle.init(md3dDevice, WHITE);
	//waves.init(md3dDevice, 257, 257, 0.5f, 0.03f, 3.25f, 0.4f);
	waves.init(md3dDevice, 257, 257, 0.5f, 0.03f, 3.25f, 0.0f);

	//Complex Geometry
	// ---------------------------ANIMATION TEST
	bouncerBox.init(&box);
	AnimationState* bbani = new AnimationState();
	bbani->addAnimation(Vector3(0, 1, 0), Vector3(0, 0, 0), Vector3(1, 1, 1), Vector3(0, 3, 0), Vector3(0, 0, 0), Vector3(1, 1, 1));
	bbani->addAnimation(Vector3(0, 3, 0), Vector3(0, 0, 0), Vector3(1, 1, 1), Vector3(0, 1, 0), Vector3(0, 0, 0), Vector3(1, 1, 1));
	bouncerBox.addChild(&box, Vector3(0, 1, 0), Vector3(0, 0, 0), Vector3(1, 1, 1), bbani, Vector4(1, 0, 0, 1));
	//wing.init(&triangle);
	//wing.addChild(&triangle, Vector3(0, 1, 0), Vector3(0, 0, 0), Vector3(1, 1, 1));
	//wing.addChild(&quad, Vector3(0, 0, 0), Vector3(0, 0, ToRadian(90)), Vector3(1, 1, 1));
	//wing.addChild(&quad, Vector3(0, 0, 0), Vector3(ToRadian(-90), 0, 0), Vector3(1, 1, 1));
	//wing.addChild(&quad, Vector3(0, 0, 1), Vector3(ToRadian(-90), ToRadian(45), 0), Vector3(1.4142, 1, 1));

	//Objects
	axis.init(&line);
	player.init(&bouncerBox, Vector3(0, 0, 0));
	player.setColor(0.5f, 0.9f, 0.4f, 1.0f);
	player.setRotation(Vector3(0, -90 * M_PI / 180, 0));
	player.setScale(Vector3(0.5, 0.5, 0.5));
	wavesObject.init(&waves, Vector3(0, 0, 0));
	wavesObject.setColor(40.0f / 255.0f, 60.0f / 255.0f, 255.0f / 255.0f, 1);
	wavesObject.setVelocity(Vector3(0, WATER_RISE_SPEED, 0));

	for (int i = 0; i < NUM_PILLARS; i++)
	{
		pillars[i].init(&pillarBox, Vector3(0, -100, 1.0f * (GAME_DEPTH + GAME_BEHIND_DEPTH) / NUM_PILLARS*i));
		pillars[i].setScale(Vector3(1.5, rand() % 2 + PILLAR_HEIGHT_START * 2, 1.5));
		pillars[i].setVelocity(Vector3(0, 0, PILLAR_SPEED));
		pillars[i].setColor(1, 0, .9, 1);
	}


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

	if (GetAsyncKeyState(VK_SPACE)) player.thrust(dt);

	if (GetAsyncKeyState(VK_UP)) player.thrustUp(dt);
	if (GetAsyncKeyState(VK_LEFT)) player.rotateLeft(dt);
	if (GetAsyncKeyState(VK_RIGHT)) player.rotateRight(dt);

	Vector3 oldPlayerPosition = player.getPosition();

	player.update(dt);
	waves.update(dt);
	wavesObject.update(dt);

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



	/* collision with bottom area, temp */
	if (player.getPosition().y - player.getScale().y < -2)
	{
		player.setVelocity(Vector3(player.getVelocity().x, -player.getVelocity().y, player.getVelocity().z));

		if (player.getVelocity().y < 7 && player.getVelocity() > 0)
			player.setVelocity(Vector3(player.getVelocity().x, 7, player.getVelocity().z));

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
	mEyePos.x =  5.0f*sinf(mPhi)*sinf(mTheta) * d;
	mEyePos.z = -5.0f*sinf(mPhi)*cosf(mTheta) * d;
	mEyePos.y =  5.0f*cosf(mPhi) * d;

	// Build the view matrix.
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
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

	player.draw(&ri);
	wavesObject.draw(&ri);

	for (int i = 0; i < NUM_PILLARS; i++)
		pillars[i].draw(&ri);


	//Draw text to screen
	std::wostringstream outs;
	outs.precision(3);
	outs << "Controls:\n"
		<< "Turn: Left/Right\n"
		<< "Up: Up\n" 
		<< "Thrust: Space\n"
		<< "Reset: R\n";
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
 