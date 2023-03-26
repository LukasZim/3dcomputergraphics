#include "dxut.h"
#include "Callback.h"
#include "Camera.h"
#include "definition.h"
#include "Shaders.h"
#include "UserInterface.h"



//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	stylization->CurTime = float(DXUTGetTime());
	shader->CurentTimeShaderVariable->SetFloat( stylization->CurTime );
	//PreFrameIndex = FrameIndex;
	bool VideoScreenCapture = DetermineChangeOfFrame();
	bool SceneScreenCapture = DetermineChangeOfSceneView();

	// If the settings dialog is being shown, then render it instead of rendering the app's scene
    if( viewer->g_D3DSettingsDlg.IsActive() )
    {
        viewer->g_D3DSettingsDlg.OnRender( fElapsedTime );
        return;
    }	
	
	switch(stylization->SelectedRenderingSession)
	{
		case VIDEO:
			renderer->RenderVideo(); break;
		case STATIC_MODEL:
			renderer->RenderSDKMesh();
			renderer->RenderImage(); 
			break;
		case STATIC_MODEL_OF:
			renderer->RenderSDKMeshWithOF(); break;
		case ANIMATED_MODEL:
			renderer->RenderSkinnedMesh(); break;
		case MODEL_ON_IMAGE:
			renderer->RenderModelWithImage();
			break;
		case MODEL_ON_VIDEO:
			renderer->RenderModelWithVideo();		
			break;
		case OBJ_MODEL:
			objMesh->RenderObjMesh();
			//renderer->RenderImage();
			break;
		default: 
			renderer->RenderImage(); break;
	}		

	if (VideoScreenCapture || renderer->OnImageCapture || SceneScreenCapture)
	{
		textureRT->ScreenCapture();  
		renderer->OnImageCapture = false;
	}
	
	ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
	DXUTGetD3D10Device()->OMSetRenderTargets(1, &pRTV, NULL);
	viewer->ChangeMode(fElapsedTime);
}

bool DetermineChangeOfSceneView()
{
	bool SceneScreenCapture = false;

	static double PreTime = DXUTGetTime();
	if (stylization->CurTime - PreTime >= TIME_INTERVAL)
	{
		PreTime = stylization->CurTime;
		if (renderer->OnScreenCapture)				
			SceneScreenCapture = true;
	}

	return SceneScreenCapture;
}

bool DetermineChangeOfFrame()
{
	bool VideoScreenCapture = false;
	if (renderer->OnVideoPlaying && renderer->OnVideoLoaded){
		static double PreTime = DXUTGetTime();

		if (stylization->CurTime - PreTime >= TIME_INTERVAL)
		{
			video->NextFrame();
			

			//Update UI
			viewer->g_SharedUI.GetSlider( IDC_VIDEOPLAY_SCALE )->SetValue(video->GetFrameIndex());
			WCHAR sz[100];
			StringCchPrintf( sz, 100, L"%0.2d",video->GetFrameIndex() ); 
            viewer->g_SharedUI.GetStatic( IDC_VIDEOPLAY_STATIC )->SetText( sz );

			//Change the texture resource
			//g_pDiffuseVariable->SetResource( VideoResource[FrameIndex] );
			PreTime = stylization->CurTime;

			if (video->GetFrameIndex() == 0)
			{
				renderer->OnInitSpriteList = true;
				renderer->ResetStrokeCount();
			}
	
			if (renderer->OnScreenCapture)
				VideoScreenCapture = true;

		}
	}
	return VideoScreenCapture;
}


//--------------------------------------------------------------------------------------
// Reject any D3D10 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
                                       DXGI_FORMAT BufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}

//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext )
{

	//SAFE_CHECK(CreateUI());
	viewer->CreateUI();

	shader = new Shader(stylization->WindowWidth, stylization->WindowHeight);
    //SAFE_CHECK(shader->LoadShaders()); 

	textureRT = new TextureRT(stylization->WindowWidth, stylization->WindowHeight);

	inputLayout = new InputLayout();

	style = new Style();
	  

	staticMesh = new StaticMesh();
	animatedMesh = new AnimatedMesh();
	objMesh = new ObjMesh();



	
    // Setup the camera's view parameters
	camera->SetupCamaraForSkinnedMesh();
    return S_OK;
}

//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* g_pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;

    V_RETURN( viewer->g_DialogResourceManager.OnD3D10ResizedSwapChain( g_pd3dDevice, pBufferSurfaceDesc ) );
    V_RETURN( viewer->g_D3DSettingsDlg.OnD3D10ResizedSwapChain( g_pd3dDevice, pBufferSurfaceDesc ) );

	// NEW Camera Setup the camera's projection parameters
	int BufferWidth = pBufferSurfaceDesc->Width;
	int BufferHeight = pBufferSurfaceDesc->Height;
    float fAspectRatio = BufferWidth / ( FLOAT )BufferHeight;
	//ResizeWindow(BufferWidth, BufferHeight); //RESTORE

	camera->InitCamera(fAspectRatio, BufferWidth, BufferHeight);
    
  
	textureRT->CreateDepthStencilTextureAndView(BufferWidth, BufferHeight, pBufferSurfaceDesc->SampleDesc);

	viewer->ResizeUI();
	return hr;
}

//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10ReleasingSwapChain( void* pUserContext )
{
    viewer->g_DialogResourceManager.OnD3D10ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10DestroyDevice( void* pUserContext )
{

    
    DXUTGetGlobalResourceCache().OnDestroyDevice();	

	//release UI
    

	//release effect
    

	//release shader recourse view
	
	//SAFE_RELEASE( pStencilShaderResource );

	//release texture
	//SAFE_RELEASE( pDepthStencil );
	//SAFE_RELEASE( pDSV );

	//this may cause memory problem
	//release the video
	


	

	//release layout
	


	

	

	//10/13



	delete shader;
	delete inputLayout;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D10 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{

    pDeviceSettings->d3d10.AutoDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pDeviceSettings->d3d10.SyncInterval = 0;

    return true;
}

//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{	
	if (stylization->SelectedRenderingSession != IMAGE && stylization->SelectedRenderingSession != VIDEO)
	{
		camera->FrameMove(fElapsedTime);
		camera->RecordEyeLookat();
		if ((stylization->SelectedRenderingSession == ANIMATED_MODEL || stylization->SelectedRenderingSession == MODEL_ON_VIDEO || stylization->SelectedRenderingSession == MODEL_ON_IMAGE) && !renderer->OnSkinnedMeshRenderingPause)
		{
			D3DXMATRIX mIdentity;
			D3DXMatrixIdentity( &mIdentity );
			//g_SkinnedMesh.TransformMesh( &mIdentity, fTime * 0.3 ); //fTime
			animatedMesh->TransformMesh( mIdentity, fTime * 0.3 );
		}
	}
}

//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows 
// messages to the application through this callback function. If the application sets 
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext )
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = viewer->g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to settings dialog if its active
    if( viewer->g_D3DSettingsDlg.IsActive() )
    {
        viewer->g_D3DSettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

	switch( viewer->gameMode )
    {
        case RENDERING:
            viewer->g_HUD.MsgProc( hWnd, uMsg, wParam, lParam ); break;
        case MAIN_MENU:
            viewer->g_MainUI.MsgProc( hWnd, uMsg, wParam, lParam ); break;
        case SPRITE_MENU:
            viewer->g_SpriteUI.MsgProc( hWnd, uMsg, wParam, lParam ); break;
        case SHARE_MENU:
            viewer->g_SharedUI.MsgProc( hWnd, uMsg, wParam, lParam ); break;
		//case VIDEO_MENU:
			//g_VideoUI.MsgProc( hWnd, uMsg, wParam, lParam ); break;
			
    }


	if( *pbNoFurtherProcessing )
		return 0;

    // Pass all remaining windows messages to camera so it can respond to user input
	camera->HandleMessage(hWnd, uMsg, wParam, lParam);
    return 0;
}


void CALLBACK MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down,
                         bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext )
{
	camera->MouseChangeCamera(xPos, yPos, bRightButtonDown, bLeftButtonDown, bMiddleButtonDown);

}