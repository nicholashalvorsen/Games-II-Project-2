//=======================================================================================
// Line.cpp by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#include "Line.h"
#include "Vertex.h"

Line::Line()
: mNumVertices(0), mNumFaces(0), md3dDevice(0), mVB(0), mIB(0)
{
}
 
Line::~Line()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}

void Line::init(ID3D10Device* device, ID3D10EffectTechnique* new_mTech)
{
	md3dDevice = device;
 
	mNumVertices = 2;
	mNumFaces    = 1;	// 2 per quad

	mTech = new_mTech;

	// Create vertex buffer
	
    Vertex vertices[] =
    {
		{D3DXVECTOR3(+0.0f, +0.0f, +0.0f), BLACK},
		{D3DXVECTOR3(+1.0f, +0.0f, +0.0f), BLACK}
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


	// Create the index buffer

	DWORD indices[] = {
		// It's a freakin' line man
		0, 1
	};

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * mNumFaces * 2;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Line::draw()
{
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
        
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
		md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
		md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);

    }
}

void Line::setColor(D3DXCOLOR c1, D3DXCOLOR c2)
{
	// Release vertex buffer
	ReleaseCOM(mVB);

	// Create vertex buffer

    Vertex vertices[] =
    {
		{D3DXVECTOR3(+0.0f, +0.0f, +0.0f), c1},
		{D3DXVECTOR3(+1.0f, +0.0f, +0.0f), c2}
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

}
void Line::setColor(D3DXCOLOR c)
{
	// Release the vertex buffer
	ReleaseCOM(mVB);

	// Create vertex buffer
	
    Vertex vertices[] =
    {
		{D3DXVECTOR3(+0.0f, +0.0f, +0.0f), c},
		{D3DXVECTOR3(+1.0f, +0.0f, +0.0f), c}
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

}
