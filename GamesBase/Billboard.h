#ifndef BILLBOARD_H
#define BILLBOARD_H
#pragma warning (disable : 4005)

#include "d3dUtil.h"
#include "Light.h"
#include <string>

struct BillboardVertex
{
	D3DXVECTOR3 centerW;
	D3DXVECTOR2 sizeW;
};

class BillBoard {
public:

	BillBoard();
	~BillBoard();

	void init(ID3D10Device* device, const D3DXVECTOR3 centers[], const float height, const float width, std::wstring filename, UINT num);
	void draw(const Light& L, const D3DXVECTOR3& eyePosW, const D3DXMATRIX& viewProj);

private:
	void buildVB(const D3DXVECTOR3 centers[], const float width, const float height);
	void buildFX();
	void buildVertexLayout();
	void buildShaderResourceView(std::wstring filename);

private:

	UINT num;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10ShaderResourceView* mMapArrayRV;

	ID3D10EffectMatrixVariable* mfxViewProjVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectShaderResourceVariable* mfxMapArrayVar;

	ID3D10InputLayout* mVertexLayout;
};

#endif // TREE_SPRITES_H