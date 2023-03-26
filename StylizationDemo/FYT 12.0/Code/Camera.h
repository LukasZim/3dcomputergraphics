#ifndef CAMERA1_H
#define CAMERA1_H
#include "dxut.h"
#include "DXUTcamera.h"

enum RENDERING_SESSION
{
	IMAGE,
	VIDEO,
	STATIC_MODEL_OF,
	STATIC_MODEL,
	ANIMATED_MODEL,
	MODEL_ON_IMAGE,
	MODEL_ON_VIDEO,
	OBJ_MODEL,
};

class Camera{
public:
	Camera();
	void 								SetupCamaraForSkinnedMesh();
	void 								SetupCamaraForSDKScene();
	void 								SetupCamaraForObjModel();
	void								SetupCamaraForImage();
	void								SetCameraViewParams( D3DXVECTOR3* pvEyePt, D3DXVECTOR3* pvLookatPt );
	void								SetCameraProjParams(float fFOV, float fAspect, float fNearPlane, float fFarPlane);
	CModelViewerCamera					GetCamera(){return g_Camera;}
	CModelViewerCamera					GetLCamera(){return g_LCamera;}
	CFirstPersonCamera					GetFCamera(){return g_FCamera;}		
	void								InitCamera(float, int, int);
	void								FrameMove(float fElapsedTime);
	void								RecordEyeLookat();
	void								HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void								MouseChangeCamera(int xPos, int yPos, bool bRightButtonDown, bool bLeftButtonDown, bool bMiddleButtonDown);
	void								RestorePreviousCamera();
	void								RecordEyeLookat(D3DXVECTOR3 eye, D3DXVECTOR3 lookat);
private:
	CModelViewerCamera                  g_Camera;               // A model viewing camera
	CModelViewerCamera                  g_LCamera;              // Camera for easy light movement control
	CFirstPersonCamera                  g_FCamera;               // A model viewing camera
	D3DXVECTOR3							preEyePt;
	D3DXVECTOR3							preLookAt;//Camara
};


#endif