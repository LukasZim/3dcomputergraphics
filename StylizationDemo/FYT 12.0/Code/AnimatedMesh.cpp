#include "DXUT.h"
#include "AnimatedMesh.h"
#include "definition.h"

AnimatedMesh::AnimatedMesh()
{
	LoadAnimatedMesh();
	CreateBoneTextureAndBuffer();
	g_vLightPos = D3DXVECTOR3( 159.47f, 74.23f, 103.60f ); //for animated model

}

AnimatedMesh::~AnimatedMesh()
{
	DeleteMesh();
}

void AnimatedMesh::DeleteMesh()
{
	SAFE_RELEASE( g_pBoneBuffer );
	SAFE_RELEASE( g_pBoneBufferRV );
	SAFE_RELEASE( g_pBoneTexture );
	SAFE_RELEASE( g_pBoneTextureRV );
	//back
	for( UINT m = 0; m < g_SkinnedMesh.GetNumMeshes(); m++ )
		SAFE_RELEASE( g_ppTransformedVBs[m] );
	for( UINT m = 0; m < g_SkinnedMesh.GetNumMeshes(); m++ )
		SAFE_RELEASE( g_ppPreTransformedVBs[m] );
	
	SAFE_DELETE_ARRAY( g_ppTransformedVBs );
	SAFE_DELETE_ARRAY( g_ppPreTransformedVBs );
	
	g_SkinnedMesh.Destroy();
}

void AnimatedMesh::LoadAnimatedMesh()
{
	switch(stylization->SelectedAnimatedModel)
	{
		case 0:
			g_SkinnedMesh.Create( DXUTGetD3D10Device(), L"Model\\Soldier\\soldier.sdkmesh", true );
			g_SkinnedMesh.LoadAnimation( L"Model\\Soldier\\soldier.sdkmesh_anim" );
			break;
		case 1:
			g_SkinnedMesh.Create( DXUTGetD3D10Device(), L"Model\\Lizard\\lizardSIG.sdkmesh", true );
			g_SkinnedMesh.LoadAnimation( L"Model\\Lizard\\lizardSIG.sdkmesh_anim" );
			break;
	}

	string styleName = style->animatedModelStyleName[stylization->SelectedAnimatedModel];
	style->LoadStyle(styleName);
	//warier isn't working here
	D3DXMATRIX mIdentity;
	D3DXMatrixIdentity( &mIdentity );
	g_SkinnedMesh.TransformBindPose( &mIdentity );
}

void AnimatedMesh::TransformMesh(D3DXMATRIX mIdentity, double fTime)
{
	g_SkinnedMesh.TransformMesh( &mIdentity, fTime );
}

HRESULT AnimatedMesh::CreateBoneTextureAndBuffer()
{
	HRESULT hr = S_OK;
	// Create a bone texture
    // It will be updated more than once per frame (in a typical game) so make it dynamic
    D3D10_TEXTURE1D_DESC dstex;
    dstex.Width = MAX_BONE_MATRICES * 4;
    dstex.MipLevels = 1;
    dstex.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    dstex.Usage = D3D10_USAGE_DYNAMIC;
    dstex.BindFlags = D3D10_BIND_SHADER_RESOURCE;
    dstex.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    dstex.MiscFlags = 0;
    dstex.ArraySize = 1;
    V_RETURN( DXUTGetD3D10Device()->CreateTexture1D( &dstex, NULL, &g_pBoneTexture ) );

    // Create the resource view for the bone texture
    D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
    ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
    SRVDesc.Format = dstex.Format;
    SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE1D;
    SRVDesc.Texture2D.MipLevels = dstex.MipLevels;
    V_RETURN( DXUTGetD3D10Device()->CreateShaderResourceView( g_pBoneTexture, &SRVDesc, &g_pBoneTextureRV ) );

	D3D10_BUFFER_DESC vbdesc =
    {
        MAX_BONE_MATRICES * sizeof( D3DXMATRIX ),
        D3D10_USAGE_DYNAMIC,
        D3D10_BIND_SHADER_RESOURCE,
        D3D10_CPU_ACCESS_WRITE,
        0
    };
    V_RETURN( DXUTGetD3D10Device()->CreateBuffer( &vbdesc, NULL, &g_pBoneBuffer ) );

    // Again, we need a resource view to use it in the shader
    ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
    SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_BUFFER;
    SRVDesc.Buffer.ElementOffset = 0;
    SRVDesc.Buffer.ElementWidth = MAX_BONE_MATRICES * 4;
    V_RETURN( DXUTGetD3D10Device()->CreateShaderResourceView( g_pBoneBuffer, &SRVDesc, &g_pBoneBufferRV ) );

    // Create VBs that will hold all of the skinned vertices that need to be streamed out
    g_ppTransformedVBs = new ID3D10Buffer*[ g_SkinnedMesh.GetNumMeshes() ];
	g_ppPreTransformedVBs = new ID3D10Buffer*[ g_SkinnedMesh.GetNumMeshes() ]; 
    if( !g_ppTransformedVBs )
        return E_OUTOFMEMORY;
	if ( !g_ppPreTransformedVBs ) 
		return E_OUTOFMEMORY;

    for( UINT m = 0; m < g_SkinnedMesh.GetNumMeshes(); m++ )
    {
        UINT iStreamedVertexCount = ( UINT )g_SkinnedMesh.GetNumVertices( m, 0 );
        D3D10_BUFFER_DESC vbdescSO =
        {
            iStreamedVertexCount * sizeof( STREAM_OUT_VERTEX ),
            D3D10_USAGE_DEFAULT,
            D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT,
            0,
            0
        };
        V_RETURN( DXUTGetD3D10Device()->CreateBuffer( &vbdescSO, NULL, &g_ppTransformedVBs[m] ) );
		V_RETURN( DXUTGetD3D10Device()->CreateBuffer( &vbdescSO, NULL, &g_ppPreTransformedVBs[m] ) );
    }

	return hr;
}

float AnimatedMesh::MatrixNorm(D3DXMATRIX m)
{
	float sum = 0;
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			sum += m(i,j) * m(i,j);
		}
	}
	
	return sqrt(sum);
}

void AnimatedMesh::GetInstanceWorldMatrix( UINT iInstance, D3DXMATRIX* pmWorld )
{
    int iSide = ( UINT )sqrtf( ( float )1 ); //g_iInstances

    int iX = iInstance % iSide;
    int iZ = iInstance / iSide;

    float xStart = -iSide * 0.25f;
    float zStart = -iSide * 0.25f;

    D3DXMatrixTranslation( pmWorld, xStart + iX * 0.5f, 0, zStart + iZ * 0.5f );
}

void AnimatedMesh::SwapBuffers()
{
	//swap the current and previous buffers
	int numMeshes = g_SkinnedMesh.GetNumMeshes();
	for( UINT m = 0; m < numMeshes; m++ )
	{
		ID3D10Buffer* pTemp = g_ppPreTransformedVBs[m];
		g_ppPreTransformedVBs[m] = g_ppTransformedVBs[m];
		g_ppTransformedVBs[m] = pTemp;
	}
}

void AnimatedMesh::RenderModelIntoTexture(int RenderTargetIndex)
{
	//********************************************************************************************
	//Render the 3D Scene into texture
	//********************************************************************************************
	ID3D10Buffer* pBuffers[1];
    UINT stride[1];
    UINT offset[1] = { 0 };

	textureRT->ClearRenderTarget(RenderTargetIndex, ClearCanvas);
	textureRT->ClearDepthStencil();

	int numMeshes = g_SkinnedMesh.GetNumMeshes();
    for( UINT m = 0; m < numMeshes; m++ )
    {
        // Set IA parameters
        pBuffers[0] = g_ppTransformedVBs[m];
        stride[0] = sizeof( STREAM_OUT_VERTEX );
        DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
        DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
        DXUTGetD3D10Device()->IASetIndexBuffer( g_SkinnedMesh.GetIB10( m ), g_SkinnedMesh.GetIBFormat10( m ), 0 );

        // Set vertex Layout
        DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetTransformedVertexLayout() );

        // Set materials
        SDKMESH_SUBSET* pSubset = NULL;
        SDKMESH_MATERIAL* pMat = NULL;

		int NumSubsets = g_SkinnedMesh.GetNumSubsets( m );
        for( UINT subset = 0; subset < NumSubsets; subset++ )
        {
            pSubset = g_SkinnedMesh.GetSubset( m, subset );

            pMat = g_SkinnedMesh.GetMaterial( pSubset->MaterialID );
            if( pMat )
            {
                shader->g_ptxDiffuse1->SetResource( pMat->pDiffuseRV10 );
                shader->g_ptxNormal1->SetResource( pMat->pNormalRV10 );
            }

			UINT indexCount = ( UINT )pSubset->IndexCount;
			UINT indexStart = ( UINT )pSubset->IndexStart;
			UINT VertexStart = ( UINT )pSubset->VertexStart;

			textureRT->NullShaderResource(RenderTargetIndex);
			textureRT->SetRenderTargetStencil(RenderTargetIndex);
            shader->GetRenderPostTransformed()->GetPassByIndex( 0 )->Apply( 0 );
            //DXUTGetD3D10Device()->DrawIndexed( ( UINT )pSubset->IndexCount, ( UINT )pSubset->IndexStart,( UINT )pSubset->VertexStart );
			DXUTGetD3D10Device()->DrawIndexed( indexCount, indexStart,VertexStart );
			DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
			textureRT->SetRTShaderView(RenderTargetIndex);
			
        }
    }
}

void AnimatedMesh::RenderOpticalFlow(bool ClearRenderTarget)
{
	ID3D10Buffer* pBuffers1[2];
	UINT stride1[2];
	UINT offset1[2] = {0, 0};

	textureRT->ClearDepthStencil();
	if (ClearRenderTarget)
		textureRT->ClearRenderTarget(RENDER_TARGET_FlOW, InitialOpticalFlowValues);	
	int numMeshes = g_SkinnedMesh.GetNumMeshes();
	for( UINT m = 0; m < numMeshes; m++ )
    {
        // Set IA parameters
        pBuffers1[0] = g_ppTransformedVBs[m];
		pBuffers1[1] = g_ppPreTransformedVBs[m];
        stride1[0] = sizeof( STREAM_OUT_VERTEX );
		stride1[1] = sizeof( STREAM_OUT_VERTEX );

        DXUTGetD3D10Device()->IASetVertexBuffers( 0, 2, pBuffers1, stride1, offset1 );
        DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
        DXUTGetD3D10Device()->IASetIndexBuffer( g_SkinnedMesh.GetIB10( m ), g_SkinnedMesh.GetIBFormat10( m ), 0 );

        // Set vertex Layout
        DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetOpticalFlowVertexLayout() );

        // Set materials
        SDKMESH_SUBSET* pSubset = NULL;
        SDKMESH_MATERIAL* pMat = NULL;

		int NumSubsets = g_SkinnedMesh.GetNumSubsets( m );
        for( UINT subset = 0; subset < NumSubsets; subset++ )
        {
            pSubset = g_SkinnedMesh.GetSubset( m, subset );

            pMat = g_SkinnedMesh.GetMaterial( pSubset->MaterialID );
            if( pMat )
            {
                shader->g_ptxDiffuse1->SetResource( pMat->pDiffuseRV10 );
                shader->g_ptxNormal1->SetResource( pMat->pNormalRV10 );
            }

			UINT indexCount = ( UINT )pSubset->IndexCount;
			UINT indexStart = ( UINT )pSubset->IndexStart;
			UINT VertexStart = ( UINT )pSubset->VertexStart;

			textureRT->NullShaderResource(RENDER_TARGET_FlOW);
			textureRT->SetRenderTargetStencil(RENDER_TARGET_FlOW);
			shader->GetComputeOpticalFlow1()->GetPassByIndex( 0 )->Apply( 0 );                
			DXUTGetD3D10Device()->DrawIndexed( indexCount, indexStart,VertexStart );
			DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);

			textureRT->SetRTShaderView(RENDER_TARGET_FlOW);

        }
    }
}

void AnimatedMesh::RenderModelMatte()
{
	//********************************************************************************************
	//Render model matte into texture
	//********************************************************************************************
	ID3D10Buffer* pBuffers[1];
    UINT stride[1];
    UINT offset[1] = { 0 };

	int numMeshes = g_SkinnedMesh.GetNumMeshes();
	textureRT->ClearDepthStencil();
	textureRT->ClearRenderTarget(RENDER_TARGET_MODELMATTE, ClearColor);	
	for( UINT m = 0; m < numMeshes; m++ )
    {
        // Set IA parameters
        pBuffers[0] = g_ppTransformedVBs[m];
        stride[0] = sizeof( STREAM_OUT_VERTEX );
        DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
        DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
        DXUTGetD3D10Device()->IASetIndexBuffer( g_SkinnedMesh.GetIB10( m ), g_SkinnedMesh.GetIBFormat10( m ), 0 );

        // Set vertex Layout
        DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetTransformedVertexLayout() );

        // Set materials
        SDKMESH_SUBSET* pSubset = NULL;
        SDKMESH_MATERIAL* pMat = NULL;

		int NumSubsets = g_SkinnedMesh.GetNumSubsets( m );
        for( UINT subset = 0; subset < NumSubsets; subset++ )
        {
            pSubset = g_SkinnedMesh.GetSubset( m, subset );

            pMat = g_SkinnedMesh.GetMaterial( pSubset->MaterialID );
            if( pMat )
            {
                shader->g_ptxDiffuse1->SetResource( pMat->pDiffuseRV10 );
                shader->g_ptxNormal1->SetResource( pMat->pNormalRV10 );
            }

			UINT indexCount = ( UINT )pSubset->IndexCount;
			UINT indexStart = ( UINT )pSubset->IndexStart;
			UINT VertexStart = ( UINT )pSubset->VertexStart;

			textureRT->NullShaderResource(RENDER_TARGET_MODELMATTE);
			textureRT->SetRenderTargetStencil(RENDER_TARGET_MODELMATTE);
			shader->GetRenderModelMatte()->GetPassByIndex( 0 )->Apply( 0 );                
			DXUTGetD3D10Device()->DrawIndexed( indexCount, indexStart,VertexStart );
			DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
			textureRT->SetRTShaderView(RENDER_TARGET_MODELMATTE);
        }
    }
}

void AnimatedMesh::StreamOutBones(int VBs)
{
	
	ID3D10EffectTechnique* pTech = shader->GetRenderConstantBuffer_SO();
	ID3D10Buffer* pBuffers[1];
    UINT stride[1];
    UINT offset[1] = { 0 };

        //***************************************************************************
        // Skin the vertices and stream them out
        //***************************************************************************
		//stream the exact same thing into two buffers for now
        for( UINT m = 0; m < g_SkinnedMesh.GetNumMeshes(); m++ )
        {
			//**************************************************************************
			//stream into current buffer
			//**************************************************************************
            // Turn on stream out
			if (VBs == 0)
				pBuffers[0] = g_ppTransformedVBs[m];
			else
				pBuffers[0] = g_ppPreTransformedVBs[m];
            DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

            // Set vertex Layout
            DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetSkinnedVertexLayout() );

            // Set source vertex buffer
            pBuffers[0] = g_SkinnedMesh.GetVB10( m, 0 );
            stride[0] = g_SkinnedMesh.GetVertexStride( m, 0 );
            DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
            DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );	// When skinning vertices, we don't care about topology.
            // Treat the entire vertex buffer as list of points.

            // Set the bone matrices
            SetBoneMatrices( m );

            // Render the vertices as an array of points
            D3D10_TECHNIQUE_DESC techDesc;
            pTech->GetDesc( &techDesc );
            for( UINT p = 0; p < techDesc.Passes; ++p )
            {
                pTech->GetPassByIndex( p )->Apply( 0 );
                DXUTGetD3D10Device()->Draw( ( UINT )g_SkinnedMesh.GetNumVertices( m, 0 ), 0 );
            }
		}

		// Turn off stream out
		pBuffers[0] = NULL;
		DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );
}

//--------------------------------------------------------------------------------------
void AnimatedMesh::SetBoneMatrices( UINT iMesh )
{
	for( UINT i = 0; i < g_SkinnedMesh.GetNumInfluences( iMesh ); i++ )
    {
        const D3DXMATRIX* pMat = g_SkinnedMesh.GetMeshInfluenceMatrix( iMesh, i );
        shader->g_pmConstBoneWorld->SetMatrixArray( ( float* )pMat, i, 1 );
    }
}
