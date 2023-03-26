#ifndef VIDEO_H
#define VIDEO_H

#include "dxut.h"

class Video{
public:

	Video();
	void NextFrame();
	void SetFrameIndex(int i){FrameIndex = i;}
	int GetFrameIndex(){return FrameIndex;}
	void SetNumFrame(int n){NumFrame = n;}
	int GetNumFrame(){return NumFrame;}
	int GetMaxIteration(){return Max_Iteration;}
	bool FrameChange(){return (PreFrameIndex != FrameIndex);}
	void UpdatePreFrame(){PreFrameIndex = FrameIndex;}
	void SetInitialFrameIndex(int i){InitialFrameIndex = i;}
	int GetInitialFrameIndex(){return InitialFrameIndex;}
	void SetMaxIteration(int i){Max_Iteration = i;}
	float GetLambda(){return Lambda;}
	void SetLambda(float l){Lambda = l;}

	HRESULT DeleteVideoFrames();
	HRESULT loadVideoFrames();

private:

	int									Max_Iteration;
	int									FrameIndex;
	int									PreFrameIndex;
	int									InitialFrameIndex;
	int									NumFrame; //230
	float								Lambda;

};



#endif