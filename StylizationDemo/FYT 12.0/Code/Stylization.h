#ifndef STYLIZATION_H
#define STYLIZATION_H
#include "DXUT.h"
#include <vector>
using namespace std;

#define TIME_INTERVAL						0.08
#define	DownSampling						0.5

class Stylization{
public:
	Stylization(int, int);
	void Init();
	HRESULT ResizeWindow(int BufferWidth, int BufferHeight);
	void SetImageContrast();
	void SetModelContrast();

public:
	int									WindowWidth;
	int									WindowHeight;

	vector<LPCWSTR> VideoName;
	vector<LPCWSTR> StaticModelName;
	vector<LPCWSTR> AnimatedModelName;
	vector<LPCWSTR> renderingMode;
	vector<LPCWSTR> Session;
	vector<LPCWSTR> ObjModelName;

	float								ContrastThreshold; //from 0.8 to 1.3
	float								FadeOutSpeedThreshold;
	float								FadeInSpeedThreshold;
	float								CurTime;
	int									MipMapLevel;

	int                                 SelectedRenderingMode;
	int									SelectedRenderingStyle;
	int									SelectedRenderingSession;
	int									SelectedAlphaMask;

	int									SelectedVideo;
	int									SelectedAnimatedModel;
	int									SelectedStaticModel;	
	int                                 SelectedImage;
	int									SelectedObjModel;


	float								FocusfThreshold;
	float								Focusr1Threshold;
	float								Focusr2Threshold;

	vector<char*> initVideoPath;
	vector<LPCWSTR> AlphaMaskTexture;
	vector<LPCWSTR> NormalMapTexture;
	vector<LPCWSTR> StrokeTexture;
	vector<LPCWSTR> ImageName;
};

#endif