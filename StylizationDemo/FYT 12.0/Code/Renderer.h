#ifndef RENDERER_H
#define RENDERER_H

class Renderer{

public:
	Renderer();
	void RenderVideo();
	void RenderImage();
	void RenderModelWithImage();
	void RenderModelWithVideo();
	void RenderSDKMeshWithOF();
	void RenderSDKMesh();
	void RenderSkinnedMesh();

	void ResetStrokeCount();


private:	
	float norm(D3DXMATRIX m);
public:

	//Rendering State
	bool								OnOpticalFlow;
	bool								OnOpticalFlowTest;
	bool                                OnVideoLoaded;
	bool								OnVideoPlaying;
	bool								OnScreenCapture;
	bool								OnImageCapture;
	bool                                OnInitSpriteList;
	bool                                OnRealGradientAngle;
	bool								OnClipping;
	bool								OnFramesRendering;
	bool								OnSkinnedMeshRenderingPause; 
	bool								OnChangingLookAtPos;
	bool								OnOpticalFlowLoaded;
	int									GaussianFilter;
	bool								SwitchingLookAtPoint;
	bool								OnFirstFrame;
	bool								OnFocusPoint;
	bool								OnEnableFocus;

	int									Num_Primitives;
	int									Num_LargePrimitives;
	int									Num_MediumPrimitives;
	int									Num_SmallPrimitives;
	int									Num_SmallDeleted;
	int									Num_MediumDeleted;
	int									Num_LargeDeleted;
	int									Num_LargeAppended;
	int									Num_MediumAppended;
	int									Num_SmallAppended;
	int									Num_LargeUpdated;
	int									Num_MediumUpdated;
	int									Num_SmallUpdated;
	int									Num_LargeStrokePreviousFrame;
	int									Num_MediumStrokePreviousFrame;
	int									Num_SmallStrokePreviousFrame;
};

#endif