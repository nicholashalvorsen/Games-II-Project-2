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

	void init(ID3D10Device* device) {
		md3dDevice = device;
		mNumVertices = 60;
		mNumFaces    = 36;

		/*	Star layout
				2

		0	1		3	4

			9		5
				7
		8				6

		*/
		Vertex vertices[] = {
			{D3DXVECTOR3(-3.0f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 0 - Top 0
			{D3DXVECTOR3(-0.75f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 1 - Top 1
			{D3DXVECTOR3(0.0f, 0.5f, 3.0f),		D3DXVECTOR3(0, 1, 0)},	// 2 - Top 2
			{D3DXVECTOR3(0.75f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 3 - Top 3
			{D3DXVECTOR3(3.0f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 4 - Top 4
			{D3DXVECTOR3(1.5f, 0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0)},	// 5 - Top 5
			{D3DXVECTOR3(2.0f, 0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0)},	// 6 - Top 6
			{D3DXVECTOR3(0.0f, 0.5f, -1.5f),	D3DXVECTOR3(0, 1, 0)},	// 7 - Top 7
			{D3DXVECTOR3(-2.0f, 0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0)},	// 8 - Top 8
			{D3DXVECTOR3(-1.5f, 0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0)},	// 9 - Top 9
			{D3DXVECTOR3(-3.0f, -0.5f, 0.75f),	D3DXVECTOR3(0, -1, 0)},	// 10 - Bottom 0
			{D3DXVECTOR3(-0.75f, -0.5f, 0.75f),	D3DXVECTOR3(0, -1, 0)},	// 11 - Bottom 1
			{D3DXVECTOR3(0.0f, -0.5f, 3.0f),	D3DXVECTOR3(0, -1, 0)},	// 12 - Bottom 2
			{D3DXVECTOR3(0.75f, -0.5f, 0.75f),	D3DXVECTOR3(0, -1, 0)},	// 13 - Bottom 3
			{D3DXVECTOR3(3.0f, -0.5f, 0.75f),	D3DXVECTOR3(0, -1, 0)},	// 14 - Bottom 4
			{D3DXVECTOR3(1.5f, -0.5f, -0.25f),	D3DXVECTOR3(0, -1, 0)},	// 15 - Bottom 5
			{D3DXVECTOR3(2.0f, -0.5f, -3.0f),	D3DXVECTOR3(0, -1, 0)},	// 16 - Bottom 6
			{D3DXVECTOR3(0.0f, -0.5f, -1.5f),	D3DXVECTOR3(0, -1, 0)},	// 17 - Bottom 7
			{D3DXVECTOR3(-2.0f, -0.5f, -3.0f),	D3DXVECTOR3(0, -1, 0)},	// 18 - Bottom 8
			{D3DXVECTOR3(-1.5f, -0.5f, -0.25f),	D3DXVECTOR3(0, -1, 0)},	// 19 - Bottom 9

			// TODO: Calculate these normals
			/*	Star layout
					2

			0	1		3	4

				9		5
					7
			8				6

			*/
			{D3DXVECTOR3(-3.0f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 20 - Face Top 0
			{D3DXVECTOR3(-0.75f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 21 - Face Top 1
			{D3DXVECTOR3(-3.0f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 22 - Face Bottom 0
			{D3DXVECTOR3(-0.75f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 23 - Face Bottom 1

			{D3DXVECTOR3(-0.75f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 24 - Face Top 1
			{D3DXVECTOR3(0.0f, 0.5f, 3.0f),		D3DXVECTOR3(0, 1, 0)},	// 25 - Face Top 2
			{D3DXVECTOR3(-0.75f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 26 - Face Bottom 1
			{D3DXVECTOR3(0.0f, -0.5f, 3.0f),	D3DXVECTOR3(0, 1, 0)},	// 27 - Face Bottom 2

			{D3DXVECTOR3(0.0f, 0.5f, 3.0f),		D3DXVECTOR3(0, 1, 0)},	// 28 - Face Top 2
			{D3DXVECTOR3(0.75f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 29 - Face Top 3
			{D3DXVECTOR3(0.0f, -0.5f, 3.0f),	D3DXVECTOR3(0, 1, 0)},	// 30 - Face Bottom 2
			{D3DXVECTOR3(0.75f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 31 - Face Bottom 3

			{D3DXVECTOR3(0.75f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 32 - Face Top 3
			{D3DXVECTOR3(3.0f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 33 - Face Top 4
			{D3DXVECTOR3(0.75f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 34 - Face Bottom 3
			{D3DXVECTOR3(3.0f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 35 - Face Bottom 4

			{D3DXVECTOR3(3.0f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 36 - Face Top 4
			{D3DXVECTOR3(1.5f, 0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0)},	// 37 - Face Top 5
			{D3DXVECTOR3(3.0f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 38 - Face Bottom 4
			{D3DXVECTOR3(1.5f, -0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0)},	// 39 - Face Bottom 5

			{D3DXVECTOR3(1.5f, 0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0)},	// 40 - Face Top 5
			{D3DXVECTOR3(2.0f, 0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0)},	// 41 - Face Top 6
			{D3DXVECTOR3(1.5f, -0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0)},	// 42 - Face Bottom 5
			{D3DXVECTOR3(2.0f, -0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0)},	// 43 - Face Bottom 6

			{D3DXVECTOR3(2.0f, 0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0)},	// 44 - Face Top 6
			{D3DXVECTOR3(0.0f, 0.5f, -1.5f),	D3DXVECTOR3(0, 1, 0)},	// 45 - Face Top 7
			{D3DXVECTOR3(2.0f, -0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0)},	// 46 - Face Bottom 6
			{D3DXVECTOR3(0.0f, -0.5f, -1.5f),	D3DXVECTOR3(0, 1, 0)},	// 47 - Face Bottom 7

			{D3DXVECTOR3(0.0f, 0.5f, -1.5f),	D3DXVECTOR3(0, 1, 0)},	// 48 - Face Top 7
			{D3DXVECTOR3(-2.0f, 0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0)},	// 49 - Face Top 8
			{D3DXVECTOR3(0.0f, -0.5f, -1.5f),	D3DXVECTOR3(0, 1, 0)},	// 50 - Face Bottom 7
			{D3DXVECTOR3(-2.0f, -0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0)},	// 51 - Face Bottom 8

			{D3DXVECTOR3(-2.0f, 0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0)},	// 52 - Face Top 8
			{D3DXVECTOR3(-1.5f, 0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0)},	// 53 - Face Top 9
			{D3DXVECTOR3(-2.0f, -0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0)},	// 54 - Face Bottom 8
			{D3DXVECTOR3(-1.5f, -0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0)},	// 55 - Face Bottom 9
			
			{D3DXVECTOR3(-1.5f, 0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0)},	// 56 - Face Top 9
			{D3DXVECTOR3(-3.0f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 57 - Face Top 0
			{D3DXVECTOR3(-1.5f, -0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0)},	// 58 - Face Bottom 9
			{D3DXVECTOR3(-3.0f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0)},	// 59 - Face Bottom 0
		};

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
			// DONE: Proper indices
			// Top face:
			1, 3, 9,
			9, 3, 5,
			9, 5, 7,
			9, 0, 1,
			1, 2, 3,
			3, 4, 5,
			5, 6, 7,
			7, 8, 9,

			// Bottom face:
			13, 11, 15,
			15, 11, 19,
			15, 19, 17,
			15, 14, 13,
			13, 12, 11,
			11, 10, 19,
			19, 18, 17,
			17, 16, 15,

			// Side faces:
			21, 20, 22,
			21, 22, 23,

			25, 24, 26,
			25, 26, 27,

			29, 28, 30,
			29, 30, 31,

			33, 32, 34,
			33, 34, 35,

			37, 36, 38,
			37, 38, 39,

			41, 40, 42,
			41, 42, 43,

			45, 44, 46,
			45, 46, 47,

			49, 48, 50,
			49, 50, 51,

			53, 52, 54,
			53, 54, 55,

			57, 56, 58,
			57, 58, 59,
			
		};

		/*	Star layout
				2

		0	1		3	4

			9		5
				7
		8				6

		*/

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

	void draw(RenderInfo* ri, Matrix world, Vector4 color = Vector4(0, 0, 0, 1), Vector4 spec = Vector4(0, 0, 0, 0)) {
		Matrix mWVP = world * ri->mView * ri->mProj;
		ri->mfxWVPVar->SetMatrix((float*)&mWVP);
		ri->mfxWorldVar->SetMatrix((float*)&world);
		ri->mfxColorVar->SetFloatVectorArray(color, 0, 4);
		ri->mfxSpecVar->SetFloatVectorArray(spec, 0, 4);
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

	float getRadius() { return 3; }

private:
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
};


#endif
