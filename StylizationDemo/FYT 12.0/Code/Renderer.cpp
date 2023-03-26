#include "DXUT.h"
#include "Renderer.h"

Renderer::Renderer()
{
	//Program State
	OnOpticalFlow			 = false;
	OnOpticalFlowTest		 = false;
	OnVideoLoaded			 = false;
	OnVideoPlaying			 = false;
	OnScreenCapture			 = false;
	OnImageCapture			 = false;
	OnInitSpriteList		 = true;
	OnRealGradientAngle		 = false;
	OnClipping				 = true;
	OnFramesRendering		 = false;
	OnSkinnedMeshRenderingPause = false; 
	OnChangingLookAtPos		 = false;
	OnOpticalFlowLoaded		 = false;
	GaussianFilter			 = 0;
	SwitchingLookAtPoint	 = true;
	OnFirstFrame			 = true;
	OnFocusPoint			 = true;
	OnEnableFocus			 = false;
	ResetStrokeCount();
	
}

void Renderer::ResetStrokeCount()
{
	Num_Primitives			 = 0;
	Num_LargePrimitives		 = 0;
	Num_MediumPrimitives	 = 0;
	Num_SmallPrimitives		 = 0;
	Num_SmallDeleted		 = 0;
	Num_MediumDeleted		 = 0;
	Num_LargeDeleted		 = 0;
	Num_LargeAppended		 = 0;
	Num_MediumAppended		 = 0;
	Num_SmallAppended		 = 0;
	Num_LargeUpdated		 = 0;
	Num_MediumUpdated		 = 0;
	Num_SmallUpdated		 = 0;
	Num_LargeStrokePreviousFrame	 = 0;
	Num_MediumStrokePreviousFrame	 = 0;
	Num_SmallStrokePreviousFrame	 = 0;
}