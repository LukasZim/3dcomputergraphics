#include "dxut.h"
#include "UserInterface.h"


//10/13
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
	HRESULT hr = S_OK;
	WCHAR sz[100];
    switch( nControlID )
    {
	
     //   case IDC_TOGGLEFULLSCREEN:
           // DXUTToggleFullScreen(); break;
		case IDC_TOGGLEVIDEOMENU:
			viewer->gameMode = VIDEO_MENU; 		
			break;
		case IDC_TOGGLESPRITEMENU:
			viewer->gameMode = SPRITE_MENU; 
			break;
		case IDC_TOGGLESHAREMENU:
			viewer->gameMode = SHARE_MENU;
		case IDC_OPTICALFLOW:
			renderer->OnOpticalFlow = viewer->g_SharedUI.GetCheckBox( IDC_OPTICALFLOW )->GetChecked();
			break;
		case IDC_LOADSTYLE:
			style->LoadStyle();
			break;
		case IDC_SAVESTYLE:
			style->SaveStyle();
			break;
		case IDC_IMAGECAPTURE:
			renderer->OnImageCapture = !renderer->OnImageCapture;
			break;
		case IDC_CLIPCAPTURE:
			renderer->OnScreenCapture = !renderer->OnScreenCapture;
			break;
		case IDC_RESETINDEX:
			textureRT->SetScreenShotIndex(0);			
			break;

		case IDC_PAUSEANIMATION:
			renderer->OnSkinnedMeshRenderingPause = !renderer->OnSkinnedMeshRenderingPause;
			break;
		case IDC_CHANGELOOKAT:
			renderer->OnChangingLookAtPos = !renderer->OnChangingLookAtPos;
			break;
		case IDC_LOADVIDEO:
			renderer->OnVideoLoaded = viewer->g_SharedUI.GetCheckBox( IDC_LOADVIDEO )->GetChecked();
			if (renderer->OnVideoLoaded)
			{	
				if (renderer->OnVideoLoaded)
					video->DeleteVideoFrames();

				hr = video->loadVideoFrames();
				if( FAILED( hr ) )
				{
					MessageBox( NULL,L"Video Path is incorrect..", L"Error", MB_OK );
					return;
				}
				
				//LoadOpticalFlow(); //TEST
				viewer->g_SharedUI.SetControlEnabled( IDC_START_VIDEO, true );
				viewer->g_SharedUI.SetControlEnabled( IDC_STOP_VIDEO, true );
				viewer->g_SharedUI.SetControlEnabled( IDC_VIDEOPLAY_SCALE, true );
				
				renderer->OnInitSpriteList = true;
				renderer->ResetStrokeCount();
				//SelectedRenderingSession = VIDEO;
				//OnVideoRendering = true;
			}
			else
			{
				video->DeleteVideoFrames();
				viewer->g_SharedUI.SetControlEnabled( IDC_START_VIDEO, false );
				viewer->g_SharedUI.SetControlEnabled( IDC_STOP_VIDEO, false );
				viewer->g_SharedUI.SetControlEnabled( IDC_VIDEOPLAY_SCALE, false );
				renderer->OnOpticalFlowLoaded = false;
			}
			break;
		

		case IDC_LOAD_VIDEOFILE:
		{
			CDXUTComboBox* pComboBox = NULL;
            pComboBox = ( CDXUTComboBox* )pControl;
            stylization->SelectedVideo = ( UINT )PtrToInt( pComboBox->GetSelectedIndex());			
            break;
		}

        case IDC_TOGGLEGRADIENTANGLE:
        {
            renderer->OnRealGradientAngle = viewer->g_SpriteUI.GetCheckBox( IDC_TOGGLEGRADIENTANGLE )->GetChecked();
			shader->ToggleAngleShaderVariable->SetBool( renderer->OnRealGradientAngle );
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
            break;
        }

		case IDC_SWITCHGRADIENTTUNE:
			{
				renderer->OnFocusPoint = !renderer->OnFocusPoint;
				shader->FocusPointMethodShaderVariable->SetBool( renderer->OnFocusPoint );
			}

		case IDC_TOGGLECLIPPING:
		{
			renderer->OnClipping = viewer->g_SpriteUI.GetCheckBox( IDC_TOGGLECLIPPING )->GetChecked();
			shader->ToggleClippingShaderVariable->SetBool( renderer->OnClipping );
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
			break;
		}
		case IDC_SESSION_SELECTION:
		{
			CDXUTComboBox* pComboBox = NULL;
            pComboBox = ( CDXUTComboBox* )pControl;
			int RenderingSession = ( UINT )PtrToInt( pComboBox->GetSelectedIndex());
            //SelectedRenderingSession = ( UINT )PtrToInt( pComboBox->GetSelectedIndex());
			if (RenderingSession != VIDEO && RenderingSession != MODEL_ON_VIDEO)
				renderer->OnVideoPlaying = false;
			switch (RenderingSession)
			{
				case IMAGE:
				{
					renderer->OnInitSpriteList = true;
					renderer->ResetStrokeCount();
					textureRT->LoadTexture1(stylization->ImageName[stylization->SelectedImage], textureRT->pRenderTargetShaderViews[RENDER_TARGET_COLOR2], textureRT->pShaderResourceVariables[RENDER_TARGET_COLOR2]);					//textureRT->LoadTexture(RENDER_TARGET_COLOR2, RENDER_TARGET_COLOR2);
					stylization->SelectedRenderingSession = RenderingSession;
					//stylization->SetImageContrast();
					camera->RecordEyeLookat();
					style->LoadStyle("Style\\imageStyle.txt");
					break;
				}
				case VIDEO:
					if (renderer->OnVideoLoaded)
					{
						renderer->OnInitSpriteList = true;	
						renderer->ResetStrokeCount();
						stylization->SelectedRenderingSession = RenderingSession;
						//stylization->SetImageContrast();
						camera->RecordEyeLookat();
						style->LoadStyle("Style\\videoStyle.txt");
					}
					else
						MessageBox( NULL,L"The video frames have not been loaded", L"Error", MB_OK );
					break;
				case STATIC_MODEL_OF:
				{
					renderer->OnInitSpriteList = true;
					renderer->ResetStrokeCount();
					stylization->SelectedRenderingSession = RenderingSession;
					//stylization->SetModelContrast();
					string styleName = style->staticModelStyleName[stylization->SelectedStaticModel];
					style->LoadStyle(styleName);					
					break;
				}
				case STATIC_MODEL:
				{
					stylization->SelectedRenderingSession = RenderingSession;
					renderer->OnInitSpriteList = true;
					renderer->ResetStrokeCount();
					//stylization->SetModelContrast();
					string styleName = style->staticModelStyleName[stylization->SelectedStaticModel];
					style->LoadStyle(styleName);
					break;
				}
				case ANIMATED_MODEL:
				{
					renderer->OnInitSpriteList = true;
					renderer->ResetStrokeCount();
					renderer->OnFirstFrame = true;
					stylization->SelectedRenderingSession = RenderingSession;
					//SetupCamaraForSkinnedMesh();
					camera->RestorePreviousCamera();
					//stylization->SetModelContrast();
					string styleName = style->animatedModelStyleName[stylization->SelectedAnimatedModel];
					style->LoadStyle(styleName);
					break;
				}
				case MODEL_ON_IMAGE:
				{
					renderer->OnInitSpriteList = true;
					stylization->SelectedRenderingSession = RenderingSession;
					//SetupCamaraForSkinnedMesh();
					//stylization->SetImageContrast();
					textureRT->LoadTexture(RENDER_TARGET_COLOR3, RENDER_TARGET_COLOR3);
					string styleName = style->animatedModelStyleName[stylization->SelectedAnimatedModel];
					style->LoadStyle(styleName);
					break;
				}
				case MODEL_ON_VIDEO:
					if (renderer->OnVideoLoaded)
					{
						renderer->OnFirstFrame = true;
						renderer->OnInitSpriteList = true;
						renderer->ResetStrokeCount();
						stylization->SelectedRenderingSession = RenderingSession;
						string styleName = style->animatedModelStyleName[stylization->SelectedAnimatedModel];
						style->LoadStyle(styleName);
						//stylization->SetImageContrast();
					}
					else
					{
						//textureRT->LoadTexture(RENDER_TARGET_COLOR2, RENDER_TARGET_COLOR2);
						MessageBox( NULL,L"The video frames have not been loaded", L"Error", MB_OK );
					}
					break;
				case OBJ_MODEL:
					renderer->OnInitSpriteList = true;
					renderer->ResetStrokeCount();
					stylization->SelectedRenderingSession = RenderingSession;
					string styleName = style->objModelStyleName[stylization->SelectedObjModel];
					style->LoadStyle(styleName);
					//camera->SetupCamaraForObjModel();
					break;
			}
			pComboBox->SetSelectedByIndex( stylization->SelectedRenderingSession );
			break;
		}

		case IDC_LOAD_ANIMATEDMODEL:
		{
			if (stylization->SelectedRenderingSession == ANIMATED_MODEL || 
				stylization->SelectedRenderingSession == MODEL_ON_IMAGE || 
				stylization->SelectedRenderingSession == MODEL_ON_VIDEO)
			{
				CDXUTComboBox* pComboBox = NULL;
				pComboBox = ( CDXUTComboBox* )pControl;
				int s = ( UINT )PtrToInt( pComboBox->GetSelectedIndex());
				stylization->SelectedAnimatedModel = s;
				pComboBox->SetSelectedByIndex( s );
				
				renderer->OnInitSpriteList = true;
				renderer->ResetStrokeCount();
				animatedMesh->DeleteMesh();
				animatedMesh->LoadAnimatedMesh();

				animatedMesh->CreateBoneTextureAndBuffer();
			}
			else
				MessageBox( NULL,L"You should select the correct session first (Animated Model, Model On Image, Model On Video)", L"Error", MB_OK );
			
			break;			
		}

		case IDC_LOAD_OBJMODEL:
		{
			if (stylization->SelectedRenderingSession == OBJ_MODEL)
			{
				CDXUTComboBox* pComboBox = NULL;
				pComboBox = ( CDXUTComboBox* )pControl;
				int s = ( UINT )PtrToInt( pComboBox->GetSelectedIndex());
				stylization->SelectedObjModel = s;
				pComboBox->SetSelectedByIndex( s );

				renderer->OnInitSpriteList = true;	
				renderer->ResetStrokeCount();
				objMesh->DeleteMesh();
				objMesh->LoadObjScene();
			}
			else
				MessageBox( NULL,L"You should select the correct session first (Obj Model)", L"Error", MB_OK );			
			break;
		}

		case IDC_LOAD_STATICMODEL:
		{
			if (stylization->SelectedRenderingSession == STATIC_MODEL || 
				stylization->SelectedRenderingSession == STATIC_MODEL_OF)
			{
				CDXUTComboBox* pComboBox = NULL;
				pComboBox = ( CDXUTComboBox* )pControl;
				int s = ( UINT )PtrToInt( pComboBox->GetSelectedIndex());
				stylization->SelectedStaticModel = s;
				pComboBox->SetSelectedByIndex( s );

				renderer->OnInitSpriteList = true;	
				renderer->ResetStrokeCount();

				staticMesh->DeleteMesh();
				staticMesh->LoadStaticMesh();
			}
			else
				MessageBox( NULL,L"You should select the correct session first (Static Model, Static Model Optical Flow)", L"Error", MB_OK );			
			break;			
		}

        case IDC_LOAD_IMAGE:
        {
            CDXUTComboBox* pComboBox = NULL;
            pComboBox = ( CDXUTComboBox* )pControl;
            int i = ( UINT )PtrToInt( pComboBox->GetSelectedIndex());
			stylization->SelectedImage = i;
			pComboBox->SetSelectedByIndex( i );
	
			//new added
			//D3DX10_IMAGE_INFO info;
			//D3DX10GetImageInfoFromFile(ImageName[SelectedImage], NULL, &info, NULL);
		
			textureRT->LoadTexture1(stylization->ImageName[stylization->SelectedImage], textureRT->pRenderTargetShaderViews[RENDER_TARGET_COLOR2], textureRT->pShaderResourceVariables[RENDER_TARGET_COLOR2]);
			//textureRT->LoadTexture(RENDER_TARGET_COLOR2, RENDER_TARGET_COLOR2);
			if (stylization->SelectedRenderingSession == MODEL_ON_IMAGE)
				textureRT->LoadTexture(RENDER_TARGET_COLOR3, RENDER_TARGET_COLOR3);
			//OnVideoRendering = false;
			//SelectedRenderingSession = IMAGE;
            break;
        }

		case IDC_STYLE_SELECTION:
		{
			CDXUTComboBox* pComboBox = NULL;
            pComboBox = ( CDXUTComboBox* )pControl;
			int s = ( UINT )PtrToInt( pComboBox->GetSelectedIndex());
			stylization->SelectedRenderingStyle = s ;
			shader->RenderingStyleShaderVariable->SetInt(s);
			textureRT->SetStrokeTexture(s);
			textureRT->GetStrokeTextureInfo(s);
			break;
		}

		case IDC_ALPHA_SELECTION:
		{
			CDXUTComboBox* pComboBox = NULL;
            pComboBox = ( CDXUTComboBox* )pControl;
			int s = ( UINT )PtrToInt( pComboBox->GetSelectedIndex());
			stylization->SelectedAlphaMask = s;
			textureRT->SetAlphaTexture(s);
			break;
		}

		case IDC_GRADIENT_SCALE:
        {
			float SliderValue = viewer->g_SpriteUI.GetSlider( IDC_GRADIENT_SCALE )->GetValue() * 0.01f;
			if (stroke->gradientThreshold != SliderValue)
			{
				stroke->gradientThreshold = SliderValue;
				StringCchPrintf( sz, 100, L"Gradient Threshold: %0.2f", float(stroke->gradientThreshold) ); 
				viewer->g_SpriteUI.GetStatic( IDC_GRADIENT_STATIC )->SetText( sz );
				shader->GradientThresholdShaderVariable->SetFloat( stroke->gradientThreshold );
				renderer->OnInitSpriteList = true;
				renderer->ResetStrokeCount();
			}
            break;
        }

		case IDC_NUMITERATION_SCALE:
        {
			int i = viewer->g_SharedUI.GetSlider( IDC_NUMITERATION_SCALE )->GetValue();
			video->SetMaxIteration(i);
			
			StringCchPrintf( sz, 100, L"Num Iteration: %d", i ); 
            viewer->g_SharedUI.GetStatic( IDC_NUMITERATION_STATIC )->SetText( sz );
		    break;
        }

		case IDC_LENGTH_SCALE:
        {
            stroke->strokeLength = viewer->g_SpriteUI.GetSlider( IDC_LENGTH_SCALE )->GetValue() * 0.01f;
            StringCchPrintf( sz, 100, L"Stroke Length: %0.2f", stroke->strokeLength ); 
            viewer->g_SpriteUI.GetStatic( IDC_LENGTH_STATIC )->SetText( sz );
			shader->StrokeLengthShaderVariable->SetFloat( stroke->strokeLength );
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
            break;
        }

		case IDC_WIDTH_SCALE:
		{
            stroke->strokeWidth = viewer->g_SpriteUI.GetSlider( IDC_WIDTH_SCALE )->GetValue() * 0.01f;
            StringCchPrintf( sz, 100, L"Stroke Width: %0.2f", stroke->strokeWidth ); 
            viewer->g_SpriteUI.GetStatic( IDC_WIDTH_STATIC )->SetText( sz );
			shader->StrokeWidthShaderVariable->SetFloat( stroke->strokeWidth );         
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
            break;
        }
		
		case IDC_FOCUSF_SCALE:
		{
				stylization->FocusfThreshold = viewer->g_SharedUI.GetSlider( IDC_FOCUSF_SCALE )->GetValue() * 0.01f;
				StringCchPrintf( sz, 100, L"Focus (f): %0.2f", stylization->FocusfThreshold ); 
				viewer->g_SharedUI.GetStatic( IDC_FOCUSF_STATIC )->SetText( sz );
				shader->FocusfVariable->SetFloat( stylization->FocusfThreshold );         
				break;
		}

		case IDC_FOCUSR1_SCALE:
		{
			float f1 = viewer->g_SharedUI.GetSlider( IDC_FOCUSR1_SCALE )->GetValue() * 0.01f;
			stylization->Focusr1Threshold = f1;
			StringCchPrintf( sz, 100, L"Focus (r1): %0.2f", f1 ); 
			viewer->g_SharedUI.GetStatic( IDC_FOCUSR1_STATIC )->SetText( sz );
			shader->Focusr1Variable->SetFloat( f1 );         
			break;
		}

		case IDC_FOCUSR2_SCALE:
		{
			float f2 = viewer->g_SharedUI.GetSlider( IDC_FOCUSR2_SCALE )->GetValue() * 0.01f;
			stylization->Focusr2Threshold = f2;
			StringCchPrintf( sz, 100, L"Focus (r2): %0.2f", f2 ); 
			viewer->g_SharedUI.GetStatic( IDC_FOCUSR2_STATIC )->SetText( sz );
			shader->Focusr2Variable->SetFloat( f2 );         
			break;
		}

		case IDC_GRADIENTANGLE_SCALE:
        {
            stroke->angleThreshold = viewer->g_SharedUI.GetSlider( IDC_GRADIENTANGLE_SCALE )->GetValue() * 0.01f;
            StringCchPrintf( sz, 100, L"Gradient Angle: %0.2f", stroke->angleThreshold ); 
            viewer->g_SharedUI.GetStatic( IDC_GRADIENTANGLE_STATIC )->SetText( sz );
			shader->GradientAngleShaderVariable->SetFloat( stroke->angleThreshold );
            break;
		}		
	
		case IDC_RENDERING_SELECTION:
        {
            CDXUTComboBox* pComboBox = NULL;
            pComboBox = ( CDXUTComboBox* )pControl;
			int s = ( UINT )PtrToInt( pComboBox->GetSelectedIndex());
            stylization->SelectedRenderingMode = s;
			pComboBox->SetSelectedByIndex( s );	//Necessary?		
            break;
        }

		case IDC_VIDEOPLAY_SCALE:
		{
			video->UpdatePreFrame();
			//shader->g_pOpticalFlowVariable->SetResource( OpticalFlowTextureSRV[FrameIndex] );
			int i = viewer->g_SharedUI.GetSlider( IDC_VIDEOPLAY_SCALE )->GetValue() - 1;
			video->SetFrameIndex(i);
			StringCchPrintf( sz, 100, L"%0.2d", i ); 
            viewer->g_SharedUI.GetStatic( IDC_VIDEOPLAY_STATIC )->SetText( sz );
			textureRT->SetVideoResource(RENDER_TARGET_COLOR1, 0);
			if (video->GetFrameIndex() == 0)
			{
				renderer->OnInitSpriteList = true;
				renderer->ResetStrokeCount();
			}
			//OnVideoRendering = true;
			//SelectedRenderingSession = VIDEO;
			break;
		}

		case IDC_LAMBDA_SCALE:
		{
			float l = viewer->g_SharedUI.GetSlider( IDC_LAMBDA_SCALE )->GetValue() * 0.01f;
			video->SetLambda(l);
			StringCchPrintf( sz, 100, L"Flow Lambda: %0.2f", l ); 
            viewer->g_SharedUI.GetStatic( IDC_LAMBDA_STATIC )->SetText( sz );
			shader->LambdaShaderVariable->SetFloat(l);
			break;
		}

		case IDC_START_VIDEO:
			renderer->OnVideoPlaying = true;
			//SelectedRenderingSession = VIDEO;
			//OnVideoRendering = true;
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
			break;

		case IDC_STOP_VIDEO:
			renderer->OnVideoPlaying = false;
			video->UpdatePreFrame();
			break;

		case IDC_SCREEN_SHOT:
			renderer->OnScreenCapture = viewer->g_SharedUI.GetCheckBox( IDC_SCREEN_SHOT )->GetChecked();
			break;

		case IDC_ENABLEFOCUS:
		{
			renderer->OnEnableFocus = viewer->g_SharedUI.GetCheckBox( IDC_ENABLEFOCUS )->GetChecked();
			float f = 1.0f;
			stylization->FocusfThreshold = f;
			shader->FocusfVariable ->SetFloat( f );			
			viewer->g_SharedUI.GetSlider( IDC_FOCUSF_SCALE )->SetValue(int(f * 100));
			StringCchPrintf( sz, 100, L"Focus (f): %0.2f", f ); 
			viewer->g_SharedUI.GetStatic( IDC_FOCUSF_STATIC )->SetText( sz );
			break;
		}

		case IDC_PROBABILITYLARGE_SCALE:
			stroke->ProbabilityLargeThreshold = viewer->g_SpriteUI.GetSlider( IDC_PROBABILITYLARGE_SCALE )->GetValue() * 0.001f;
			StringCchPrintf( sz, 100, L"Probability (Large): %0.3f", stroke->ProbabilityLargeThreshold ); 
            viewer->g_SpriteUI.GetStatic( IDC_PROBABILITYLARGE_STATIC )->SetText( sz );
			shader->ProbabilityLargeShaderVariable->SetFloat(stroke->ProbabilityLargeThreshold);
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
			break;

		case IDC_PROBABILITYSMALL_SCALE:
			stroke->ProbabilitySmallThreshold = viewer->g_SpriteUI.GetSlider( IDC_PROBABILITYSMALL_SCALE )->GetValue() * 0.01f;
			StringCchPrintf( sz, 100, L"Probability (Small): %0.2f", stroke->ProbabilitySmallThreshold ); 
            viewer->g_SpriteUI.GetStatic( IDC_PROBABILITYSMALL_STATIC )->SetText( sz );
			shader->ProbabilitySmallShaderVariable->SetFloat(stroke->ProbabilitySmallThreshold);
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
			break;

		case IDC_PROBABILITYMEDIUM_SCALE:
			stroke->ProbabilityMediumThreshold = viewer->g_SpriteUI.GetSlider( IDC_PROBABILITYMEDIUM_SCALE )->GetValue() * 0.01f;
			StringCchPrintf( sz, 100, L"Probability (Medium): %0.2f", stroke->ProbabilityMediumThreshold ); 
            viewer->g_SpriteUI.GetStatic( IDC_PROBABILITYMEDIUM_STATIC )->SetText( sz );
			shader->ProbabilityMediumShaderVariable->SetFloat(stroke->ProbabilityMediumThreshold);
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
			break;

		case IDC_GRADIENTLARGE_SCALE:
			stroke->GradientLargeThreshold = viewer->g_SpriteUI.GetSlider( IDC_GRADIENTLARGE_SCALE )->GetValue() * 0.001f;
			StringCchPrintf( sz, 100, L"Gradient Threshold 1: %0.3f", stroke->GradientLargeThreshold ); 
            viewer->g_SpriteUI.GetStatic( IDC_GRADIENTLARGE_STATIC )->SetText( sz );
			shader->GradientLargeShaderVariable->SetFloat(stroke->GradientLargeThreshold);
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
			break;

		case IDC_GRADIENTMEDIUM_SCALE:
			stroke->GradientMediumThreshold = viewer->g_SpriteUI.GetSlider( IDC_GRADIENTMEDIUM_SCALE )->GetValue() * 0.01f;
			StringCchPrintf( sz, 100, L"Gradient Threshold 2: %0.2f", stroke->GradientMediumThreshold ); 
            viewer->g_SpriteUI.GetStatic( IDC_GRADIENTMEDIUM_STATIC )->SetText( sz );
			shader->GradientMediumShaderVariable->SetFloat(stroke->GradientMediumThreshold);
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
			break;

		case IDC_ALPHALARGE_SCALE:
			stroke->AlphaLargeThreshold = viewer->g_SpriteUI.GetSlider( IDC_ALPHALARGE_SCALE )->GetValue() * 0.01f;
			StringCchPrintf( sz, 100, L"Alpha (Large): %0.2f", stroke->AlphaLargeThreshold ); 
            viewer->g_SpriteUI.GetStatic( IDC_ALPHALARGE_STATIC )->SetText( sz );
			shader->AlphaLargeShaderVariable->SetFloat(stroke->AlphaLargeThreshold);
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
			break;

		case IDC_ALPHASMALL_SCALE:
			stroke->AlphaSmallThreshold = viewer->g_SpriteUI.GetSlider( IDC_ALPHASMALL_SCALE )->GetValue() * 0.01f;
			StringCchPrintf( sz, 100, L"Alpha (Small): %0.2f", stroke->AlphaSmallThreshold ); 
            viewer->g_SpriteUI.GetStatic( IDC_ALPHASMALL_STATIC )->SetText( sz );
			shader->AlphaSmallShaderVariable->SetFloat(stroke->AlphaSmallThreshold);
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
			break;

		case IDC_ALPHAMEDIUM_SCALE:
			stroke->AlphaMediumThreshold = viewer->g_SpriteUI.GetSlider( IDC_ALPHAMEDIUM_SCALE )->GetValue() * 0.01f;
			StringCchPrintf( sz, 100, L"Alpha (Medium): %0.2f", stroke->AlphaMediumThreshold ); 
            viewer->g_SpriteUI.GetStatic( IDC_ALPHAMEDIUM_STATIC )->SetText( sz );
			shader->AlphaMediumShaderVariable->SetFloat(stroke->AlphaMediumThreshold);
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
			break;

		case IDC_SIZESMALL_SCALE:
			stroke->SizeSmallThreshold = viewer->g_SpriteUI.GetSlider( IDC_SIZESMALL_SCALE )->GetValue() * 0.01f;
			StringCchPrintf( sz, 100, L"Relative Size (Small): %0.2f", stroke->SizeSmallThreshold ); 
            viewer->g_SpriteUI.GetStatic( IDC_SIZESMALL_STATIC )->SetText( sz );
			shader->SizeSmallShaderVariable->SetFloat(stroke->SizeSmallThreshold);
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
			break;

		case IDC_SIZEMEDIUM_SCALE:
			stroke->SizeMediumThreshold = viewer->g_SpriteUI.GetSlider( IDC_SIZEMEDIUM_SCALE )->GetValue() * 0.01f;
			StringCchPrintf( sz, 100, L"Relative Size (Medium): %0.2f", stroke->SizeMediumThreshold ); 
            viewer->g_SpriteUI.GetStatic( IDC_SIZEMEDIUM_STATIC )->SetText( sz );
			shader->SizeMediumShaderVariable->SetFloat(stroke->SizeMediumThreshold);
			renderer->OnInitSpriteList = true;
			renderer->ResetStrokeCount();
			break;

		case IDC_CONTRAST_SCALE:
		{
			float c = viewer->g_SharedUI.GetSlider( IDC_CONTRAST_SCALE )->GetValue() * 0.01f;
			stylization->ContrastThreshold = c;
			StringCchPrintf( sz, 100, L"Contrast: %0.2f", c ); 
            viewer->g_SharedUI.GetStatic( IDC_CONTRAST_STATIC )->SetText( sz );
			shader->ContrastShaderVariable->SetFloat(c);
			break;
		}

		case IDC_FADEOUTSPEED_SCALE:
		{
			float f = viewer->g_SharedUI.GetSlider( IDC_FADEOUTSPEED_SCALE )->GetValue() * 0.01f;
			stylization->FadeOutSpeedThreshold = f;
			StringCchPrintf( sz, 100, L"Fade Out Speed: %0.2f", f ); 
            viewer->g_SharedUI.GetStatic( IDC_FADEOUTSPEED_STATIC )->SetText( sz );
			shader->FadeOutSpeedShaderVariable->SetFloat(f);
			break;
		}

		case IDC_FADEINSPEED_SCALE:
		{
			float f = viewer->g_SharedUI.GetSlider( IDC_FADEINSPEED_SCALE )->GetValue() * 0.01f; 
			stylization->FadeInSpeedThreshold = f;
			StringCchPrintf( sz, 100, L"Fade In Speed: %0.2f", f ); 
            viewer->g_SharedUI.GetStatic( IDC_FADEINSPEED_STATIC )->SetText( sz );
			shader->FadeInSpeedShaderVariable->SetFloat(f);
			break;
		}

		case IDC_INITIAL_FRAME_SCALE:
		{
			int i = viewer->g_SharedUI.GetSlider( IDC_INITIAL_FRAME_SCALE )->GetValue();
			video->SetInitialFrameIndex(i);
			StringCchPrintf( sz, 100, L"Initial Frame: %d", i ); 
            viewer->g_SharedUI.GetStatic( IDC_INITIAL_FRAME_STATIC )->SetText( sz );			
			break;
		}

		case IDC_NUM_FRAME_SCALE:
		{
			int n = viewer->g_SharedUI.GetSlider( IDC_NUM_FRAME_SCALE )->GetValue();
			video->SetNumFrame(n);
			StringCchPrintf( sz, 100, L"Num Frame: %d", n ); 
            viewer->g_SharedUI.GetStatic( IDC_NUM_FRAME_STATIC )->SetText( sz );			
			break;
		}

		case IDC_MIPMAP_LEVEL_SCALE:
		{			
			int m = viewer->g_SharedUI.GetSlider( IDC_MIPMAP_LEVEL_SCALE )->GetValue();
			stylization->MipMapLevel = m;
			StringCchPrintf( sz, 100, L"Mipmap Level: %d", m ); 
            viewer->g_SharedUI.GetStatic( IDC_MIPMAP_LEVEL_STATIC )->SetText( sz );

			D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
			SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;
			SRVDesc.Texture2D.MostDetailedMip = 0;
			SRVDesc.Texture2D.MipLevels = stylization->MipMapLevel;
			SRVDesc.Texture2D.MostDetailedMip = 0;
			SRVDesc.Format = textureRT->GetRenderTargetFormats(RENDER_TARGET_MIPRENDERING);
			textureRT->CreateRTTextureAsShaderResource(RENDER_TARGET_MIPRENDERING,"MipRendering",shader->GetImageEffect(),&SRVDesc);
			shader->MipMapLevelVariable->SetInt(stylization->MipMapLevel - 1);
			break;
		}
    }
}

//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    if( bKeyDown )
    {
        switch( nChar )
        {
			case VK_F1:
				renderer->OnScreenCapture = !renderer->OnScreenCapture;			
				break;	
			case VK_F2:
				renderer->OnImageCapture = !renderer->OnImageCapture;
				break;	
			case VK_F3:
				renderer->SwitchingLookAtPoint = !renderer->SwitchingLookAtPoint;
				break;			
			case VK_SPACE:
                viewer->ToggleMenu();
                break;
			//Load the files
			case VK_F12:
			{			
				style->LoadStyle();
				break;
			}

			case VK_F11:
			{
				style->SaveStyle();
				break;
			}							
	
			//not useful
			case '1':
			{
				renderer->OnFramesRendering = !renderer->OnFramesRendering;
				break;
			}

			case '2' :
			{
				renderer->OnOpticalFlowTest = !renderer->OnOpticalFlowTest;
				break;
			}
			
			case 'f': case'F':
			{
				video->NextFrame();
				viewer->g_SharedUI.GetSlider( IDC_VIDEOPLAY_SCALE )->SetValue(video->GetFrameIndex());
				WCHAR sz[100];
				StringCchPrintf( sz, 100, L"%0.2d", video->GetFrameIndex() ); 
				viewer->g_SharedUI.GetStatic( IDC_VIDEOPLAY_STATIC )->SetText( sz );

				if (video->GetFrameIndex() == 0)
				{
					renderer->OnInitSpriteList = true;	
					renderer->ResetStrokeCount();
				}
				break;
			}

			case 'g': case 'G':
			{
				renderer->GaussianFilter = (renderer->GaussianFilter+1) % 2;
				break;
			}
			case 'h': case 'H':
			{
				renderer->OnInitSpriteList = true;
				renderer->ResetStrokeCount();
				stylization->SelectedRenderingSession = IMAGE;
				textureRT->LoadTexture(RENDER_TARGET_COLOR2, RENDER_TARGET_COLOR2);
				break;
			}		

			case 'i': case 'I':
			{
				renderer->OnInitSpriteList = true;
				renderer->ResetStrokeCount();
				stylization->SelectedRenderingSession = MODEL_ON_IMAGE;
				camera->SetupCamaraForSkinnedMesh();
				textureRT->LoadTexture(RENDER_TARGET_COLOR3, RENDER_TARGET_COLOR3);
				
				break;
			}

			case 'c': case 'C':
			{
				if (renderer->OnVideoLoaded)
				{
					renderer->OnFirstFrame = true;
					renderer->OnInitSpriteList = true;
					renderer->ResetStrokeCount();
					stylization->SelectedRenderingSession  = MODEL_ON_VIDEO;
					camera->SetupCamaraForSkinnedMesh(); 
				}
				break;
			}
			//10/13
			case 'l': case 'L':
			{
				renderer->OnInitSpriteList = true;
				renderer->ResetStrokeCount();
				renderer->OnFirstFrame = true;
				stylization->SelectedRenderingSession  = ANIMATED_MODEL;
				camera->SetupCamaraForSkinnedMesh();
				break;
			}
		
			case 'm': case 'M':	
			{
				stylization->SelectedRenderingSession = STATIC_MODEL;
				renderer->OnInitSpriteList = true;
				renderer->ResetStrokeCount();
				camera->SetupCamaraForSDKScene();
				break;	
			}
			case 'n': case'N':
			{
				renderer->OnInitSpriteList = true;
				renderer->ResetStrokeCount();
				stylization->SelectedRenderingSession = STATIC_MODEL_OF;
				camera->SetupCamaraForSDKScene();
				break;
			}
			case 'o': case'O':
				renderer->OnOpticalFlow = !renderer->OnOpticalFlow;
				break;				

			case 's': case 'S':  
				
				if (stylization->SelectedRenderingSession == ANIMATED_MODEL || 
					stylization->SelectedRenderingSession == MODEL_ON_IMAGE || 
					stylization->SelectedRenderingSession == MODEL_ON_VIDEO)
					renderer->OnSkinnedMeshRenderingPause = !renderer->OnSkinnedMeshRenderingPause;
				break;

			case 't':case 'T':	
				if (viewer->g_SharedUI.GetCheckBox( IDC_LOADVIDEO)->GetChecked())
				{
					renderer->OnVideoPlaying = true;
				}
				break;	

			case 'u': case'U':
				textureRT->SetScreenShotIndex(0);
				break;

			case 'v': case 'V':
			{
				if (renderer->OnVideoLoaded)
				{
					renderer->OnInitSpriteList = true;
					renderer->ResetStrokeCount();
					stylization->SelectedRenderingSession = VIDEO;
				}
				break;
			}	
			
        }
    }
}
