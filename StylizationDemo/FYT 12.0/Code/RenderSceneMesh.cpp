#include "dxut.h"
#include "Shaders.h"
#include "UserInterface.h"

//this function tries to extract the optical flow values and use them for rendering strokes
void Renderer::RenderSDKMeshWithOF()
{

	D3DXVECTOR3 vec3 = *camera->GetCamera().GetEyePt();
    D3DXVECTOR4 vEyePt;
    vEyePt.x = vec3.x;
    vEyePt.y = vec3.y;
    vEyePt.z = vec3.z;
	shader->g_pvEyePt0->SetFloatVector( (float*)&vEyePt );

	D3DXVECTOR4 vWorldLightDir1;
	D3DXVec3Normalize( (D3DXVECTOR3*)&vWorldLightDir1, &staticMesh->GetLightDir1() );
	shader->g_pvWorldLightDir1->SetFloatVector( (float*)&vWorldLightDir1);

	D3DXVECTOR4 vWorldLightDir2;
	D3DXVec3Normalize( (D3DXVECTOR3*)&vWorldLightDir2, &staticMesh->GetLightDir2()  );
	shader->g_pvWorldLightDir2->SetFloatVector( (float*)&vWorldLightDir2);

	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
    D3DXMATRIX mProj;
	D3DXMatrixIdentity( &mWorld );
	//mWorld =  *g_FCamera.GetWorldMatrix(); //testing
    mProj = *camera->GetCamera().GetProjMatrix();
    mView = *camera->GetCamera().GetViewMatrix();
	D3DXMATRIX mWorldViewProj = mWorld*mView*mProj;

	shader->g_pWorld0->SetMatrix( (float*)&mWorld );
	shader->g_pWorldViewProjection->SetMatrix( (float*)&mWorldViewProj );


	static D3DXMATRIX mPreWorldViewProj = mWorldViewProj;
	D3DXMATRIX diff = mWorldViewProj - mPreWorldViewProj;
	bool bUpdateParticles = (animatedMesh->MatrixNorm(diff) > 0.0001);

	shader->g_pPreWorldViewProjection->SetMatrix( (float*)&mPreWorldViewProj );

	mPreWorldViewProj = mWorldViewProj; //update the previous camera position


	staticMesh->RenderModelIntoTexture(RENDER_TARGET_COLOR2);
	staticMesh->RenderOpticalFlow();	
	
	
	inputLayout->EnableQuadVertexLayout();

	//not working here
	if (OnOpticalFlow)	
	{
		if (OnOpticalFlowTest)
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
	if (GaussianFilter == 0)
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

	bool bUpdateFrame = false;
	static double PreTime = DXUTGetTime();
	if (stylization->CurTime - PreTime >= TIME_INTERVAL)
	{
		bUpdateFrame = true;
		PreTime = stylization->CurTime;
	}

	if (OnInitSpriteList)
	{					
		shader->DetermineSprites();
		OnInitSpriteList = false;
	}
	else if(bUpdateParticles && bUpdateFrame)
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


//this function render individual frame as individual images
void Renderer::RenderSDKMesh()
{

	//set camera parameters
	D3DXVECTOR3 vec3 = *camera->GetCamera().GetEyePt();
    D3DXVECTOR4 vEyePt;
    vEyePt.x = vec3.x;
    vEyePt.y = vec3.y;
    vEyePt.z = vec3.z;
	shader->g_pvEyePt0->SetFloatVector( (float*)&vEyePt );

	D3DXVECTOR4 vWorldLightDir1;
	D3DXVec3Normalize( (D3DXVECTOR3*)&vWorldLightDir1, &staticMesh->GetLightDir1()  );
	shader->g_pvWorldLightDir1->SetFloatVector( (float*)&vWorldLightDir1);

	D3DXVECTOR4 vWorldLightDir2;
	D3DXVec3Normalize( (D3DXVECTOR3*)&vWorldLightDir2, &staticMesh->GetLightDir2()  );
	shader->g_pvWorldLightDir2->SetFloatVector( (float*)&vWorldLightDir2);

	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
    D3DXMATRIX mProj;
	D3DXMatrixIdentity( &mWorld );
	//mWorld =  *g_FCamera.GetWorldMatrix();
    mProj = *camera->GetCamera().GetProjMatrix();
    mView = *camera->GetCamera().GetViewMatrix();
	D3DXMATRIX mWorldViewProj = mWorld*mView*mProj;

	shader->g_pWorld0->SetMatrix( (float*)&mWorld );
	shader->g_pWorldViewProjection->SetMatrix( (float*)&mWorldViewProj );


	staticMesh->RenderModelIntoTexture(RENDER_TARGET_COLOR2);
}


