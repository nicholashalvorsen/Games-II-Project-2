#ifndef RENDER_INFO_H
#define RENDER_INFO_H

#include "constants.h"
#include "d3dApp.h"

struct RenderInfo {
	Matrix mView;
	Matrix mProj;

	ID3D10EffectTechnique* mTech;

	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxTexMtxVar;

	ID3D10EffectVectorVariable* mfxColorVar;
	ID3D10EffectVectorVariable* mfxSpecVar;

	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;

	ID3D10EffectScalarVariable* mfxHasTexture;
};

#endif