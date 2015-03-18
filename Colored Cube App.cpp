//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================

#include <ctime>
#include <random>

#include "d3dApp.h"
#include "Box.h"
#include "Axes.h"
#include "GameObject.h"
#include <d3dx9math.h>
#include "constants.h"
#include "audio.h"
#include "Player.h"
#include <sstream>

class ColoredCubeApp : public D3DApp
{
public:
	ColoredCubeApp(HINSTANCE hInstance);
	~ColoredCubeApp();
	void init_audio();
	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 
	ID3DX10Font* Font;

private:
	void buildFX();
	void buildVertexLayouts();
	void setScaleRange(float low, float high);
 
private:
	
	Box redBox, mBox;
	Axes mAxes;

	Player player;

	Audio   *audio;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectScalarVariable* mfxColorVar;
	ID3D10EffectScalarVariable* mfxSlidingVar;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	float mTheta;
	float mPhi;

	float previousPlayerScale;

	float distance;

	std::uniform_real_distribution<float> randomScaleDistribution;
	std::mt19937 generator;

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	ColoredCubeApp theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), mTheta(0.0f), mPhi(PI*0.30f), randomScaleDistribution(0.1f, 2.25f)
{
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 
}

ColoredCubeApp::~ColoredCubeApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
}

void ColoredCubeApp::init_audio(){
	 audio = new Audio();
     audio->initialize();
}

void ColoredCubeApp::initApp()
{
	D3DApp::initApp();

	distance = 0.0f;

	D3DX10_FONT_DESC fd;
	fd.Height = 100;
	fd.Width = 0;
	fd.Weight = 0;
	fd.MipLevels = 1;
	fd.Italic = false;
	fd.CharSet = OUT_DEFAULT_PRECIS;
	fd.Quality = DEFAULT_QUALITY;
	fd.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	wcscpy(fd.FaceName, L"Impact");

	D3DX10CreateFontIndirect(md3dDevice, &fd, &Font);

	buildFX();
	buildVertexLayouts();
	srand(time(0));

	//Play startup music
	init_audio();

	redBox.init(md3dDevice, mTech);
	mBox.init(md3dDevice, mTech);
	mAxes.init(md3dDevice, &mView, &mProj, mfxWVPVar, mTech);
	player.init(&mBox, 1, Vector3(0.0f, 2.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), 0, 1);

}

void ColoredCubeApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.75f*PI, aspect, 1.0f, 500.0f);
}

void ColoredCubeApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);

	// Update angles based on input to orbit camera around box.
	 if(GetAsyncKeyState('A') & 0x8000)	mTheta -= 2.0f*dt;
	 if(GetAsyncKeyState('D') & 0x8000)	mTheta += 2.0f*dt;
	 if(GetAsyncKeyState('W') & 0x8000)	mPhi -= 2.0f*dt;
	 if(GetAsyncKeyState('S') & 0x8000)	mPhi += 2.0f*dt;

	if (GetAsyncKeyState(VK_ESCAPE)) exit(0);

	if (GetAsyncKeyState('R')) player.resetPos();

	if (GetAsyncKeyState(VK_SPACE)) player.thrust(dt);


	if (GetAsyncKeyState(VK_UP)) player.thrustUp(dt);
	if (GetAsyncKeyState(VK_LEFT)) player.rotateLeft(dt);
	if (GetAsyncKeyState(VK_RIGHT)) player.rotateRight(dt);

	player.update(dt);

	// Restrict the angle mPhi. 
	if( mPhi < 0.1f )	mPhi = 0.1f;
	if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

	// Convert Spherical to Cartesian coordinates: mPhi measured from +y
	// and mTheta measured counterclockwise from -z.
	float x =  5.0f*sinf(mPhi)*sinf(mTheta);
	float z = -5.0f*sinf(mPhi)*cosf(mTheta);
	float y =  5.0f*cosf(mPhi);

	// Build the view matrix.
	D3DXVECTOR3 pos(x, y, z);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &pos, &target, &up);
}

void ColoredCubeApp::drawScene()
{
	D3DApp::drawScene();

	mClearColor = D3DXCOLOR(100.0f / 255.0f, 100.0f / 255.0f, 255.0f / 255.0f, 1.0f);

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
    md3dDevice->IASetInputLayout(mVertexLayout);

	mAxes.draw();
	
	// how to draw
	//mWVP = myObject.getWorldMatrix()  *mView*mProj;
	//mfxWVPVar->SetMatrix((float*)&mWVP);
	//myObject.setMTech(mTech);
	//myObject.draw();

	mWVP = player.getWorldMatrix()*mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	player.setMTech(mTech);
	player.draw();


	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFrameStats.clear();

	std::wostringstream outs;   
	outs.precision(3);
	outs << L"Rotation:" << player.rotationAngle;
	mFrameStats.append(outs.str());

	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);
	mSwapChain->Present(0, 0);
}

void ColoredCubeApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"color.fx", 0, 0,
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("ColorTech");
	
	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxColorVar = mFX->GetVariableByName("mode")->AsScalar();
	mfxSlidingVar = mFX->GetVariableByName("diff")->AsScalar();
}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 2, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}

void ColoredCubeApp::setScaleRange(float low, float high) {
	randomScaleDistribution = std::uniform_real_distribution<float>(low, high);
}
