//=======================================================================================
// Line.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef Line_H
#define Line_H

#include "d3dUtil.h"

class Line
{
public:

	Line();
	~Line();

	void init(ID3D10Device* device,
		      ID3D10EffectTechnique* new_mTech);
	void draw();
	void setColor(D3DXCOLOR, D3DXCOLOR);
	void setColor(D3DXCOLOR);

private:
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	ID3D10EffectTechnique* mTech;
	D3D10_TECHNIQUE_DESC* techDesc;
};


#endif // Line_H
