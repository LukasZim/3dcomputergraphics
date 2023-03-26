#include "dxut.h"
#include "Shaders.h"
#include "UserInterface.h"
#include "Renderer.h"

//RENDER_TARGET_COLOR2 : The current frame
//RENDER_TARGET_COLOR1 : The next frame
//RENDER_TARGET_COLOR3 : The previous frame
//RENDER_TARGET_COLOR0 : The result from optical flow precomputation, then hold the smoothed current frame
//RENDER_TARGET_FlOW : The optical flow result
//RENDER_TARGET_INFO : r : stroke size, g : stroke alpha, b : gradient mag, a : gradient dir
//RENDER_TARGET_STROKE : r : alpha mask, g : lighting factor, b : gradient mag
//RENDER_TARGET_TEMP : r : Intermediate render result, used for stroke deletion and addition
//RENDER_TARGET_MIPRENDERING : Render result
void Renderer::RenderVideo()
{	
	//set the quad vertex layout as the active one
	inputLayout->EnableQuadVertexLayout();

	//***************************************************************************************************************//
	//Set resource to be correct video frame, using the previous, current and the next frame
	//***************************************************************************************************************//
	if (renderer->OnVideoLoaded)
	{
		textureRT->SetVideoResource(RENDER_TARGET_COLOR1, min(video->GetFrameIndex() + 1, video->GetNumFrame() - 1) % video->GetNumFrame());
		textureRT->SetVideoResource(RENDER_TARGET_COLOR2, video->GetFrameIndex()                        % video->GetNumFrame());
		textureRT->SetVideoResource(RENDER_TARGET_COLOR3, max(video->GetFrameIndex() - 1, 0           ) % video->GetNumFrame());	
	}

	if (stylization->SelectedRenderingMode == 0)
	{		
		shader->RenderOriginal();
		return;
	}

	//***************************************************************************************************************//
	//Calculate optical flow
	//***************************************************************************************************************//
	//optical flow here is problemsome
	if (OnVideoLoaded)
	{
		shader->PreOpticalFlow(); //precompute intermediate results that are shared for all the future iterations

		for (int i = 0; i < video->GetMaxIteration(); i++)
		{	
			shader->OpticalFlow(i);
		}		
	}

	//output the optical flow
	if (OnOpticalFlow)	
	{
		if (OnOpticalFlowTest)
			shader->RenderOpticalFlowTest();
		else
			shader->RenderOpticalFlow();
		return;
	}

	//***************************************************************************************************************//
	//Smooth Frame
	//***************************************************************************************************************//
	//smooth and calculate the edge only for the current frame	
	if (GaussianFilter == 0)
		shader->GaussianSmoothPicture("GaussianSmooth3x3");
	else
		shader->GaussianSmoothPicture("GaussianSmooth5x5");
	if (stylization->SelectedRenderingMode == 1)	return;

	//***************************************************************************************************************//
	//edge detection and determination of stroke properties
	//***************************************************************************************************************//
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

	//***************************************************************************************************************//
	//Stroke construction, update and rendering
	//***************************************************************************************************************//	
	if (video->GetFrameIndex() == 0 || OnInitSpriteList)					
		shader->DetermineSprites();
	else if (video->FrameChange())
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
		return;
	}	

	if (stylization->SelectedRenderingMode != 6)
		shader->RenderSprites();
}

//RENDER_TARGET_COLOR0 : The smoothed original image
//RENDER_TARGET_COLOR2 : The original image
//RENDER_TARGET_INFO : r : stroke size, g : stroke alpha, b : gradient mag, a : gradient dir
//RENDER_TARGET_STROKE : r : alpha mask, g : lighting factor, b : gradient mag
//RENDER_TARGET_MIPRENDERING : Render result
void Renderer::RenderImage()
{	

	//set the quad vertex layout as the active one
	inputLayout->EnableQuadVertexLayout();
	OnInitSpriteList = true;

	//***************************************************************************************************************//
	//set the image content
	//***************************************************************************************************************//	
	if (OnFramesRendering && renderer->OnVideoLoaded)
		textureRT->SetVideoResource(RENDER_TARGET_COLOR2, video->GetFrameIndex() % video->GetNumFrame());

	if (stylization->SelectedRenderingMode == 0)
	{
		shader->RenderOriginal();
		return;
	}

	//***************************************************************************************************************//
	//Smooth the image
	//***************************************************************************************************************//	
	//MedianFilterPicture();
	if (GaussianFilter == 0)
		shader->GaussianSmoothPicture("GaussianSmooth3x3");
	else
		shader->GaussianSmoothPicture("GaussianSmooth5x5");
	if (stylization->SelectedRenderingMode == 1)	return;
	
	//***************************************************************************************************************//
	//edge detection and determination of stroke properties
	//***************************************************************************************************************//	
	shader->AddSobelFilterWithFocus(); //with focus
	//AddSobelFilter(); //Generate Warning, not sure whether it will influence the program
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

	//***************************************************************************************************************//
	//Construct strokes (eliminate the vertices, not corresponding to strokes, from the stream) 
	//from vertex buffer to vertex buffer
	//***************************************************************************************************************//	
	shader->DetermineLargeSprites();	
	shader->DetermineMediumSprites();
	shader->DetermineSmallSprites();

	//***************************************************************************************************************//
	//Render sprites (generate rectangular sprite from vertices )  
	//from vertex buffer to screen pixel
	//***************************************************************************************************************//	
	shader->RenderLargeSprites();
	shader->RenderMediumSprites();
	shader->RenderSmallSprites();

	inputLayout->EnableQuadVertexLayout();
}