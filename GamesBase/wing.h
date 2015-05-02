
#ifndef WING_H
#define WING_H

#include "d3dUtil.h"
#include "Geometry.h"
#include "Vertex.h"

class Wing : public Geometry {
public:
	Wing(): mNumVertices(0), mNumFaces(0), md3dDevice(0), mVB(0), mIB(0) {}
	~Wing() {
		ReleaseCOM(mVB);
		ReleaseCOM(mIB);
	}

	void init(ID3D10Device* device) {
		md3dDevice = device;
		mNumVertices = 4;
		mNumFaces    = 4;

		Vertex vertices[] = {
			{D3DXVECTOR3(0.0f, 0.0f , 0.0f), D3DXVECTOR3(0, 1, 0)},
			{D3DXVECTOR3(1.75f, 0.0f, -0.75f), D3DXVECTOR3(0, 1, 0)},
			{D3DXVECTOR3(2.75f, 0.0f, -3.0f), D3DXVECTOR3(0, 1, 0)},
			{D3DXVECTOR3(0.0f, 0.0f, -4.0f), D3DXVECTOR3(0, 1, 0)}};

		for(int i =0; i < 4; i++){
			vertices[i].normal = Vector3(0, 1 + .1*i, 0);
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
			// front face
			1, 2, 3,
			2, 3, 4,
			

			//Back face
			3, 2, 1, 
			4, 3, 2
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
			md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
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

