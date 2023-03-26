#include "DXUT.h"
#include "Camera.h"
#include "Shaders.h"
#include "Stylization.h"

extern Stylization* stylization;
extern Shader* shader;
extern Renderer* renderer;

Camera::Camera()
{
	preEyePt = D3DXVECTOR3(0, 0, 0);
	preLookAt = D3DXVECTOR3(0, 0, 0);
}

void Camera::SetupCamaraForSkinnedMesh()
{
	D3DXVECTOR3 vecEye;
	D3DXVECTOR3 vecAt;
	switch(stylization->SelectedAnimatedModel)
	{
		case 0:
			vecEye = D3DXVECTOR3( -0.556f, 0.09f, 0.687f );
			vecAt = D3DXVECTOR3( -0.04f, 0.57f, -1.0f );
			break;
		case 1:
			vecEye = D3DXVECTOR3( 6.8f, 6.4f, -9.5f );
			vecAt = D3DXVECTOR3( 0.0f, 0.5f, -1.0f );
			break;
	}
	SetCameraViewParams( &vecEye, &vecAt );
	preEyePt = *g_Camera.GetEyePt();
	preLookAt = *g_Camera.GetLookAtPt();
}

void Camera::SetupCamaraForSDKScene()
{
	D3DXVECTOR3 vecAt(0,0,0);
	D3DXVECTOR3 vecEye(0.27f, 3.8f, -11.0f);
    SetCameraViewParams( &vecEye, &vecAt );
    g_Camera.SetRadius( 10.0f, 1.0f, 20.0f );
	g_LCamera.SetRadius( 10.0f, 1.0f, 20.0f );
}

void Camera::SetupCamaraForObjModel()
{
	// Setup the camera's view parameters
	D3DXVECTOR3 vecEye( 2.0f, 1.0f, 0.0f );
	D3DXVECTOR3 vecAt ( 0.0f, 0.0f, 0.0f );
	SetCameraViewParams( &vecEye, &vecAt );	
}

//void Camera::SetupCameraForIslandScene()
//{
//	D3DXVECTOR3 vEyeStart( 100, 400, 2000 );
//    D3DXVECTOR3 vAtStart( 0, 0, -2000 );
//    g_Camera.SetViewParams( &vEyeStart, &vAtStart );
//}

void Camera::SetupCamaraForImage()
{
	float scalingFactorX = stylization->WindowWidth * 0.002f;
	float scalingFactorY = stylization->WindowHeight * 0.002f;
	// Setup the camera's view parameters

    D3DXVECTOR3 vecEye( 0.0f, 0.0f, -200.0f );
    D3DXVECTOR3 vecAt ( 0.0f, 0.0f, 0.0f );
    SetCameraViewParams( &vecEye, &vecAt );
    g_Camera.SetRadius( 200.0f * scalingFactorX, 200.0f * scalingFactorY, 200.0f );
	//g_FCamera.SetRadius( 200.0f * scalingFactorX, 200.0f * scalingFactorY, 200.0f );
	g_LCamera.SetRadius( 200.0f * scalingFactorX, 200.0f * scalingFactorY, 200.0f );
	
    D3DXMATRIX mWorldViewProj;

	// Set Effects Parameters
    D3DXMatrixMultiply( &mWorldViewProj, g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix() );
    shader->g_pmWorldViewProj0->SetMatrix( ( float* )&mWorldViewProj );

	D3DXMATRIX mInvView;
    D3DXMatrixInverse( &mInvView, NULL, g_Camera.GetViewMatrix() );
    shader->g_pmInvView->SetMatrix( ( float* )&mInvView );
}

void Camera::SetCameraViewParams( D3DXVECTOR3* EyePt, D3DXVECTOR3* LookAt )
{
	g_Camera.SetViewParams( EyePt, LookAt );
	g_FCamera.SetViewParams( EyePt, LookAt );
	g_LCamera.SetViewParams( EyePt, LookAt ); 
}

void Camera::RestorePreviousCamera()
{
	g_Camera.SetViewParams( &preEyePt, &preLookAt );
	g_FCamera.SetViewParams( &preEyePt, &preLookAt ); 
	g_LCamera.SetViewParams( &preEyePt, &preLookAt ); 
}

void Camera::SetCameraProjParams(float fFOV, float fAspect, float fNearPlane,
                                 float fFarPlane)
{
	g_Camera.SetProjParams( fFOV, fAspect, fNearPlane, fFarPlane );
	g_FCamera.SetProjParams( fFOV, fAspect, fNearPlane, fFarPlane );
	g_LCamera.SetProjParams( fFOV, fAspect, fNearPlane, fFarPlane );
}

void Camera::InitCamera(float fAspectRatio, int BufferWidth, int BufferHeight)
{
	if (stylization->SelectedRenderingSession == STATIC_MODEL || 
		stylization->SelectedRenderingSession == STATIC_MODEL_OF)
	{
		float fNearPlane = 0.1f; //0.1
		float fFarPlane = 150.0f; //150
		SetCameraProjParams( 54.43f*(D3DX_PI/180.0f), fAspectRatio, fNearPlane, fFarPlane );
		g_FCamera.SetRotateButtons( true, false, false );
		g_Camera.SetButtonMasks( MOUSE_RIGHT_BUTTON, MOUSE_WHEEL, 0);
	}		
	else if(stylization->SelectedRenderingSession == ANIMATED_MODEL ||
			stylization->SelectedRenderingSession == OBJ_MODEL )
		g_Camera.SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 1000.0f );
	else
	{
		g_Camera.SetProjParams( D3DX_PI * 0.25, fAspectRatio, 0.1f, 5000.0f );			
	}
	g_Camera.SetWindow( BufferWidth, BufferHeight );
	g_LCamera.SetWindow( BufferWidth, BufferHeight );
}

void Camera::FrameMove(float fElapsedTime)
{
	g_Camera.FrameMove( fElapsedTime );
	g_FCamera.FrameMove( fElapsedTime );
	g_LCamera.FrameMove( fElapsedTime );
}

void Camera::RecordEyeLookat()
{
	preEyePt = *g_Camera.GetEyePt();
	preLookAt = *g_Camera.GetLookAtPt();
}

void Camera::RecordEyeLookat(D3DXVECTOR3 eye, D3DXVECTOR3 lookat)
{
	preEyePt = eye;
	preLookAt = lookat;
}

void Camera::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );	
	g_FCamera.HandleMessages( hWnd, uMsg, wParam, lParam );
    g_LCamera.HandleMessages( hWnd, uMsg, wParam, lParam );
}

void Camera::MouseChangeCamera(int xPos, int yPos, bool bRightButtonDown, bool bLeftButtonDown, bool bMiddleButtonDown)
{
	static int preMousePosX = xPos;
	static int preMousePosY = yPos;


	if( bRightButtonDown )
		g_Camera.SetEnablePositionMovement( true );

	
	if ( bLeftButtonDown )
	{
		if (renderer->OnEnableFocus)
		{
			shader->MouseXVariable->SetFloat(xPos/float(stylization->WindowWidth));
			shader->MouseYVariable->SetFloat(yPos/float(stylization->WindowHeight));
			g_FCamera.SetEnablePositionMovement( true );
		}
		else if (renderer->OnChangingLookAtPos)
		{
			D3DXVECTOR3 vecAt = *g_Camera.GetLookAtPt();		
			vecAt.x += (xPos - preMousePosX) * 0.01f;
			vecAt.y -= (yPos - preMousePosY) * 0.01f;
					
			D3DXVECTOR3 eyePt = *g_Camera.GetEyePt();
			SetCameraViewParams( &eyePt, &vecAt );
		}		
	}

	if ( bMiddleButtonDown )
	{
		D3DXVECTOR3 eyePt = *g_Camera.GetEyePt();		
		eyePt.x += (xPos - preMousePosX) * 0.01f;
		eyePt.y -= (yPos - preMousePosY) * 0.01f;
				
		D3DXVECTOR3 vecAt = *g_Camera.GetLookAtPt();
		SetCameraViewParams( &eyePt, &vecAt );	
	}

	preMousePosX = xPos;
	preMousePosY = yPos;
}

