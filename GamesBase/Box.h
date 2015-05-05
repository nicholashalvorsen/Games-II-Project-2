//=======================================================================================
// Box.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef BOX_H
#define BOX_H
#pragma warning (disable : 4005)

#include "d3dUtil.h"
#include "constants.h"
#include "RenderInfo.h"
#include "Geometry.h"

class Box : public Geometry
{
public:

	Box();
	~Box();

	void init(ID3D10Device* device);
	void draw(RenderInfo* ri, Matrix world, Vector4 color = Vector4(0, 0, 0, 1), Vector4 spec = Vector4(0, 0, 0, 0));
	float getRadius() { return 1; }
private:
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
};

#endif // BOX_H
