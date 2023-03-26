#include "DXUT.h"
#include "Style.h"
#include <fstream>
#include "UserInterface.h"


#define FILE_FILTER_TEXT \
	TEXT("Text Files (*.txt)\0*.txt;\0\0")
	 //TEXT("All Files (*.*)\0*.*;\0\0")
    //TEXT("Image files (*.bmp; *.jpg;)\0*.bmp; *.jpg;\0")\
    //TEXT("Video Files (*.avi; *.mov; *.mpg; *.mpeg; *.wmv;)\0*.avi; *.mov; *.mpg; *.mpeg; *.wmv;\0")\

Style*	style;

Style::Style(){
	staticModelStyleName.push_back("Style\\tankStyle.txt");
	staticModelStyleName.push_back("Style\\tankStyle.txt");
	staticModelStyleName.push_back("Style\\nightStyle.txt");

	animatedModelStyleName.push_back("Style\\sodierStyle.txt");
	animatedModelStyleName.push_back("Style\\lizardStyle.txt");

	objModelStyleName.push_back("Style\\cupStyle.txt" );
	objModelStyleName.push_back("Style\\clockStyle.txt" );
	objModelStyleName.push_back("Style\\benchStyle.txt" );
	
}

void Style::LoadStyle(string givenName)
{
	ifstream in;
	if (givenName.length() != 0)
		in.open(givenName.c_str());
	else{
		WCHAR ParameterFileName[MAX_PATH] = L"";		// Playback File Name
		if (! GetClipFileName(ParameterFileName))
		{
			DWORD dwDlgErr = CommDlgExtendedError();
			// Don't show output if user cancelled the selection (no dlg error)
			if (dwDlgErr)
				MessageBox( NULL,L"File Loading Error", L"Error", MB_OK );
			return;
		}
		ParameterFileName[MAX_PATH-1] = 0;    // NULL-terminate
		char FileName[MAX_PATH];
		wcstombs(FileName, ParameterFileName, MAX_PATH);
		in.open(FileName);
	}
	if (in.is_open())
	{
		int TexIndex, AlphaIndex;
		float Contrast, Len, Wid;
		float ProL, ProM, ProS;
		float Grad1, Grad2;
		float AlphaL, AlphaM, AlphaS;
		float RSizeM, RSizeS;
		float GradThresh;
		D3DXVECTOR3 eyePt, lookAt;
		float FadeIn, FadeOut;
		float f, r1, r2;
		bool Clipping, UseRed;
		WCHAR sz[100];
		in>>TexIndex>>AlphaIndex>>Contrast>>Len>>Wid>>
			ProL>>ProM>>ProS>>Grad1>>Grad2>>AlphaL>>
			AlphaM>>AlphaS>>RSizeM>>RSizeS>>GradThresh
			>>eyePt.x>>eyePt.y>>eyePt.z>>lookAt.x>>lookAt.y>>lookAt.z
			>>FadeIn>>FadeOut>>f>>r1>>r2>>Clipping>>UseRed;

		//camera->RecordEyeLookat(eyePt, lookAt);
		
		camera->SetCameraViewParams( &eyePt, &lookAt );

		shader->GradientThresholdShaderVariable   ->SetFloat( GradThresh );
		stroke->gradientThreshold = GradThresh;
		viewer->g_SpriteUI.GetSlider( IDC_GRADIENT_SCALE )->SetValue(int(GradThresh * 100));
		StringCchPrintf( sz, 100, L"Gradient Threshold: %0.2f", float(GradThresh) ); 
		viewer->g_SpriteUI.GetStatic( IDC_GRADIENT_STATIC )->SetText( sz );
		

		shader->StrokeLengthShaderVariable        ->SetFloat( Len );
		stroke->strokeLength = Len;
		viewer->g_SpriteUI.GetSlider( IDC_LENGTH_SCALE )->SetValue(int(Len * 100));
		StringCchPrintf( sz, 100, L"Stroke Length: %0.2f", Len ); 
		viewer->g_SpriteUI.GetStatic( IDC_LENGTH_STATIC )->SetText( sz );

		shader->StrokeWidthShaderVariable         ->SetFloat( Wid );
		stroke->strokeWidth = Wid;
		viewer->g_SpriteUI.GetSlider( IDC_WIDTH_SCALE )->SetValue(int(Wid * 100));
		StringCchPrintf( sz, 100, L"Stroke Width: %0.2f", Wid ); 
		viewer->g_SpriteUI.GetStatic( IDC_WIDTH_STATIC )->SetText( sz );

		shader->ProbabilityLargeShaderVariable	  ->SetFloat( ProL );
		stroke->ProbabilityLargeThreshold = ProL;
		viewer->g_SpriteUI.GetSlider( IDC_PROBABILITYLARGE_SCALE )->SetValue(int(ProL * 1000));
		StringCchPrintf( sz, 100, L"Probability (Large): %0.3f", ProL ); 
		viewer->g_SpriteUI.GetStatic( IDC_PROBABILITYLARGE_STATIC )->SetText( sz );				

		shader->ProbabilityMediumShaderVariable	  ->SetFloat( ProM );
		stroke->ProbabilityMediumThreshold = ProM;
		viewer->g_SpriteUI.GetSlider( IDC_PROBABILITYMEDIUM_SCALE )->SetValue(int(ProM * 100));
		StringCchPrintf( sz, 100, L"Probability (Medium): %0.3f", ProM ); 
		viewer->g_SpriteUI.GetStatic( IDC_PROBABILITYMEDIUM_STATIC )->SetText( sz );	

		shader->ProbabilitySmallShaderVariable	  ->SetFloat( ProS );
		stroke->ProbabilitySmallThreshold = ProS;
		viewer->g_SpriteUI.GetSlider( IDC_PROBABILITYSMALL_SCALE )->SetValue(int(ProS * 100));
		StringCchPrintf( sz, 100, L"Probability (Small): %0.3f", ProS ); 
		viewer->g_SpriteUI.GetStatic( IDC_PROBABILITYSMALL_STATIC )->SetText( sz );

		shader->GradientMediumShaderVariable	  ->SetFloat( Grad2 );
		stroke->GradientMediumThreshold = Grad2;
		viewer->g_SpriteUI.GetSlider( IDC_GRADIENTMEDIUM_SCALE )->SetValue(int(Grad2 * 100));
		StringCchPrintf( sz, 100, L"Gradient Threshold 2: %0.2f", Grad2 ); 
		viewer->g_SpriteUI.GetStatic( IDC_GRADIENTMEDIUM_STATIC )->SetText( sz );				

		shader->GradientLargeShaderVariable	      ->SetFloat( Grad1 );
		stroke->GradientLargeThreshold = Grad1;
		viewer->g_SpriteUI.GetSlider( IDC_GRADIENTLARGE_SCALE )->SetValue(int(Grad1 * 100));
		StringCchPrintf( sz, 100, L"Gradient Threshold 1: %0.2f", Grad1 ); 
		viewer->g_SpriteUI.GetStatic( IDC_GRADIENTLARGE_STATIC )->SetText( sz );	

		shader->AlphaLargeShaderVariable	      ->SetFloat( AlphaL );
		stroke->AlphaLargeThreshold = AlphaL;
		viewer->g_SpriteUI.GetSlider( IDC_ALPHALARGE_SCALE )->SetValue(int(AlphaL * 100));
		StringCchPrintf( sz, 100, L"Alpha (Large): %0.2f", AlphaL ); 
		viewer->g_SpriteUI.GetStatic( IDC_ALPHALARGE_STATIC )->SetText( sz );					

		shader->AlphaMediumShaderVariable	      ->SetFloat( AlphaM );
		stroke->AlphaMediumThreshold = AlphaM;
		viewer->g_SpriteUI.GetSlider( IDC_ALPHAMEDIUM_SCALE )->SetValue(int(AlphaM * 100));
		StringCchPrintf( sz, 100, L"Alpha (Medium): %0.2f", AlphaM ); 
		viewer->g_SpriteUI.GetStatic( IDC_ALPHAMEDIUM_STATIC )->SetText( sz );

		shader->AlphaSmallShaderVariable	      ->SetFloat( AlphaS );
		stroke->AlphaSmallThreshold = AlphaS;
		viewer->g_SpriteUI.GetSlider( IDC_ALPHASMALL_SCALE )->SetValue(int(AlphaS * 100));
		StringCchPrintf( sz, 100, L"Alpha (Small): %0.2f", AlphaS ); 
		viewer->g_SpriteUI.GetStatic( IDC_ALPHASMALL_STATIC )->SetText( sz );

		shader->SizeMediumShaderVariable	      ->SetFloat( RSizeM );
		stroke->SizeMediumThreshold = RSizeM;
		viewer->g_SpriteUI.GetSlider( IDC_SIZEMEDIUM_SCALE )->SetValue(int(RSizeM * 100));
		StringCchPrintf( sz, 100, L"Relative Size (Medium): %0.2f", RSizeM ); 
		viewer->g_SpriteUI.GetStatic( IDC_SIZEMEDIUM_STATIC )->SetText( sz );

		shader->SizeSmallShaderVariable	          ->SetFloat( RSizeS );
		stroke->SizeSmallThreshold = RSizeS;
		viewer->g_SpriteUI.GetSlider( IDC_SIZESMALL_SCALE )->SetValue(int(RSizeS * 100));
		StringCchPrintf( sz, 100, L"Relative Size (Small): %0.2f", RSizeS ); 
		viewer->g_SpriteUI.GetStatic( IDC_SIZESMALL_STATIC )->SetText( sz );

		shader->ContrastShaderVariable			  ->SetFloat( Contrast );
		stylization->ContrastThreshold = Contrast;
		viewer->g_SharedUI.GetSlider( IDC_CONTRAST_SCALE )->SetValue(int(Contrast * 100));
		StringCchPrintf( sz, 100, L"Contrast: %0.2f", Contrast ); 
		viewer->g_SharedUI.GetStatic( IDC_CONTRAST_STATIC )->SetText( sz );					

		//Set Alpha Texture
		textureRT->SetAlphaTexture(AlphaIndex);
		stylization->SelectedAlphaMask = AlphaIndex;
		viewer->g_SpriteUI.GetComboBox(IDC_ALPHA_SELECTION)->SetSelectedByIndex(AlphaIndex);

		//Set Stroke Texture
		shader->RenderingStyleShaderVariable->SetInt(TexIndex);
		
		textureRT->SetStrokeTexture(TexIndex);
		textureRT->GetStrokeTextureInfo(TexIndex);
		stylization->SelectedRenderingStyle = TexIndex;
		viewer->g_SpriteUI.GetComboBox(IDC_STYLE_SELECTION)->SetSelectedByIndex(TexIndex);

		shader->FocusfVariable			  ->SetFloat( f );
		stylization->FocusfThreshold = f;
		viewer->g_SharedUI.GetSlider( IDC_FOCUSF_SCALE )->SetValue(int(f * 100));
		StringCchPrintf( sz, 100, L"Focus (f): %0.2f", f ); 
		viewer->g_SharedUI.GetStatic( IDC_FOCUSF_STATIC )->SetText( sz );	

		shader->Focusr1Variable			  ->SetFloat( r1 );
		stylization->Focusr1Threshold = r1;
		viewer->g_SharedUI.GetSlider( IDC_FOCUSR1_SCALE )->SetValue(int(r1 * 100));
		StringCchPrintf( sz, 100, L"Focus (r1): %0.2f", r1 ); 
		viewer->g_SharedUI.GetStatic( IDC_FOCUSR1_STATIC )->SetText( sz );

		shader->Focusr2Variable			  ->SetFloat( r2 );
		stylization->Focusr2Threshold = r2;
		viewer->g_SharedUI.GetSlider( IDC_FOCUSR2_SCALE )->SetValue(int(r2 * 100));
		StringCchPrintf( sz, 100, L"Focus (r2): %0.2f", r2 ); 
		viewer->g_SharedUI.GetStatic( IDC_FOCUSR2_STATIC )->SetText( sz );

		shader->FadeOutSpeedShaderVariable			  ->SetFloat( FadeOut );
		stylization->FadeOutSpeedThreshold = FadeOut;
		viewer->g_SharedUI.GetSlider( IDC_FADEOUTSPEED_SCALE )->SetValue(int(FadeOut * 100));
		StringCchPrintf( sz, 100, L"Fade Out Speed: %0.2f", FadeOut ); 
		viewer->g_SharedUI.GetStatic( IDC_FADEOUTSPEED_STATIC )->SetText( sz );

		shader->FadeInSpeedShaderVariable			  ->SetFloat( FadeIn );
		stylization->FadeInSpeedThreshold = FadeIn;
		viewer->g_SharedUI.GetSlider( IDC_FADEINSPEED_SCALE )->SetValue(int(FadeIn * 100));
		StringCchPrintf( sz, 100, L"Fade In Speed: %0.2f", FadeIn ); 
		viewer->g_SharedUI.GetStatic( IDC_FADEINSPEED_STATIC )->SetText( sz );


		shader->ToggleClippingShaderVariable->SetBool( Clipping );
		renderer->OnClipping = Clipping;
		viewer->g_SpriteUI.GetCheckBox( IDC_TOGGLECLIPPING )->SetChecked(Clipping);		

		shader->ToggleAngleShaderVariable->SetBool( UseRed );
		renderer->OnRealGradientAngle  = UseRed;
		viewer->g_SpriteUI.GetCheckBox( IDC_TOGGLEGRADIENTANGLE  )->SetChecked(UseRed);	
	}				
}

void Style::SaveStyle()
{
	WCHAR ParameterFileName[MAX_PATH] = L"";		// Playback File Name
	if (! SaveClipFileName(ParameterFileName))
	{
		DWORD dwDlgErr = CommDlgExtendedError();
		// Don't show output if user cancelled the selection (no dlg error)
		if (dwDlgErr)
			MessageBox( NULL,L"File Saving Error", L"Error", MB_OK );
		return;
	}
	ParameterFileName[MAX_PATH-1] = 0;    // NULL-terminate
	char FileName[MAX_PATH];
	wcstombs(FileName, ParameterFileName, MAX_PATH);
	ofstream out(FileName);
	if (out.is_open())
	{
		//int TexIndex, AlphaIndex;
		float Contrast, Len, Wid;
		float ProL, ProM, ProS;
		float Grad1, Grad2;
		float AlphaL, AlphaM, AlphaS;
		float RSizeM, RSizeS;
		float GradThresh;
		float FadeIn, FadeOut;
		float f, r1, r2;
		bool Clipping, UseRed;
		shader->GradientThresholdShaderVariable   ->GetFloat( &GradThresh );
		shader->StrokeLengthShaderVariable        ->GetFloat( &Len );
		shader->StrokeWidthShaderVariable         ->GetFloat( &Wid );
		shader->ProbabilityLargeShaderVariable	  ->GetFloat( &ProL );
		shader->ProbabilityMediumShaderVariable	  ->GetFloat( &ProM );
		shader->ProbabilitySmallShaderVariable	  ->GetFloat( &ProS );
		shader->GradientMediumShaderVariable	  ->GetFloat( &Grad2 );
		shader->GradientLargeShaderVariable	      ->GetFloat( &Grad1 );
		shader->AlphaLargeShaderVariable	      ->GetFloat( &AlphaL );
		shader->AlphaMediumShaderVariable	      ->GetFloat( &AlphaM );
		shader->AlphaSmallShaderVariable	      ->GetFloat( &AlphaS );
		shader->SizeMediumShaderVariable	      ->GetFloat( &RSizeM );
		shader->SizeSmallShaderVariable	          ->GetFloat( &RSizeS );
		shader->ContrastShaderVariable			  ->GetFloat( &Contrast );
		shader->FocusfVariable					  ->GetFloat( &f );
		shader->Focusr1Variable					  ->GetFloat( &r1 );
		shader->Focusr2Variable					  ->GetFloat( &r2 );
		shader->FadeInSpeedShaderVariable		  ->GetFloat( &FadeIn );
		shader->FadeOutSpeedShaderVariable		  ->GetFloat( &FadeOut );
		UseRed = renderer->OnRealGradientAngle;
		Clipping = renderer->OnClipping;

		D3DXVECTOR3 eyePt = *camera->GetCamera().GetEyePt();
		D3DXVECTOR3 lookAt = *camera->GetCamera().GetLookAtPt();
		out<<stylization->SelectedRenderingStyle<<"\n"<<stylization->SelectedAlphaMask<<"\n"
			<<Contrast<<"\n"<<Len<<"\n"<<Wid<<"\n"<<ProL<<"\n"<<ProM<<"\n"
			<<ProS<<"\n"<<Grad1<<"\n"<<Grad2<<"\n"<<AlphaL<<"\n"<<AlphaM<<"\n"
			<<AlphaS<<"\n"<<RSizeM<<"\n"<<RSizeS<<"\n"<<GradThresh<<"\n"
			<<eyePt.x<<" "<<eyePt.y<<" "<<eyePt.z<<"\n"
			<<lookAt.x<<" "<<lookAt.y<<" "<<lookAt.z<<"\n"
			<<FadeIn<<" "<<FadeOut<<"\n"<<f<<" "<<r1<<" "<<r2<<"\n"
			<<Clipping<<"\n"<<UseRed<<endl;

	}
}

//not working well, after this is called, the DXUT stuff is lost, don't know the reason
BOOL Style::GetClipFileName(LPTSTR szName)
{
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));


    // Reset filename
    *szName = 0;

	TCHAR szDirectory[MAX_PATH] = L"";
	if(!::GetCurrentDirectory(sizeof(szDirectory) - 1, szDirectory))
		szDirectory;

	TCHAR StylePath[MAX_PATH];
	_stprintf(StylePath,L"%s%s",szDirectory, L"\\Style"); 

    // Fill in standard structure fields
    ofn.lStructSize       = sizeof(OPENFILENAME);
    //ofn.hwndOwner         = g_hWnd;
    ofn.lpstrFilter       = FILE_FILTER_TEXT;
    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex      = 1;
    ofn.lpstrFile         = szName;
    ofn.nMaxFile          = MAX_PATH;
    ofn.lpstrTitle        = TEXT("Open Parameter File...\0");
    ofn.lpstrFileTitle    = NULL;
    ofn.lpstrDefExt       = TEXT("*\0");
    ofn.Flags             = OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST;
	//ofn.lpstrInitialDir   = TEXT("C:\\Users\\Cynthia Lu\\Documents\\FYT 7.0\\VideoFrames");
	//ofn.lpstrInitialDir   = TEXT("C:\\Users\\Cynthia Lu\\Documents\\ResearchProjects\\VideoStylization\\FYT 12.0");
	ofn.lpstrInitialDir	  = StylePath;
	

    // Create the standard file open dialog and return its result
    BOOL result = GetOpenFileName((LPOPENFILENAME)&ofn);
	SetCurrentDirectory( szDirectory );
	return result;
}


//not working well, after this is called, the DXUT stuff is lost, don't know the reason
BOOL Style::SaveClipFileName(LPTSTR szName)
{
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));

	TCHAR szDirectory[MAX_PATH] = L"";
	if(!::GetCurrentDirectory(sizeof(szDirectory) - 1, szDirectory))
		szDirectory;

	TCHAR StylePath[MAX_PATH];
	_stprintf(StylePath,L"%s%s",szDirectory, L"\\Style"); 


    // Reset filename
    *szName = 0;

    // Fill in standard structure fields
    ofn.lStructSize       = sizeof(OPENFILENAME);
    //ofn.hwndOwner         = g_hWnd;
    ofn.lpstrFilter       = FILE_FILTER_TEXT;
    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex      = 1;
    ofn.lpstrFile         = szName;
    ofn.nMaxFile          = MAX_PATH;
    ofn.lpstrTitle        = TEXT("Save Parameter File...\0");
    ofn.lpstrFileTitle    = NULL;
    ofn.lpstrDefExt       = TEXT("*\0");
    ofn.Flags             = OFN_PATHMUSTEXIST;
	//ofn.lpstrInitialDir   = TEXT("C:\\Users\\Cynthia Lu\\Documents\\ResearchProjects\\VideoStylization\\FYT 11.0");
	ofn.lpstrInitialDir    = StylePath;

	//Get files in a folder??
	//WIN32_FIND_DATA FindFileData;

	//FindFirstFile(StylePath, &FindFileData);
	//WCHAR* a = FindFileData.cFileName;
	

    // Create the standard file open dialog and return its result
    BOOL result = GetSaveFileName((LPOPENFILENAME)&ofn);
	SetCurrentDirectory( szDirectory );
	return result;
}