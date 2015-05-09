#include "Ship.h"

Ship::Ship()
{
	ZeroMemory(this, sizeof(Ship));
}

Ship::~Ship() {
	ReleaseCOM(mVB);
	ReleaseCOM(mFX);
	ReleaseCOM(mMapArrayRV);
	ReleaseCOM(mVertexLayout);
}

void Ship::setCenters(const D3DXVECTOR3 centers[], const float height, const float width) {
	buildVB(centers, width, height);
}

void Ship::init(ID3D10Device* device, const D3DXVECTOR3 centers[], const float height, const float width, std::wstring filename, UINT num) {
	md3dDevice = device;
	this->num = num;

	buildShaderResourceView(filename);
	buildVB(centers, width, height);
	buildFX();
	buildVertexLayout();
}

void Ship::draw(const Light& L, const D3DXVECTOR3& eyePosW, const D3DXMATRIX& viewProj)
{
	mfxEyePosVar->SetRawValue((void*)&eyePosW, 0, sizeof(D3DXVECTOR3));
	mfxLightVar->SetRawValue((void*)&L, 0, sizeof(Light));
	mfxViewProjVar->SetMatrix((float*)&viewProj);
	mfxMapArrayVar->SetResource(mMapArrayRV);

	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	md3dDevice->IASetInputLayout(mVertexLayout);

	D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );

    for(UINT i = 0; i < techDesc.Passes; ++i)
    {
        ID3D10EffectPass* pass = mTech->GetPassByIndex(i);
		pass->Apply(0);

		UINT stride = sizeof(ShipVertex);
		UINT offset = 0;
		md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dDevice->Draw(num, 0);
	}
}

void Ship::buildVB(const D3DXVECTOR3 centers[], const float width, const float height)
{
	ShipVertex* v = new ShipVertex[num];

	for(UINT i = 0; i < num; ++i)
	{
		v[i].centerW  = centers[i];
		v[i].sizeW    = D3DXVECTOR2(width, height);
	}
     
    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(ShipVertex) * num;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = v;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	delete[] v;
}

void Ship::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
  
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"shipBillboard.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("BillboardTech");
	
	mfxViewProjVar     = mFX->GetVariableByName("gViewProj")->AsMatrix();
	mfxEyePosVar       = mFX->GetVariableByName("gEyePosW");
	mfxLightVar        = mFX->GetVariableByName("gLight");
	mfxMapArrayVar = mFX->GetVariableByName("gDiffuseMapArray")->AsShaderResource();
}

void Ship::buildVertexLayout()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 2, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}

void Ship::buildShaderResourceView(std::wstring filename)
{
	ID3D10Texture2D* srcTex[1];
	D3DX10_IMAGE_LOAD_INFO loadInfo;

    loadInfo.Width  = D3DX10_FROM_FILE;
    loadInfo.Height = D3DX10_FROM_FILE;
    loadInfo.Depth  = D3DX10_FROM_FILE;
    loadInfo.FirstMipLevel = 0;
    loadInfo.MipLevels = D3DX10_FROM_FILE;
    loadInfo.Usage = D3D10_USAGE_STAGING;
    loadInfo.BindFlags = 0;
    loadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
    loadInfo.MiscFlags = 0;
    loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    loadInfo.Filter = D3DX10_FILTER_NONE;
    loadInfo.MipFilter = D3DX10_FILTER_NONE;
	loadInfo.pSrcInfo  = 0;

    HR(D3DX10CreateTextureFromFile(md3dDevice, filename.c_str(), &loadInfo, 0, (ID3D10Resource**)&srcTex[0], 0));

	//
	// Create the texture array.  Each element in the texture 
	// array has the same format/dimensions.
	//

	D3D10_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);

	D3D10_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width              = texElementDesc.Width;
	texArrayDesc.Height             = texElementDesc.Height;
	texArrayDesc.MipLevels          = texElementDesc.MipLevels;
	texArrayDesc.ArraySize          = 1;
	texArrayDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	texArrayDesc.SampleDesc.Count   = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage              = D3D10_USAGE_DEFAULT;
	texArrayDesc.BindFlags          = D3D10_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags     = 0;
	texArrayDesc.MiscFlags          = 0;

	ID3D10Texture2D* texArray = 0;
	HR(md3dDevice->CreateTexture2D( &texArrayDesc, 0, &texArray));

	//
	// Copy individual texture elements into texture array.
	//

	// for each mipmap level...
	for(UINT j = 0; j < texElementDesc.MipLevels; ++j)
	{
		D3D10_MAPPED_TEXTURE2D mappedTex2D;
		srcTex[0]->Map(j, D3D10_MAP_READ, 0, &mappedTex2D);
                    
        md3dDevice->UpdateSubresource(texArray, 
			D3D10CalcSubresource(j, 0, texElementDesc.MipLevels),
            0, mappedTex2D.pData, mappedTex2D.RowPitch, 0);

        srcTex[0]->Unmap(j);
	}

	//
	// Create a resource view to the texture array.
	//
	
	D3D10_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = 1;

	HR(md3dDevice->CreateShaderResourceView(texArray, &viewDesc, &mMapArrayRV));

	//
	// Cleanup--we only need the resource view.
	//

	ReleaseCOM(texArray);
	ReleaseCOM(srcTex[0]);
}