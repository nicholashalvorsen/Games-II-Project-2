#ifndef LINE_H
#define LINE_H

#include "Geometry.h"
#include "d3dUtil.h"
#include "Vertex.h"
#include "constants.h"

class Line : public Geometry {
public:
	Line():mNumVertices(0), md3dDevice(0), mVB(0), mIB(0) {}

	~Line() {
		ReleaseCOM(mVB);
		ReleaseCOM(mIB);
	}

	void init(ID3D10Device* device, D3DXCOLOR c) {
		md3dDevice = device;
		mNumVertices = 2;

		Vertex vertices[] = {
			{D3DXVECTOR3(0.0f, 0.0f, 0.0f), c},
			{D3DXVECTOR3(1.0f, 0.0f, 0.0f), c}};

		D3D10_BUFFER_DESC vbd;
		vbd.Usage = D3D10_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
		vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = vertices;
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));
	}

	void draw(RenderInfo* ri, Matrix world, Vector4 color) {
		Matrix mWVP = world * ri->mView * ri->mProj;
		ri->mfxWVPVar->SetMatrix((float*)&mWVP);
		ri->mfxWorldVar->SetMatrix((float*)&world);
		ri->mfxColorVar->SetFloatVectorArray(color, 0, 4);
		D3D10_TECHNIQUE_DESC techDesc;
		ri->mTech->GetDesc( &techDesc );
		for(UINT p = 0; p < techDesc.Passes; ++p) {
			ri->mTech->GetPassByIndex( p )->Apply(0);

			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
			md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
			md3dDevice->Draw(2,0);
		}
	}

	float getRadius() { return 0; }
private:
	DWORD mNumVertices;
	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
};

#endif 
