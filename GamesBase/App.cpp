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

#define _USE_MATH_DEFINES
#include <math.h>
#include <ctime>
#include <random>

const int NUM_OBSTACLES = 400;
const int AREA_WIDTH = 600;
const int AREA_DEPTH = 600;
const int OBSTACLE_SPEED = 40;
const int PLAYER_TURN_SPEED = 30;
const float ABSORPTION_RATE = 30.0f;

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

	//Geometry
	Line line;
	Box box;
	Quad quad;
	Pyramid pyramid;
	Triangle triangle;

	//Models
	ComplexGeometry wing;
	ComplexGeometry shipGeo;

	//Objects
	Axis axis;
	Object ship;
	Object blocks[NUM_OBSTACLES];

	RenderInfo ri;
	ID3D10Effect* mFX;
	ID3D10InputLayout* mVertexLayout;

	float mTheta;
	float mPhi;
	float mx, my, mz;

	float previousPlayerScale;

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
	audio.initialize();

	//Geometry
	line.init(md3dDevice, WHITE);
	box.init(md3dDevice, WHITE);
	quad.init(md3dDevice, WHITE);
	pyramid.init(md3dDevice, WHITE);
	triangle.init(md3dDevice, WHITE);

	//Complex Geometry
	wing.init(&triangle);
	wing.addChild(&triangle, Vector3(0, 1, 0), Vector3(0, 0, 0), Vector3(1, 1, 1));
	wing.addChild(&quad, Vector3(0, 0, 0), Vector3(0, 0, ToRadian(90)), Vector3(1, 1, 1));
	wing.addChild(&quad, Vector3(0, 0, 0), Vector3(ToRadian(-90), 0, 0), Vector3(1, 1, 1));
	wing.addChild(&quad, Vector3(0, 0, 1), Vector3(ToRadian(-90), ToRadian(45), 0), Vector3(1.4142, 1, 1));

	shipGeo.init(&box);
	shipGeo.addChild(&pyramid, Vector3(1, 1, 0), Vector3(0, 0, ToRadian(-90)), Vector3(1, 1, 1), Vector4(0, 1, 0, 1));
	shipGeo.addChild(&wing, Vector3(0, .4, 1.01), Vector3(0, 0, 0), Vector3(1, 0.2, 1),  Vector4(1, 0, 0, 1));
	shipGeo.addChild(&wing, Vector3(0, .6, -0.01), Vector3(ToRadian(180), 0, 0), Vector3(1, 0.2, 1),  Vector4(1, 0, 0, 1));

	//Objects
	axis.init(&line);
	ship.init(&shipGeo, Vector3(0, 0, 0));
	ship.setColor(0.0f, 1.0f, 0.0f, 1.0f);
	ship.setRotation(Vector3(0, -90 * M_PI / 180, 0));
	ship.setScale(Vector3(0.5, 0.5, 0.5));
	previousPlayerScale = 0.5f;

	for (int i = 0; i < NUM_OBSTACLES; i++) {
		float randScale = randomScaleDistribution(generator);
		// spawn the obstacles evenly over the z direction. if they spawn too close to the player, hide them until they hit the back wall.
		blocks[i].init(&box, Vector3(rand() % AREA_WIDTH - AREA_WIDTH / 2, 0, 1.0f * AREA_DEPTH/NUM_OBSTACLES*i));
		blocks[i].setColor(1.0f, 0.0f, 0.0f, 0.0f);
		blocks[i].setVelocity(Vector3(0, 0, -OBSTACLE_SPEED));
		blocks[i].setScale(Vector3(randScale, randScale, randScale));
		if (blocks[i].getPosition().z < AREA_DEPTH / 3)
			blocks[i].setInActive();
	}
}

void App::onResize() {
	D3DApp::onResize();
	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&ri.mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void App::updateScene(float dt) {
	D3DApp::updateScene(dt);

	float posChange = 0.0f;
	if (GetAsyncKeyState(VK_LEFT)) {
		posChange  = + PLAYER_TURN_SPEED * dt;
	}
	if (GetAsyncKeyState(VK_RIGHT)) {
		posChange = - PLAYER_TURN_SPEED * dt;
	}

	if(abs(ship.getScale().x - previousPlayerScale) > 0.25f) {
		float ps = ship.getScale().x;
		randomScaleDistribution = std::uniform_real_distribution<float>(ps - 0.15, ps + 0.75);
		previousPlayerScale = ship.getScale().x;
	}

	ship.update(dt);

	for (int i = 0; i < NUM_OBSTACLES; i++) {
		blocks[i].setPosition(blocks[i].getPosition() + Vector3(posChange, 0, 0));
		if (blocks[i].getPosition().z < -20) {
			int x = rand() % AREA_WIDTH - AREA_WIDTH / 2;
			blocks[i].setPosition(Vector3(x, 0, AREA_DEPTH));
			blocks[i].setActive();
		}
		blocks[i].update(dt);
		if(blocks[i].collided(&ship) && blocks[i].getActiveState() && ship.getActiveState()) {
			if(blocks[i].getScale().x >= ship.getScale().x) {
				float absorb = min(ABSORPTION_RATE * dt, ship.getScale().x);
				ship.setScale(ship.getScale() - Vector3(ABSORPTION_RATE * .05, ABSORPTION_RATE * .05, ABSORPTION_RATE * .05));
				blocks[i].setScale(blocks[i].getScale() + Vector3(ABSORPTION_RATE * .05, ABSORPTION_RATE * .05, ABSORPTION_RATE * .05));
				if (ship.getScale().x <= 0) ship.setInActive();
			} else {
				float absorb = min(ABSORPTION_RATE * dt, blocks[i].getScale().x);
				ship.setScale(ship.getScale() + Vector3(ABSORPTION_RATE * .05, ABSORPTION_RATE * .05, ABSORPTION_RATE * .05));
				blocks[i].setScale(blocks[i].getScale() - Vector3(ABSORPTION_RATE * .05, ABSORPTION_RATE * .05, ABSORPTION_RATE * .05));
				if(blocks[i].getScale().x <= 0) blocks[i].setInActive();
			}
		}
	}

	for (int i = 0; i < NUM_OBSTACLES; i++) {
		if (blocks[i].getPosition().z >= AREA_DEPTH) {
			float newScale = randomScaleDistribution(generator);
			blocks[i].setScale(Vector3(newScale, newScale, newScale));
		}
	}

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

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
    md3dDevice->IASetInputLayout(mVertexLayout);

	//Draw Axis
	axis.draw(&ri);

	//Draw Object
	ship.draw(&ri);
	for (int i = 0; i < NUM_OBSTACLES; i++) {
		blocks[i].draw(&ri);
	}

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);
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
 