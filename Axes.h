//=======================================================================================
// Axes.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef Axes_H
#define Axes_H

#include "d3dUtil.h"
#include "Line.h"

class Axes
{
public:

	Axes();
	~Axes();

	void init(ID3D10Device* device,
			  D3DXMATRIX* new_mView,
			  D3DXMATRIX* new_mProj,
			  ID3D10EffectMatrixVariable* new_mfxWVPVar,
			  ID3D10EffectTechnique* new_mTech);
	void draw();

private:
	ID3D10Device* md3dDevice;
	Line mLine;
	D3DXMATRIX* mView;
	D3DXMATRIX* mProj;
	ID3D10EffectMatrixVariable* mfxWVPVar;
};


#endif // Axes_H