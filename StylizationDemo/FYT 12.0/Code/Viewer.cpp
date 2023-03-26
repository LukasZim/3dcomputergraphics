#include "dxut.h"
#include "Viewer.h"
#include "Video.h"
#include "TextureRT.h"
#include "UserInterface.h"
#include "Video.h"

Viewer::Viewer(int w, int h)
{
	WindowWidth = w;
	WindowHeight = h;
	gameMode = RENDERING;
	InitApp();
}

Viewer::~Viewer()
{
	g_DialogResourceManager.OnD3D10DestroyDevice();
	g_D3DSettingsDlg.OnD3D10DestroyDevice();
	SAFE_RELEASE( g_pFont );
	SAFE_RELEASE( g_pSprite );
	SAFE_DELETE( g_pTxtHelper );
}

void Viewer::InitApp()
{
	
    g_D3DSettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SharedUI.Init( &g_DialogResourceManager );
	g_SpriteUI.Init( &g_DialogResourceManager );
	g_MainUI.Init( &g_DialogResourceManager );
	
	float scalingFactorX = WindowWidth * 0.002f;
	float scalingFactorY = WindowHeight * 0.002f;
    g_HUD.SetCallback( OnGUIEvent );  

    g_SharedUI.SetCallback( OnGUIEvent );   
	g_SpriteUI.SetCallback( OnGUIEvent ); 
	g_MainUI.SetCallback( OnGUIEvent ); 

    int iY                 = int(-200 * scalingFactorY);	
	const int ButtonWidth        = int(125 * scalingFactorX);
	const int ButtonHeight       = int(18 * scalingFactorY);
	const int SliderWidth        = int(100 * scalingFactorX);
	const int SliderHeight       = int(15 * scalingFactorY);
	const int TextWidth          = int(150 * scalingFactorX);
	const int xLocation1	       = int(90 * scalingFactorX);
	const int xLocation2         = int(-250 * scalingFactorX);
	const int xLocation3         = int(-80 * scalingFactorX);
	const int ComboBoxDropHeight = int(120 * scalingFactorY);

	WCHAR sz[100];
 
	//g_MainUI.AddButton( IDC_TOGGLEVIDEOMENU,  L"Video Specifics", xLocation1, iY += ButtonHeight, ButtonWidth, ButtonHeight, VK_F1 );	
	g_SharedUI.AddButton( IDC_LOADSTYLE,  L"Load Style", xLocation1, iY += ButtonHeight, ButtonWidth, ButtonHeight, VK_F12 );	
	g_MainUI.AddButton( IDC_TOGGLESPRITEMENU, L"Stroke Properties", xLocation1, iY += ButtonHeight, ButtonWidth, ButtonHeight, VK_F2 );
    g_SharedUI.AddButton( IDC_SAVESTYLE,  L"Save Style", xLocation1, iY, ButtonWidth, ButtonHeight, VK_F11 );
	g_MainUI.AddButton( IDC_TOGGLESHAREMENU,  L"Shared Selection ", xLocation1, iY += ButtonHeight, ButtonWidth, ButtonHeight, VK_F3);
	g_SharedUI.AddButton( IDC_IMAGECAPTURE,  L"Image Capture", xLocation1, iY, ButtonWidth, ButtonHeight, VK_F2 );
	//g_SharedUI.AddButton( IDC_CLIPCAPTURE,  L"Clip Capture", xLocation1, iY += ButtonHeight, ButtonWidth, ButtonHeight, VK_F1 );
	g_SharedUI.AddCheckBox( IDC_CLIPCAPTURE, L"Clip Capture", xLocation1, iY += ButtonHeight, ButtonWidth, ButtonHeight, renderer->OnScreenCapture );
	g_SharedUI.AddButton( IDC_RESETINDEX,  L"Reset Index", xLocation1, iY += ButtonHeight, ButtonWidth, ButtonHeight);
	g_SharedUI.AddButton( IDC_SWITCHGRADIENTTUNE,  L"Switch Focus", xLocation1, iY += ButtonHeight, ButtonWidth, ButtonHeight);

	iY = int(-200 * scalingFactorY);
	g_SharedUI.AddCheckBox( IDC_PAUSEANIMATION,     L"Pause Animation(model)", xLocation2, iY + ButtonHeight * 14, ButtonWidth + 10, SliderHeight, renderer->OnSkinnedMeshRenderingPause );
	g_SharedUI.AddCheckBox( IDC_CHANGELOOKAT,     L"Change LookAt Pos", xLocation2, iY + ButtonHeight * 15, ButtonWidth, SliderHeight, renderer->OnChangingLookAtPos );
	g_SharedUI.AddCheckBox( IDC_LOADVIDEO,     L"Load Video", xLocation2, iY + ButtonHeight * 16, ButtonWidth, SliderHeight, renderer->OnVideoLoaded );
	g_SharedUI.AddCheckBox( IDC_OPTICALFLOW, L"Optical Flow", xLocation2, iY + ButtonHeight * 17, ButtonWidth, SliderHeight, renderer->OnOpticalFlow );
    g_SpriteUI.AddCheckBox( IDC_TOGGLEGRADIENTANGLE, L"Use Red for Orientation", xLocation1, iY += ButtonHeight, ButtonWidth, SliderHeight, renderer->OnRealGradientAngle );
	g_SpriteUI.AddCheckBox( IDC_TOGGLECLIPPING, L"Clip Strokes at edges", xLocation1 , iY += ButtonHeight, ButtonWidth, SliderHeight, renderer->OnClipping );
	iY += ButtonHeight;


	CDXUTComboBox* pComboBox = NULL;

	g_SpriteUI.AddStatic( IDC_STATIC, L"Stroke Texture", xLocation1, iY, ButtonWidth, SliderHeight );
	iY += ButtonHeight;
	g_SpriteUI.AddComboBox( IDC_STYLE_SELECTION, xLocation1, iY, ButtonWidth, ButtonHeight, 'Q', false, &pComboBox );
    if( pComboBox )pComboBox->SetDropHeight( ComboBoxDropHeight );
	for (unsigned int i = 0; i < stylization->StrokeTexture.size(); i++)
		pComboBox->AddItem(stylization->StrokeTexture[i], NULL);
	pComboBox->SetSelectedByIndex(stylization->SelectedRenderingStyle);

	iY += ButtonHeight;
	g_SpriteUI.AddStatic( IDC_STATIC, L"Alpha Texture", xLocation1, iY, ButtonWidth, SliderHeight );
	iY += ButtonHeight;
	g_SpriteUI.AddComboBox( IDC_ALPHA_SELECTION, xLocation1, iY, ButtonWidth, ButtonHeight, 'Q', false, &pComboBox );
    if( pComboBox ) pComboBox->SetDropHeight( ComboBoxDropHeight );
	for (unsigned int i = 0; i < stylization->AlphaMaskTexture.size(); i++)
		pComboBox->AddItem(stylization->AlphaMaskTexture[i], NULL);	
	pComboBox->SetSelectedByIndex(stylization->SelectedAlphaMask);


	iY += ButtonHeight;
    g_SharedUI.AddStatic( IDC_STATIC, L"Input Image", xLocation1, iY, ButtonWidth, SliderHeight );
	

    iY += ButtonHeight;
    g_SharedUI.AddComboBox( IDC_LOAD_IMAGE, xLocation1, iY, ButtonWidth, ButtonHeight, 'Q', false, &pComboBox );
    if( pComboBox ) pComboBox->SetDropHeight( ComboBoxDropHeight );
	for (unsigned int i = 0; i < stylization->ImageName.size(); i++)
		pComboBox->AddItem(stylization->ImageName[i], NULL);
	pComboBox->SetSelectedByIndex(stylization->SelectedImage);


	iY += ButtonHeight;
	g_SharedUI.AddStatic( IDC_STATIC, L"Input Video", xLocation1, iY, ButtonWidth, SliderHeight );

	StringCchPrintf( sz, 100, L"Contrast: %0.2f", float(stylization->ContrastThreshold) ); 
    g_SharedUI.AddStatic( IDC_CONTRAST_STATIC, sz, xLocation2, iY, TextWidth, SliderHeight );


	iY += ButtonHeight;
	g_SharedUI.AddComboBox( IDC_LOAD_VIDEOFILE, xLocation1, iY, ButtonWidth, ButtonHeight, 'Q', false, &pComboBox );
    if( pComboBox ) pComboBox->SetDropHeight( ComboBoxDropHeight );
	for (unsigned int i = 0; i < stylization->VideoName.size(); i++)
		pComboBox->AddItem(stylization->VideoName[i], NULL);
	pComboBox->SetSelectedByIndex(stylization->SelectedVideo);

	g_SharedUI.AddSlider( IDC_CONTRAST_SCALE, xLocation2, iY, SliderWidth, SliderHeight, 60, 140, int(stylization->ContrastThreshold * 100) );
	

	iY += ButtonHeight;
	g_SharedUI.AddStatic( IDC_STATIC, L"Input Animated Model", xLocation1, iY, ButtonWidth, SliderHeight );
	g_SharedUI.AddStatic( IDC_STATIC, L"Session", xLocation2, iY, ButtonWidth, SliderHeight );

	iY += ButtonHeight;
	g_SharedUI.AddComboBox( IDC_LOAD_ANIMATEDMODEL, xLocation1, iY, ButtonWidth, ButtonHeight, 'Q', false, &pComboBox );
    if( pComboBox ) pComboBox->SetDropHeight( ComboBoxDropHeight );
	for (unsigned int i = 0; i < stylization->AnimatedModelName.size(); i++)
		pComboBox->AddItem(stylization->AnimatedModelName[i], NULL);
	pComboBox->SetSelectedByIndex(stylization->SelectedAnimatedModel);

	g_SharedUI.AddComboBox( IDC_SESSION_SELECTION, xLocation2, iY, ButtonWidth, ButtonHeight, 'Q', false, &pComboBox );
    if( pComboBox )pComboBox->SetDropHeight( ComboBoxDropHeight );
	for (unsigned int i = 0; i < stylization->Session.size(); i++)
		pComboBox->AddItem(stylization->Session[i], NULL);
	pComboBox->SetSelectedByIndex(stylization->SelectedRenderingSession);

	
	iY += ButtonHeight;
	g_SharedUI.AddStatic( IDC_STATIC, L"Input Static Model", xLocation1, iY, ButtonWidth, SliderHeight );
	iY += ButtonHeight;
	g_SharedUI.AddComboBox( IDC_LOAD_STATICMODEL, xLocation1, iY, ButtonWidth, ButtonHeight, 'Q', false, &pComboBox );
    if( pComboBox ) pComboBox->SetDropHeight( ComboBoxDropHeight );
	for (unsigned int i = 0; i < stylization->StaticModelName.size(); i++)
		pComboBox->AddItem(stylization->StaticModelName[i], NULL);
	pComboBox->SetSelectedByIndex(stylization->SelectedStaticModel);

	iY += ButtonHeight;
	g_SharedUI.AddStatic( IDC_STATIC, L"Input Obj Model", xLocation1, iY, ButtonWidth, SliderHeight );

	iY += ButtonHeight;
	g_SharedUI.AddComboBox( IDC_LOAD_OBJMODEL, xLocation1, iY, ButtonWidth, ButtonHeight, 'Q', false, &pComboBox );
    if( pComboBox ) pComboBox->SetDropHeight( ComboBoxDropHeight );
	for (unsigned int i = 0; i < stylization->ObjModelName.size(); i++)
		pComboBox->AddItem(stylization->ObjModelName[i], NULL);
	pComboBox->SetSelectedByIndex(stylization->SelectedObjModel);

	iY += int(ButtonHeight * 0.5);
	StringCchPrintf( sz, 100, L"Gradient Threshold: %0.2f", float(stroke->gradientThreshold) ); 
    g_SpriteUI.AddStatic( IDC_GRADIENT_STATIC, sz, xLocation1, iY += ButtonHeight, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"Probability (Large): %0.2f", float(stroke->ProbabilityLargeThreshold) ); 
	g_SpriteUI.AddStatic( IDC_PROBABILITYLARGE_STATIC, sz, xLocation2, iY, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"Alpha (Large): %0.2f", float(stroke->AlphaLargeThreshold) ); 
	g_SpriteUI.AddStatic( IDC_ALPHALARGE_STATIC, sz, xLocation3, iY, TextWidth, SliderHeight );
	g_SharedUI.AddStatic( IDC_STATIC, L"Rendering Stage", xLocation1, iY, ButtonWidth, SliderHeight );
	
	

    g_SpriteUI.AddSlider( IDC_GRADIENT_SCALE, xLocation1, iY += ButtonHeight, SliderWidth, SliderHeight, 0, 100, int(stroke->gradientThreshold * 100) );
	g_SpriteUI.AddSlider( IDC_PROBABILITYLARGE_SCALE, xLocation2, iY, SliderWidth, SliderHeight, 0, 600, int(stroke->ProbabilityLargeThreshold * 1000) );
	g_SpriteUI.AddSlider( IDC_ALPHALARGE_SCALE, xLocation3, iY, SliderWidth, SliderHeight, 1, 100, int(stroke->AlphaLargeThreshold * 100) );
	
	
	g_SharedUI.AddComboBox( IDC_RENDERING_SELECTION, xLocation1, iY, ButtonWidth, ButtonHeight, 'Q', false, &pComboBox );
    if( pComboBox ) pComboBox->SetDropHeight( ComboBoxDropHeight );
	for (unsigned int i = 0; i < stylization->renderingMode.size(); i++)
		pComboBox->AddItem(stylization->renderingMode[i], NULL);	
	pComboBox->SetSelectedByIndex(stylization->SelectedRenderingMode);

	StringCchPrintf( sz, 100, L"Probability (Medium): %0.2f", float(stroke->ProbabilityMediumThreshold) ); 
	g_SpriteUI.AddStatic( IDC_PROBABILITYMEDIUM_STATIC, sz, xLocation2, iY += ButtonHeight, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"Alpha (Medium): %0.2f", float(stroke->AlphaMediumThreshold) ); 
	g_SpriteUI.AddStatic( IDC_ALPHAMEDIUM_STATIC, sz, xLocation3, iY, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"Num Frame: %d", video->GetNumFrame() );
	g_SharedUI.AddStatic( IDC_NUM_FRAME_STATIC, sz, xLocation2, iY, TextWidth, SliderHeight );
	g_SharedUI.AddCheckBox( IDC_ENABLEFOCUS, L"Enable Focus", xLocation1, iY, ButtonWidth, ButtonHeight, renderer->OnEnableFocus );


    
	g_SpriteUI.AddSlider( IDC_PROBABILITYMEDIUM_SCALE, xLocation2, iY += ButtonHeight, SliderWidth, SliderHeight, 0, 100, int(stroke->ProbabilityMediumThreshold * 100) );
	g_SpriteUI.AddSlider( IDC_ALPHAMEDIUM_SCALE, xLocation3, iY, SliderWidth, SliderHeight, 1, 100, int(stroke->AlphaMediumThreshold * 100) );
	g_SharedUI.AddSlider( IDC_NUM_FRAME_SCALE, xLocation2, iY, SliderWidth, SliderHeight, 0, 300, video->GetNumFrame());

	//StringCchPrintf( sz, 100, L"Num Frame: %d", NumFrame );
	//g_SharedUI.AddStatic( IDC_NUM_FRAME_STATIC, sz, xLocation2, iY, TextWidth, SliderHeight );
	//StringCchPrintf( sz, 100, L"Num Iteration: %d", Max_Interation );
	//g_SharedUI.AddStatic( IDC_NUMITERATION_STATIC, sz, xLocation2, iY += ButtonHeight, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"Focus (f): %0.2f", float(stylization->FocusfThreshold) ); 
	g_SharedUI.AddStatic( IDC_FOCUSF_STATIC, sz, xLocation1, iY += ButtonHeight, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"Probability (Small): %0.2f", float(stroke->ProbabilitySmallThreshold) ); 
	g_SpriteUI.AddStatic( IDC_PROBABILITYSMALL_STATIC, sz, xLocation2, iY, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"Alpha (Small): %0.2f", float(stroke->AlphaSmallThreshold) ); 
	g_SpriteUI.AddStatic( IDC_ALPHASMALL_STATIC, sz, xLocation3, iY, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"Flow Lambda: %0.2f", video->GetLambda() ); 
	g_SharedUI.AddStatic( IDC_LAMBDA_STATIC, sz, xLocation2, iY, TextWidth, SliderHeight );
	

	
	//g_SharedUI.AddSlider( IDC_DENSITY_SCALE, xLocation1, iY += ButtonHeight, SliderWidth, SliderHeight, 1, 20, Max_Interation );
	g_SharedUI.AddSlider( IDC_FOCUSF_SCALE, xLocation1, iY += ButtonHeight, SliderWidth, SliderHeight, 1, 100, int(stylization->FocusfThreshold * 100) );
	g_SpriteUI.AddSlider( IDC_PROBABILITYSMALL_SCALE, xLocation2, iY, SliderWidth, SliderHeight, 0, 100, int(stroke->ProbabilitySmallThreshold * 100) );
	g_SpriteUI.AddSlider( IDC_ALPHASMALL_SCALE, xLocation3, iY, SliderWidth, SliderHeight, 1, 100, int(stroke->AlphaSmallThreshold * 100) );
    g_SharedUI.AddSlider( IDC_LAMBDA_SCALE, xLocation2, iY, SliderWidth, SliderHeight, 0, 100, int(video->GetLambda() * 100) );
    

	StringCchPrintf( sz, 100, L"Stroke Length: %0.2f", float(stroke->strokeLength) ); 
    g_SpriteUI.AddStatic( IDC_LENGTH_STATIC, sz, xLocation1, iY += ButtonHeight, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"Focus (r1): %0.2f", float(stylization->Focusr1Threshold) ); 
	g_SharedUI.AddStatic( IDC_FOCUSR1_STATIC, sz, xLocation1, iY, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"Gradient Threshold 1: %0.2f", float(stroke->GradientLargeThreshold) ); 
	g_SpriteUI.AddStatic( IDC_GRADIENTLARGE_STATIC, sz, xLocation2, iY, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"Relative Size (Medium): %0.2f", float(stroke->SizeMediumThreshold) ); 
	g_SpriteUI.AddStatic( IDC_SIZEMEDIUM_STATIC, sz, xLocation3, iY, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"FadeIn Speed: %0.2f", stylization->FadeInSpeedThreshold );
	g_SharedUI.AddStatic( IDC_FADEINSPEED_STATIC, sz, xLocation2, iY, TextWidth, SliderHeight );
	

    g_SpriteUI.AddSlider( IDC_LENGTH_SCALE, xLocation1, iY += ButtonHeight, SliderWidth, SliderHeight, 100, 3000, int(stroke->strokeLength * 100) );
	g_SharedUI.AddSlider( IDC_FOCUSR1_SCALE, xLocation1, iY, SliderWidth, SliderHeight, 1, 100, int(stylization->Focusr1Threshold * 100) );
	g_SpriteUI.AddSlider( IDC_GRADIENTLARGE_SCALE, xLocation2, iY, SliderWidth, SliderHeight, 0, 300, int(stroke->GradientLargeThreshold * 1000) );
	g_SpriteUI.AddSlider( IDC_SIZEMEDIUM_SCALE, xLocation3, iY, SliderWidth, SliderHeight, 0, 100, int(stroke->SizeMediumThreshold * 100) );
	g_SharedUI.AddSlider( IDC_FADEINSPEED_SCALE, xLocation2, iY , SliderWidth, SliderHeight, 0, 50, int(stylization->FadeInSpeedThreshold * 100) );

	StringCchPrintf( sz, 100, L"Stroke Width: %0.2f", float(stroke->strokeWidth) ); 
    g_SpriteUI.AddStatic( IDC_WIDTH_STATIC, sz, xLocation1, iY += ButtonHeight, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"Focus (r2): %0.2f", float(stylization->Focusr2Threshold) ); 
	g_SharedUI.AddStatic( IDC_FOCUSR2_STATIC, sz, xLocation1, iY, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"Gradient Threshold 2: %0.2f", float(stroke->GradientMediumThreshold) ); 
	g_SpriteUI.AddStatic( IDC_GRADIENTMEDIUM_STATIC, sz, xLocation2, iY, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"Relative Size (Small): %0.2f", float(stroke->SizeSmallThreshold) ); 
	g_SpriteUI.AddStatic( IDC_SIZESMALL_STATIC, sz, xLocation3, iY, TextWidth, SliderHeight );
	StringCchPrintf( sz, 100, L"FadeOut Speed: %0.2f", stylization->FadeOutSpeedThreshold );
	g_SharedUI.AddStatic( IDC_FADEOUTSPEED_STATIC, sz, xLocation2, iY, TextWidth, SliderHeight );

    g_SpriteUI.AddSlider( IDC_WIDTH_SCALE, xLocation1, iY += ButtonHeight, SliderWidth, SliderHeight, 100, 2000, int(stroke->strokeWidth * 100) );
	g_SharedUI.AddSlider( IDC_FOCUSR2_SCALE, xLocation1, iY, SliderWidth, SliderHeight, 1, 100, int(stylization->Focusr2Threshold * 100) );
	g_SpriteUI.AddSlider( IDC_GRADIENTMEDIUM_SCALE, xLocation2, iY, SliderWidth, SliderHeight, 0, 100, int(stroke->GradientMediumThreshold * 100) );
	g_SpriteUI.AddSlider( IDC_SIZESMALL_SCALE, xLocation3, iY, SliderWidth, SliderHeight, 0, 100, int(stroke->SizeSmallThreshold * 100) );
	g_SharedUI.AddSlider( IDC_FADEOUTSPEED_SCALE, xLocation2, iY, SliderWidth, SliderHeight, 0, 50, int(stylization->FadeOutSpeedThreshold * 100) );

	iY += ButtonHeight;
	StringCchPrintf( sz, 100, L"%d", video->GetFrameIndex() ); 
    g_SharedUI.AddStatic( IDC_VIDEOPLAY_STATIC, sz, int(75 * scalingFactorX), iY, int(30 * scalingFactorX), SliderHeight );
    g_SharedUI.AddSlider( IDC_VIDEOPLAY_SCALE, int(-20 * scalingFactorX), iY, SliderWidth, SliderHeight, 1, video->GetNumFrame(), video->GetFrameIndex() );
	

	g_SharedUI.AddButton( IDC_START_VIDEO, L"Start", int(-70 * scalingFactorX), iY, int(30 * scalingFactorX), SliderHeight );	
	g_SharedUI.AddButton( IDC_STOP_VIDEO, L"Stop", int(100 * scalingFactorX), iY, int(30 * scalingFactorX), SliderHeight );
	g_SharedUI.AddCheckBox( IDC_SCREEN_SHOT, L"Shot", int(-130 * scalingFactorX), iY, int(60 * scalingFactorX), SliderHeight, renderer->OnScreenCapture );

	g_SharedUI.SetControlEnabled( IDC_START_VIDEO, false );
	g_SharedUI.SetControlEnabled( IDC_STOP_VIDEO, false );
	g_SharedUI.SetControlEnabled( IDC_VIDEOPLAY_SCALE, false );
}

HRESULT Viewer::CreateUI()
{
	HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D10CreateDevice( DXUTGetD3D10Device() ) );
    V_RETURN( g_D3DSettingsDlg.OnD3D10CreateDevice( DXUTGetD3D10Device() ) );
    V_RETURN( D3DX10CreateFont( DXUTGetD3D10Device(), 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                                OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                                L"Arial", &g_pFont ) );
    V_RETURN( D3DX10CreateSprite( DXUTGetD3D10Device(), 512, &g_pSprite ) );
    g_pTxtHelper = new CDXUTTextHelper( NULL, NULL, g_pFont, g_pSprite, 15 );
	return hr;
}

void Viewer::ToggleMenu()
{
	switch( gameMode )
    {
        case RENDERING:
            gameMode = MAIN_MENU; 
			break;
        case MAIN_MENU:
            gameMode = RENDERING; 			
			break;
		case VIDEO_MENU: case SPRITE_MENU: case SHARE_MENU:	
			gameMode = MAIN_MENU; 
			break;
    }
    //DXUTSetCursorSettings( ( gameMode != RENDERING ), true );
}

void Viewer::RenderText()
{
    g_pTxtHelper->Begin();
	
    g_pTxtHelper->SetInsertionPos( 2, 0 );
    g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
	g_pTxtHelper->DrawTextLine( DXUTGetFrameStats(true) );
    //g_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
    //g_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );	

	g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 1.0f, 1.0f ) );
	if (renderer->OnVideoLoaded)
	{
		g_pTxtHelper->DrawTextLine(L"Video is loaded");
		if (renderer->OnFramesRendering)
		{
			g_pTxtHelper->DrawTextLine(L"Render Frame by Frame");
		}
		if (renderer->OnOpticalFlowLoaded)
		{
			g_pTxtHelper->DrawTextLine(L"Optical Flow Loaded");
		}
	}
	
	switch(stylization->SelectedRenderingSession )
	{
		case STATIC_MODEL_OF:
			g_pTxtHelper->DrawTextLine(L"Static Model With Optical Flow"); break;
		case STATIC_MODEL:
			g_pTxtHelper->DrawTextLine(L"Static Model Without Optical Flow"); break;
		case ANIMATED_MODEL:
			g_pTxtHelper->DrawTextLine(L"Animated Model"); break;
		case MODEL_ON_IMAGE:
			g_pTxtHelper->DrawTextLine(L"Model on Image"); break;
		case MODEL_ON_VIDEO:
			g_pTxtHelper->DrawTextLine(L"Model on video"); break;
		case VIDEO:
			g_pTxtHelper->DrawTextLine(L"Video Rendering"); break;
		case IMAGE:
			g_pTxtHelper->DrawTextLine(L"Image Rendering"); break;
		case OBJ_MODEL:
			g_pTxtHelper->DrawTextLine(L"Obj Model Rendering"); break;
	}

	if (renderer->OnSkinnedMeshRenderingPause)
		g_pTxtHelper->DrawTextLine(L"Animation Paused");

	g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
	renderer->Num_Primitives = renderer->Num_SmallPrimitives + renderer->Num_MediumPrimitives + renderer->Num_LargePrimitives;
	g_pTxtHelper->DrawFormattedTextLine(L"Total Brush Stroke Count : %d", (int)renderer->Num_Primitives);
	g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 1.0f, 1.0f ) );
	g_pTxtHelper->DrawFormattedTextLine(L"          Pre:        Cur:      Append:   Delete:");
	g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );

	g_pTxtHelper->DrawFormattedTextLine(L"S : %06d   %06d   %06d   %06d", (int)renderer->Num_SmallStrokePreviousFrame, (int)renderer->Num_SmallPrimitives, (int)renderer->Num_SmallAppended, (int)renderer->Num_SmallDeleted + (int)renderer->Num_SmallUpdated);
	g_pTxtHelper->DrawFormattedTextLine(L"M : %06d   %06d   %06d   %06d", (int)renderer->Num_MediumStrokePreviousFrame, (int)renderer->Num_MediumPrimitives, (int)renderer->Num_MediumAppended, (int)renderer->Num_MediumDeleted + (int)renderer->Num_MediumUpdated);
	g_pTxtHelper->DrawFormattedTextLine(L"L : %06d   %06d   %06d   %06d", (int)renderer->Num_LargeStrokePreviousFrame, (int)renderer->Num_LargePrimitives, (int)renderer->Num_LargeAppended, (int)renderer->Num_LargeDeleted + (int)renderer->Num_LargeUpdated);
	//g_pTxtHelper->DrawFormattedTextLine(L"Current Time : %f", CurTime);

	if (stylization->SelectedRenderingSession != IMAGE && 
		stylization->SelectedRenderingSession != VIDEO)
	{
		g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 1.0f, 1.0f ) );
		D3DXVECTOR3 eyePt = *camera->GetCamera().GetEyePt();
		D3DXVECTOR3 lookAt = *camera->GetCamera().GetLookAtPt();
		g_pTxtHelper->DrawFormattedTextLine(L"Camera Position : %.4f %.4f %.4f", eyePt.x, eyePt.y, eyePt.z);
		g_pTxtHelper->DrawFormattedTextLine(L"LookAt Position : %.4f %.4f %.4f", lookAt.x, lookAt.y, lookAt.z);
	}
    g_pTxtHelper->End();
}

void Viewer::ChangeMode(float fElapsedTime)
{
	//DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
	switch( gameMode )
    {
        case RENDERING:
			g_HUD.OnRender( fElapsedTime ); 
            break;
        case MAIN_MENU:
		{
			RenderText(); 
            g_MainUI.OnRender( fElapsedTime ); 			
			break;   
		}
		case VIDEO_MENU:
		{
			RenderText(); 			
            g_SharedUI.OnRender( fElapsedTime );			
			break;   
		}
		case SPRITE_MENU:
		{
			RenderText(); 			
            g_SpriteUI.OnRender( fElapsedTime ); 			
			break;   
		}
		case SHARE_MENU:
		{
			RenderText();
			g_SharedUI.OnRender( fElapsedTime );	
			break; 
		}	
		
    }
	//DXUT_EndPerfEvent();
	//g_HUD.OnRender( fElapsedTime ); 
}

void Viewer::ResizeUI()
{
	float scalingFactorX = WindowWidth * 0.002f; 
	float scalingFactorY = WindowHeight * 0.002f;
    g_HUD.SetLocation     ( WindowWidth - int(170 * scalingFactorX),  0 );
    g_HUD.SetSize         ( int(170               * scalingFactorX), int(170                * scalingFactorY) );
    g_SharedUI.SetLocation( WindowWidth - int(230 * scalingFactorX), WindowHeight - int(300 * scalingFactorY) );
    g_SharedUI.SetSize    ( int(170               * scalingFactorX), int(300                * scalingFactorY) );
	g_SharedUI.SetLocation ( WindowWidth - int(230 * scalingFactorX), WindowHeight - int(300 * scalingFactorY) );
    g_SharedUI.SetSize     ( int(170               * scalingFactorX), int(300                * scalingFactorY) );
	g_SpriteUI.SetLocation( WindowWidth - int(230 * scalingFactorX), WindowHeight - int(300 * scalingFactorY) );
    g_SpriteUI.SetSize    ( int(170               * scalingFactorX), int(300                * scalingFactorY) );
	g_MainUI.SetLocation  ( WindowWidth - int(230 * scalingFactorX), WindowHeight - int(300 * scalingFactorY) );
    g_MainUI.SetSize      ( int(170               * scalingFactorX), int(300                * scalingFactorY) );
}