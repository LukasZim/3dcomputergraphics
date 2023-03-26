#ifndef CALLBACK_H
#define CALLBACK_H

#include "dxut.h"
#include "InputLayout.h"
#include "DXUTgui.h"

bool CALLBACK    IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,DXGI_FORMAT BufferFormat, bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBufferSurfaceDesc,void* pUserContext );
HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,const DXGI_SURFACE_DESC* pBufferSurfaceDesc, void* pUserContext );
void CALLBACK    OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK    OnD3D10ReleasingSwapChain( void* pUserContext );
void CALLBACK    OnD3D10DestroyDevice( void* pUserContext );
void CALLBACK    OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,void* pUserContext );
void CALLBACK    OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK    ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
bool			 DetermineChangeOfFrame();
bool			 DetermineChangeOfSceneView();
void CALLBACK	 MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down,
                         bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext );


#endif