#ifndef TRAMPOLINE_H
#define TRAMPOLINE_H

#include "d3dUtil.h"
#include "Geometry.h"
#include "Vertex.h"

class Trampoline : public Geometry {
public:
	Trampoline(): mNumVertices(0), mNumFaces(0), md3dDevice(0), mVB(0), mIB(0) {}
	~Trampoline() {
		ReleaseCOM(mVB);
		ReleaseCOM(mIB);
	}

	void init(ID3D10Device* device, D3DXCOLOR c) {
		md3dDevice = device;
		mNumVertices = 20;
		mNumFaces    = 20;


		Vertex vertices[] = {
			{D3DXVECTOR3(-3.0f, 0.5f, 0.75f), D3DXVECTOR3(0, 1, 0), c},
			{D3DXVECTOR3(-0.75f, 0.5f, 0.75f), D3DXVECTOR3(0, 1, 0), c},
			{D3DXVECTOR3(0.0f, 0.5f, 3.0f), D3DXVECTOR3(0, 1, 0), c},
			{D3DXVECTOR3(0.75f, 0.5f, 0.75f), D3DXVECTOR3(0, 1, 0), c},
			{D3DXVECTOR3(3.0f, 0.5f, 0.75f), D3DXVECTOR3(0, 1, 0), c},
			{D3DXVECTOR3(1.5f, 0.5f, -0.25f), D3DXVECTOR3(0, 1, 0), c},
			{D3DXVECTOR3(2.0f, 0.5f, -3.0f), D3DXVECTOR3(0, 1, 0), c},
			{D3DXVECTOR3(0.0f, 0.5f, -1.5f), D3DXVECTOR3(0, 1, 0), c},
			{D3DXVECTOR3(-2.0f, 0.5f, -3.0f), D3DXVECTOR3(0, 1, 0), c},

			{D3DXVECTOR3(-1.5f, 0.5f, -0.25f), D3DXVECTOR3(0, 1, 0), c}};

		D3D10_BUFFER_DESC vbd;
		vbd.Usage = D3D10_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
		vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = vertices;
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

		DWORD indices[] = {
			// front face
			1, 2, 3,
			9, 0, 1,

			// back face
			3, 4, 5,
			5, 6, 7,

			// left face
			8, 9, 7,
			7, 3, 5,

			// right face
			9, 3, 7,
			1, 3, 9,



			// front face
			18, 8, 7,
			7, 17, 18,

			// back face
			17, 7, 6,
			6, 16, 17,

			// left face
			6, 5, 15,
			15, 16, 6,

			// right face
			5, 4, 14,
			14, 15, 5,

			9, 8, 18,
			18, 19, 9,

			// back face
			0, 9, 19,
			19, 10, 0,

		};

		D3D10_BUFFER_DESC ibd;
		ibd.Usage = D3D10_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
		ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = indices;
		HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
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
			md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
			md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
			md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);
		}
	}

	float getRadius() { return 1; }

private:
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
};


#endif
