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
			{D3DXVECTOR3(-3.0f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 0 - Top 0
			{D3DXVECTOR3(-0.75f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 1 - Top 1
			{D3DXVECTOR3(0.0f, 0.5f, 3.0f),		D3DXVECTOR3(0, 1, 0), c},	// 2 - Top 2
			{D3DXVECTOR3(0.75f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 3 - Top 3
			{D3DXVECTOR3(3.0f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 4 - Top 4
			{D3DXVECTOR3(1.5f, 0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0), c},	// 5 - Top 5
			{D3DXVECTOR3(2.0f, 0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0), c},	// 6 - Top 6
			{D3DXVECTOR3(0.0f, 0.5f, -1.5f),	D3DXVECTOR3(0, 1, 0), c},	// 7 - Top 7
			{D3DXVECTOR3(-2.0f, 0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0), c},	// 8 - Top 8
			{D3DXVECTOR3(-1.5f, 0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0), c},	// 9 - Top 9
			{D3DXVECTOR3(-3.0f, -0.5f, 0.75f),	D3DXVECTOR3(0, -1, 0), c},	// 10 - Bottom 0
			{D3DXVECTOR3(-0.75f, -0.5f, 0.75f),	D3DXVECTOR3(0, -1, 0), c},	// 11 - Bottom 1
			{D3DXVECTOR3(0.0f, -0.5f, 3.0f),	D3DXVECTOR3(0, -1, 0), c},	// 12 - Bottom 2
			{D3DXVECTOR3(0.75f, -0.5f, 0.75f),	D3DXVECTOR3(0, -1, 0), c},	// 13 - Bottom 3
			{D3DXVECTOR3(3.0f, -0.5f, 0.75f),	D3DXVECTOR3(0, -1, 0), c},	// 14 - Bottom 4
			{D3DXVECTOR3(1.5f, -0.5f, -0.25f),	D3DXVECTOR3(0, -1, 0), c},	// 15 - Bottom 5
			{D3DXVECTOR3(2.0f, -0.5f, -3.0f),	D3DXVECTOR3(0, -1, 0), c},	// 16 - Bottom 6
			{D3DXVECTOR3(0.0f, -0.5f, -1.5f),	D3DXVECTOR3(0, -1, 0), c},	// 17 - Bottom 7
			{D3DXVECTOR3(-2.0f, -0.5f, -3.0f),	D3DXVECTOR3(0, -1, 0), c},	// 18 - Bottom 8
			{D3DXVECTOR3(-1.5f, -0.5f, -0.25f),	D3DXVECTOR3(0, -1, 0), c},	// 19 - Bottom 9

			// TODO: Calculate these normals
			/*	Star layout
					2

			0	1		3	4

				9		5
					7
			8				6

			*/
			{D3DXVECTOR3(-3.0f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 20 - Face Top 0
			{D3DXVECTOR3(-0.75f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 21 - Face Top 1
			{D3DXVECTOR3(-3.0f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 22 - Face Bottom 0
			{D3DXVECTOR3(-0.75f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 23 - Face Bottom 1

			{D3DXVECTOR3(-0.75f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 24 - Face Top 1
			{D3DXVECTOR3(0.0f, 0.5f, 3.0f),		D3DXVECTOR3(0, 1, 0), c},	// 25 - Face Top 2
			{D3DXVECTOR3(-0.75f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 26 - Face Bottom 1
			{D3DXVECTOR3(0.0f, -0.5f, 3.0f),	D3DXVECTOR3(0, 1, 0), c},	// 27 - Face Bottom 2

			{D3DXVECTOR3(0.0f, 0.5f, 3.0f),		D3DXVECTOR3(0, 1, 0), c},	// 28 - Face Top 2
			{D3DXVECTOR3(0.75f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 29 - Face Top 3
			{D3DXVECTOR3(0.0f, -0.5f, 3.0f),	D3DXVECTOR3(0, 1, 0), c},	// 30 - Face Bottom 2
			{D3DXVECTOR3(0.75f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 31 - Face Bottom 3

			{D3DXVECTOR3(0.75f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 32 - Face Top 3
			{D3DXVECTOR3(3.0f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 33 - Face Top 4
			{D3DXVECTOR3(0.75f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 34 - Face Bottom 3
			{D3DXVECTOR3(3.0f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 35 - Face Bottom 4

			{D3DXVECTOR3(3.0f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 36 - Face Top 4
			{D3DXVECTOR3(1.5f, 0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0), c},	// 37 - Face Top 5
			{D3DXVECTOR3(3.0f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 38 - Face Bottom 4
			{D3DXVECTOR3(1.5f, -0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0), c},	// 39 - Face Bottom 5

			{D3DXVECTOR3(1.5f, 0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0), c},	// 40 - Face Top 5
			{D3DXVECTOR3(2.0f, 0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0), c},	// 41 - Face Top 6
			{D3DXVECTOR3(1.5f, -0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0), c},	// 42 - Face Bottom 5
			{D3DXVECTOR3(2.0f, -0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0), c},	// 43 - Face Bottom 6

			{D3DXVECTOR3(2.0f, 0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0), c},	// 44 - Face Top 6
			{D3DXVECTOR3(0.0f, 0.5f, -1.5f),	D3DXVECTOR3(0, 1, 0), c},	// 45 - Face Top 7
			{D3DXVECTOR3(2.0f, -0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0), c},	// 46 - Face Bottom 6
			{D3DXVECTOR3(0.0f, -0.5f, -1.5f),	D3DXVECTOR3(0, 1, 0), c},	// 47 - Face Bottom 7

			{D3DXVECTOR3(0.0f, 0.5f, -1.5f),	D3DXVECTOR3(0, 1, 0), c},	// 48 - Face Top 7
			{D3DXVECTOR3(-2.0f, 0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0), c},	// 49 - Face Top 8
			{D3DXVECTOR3(0.0f, -0.5f, -1.5f),	D3DXVECTOR3(0, 1, 0), c},	// 50 - Face Bottom 7
			{D3DXVECTOR3(-2.0f, -0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0), c},	// 51 - Face Bottom 8

			{D3DXVECTOR3(-2.0f, 0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0), c},	// 52 - Face Top 8
			{D3DXVECTOR3(-1.5f, 0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0), c},	// 53 - Face Top 9
			{D3DXVECTOR3(-2.0f, -0.5f, -3.0f),	D3DXVECTOR3(0, 1, 0), c},	// 54 - Face Bottom 8
			{D3DXVECTOR3(-1.5f, -0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0), c},	// 55 - Face Bottom 9
			
			{D3DXVECTOR3(-1.5f, 0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0), c},	// 56 - Face Top 9
			{D3DXVECTOR3(-3.0f, 0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 57 - Face Top 0
			{D3DXVECTOR3(-1.5f, -0.5f, -0.25f),	D3DXVECTOR3(0, 1, 0), c},	// 58 - Face Bottom 9
			{D3DXVECTOR3(-3.0f, -0.5f, 0.75f),	D3DXVECTOR3(0, 1, 0), c},	// 59 - Face Bottom 0
		};

		// Calculate some surface normals
		for(int i=20; i<60; i+=4) {
			Vector3 p1 = vertices[i].pos;
			Vector3 p2 = vertices[i+1].pos;
			Vector3 p3 = vertices[i+2].pos;

			Vector3 calculatedNormal;
			Vector3 diff1 = p1-p2;
			Vector3 diff2 = p1-p3;
			if(i==52) {		// Weird workaround for some broken normals?
				diff1 = p1-p3;
				diff2 = p1-p2;
			}
			D3DXVec3Cross(&calculatedNormal, &diff1, &diff2);

			for(int j=0;j<4;++j) {
				vertices[i+j].normal = calculatedNormal;
			}
		}

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

	float getRadius() { return 3; }

private:
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
};


#endif
