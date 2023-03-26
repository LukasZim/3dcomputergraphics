#include "dxut.h"
#include "UserInterface.h"
#include "Shaders.h"

extern Camera* camera;


void Renderer::RenderModelWithImage()
{
	camera->RestorePreviousCamera();

	animatedMesh->StreamOutBones(0);
	if (OnFirstFrame)
	{
		OnFirstFrame = false;
		animatedMesh->StreamOutBones(1);
	}	
       
	//********************************************************************************************
	// Set effect variables
	//********************************************************************************************
	D3DXMATRIX mWorldViewProj;
    D3DXMATRIX mViewProj;
    D3DXMATRIX mWorld;
    D3DXMATRIX mView;
    D3DXMATRIX mProj;

	// Get the projection & view matrix from the camera class
    animatedMesh->GetInstanceWorldMatrix( 0, &mWorld );
	//mWorld =  *g_FCamera.GetWorldMatrix(); //testing
    mProj = *camera->GetCamera().GetProjMatrix();
    mView = *camera->GetCamera().GetViewMatrix();
    mViewProj = mView * mProj;
	mWorldViewProj = mWorld * mViewProj;    
    

	static D3DXMATRIX mPreWorldViewProj = mWorldViewProj;		
	float diff = norm(mWorldViewProj - mPreWorldViewProj);
	bool bUpdateParticles = (diff > 0.001);
	
	shader->g_pmWorldViewProj1->SetMatrix( ( float* )&mWorldViewProj );
	shader->g_pmPreWorldViewProj1->SetMatrix( (float*)&mPreWorldViewProj );
	shader->g_pmWorld1->SetMatrix( ( float* )&mWorld );	
	mPreWorldViewProj = mWorldViewProj; //update the previous camera position  

	// Set general effect values
    shader->g_pvLightPos->SetFloatVector( ( float* )&animatedMesh->GetLightPos() );
    D3DXVECTOR3 vEye = *camera->GetCamera().GetEyePt();
    shader->g_pvEyePt1->SetFloatVector( ( float* )&vEye );
	
    
	animatedMesh->RenderModelMatte();

	animatedMesh->RenderOpticalFlow();

	animatedMesh->RenderModelIntoTexture(RENDER_TARGET_COLOR0);

	animatedMesh->SwapBuffers();


	camera->RecordEyeLookat();

	camera->SetupCamaraForImage();
	inputLayout->EnableQuadVertexLayout();

	if (renderer->OnVideoLoaded)
	{
		textureRT->LoadTexture(RENDER_TARGET_COLOR3, RENDER_TARGET_COLOR3);	
	}
	shader->RenderModelOnVideoFrame(); //now the RENDER_TARGET_COLOR2 contains the composite rendering of model and frame


	//********************************************************************************************
	//The other steps to stylize the frame
	//********************************************************************************************
	if (stylization->SelectedRenderingMode == 0)
	{		
		shader->RenderOriginal();
		camera->RestorePreviousCamera();
		return;
	}

	if (OnOpticalFlow)	
	{
		if (OnOpticalFlowTest)
			shader->RenderOpticalFlowTest();
		else
			shader->RenderOpticalFlow();
		camera->RestorePreviousCamera();
		return;
	}	

	//smooth and calculate the edge only for the current frame	
	if (GaussianFilter == 0)
		shader->GaussianSmoothPicture("GaussianSmooth3x3");
	else
		shader->GaussianSmoothPicture("GaussianSmooth5x5");
	if (stylization->SelectedRenderingMode == 1)
	{
		camera->RestorePreviousCamera();
		return;
	}

	shader->AddSobelFilterWithFocus();	
	if (stylization->SelectedRenderingMode == 2)
	{
		shader->RenderGradient();
		camera->RestorePreviousCamera();
		return;
	}	
	if (stylization->SelectedRenderingMode == 3)
	{
		shader->RenderStrokePosition();
		camera->RestorePreviousCamera();
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
	else if(!OnSkinnedMeshRenderingPause || bUpdateParticles)
	{
		shader->UpdateSprites();	
		
		if (stylization->SelectedRenderingMode == 6)
			shader->RenderEmptyPosition();

		shader->AppendSprites();	
		
		shader->DeleteSprites();	
		video->UpdatePreFrame();
	}
	
	if (stylization->SelectedRenderingMode == 4)
	{
		shader->RenderDeletePosition();
		camera->RestorePreviousCamera();
		return;
	}	

	if (stylization->SelectedRenderingMode != 6)
	{
		shader->RenderSprites();
		
		if (stylization->SelectedRenderingMode == 7)
		{
			camera->RestorePreviousCamera();
			return;
		}
		shader->RenderResult();
	}
	camera->RestorePreviousCamera();
}