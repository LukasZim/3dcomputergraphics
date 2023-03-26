#include "dxut.h"
#include "InputLayout.h"
#include "Shaders.h"
#include "UserInterface.h"

float Renderer::norm(D3DXMATRIX m){
	float sum = 0;
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			sum += m(i,j) * m(i,j);
		}
	}
	return sum;
}

void Renderer::RenderSkinnedMesh()
{	

	D3DXMATRIX mWorldViewProj;
    D3DXMATRIX mViewProj;
    D3DXMATRIX mWorld;
    D3DXMATRIX mView;
    D3DXMATRIX mProj;

    ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
    DXUTGetD3D10Device()->ClearRenderTargetView( pRTV, ClearCanvas );

    // Clear the depth stencil
    ID3D10DepthStencilView* pDSV = DXUTGetD3D10DepthStencilView();
    DXUTGetD3D10Device()->ClearDepthStencilView( pDSV, D3D10_CLEAR_DEPTH, 1.0, 0 );

    // Get the projection & view matrix from the camera class
    D3DXMatrixIdentity( &mWorld );
	//mWorld =  *g_FCamera.GetWorldMatrix();
    mProj = *camera->GetCamera().GetProjMatrix();
    mView = *camera->GetCamera().GetViewMatrix();
    mViewProj = mView * mProj;

    // Set general effect values
    shader->g_pvLightPos->SetFloatVector( ( float* )&animatedMesh->GetLightPos() );
    D3DXVECTOR3 vEye = *camera->GetCamera().GetEyePt();
    shader->g_pvEyePt1->SetFloatVector( ( float* )&vEye );

	animatedMesh->StreamOutBones(0);
  
	if (OnFirstFrame)
	{
		OnFirstFrame = false;
		animatedMesh->StreamOutBones(1);
	}	

		
	//***************************************************************************
    // Render the transformed mesh from the streamed out vertices 
    //***************************************************************************
	// Set effect variables
    animatedMesh->GetInstanceWorldMatrix( 0, &mWorld );
	//mWorld =  *g_FCamera.GetWorldMatrix();
    mWorldViewProj = mWorld * mViewProj;
    shader->g_pmWorldViewProj1->SetMatrix( ( float* )&mWorldViewProj );

	static D3DXMATRIX mPreWorldViewProj = mWorldViewProj;
	D3DXMATRIX diff = mWorldViewProj - mPreWorldViewProj;
	bool bUpdateParticles = (animatedMesh->MatrixNorm(diff) > 0.0001);
	//bool bUpdateParticles = !(mWorldViewProj == mPreWorldViewProj)				;
	shader->g_pmPreWorldViewProj1->SetMatrix( (float*)&mPreWorldViewProj );
	mPreWorldViewProj = mWorldViewProj; //update the previous camera position

    shader->g_pmWorld1->SetMatrix( ( float* )&mWorld );


	animatedMesh->RenderModelIntoTexture(RENDER_TARGET_COLOR2);		


	animatedMesh->RenderOpticalFlow();

	animatedMesh->SwapBuffers();
   
	//***************************************************************************
    // The rest of the steps for generating and updating the strokes
    //***************************************************************************
	camera->RecordEyeLookat();
	camera->SetupCamaraForImage();
	inputLayout->EnableQuadVertexLayout();

	bool bUpdateFrame = false;
	static double PreTime = DXUTGetTime();
	if (stylization->CurTime - PreTime >= TIME_INTERVAL)
	{
		bUpdateFrame = true;
		PreTime = stylization->CurTime;
	}

	//not working here
	if (OnOpticalFlow)	
	{
		if (OnOpticalFlowTest)
			shader->RenderOpticalFlowTest();
		else
			shader->RenderOpticalFlow();
		camera->RestorePreviousCamera();
		return;
	}

	if (stylization->SelectedRenderingMode == 0)
	{		
		shader->RenderOriginal();
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

	if (OnInitSpriteList)
	{					
		shader->DetermineSprites();	
		OnInitSpriteList = false;
	}

	
	else if (bUpdateFrame && (!OnSkinnedMeshRenderingPause || bUpdateParticles))
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