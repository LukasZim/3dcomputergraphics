#include "dxut.h"
#include "Video.h"
#include "definition.h"

Video::Video()
{
	Max_Iteration			 = 1;
	FrameIndex				 = 0;
	PreFrameIndex			 = 0;
	InitialFrameIndex		 = 0;
	NumFrame				 = 50; //230
	Lambda					 = 0.9f;
}

void Video::NextFrame()
{
	PreFrameIndex = FrameIndex;			
	FrameIndex = (FrameIndex + 1) % NumFrame;
}

HRESULT Video::DeleteVideoFrames()
{
	if (textureRT)
		return textureRT->DeleteVideoResource();	
	return S_OK;
}

HRESULT Video::loadVideoFrames()
{	
	if (textureRT)
		return textureRT->LoadVideoResource();
	return S_OK;
}