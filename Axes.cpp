//=======================================================================================
// Axes.cpp by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#include "Axes.h"
#include "Vertex.h"

Axes::Axes()
{
}
 
Axes::~Axes()
{
}

void Axes::init(ID3D10Device* device,
				D3DXMATRIX* new_mView,
				D3DXMATRIX* new_mProj,
				ID3D10EffectMatrixVariable* new_mfxWVPVar,
				ID3D10EffectTechnique* new_mTech)
{
	mView = new_mView;
	mProj = new_mProj;
	mfxWVPVar = new_mfxWVPVar;
	md3dDevice = device;
	mLine.init(device, new_mTech);
}

void Axes::draw()
{
	D3DXMATRIX mScale;
	D3DXMatrixScaling(&mScale, 10.0f, 10.0f, 10.0f);

	// Do the transforms, draw the line
	D3DXMATRIX mWVP = mScale*(*mView)*(*mProj);
	mfxWVPVar->SetMatrix((float*)&mWVP);

	mLine.setColor(RED);	// X Axis
	mLine.draw();

	// Rotation in Y
	D3DXMATRIX mRotate;
	D3DXMatrixRotationY(&mRotate, D3DXToRadian(-90));
	mWVP = (mRotate)*(mScale)*(*mView)*(*mProj);
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mLine.setColor(BLUE);	// Z Axis
	mLine.draw();

	// Rotation in Z
	mRotate;
	D3DXMatrixRotationZ(&mRotate, D3DXToRadian(90));
	mWVP = (mRotate)*(mScale)*(*mView)*(*mProj);
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mLine.setColor(GREEN);	// Y Axis
	mLine.draw();
}