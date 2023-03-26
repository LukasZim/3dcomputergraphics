#include "DXUT.h"
#include "definition.h"

extern TextureRT* textureRT;
extern InputLayout* inputLayout;
extern Shader* shader;
extern Camera* camera;

ObjMesh::ObjMesh()
{
	LoadObjScene();
}

ObjMesh::~ObjMesh()
{
	DeleteMesh();
}

void ObjMesh::DeleteMesh()
{
	g_MeshLoader.Destroy();
}

HRESULT ObjMesh::LoadObjScene()
{

	HRESULT hr = S_OK;
	switch(stylization->SelectedObjModel)
	{
		case 0:			
			V_RETURN( g_MeshLoader.Create( DXUTGetD3D10Device(), L"Model\\Cup\\cup.obj" ) );
			break;
		case 1:
			V_RETURN( g_MeshLoader.Create( DXUTGetD3D10Device(), L"Model\\clock\\clock.obj" ) );
			break;
		case 2:
			V_RETURN( g_MeshLoader.Create( DXUTGetD3D10Device(), L"Model\\bench\\bench.obj" ) );
			break;
	}   

	string styleName = style->objModelStyleName[stylization->SelectedObjModel];
	style->LoadStyle(styleName);
	

	// Store the correct technique for each material
    for ( UINT i = 0; i < g_MeshLoader.GetNumMaterials(); ++i )
    {
        Material* pMaterial = g_MeshLoader.GetMaterial( i );

        const char* strTechnique = "";

        if( pMaterial->pTextureRV10 && pMaterial->bSpecular )
            strTechnique = "TexturedSpecular";
        else if( pMaterial->pTextureRV10 && !pMaterial->bSpecular )
            strTechnique = "TexturedNoSpecular";
        else if( !pMaterial->pTextureRV10 && pMaterial->bSpecular )
            strTechnique = "Specular";
        else if( !pMaterial->pTextureRV10 && !pMaterial->bSpecular )
            strTechnique = "NoSpecular";

        pMaterial->pTechnique = shader->GetObjModelEffect()->GetTechniqueByName( strTechnique );
		pMaterial->pTechnique1 = shader->GetObjModelEffect()->GetTechniqueByName( "RenderSceneOpticalFlow" );
    }
	return hr;
}

void ObjMesh::RenderSubset( UINT iSubset )
{
    HRESULT hr;
    
    Material* pMaterial = g_MeshLoader.GetSubsetMaterial( iSubset );

    V( shader->g_pAmbient->SetFloatVector( pMaterial->vAmbient ) );
    V( shader->g_pDiffuse->SetFloatVector( pMaterial->vDiffuse ) );
    V( shader->g_pSpecular->SetFloatVector( pMaterial->vSpecular ) );
    V( shader->g_pOpacity->SetFloat( pMaterial->fAlpha ) );
    V( shader->g_pSpecularPower->SetInt( pMaterial->nShininess ) );

    if ( !IsErrorResource( pMaterial->pTextureRV10 ) )
        shader->g_pMeshTextureVariable->SetResource( pMaterial->pTextureRV10 );

    D3D10_TECHNIQUE_DESC techDesc;
    pMaterial->pTechnique->GetDesc( &techDesc );

    for ( UINT p = 0; p < techDesc.Passes; ++p )
    {
        pMaterial->pTechnique->GetPassByIndex(p)->Apply(0);
        g_MeshLoader.GetMesh()->DrawSubset(iSubset);
    }
}


void ObjMesh::RenderOpticalFlow( UINT iSubset )
{
    
    Material* pMaterial = g_MeshLoader.GetSubsetMaterial( iSubset );

    if ( !IsErrorResource( pMaterial->pTextureRV10 ) )
        shader->g_pMeshTextureVariable->SetResource( pMaterial->pTextureRV10 );

    D3D10_TECHNIQUE_DESC techDesc;
    pMaterial->pTechnique1->GetDesc( &techDesc );

    for ( UINT p = 0; p < techDesc.Passes; ++p )
    {
        pMaterial->pTechnique1->GetPassByIndex(p)->Apply(0);
        g_MeshLoader.GetMesh()->DrawSubset(iSubset);
    }
}


void ObjMesh::RenderObjMesh()
{
    HRESULT hr;
    D3DXMATRIXA16 mWorld;
    D3DXMATRIXA16 mView;
    D3DXMATRIXA16 mProj;
    D3DXMATRIXA16 mWorldViewProjection;

    // Get the projection & view matrix from the camera class
    mWorld = *camera->GetCamera().GetWorldMatrix();
    mView = *camera->GetCamera().GetViewMatrix();
    mProj = *camera->GetCamera().GetProjMatrix();

    mWorldViewProjection = mWorld * mView * mProj;

    // Update the effect's variables. 
    V( shader->g_pWorldViewProjection1->SetMatrix( (float*)&mWorldViewProjection ) );
    V( shader->g_pWorld1->SetMatrix( (float*)&mWorld ) );    
    V( shader->g_pCameraPosition->SetFloatVector( (float*)camera->GetCamera().GetEyePt() ) );  

	
	static D3DXMATRIX mPreWorldViewProjection = mWorldViewProjection;
	D3DXMATRIX diff = mWorldViewProjection - mPreWorldViewProjection;
	bool bUpdateParticles = (animatedMesh->MatrixNorm(diff) > 0.0001);

	shader->g_pPreWorldViewProjection1->SetMatrix( (float*)&mPreWorldViewProjection );

	mPreWorldViewProjection = mWorldViewProjection; //update the previous camera position


	//Render the model into texture
    DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetObjMeshVertexlayout() );

	textureRT->NullShaderResource(RENDER_TARGET_COLOR2);	
	textureRT->ClearDepthStencil();
	textureRT->ClearRenderTarget(RENDER_TARGET_COLOR2, ClearCanvas);
	textureRT->SetRenderTargetStencil(RENDER_TARGET_COLOR2);
   
    for ( UINT iSubset = 0; iSubset < g_MeshLoader.GetNumSubsets(); ++iSubset )
    {
        RenderSubset( iSubset );
    }

	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
	textureRT->SetRTShaderView(RENDER_TARGET_COLOR2);	


	//render the optical flow into texture
	textureRT->NullShaderResource(RENDER_TARGET_FlOW);	
	textureRT->ClearDepthStencil();
	textureRT->ClearRenderTarget(RENDER_TARGET_FlOW, InitialOpticalFlowValues);
	textureRT->SetRenderTargetStencil(RENDER_TARGET_FlOW);
   
    for ( UINT iSubset = 0; iSubset < g_MeshLoader.GetNumSubsets(); ++iSubset )
    {
        RenderOpticalFlow( iSubset );
    }

	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
	textureRT->SetRTShaderView(RENDER_TARGET_FlOW);	


	inputLayout->EnableQuadVertexLayout();

	//not working here
	if (renderer->OnOpticalFlow)	
	{
		if (renderer->OnOpticalFlowTest)
			shader->RenderOpticalFlowTest();
		else
			shader->RenderOpticalFlow();
		return;
	}

	if (stylization->SelectedRenderingMode == 0)
	{		
		shader->RenderOriginal();
		return;
	}

	//smooth and calculate the edge only for the current frame	
	if (renderer->GaussianFilter == 0)
		shader->GaussianSmoothPicture("GaussianSmooth3x3");
	else
		shader->GaussianSmoothPicture("GaussianSmooth5x5");
	if (stylization->SelectedRenderingMode == 1)	return;


	shader->AddSobelFilterWithFocus();	
	if (stylization->SelectedRenderingMode == 2)
	{
		shader->RenderGradient();
		return;
	}	
	if (stylization->SelectedRenderingMode == 3)
	{
		shader->RenderStrokePosition();
		return;
	}
	shader->ConstructStrokeDetails();

	if (renderer->OnInitSpriteList)
	{					
		shader->DetermineSprites();
		renderer->OnInitSpriteList = false;
	}
	else if(bUpdateParticles)
	{
		shader->UpdateSprites();
		
		if (stylization->SelectedRenderingMode == 6)
			shader->RenderEmptyPosition();

		shader->AppendSpritesModel();
		shader->DeleteSprites();

		video->UpdatePreFrame();
	}
	
	if (stylization->SelectedRenderingMode == 4)
	{
		shader->RenderDeletePosition();
		return;
	}	

	if (stylization->SelectedRenderingMode != 6)
	{
		shader->RenderSprites();

		if (stylization->SelectedRenderingMode == 7) return;
		shader->RenderResult();
	}
}
