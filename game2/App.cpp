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

#define _USE_MATH_DEFINES
#include <math.h>
#include <ctime>
#include <random>

const int NBLOCKS = 40;

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

	//Geometry
	Line line;
	Box box;
	Quad quad;
	Pyramid pyramid;
	Triangle triangle;

	//Models

	//Objects
	Axis axis;
	Object player;
	Object left[NBLOCKS];
	Object right[NBLOCKS];
	Object floor;

	RenderInfo ri;
	ID3D10Effect* mFX;
	ID3D10InputLayout* mVertexLayout;
	float timer;
	float mTheta;
	float mPhi;
	float mx, my, mz;

	int ndeaths;
	bool win, jumped,collided;
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

App::App(HINSTANCE hInstance):D3DApp(hInstance), mFX(0), mVertexLayout(0) {
	ri.mTech = 0;
	ri.mfxWVPVar = 0;
	D3DXMatrixIdentity(&ri.mView);
	D3DXMatrixIdentity(&ri.mProj);

	mTheta = 0;
	mPhi = -M_PI;
	mx = 0;
	my = 0;
	mz = 0;

	ndeaths = 0;
	win = false;
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

	//Geometry
	line.init(md3dDevice, WHITE);
	box.init(md3dDevice, WHITE);
	quad.init(md3dDevice, WHITE);
	pyramid.init(md3dDevice, WHITE);
	triangle.init(md3dDevice, WHITE);

	//Complex Geometry

	//Objects
	axis.init(&line);
	player.init(&box, Vector3(0, 0, 0));
	player.setScale(Vector3(.5, .5, .5));
	player.setColor(0, 0, 1, 0);
	jumped = false;
	collided = false;
	timer =0;
	int m = 2;
	int bpc = 2;
	for (int i = 0; i < NBLOCKS; i++) {
		left[i].init(&box, Vector3(0, 0, 0));
		right[i].init(&box, Vector3(0, 0, 0));
		left[i].setColor(1, 0, 0, 1);
		right[i].setColor(0, 1, 0, 1);
		left[i].setScale(Vector3(2, 2, 2));
		right[i].setScale(Vector3(2, 3, 2));

		float deltTheta = 2*PI/100;
		
		
		left[i].setPosition(Vector3(3*i*cos(i* deltTheta), 0, 3*i*sin(i * deltTheta)));


		right[i].setPosition(left[i].getPosition() + Vector3(10, 0 , 0));

		if (i % bpc == 0) m++;
		if (m % 6 == 0) bpc++;
	}
	floor.init(&quad, Vector3(-20, 0, -15));
	floor.setColor(1, 1, 1, 1);
	floor.setScale(Vector3(40, 1, 30));

}

void App::onResize() {
	D3DApp::onResize();
	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&ri.mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void App::updateScene(float dt) {



	D3DApp::updateScene(dt);
		
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
	float d = 8;
	float x =  5.0f*sinf(mPhi)*sinf(mTheta) * d;
	float z = -5.0f*sinf(mPhi)*cosf(mTheta) * d;
	float y =  5.0f*cosf(mPhi) * d;



	player.setVelocity(Vector3(0, player.getVelocity().y, 0));
	if(GetAsyncKeyState(VK_UP)){
		player.setVelocity(player.getVelocity() + Vector3(-x*.1, 0 , -z*.1));
	}
	if(GetAsyncKeyState(VK_DOWN)){
		player.setVelocity(player.getVelocity() + Vector3(x*.1, 0 , z*.1));
	}
	if(GetAsyncKeyState(VK_RIGHT)){
		player.setVelocity(player.getVelocity() + Vector3(z*.1, 0 , x*.1));
	}
	if(GetAsyncKeyState(VK_LEFT)){
		player.setVelocity(player.getVelocity() + Vector3(-z*.1, 0 , -x*.1));
	}
	if(GetAsyncKeyState(VK_SPACE) && !jumped){
		player.setVelocity(Vector3(player.getVelocity().x, 15, player.getVelocity().z));
		jumped = true;
	}

	


	for (int i = 0; i < NBLOCKS; i++) {

		left[i].update(dt);
		right[i].update(dt);

		if ((player.collided(&right[i]) || player.collided(&left[i]) && player.getVelocity().y < 0)) {
			player.setVelocity(Vector3(player.getVelocity().x, 20, player.getVelocity().z));
			collided = true;
			jumped = false;
		}

	}

	if(player.getPosition().y < 0.1){
			player.setPosition(Vector3(player.getPosition().x, 0.1, player.getPosition().z));
			timer = 0;
			jumped = false;
	}
	else if(!collided){
			player.setVelocity(player.getVelocity() - Vector3(0, 1.3*timer, 0));
			timer+=dt;
	}
	collided = false;
	player.update(dt);
	floor.update(dt);







	// Build the view matrix.
	D3DXVECTOR3 pos(x, y, z);
	D3DXVECTOR3 target(player.getPosition());
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
	player.draw(&ri);
	floor.draw(&ri);

	for (int i = 0; i < NBLOCKS; i++) {
		left[i].draw(&ri);
		right[i].draw(&ri);
	}

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);

	RECT R3 = {mClientWidth - 110, 5, 0, 0 };
	std::wstring s(L"Deaths ");

	mFont->DrawText(0, (s + std::to_wstring(ndeaths)).c_str(), -1, &R3, DT_NOCLIP, BLACK);

	RECT R2 = {mClientWidth / 2, mClientHeight / 2, 0, 0 };
	if (win) mFont->DrawText(0, std::wstring(L"You Win").c_str(), -1, &R2, DT_NOCLIP, BLACK);
	mSwapChain->Present(0, 0);
}

void App::buildFX() {
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
	//#if defined( DEBUG ) || defined( _DEBUG )
	//	shaderFlags |= D3D10_SHADER_DEBUG;
	//	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
	//#endif
 
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
 