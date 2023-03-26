#include "DXUT.h"
#include "StaticMesh.h"
#include "definition.h"

StaticMesh::StaticMesh()
{
	LoadStaticMesh();	
	g_vLightDir1 = D3DXVECTOR3(1.705f,5.557f,-9.380f);
	g_vLightDir2 = D3DXVECTOR3(-5.947f,-5.342f,-5.733f);
}

StaticMesh::~StaticMesh()
{
	DeleteMesh();
}

void StaticMesh::DeleteMesh()
{
	g_ObjectMesh.Destroy();
	g_ObjectMesh1.Destroy();
	g_ObjectMesh2.Destroy();
	g_SkyMesh.Destroy();
}

void StaticMesh::LoadStaticMesh()
{
	// Load still models

	switch(stylization->SelectedStaticModel)
	{
		case 0:
			g_ObjectMesh.Create( DXUTGetD3D10Device(), L"Model\\Tank\\TankScene.sdkmesh", true  );
			g_SkyMesh.Create( DXUTGetD3D10Device(), L"Model\\Tank\\desertsky.sdkmesh", true  );
			break;
		case 1:
			g_ObjectMesh.Create( DXUTGetD3D10Device(), L"Model\\NightScene\\crypt.sdkmesh", true ) ;	
			g_SkyMesh.Create( DXUTGetD3D10Device(), L"Model\\NightScene\\cloud_skybox.sdkmesh", false  );			
			break;
		case 2:
			g_ObjectMesh.Create( DXUTGetD3D10Device(), L"Model\\Room\\blackholeroom.sdkmesh", true  );
			g_ObjectMesh1.Create( DXUTGetD3D10Device(), L"Model\\Room\\blackhole.sdkmesh", true  );
			g_ObjectMesh2.Create( DXUTGetD3D10Device(), L"Model\\Room\\ball.sdkmesh", true  );
			g_SkyMesh.Create( DXUTGetD3D10Device(), L"Model\\NightScene\\cloud_skybox.sdkmesh", false  );
			break;
	}
	string styleName = style->staticModelStyleName[stylization->SelectedStaticModel];
	style->LoadStyle(styleName);
}

void StaticMesh::RenderModelIntoTexture(int RenderTargetIndex)
{
	textureRT->NullShaderResource(RenderTargetIndex);	
	textureRT->ClearDepthStencil();
	textureRT->ClearRenderTarget(RenderTargetIndex, ClearColor);
	textureRT->SetRenderTargetStencil(RenderTargetIndex);
	
	// Render the scene
    DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetSDKSceneVertexLayout() );
	//the front-end objects are rendered on top of the background sphere with no blending
    g_SkyMesh.Render( DXUTGetD3D10Device(), shader->GetRenderSky(), shader->g_pDiffuseTex0 );
    g_ObjectMesh.Render( DXUTGetD3D10Device(), shader->GetRenderScene(), shader->g_pDiffuseTex0, shader->g_pNormalTex0 );
    g_ObjectMesh1.Render( DXUTGetD3D10Device(), shader->GetRenderScene(), shader->g_pDiffuseTex0, shader->g_pNormalTex0 );
	//g_ObjectMesh2.Render( DXUTGetD3D10Device(), shader->GetRenderScene(), shader->g_pDiffuseTex0, shader->g_pNormalTex0 );


	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
	textureRT->SetRTShaderView(RenderTargetIndex);	
}

void StaticMesh::RenderOpticalFlow()
{
	textureRT->NullShaderResource(RENDER_TARGET_FlOW);
	textureRT->ClearRenderTarget(RENDER_TARGET_FlOW, InitialOpticalFlowValues);
	textureRT->ClearDepthStencil();

	textureRT->SetRenderTargetStencil(RENDER_TARGET_FlOW);

	g_SkyMesh.Render( DXUTGetD3D10Device(), shader->GetRenderSkyOpticalFlow(), shader->g_pDiffuseTex0 );
	g_ObjectMesh.Render( DXUTGetD3D10Device(), shader->GetRenderSceneOpticalFlow(), shader->g_pDiffuseTex0, shader->g_pNormalTex0 );	
	g_ObjectMesh1.Render( DXUTGetD3D10Device(), shader->GetRenderSceneOpticalFlow(), shader->g_pDiffuseTex0, shader->g_pNormalTex0 );
	//g_ObjectMesh2.Render( DXUTGetD3D10Device(), shader->GetRenderSceneOpticalFlow(), shader->g_pDiffuseTex0, shader->g_pNormalTex0 );


	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
	textureRT->SetRTShaderView(RENDER_TARGET_FlOW);			
}