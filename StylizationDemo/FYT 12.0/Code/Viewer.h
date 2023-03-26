#ifndef VIEWER_H
#define VIEWER_H
#include "dxut.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"

enum UI{
	IDC_TOGGLEFULLSCREEN,
	IDC_TOGGLEREF,
	IDC_CHANGEDEVICE,
	
	IDC_LOAD_IMAGE,
	IDC_LOAD_VIDEOFILE,
	IDC_LOAD_ANIMATEDMODEL,
	IDC_LOAD_STATICMODEL,
	IDC_LOAD_OBJMODEL,
	IDC_LOADSTROKETEXTURE,
	
	IDC_NUMITERATION_STATIC,
	IDC_NUMITERATION_SCALE,

	IDC_RENDERING_SELECTION,
	IDC_SESSION_SELECTION,
	IDC_STYLE_SELECTION,
	IDC_ALPHA_SELECTION,

	IDC_LENGTH_STATIC,
	IDC_LENGTH_SCALE,
	IDC_WIDTH_STATIC,
	IDC_WIDTH_SCALE,
	
	IDC_LOADVIDEO,
	IDC_VIDEOPLAY_STATIC,
	IDC_VIDEOPLAY_SCALE,
	IDC_START_VIDEO,
	IDC_STOP_VIDEO,
	IDC_SAMPLING_SCALE,
	IDC_SAMPLING_STATIC,
	IDC_LAMBDA_SCALE,
	IDC_LAMBDA_STATIC,
	IDC_OPTICALFLOW,
	IDC_NUM_FRAME_STATIC,
	IDC_NUM_FRAME_SCALE,
	IDC_INITIAL_FRAME_STATIC,
	IDC_INITIAL_FRAME_SCALE,

	IDC_SCREEN_SHOT,
	IDC_IMAGECAPTURE,
	IDC_CLIPCAPTURE,
	IDC_RESETINDEX,

	IDC_TOGGLEVIDEOMENU,
	IDC_TOGGLESPRITEMENU,
	IDC_TOGGLESHAREMENU,
	IDC_TOGGLECLIPPING,
	IDC_SWITCHGRADIENTTUNE,	

	IDC_CONTRAST_SCALE,
	IDC_CONTRAST_STATIC,

	IDC_FADEOUTSPEED_SCALE,
	IDC_FADEOUTSPEED_STATIC,
	IDC_FADEINSPEED_SCALE,
	IDC_FADEINSPEED_STATIC,	

	IDC_PROBABILITYLARGE_SCALE,
	IDC_PROBABILITYLARGE_STATIC,
	IDC_PROBABILITYMEDIUM_SCALE,
	IDC_PROBABILITYMEDIUM_STATIC,
	IDC_PROBABILITYSMALL_SCALE,
	IDC_PROBABILITYSMALL_STATIC,

	IDC_TOGGLEGRADIENTANGLE,
	IDC_GRADIENTANGLE_STATIC,
	IDC_GRADIENTANGLE_SCALE,
	IDC_GRADIENT_STATIC,
	IDC_GRADIENT_SCALE,
	IDC_GRADIENTLARGE_SCALE,
	IDC_GRADIENTLARGE_STATIC,
	IDC_GRADIENTMEDIUM_SCALE,
	IDC_GRADIENTMEDIUM_STATIC,
	IDC_GRADIENTSMALL_SCALE,
	IDC_GRADIENTSMALL_STATIC,

	IDC_SIZEMEDIUM_SCALE,
	IDC_SIZEMEDIUM_STATIC,
	IDC_SIZESMALL_SCALE,
	IDC_SIZESMALL_STATIC,
	
	IDC_ALPHALARGE_SCALE,
	IDC_ALPHALARGE_STATIC,
	IDC_ALPHAMEDIUM_SCALE,
	IDC_ALPHAMEDIUM_STATIC,
	IDC_ALPHASMALL_SCALE,
	IDC_ALPHASMALL_STATIC,
	
	IDC_MIPMAP_LEVEL_STATIC,
	IDC_MIPMAP_LEVEL_SCALE,	

	IDC_LOADSTYLE,
	IDC_SAVESTYLE,	

	IDC_ENABLEFOCUS,
	IDC_FOCUSF_STATIC,
	IDC_FOCUSF_SCALE,
	IDC_FOCUSR1_STATIC,
	IDC_FOCUSR1_SCALE,
	IDC_FOCUSR2_STATIC,
	IDC_FOCUSR2_SCALE,
		
	IDC_PAUSEANIMATION,
	IDC_CHANGELOOKAT
};


enum PROGRAM_STATE
{
    RENDERING,
    MAIN_MENU,
	VIDEO_MENU,
	SPRITE_MENU,
	SHARE_MENU
};

class Viewer{
public:
	Viewer(int, int);
	~Viewer();
	HRESULT CreateUI();
	void InitApp();
	void ToggleMenu();
	void RenderText();
	void ChangeMode(float fElapsedTime);
	void ResizeUI();

public:
	CDXUTDialogResourceManager          g_DialogResourceManager;// manager for shared resources of dialogs
	CD3DSettingsDlg                     g_D3DSettingsDlg;       // Device settings dialog
	CDXUTDialog                         g_HUD;                  // manages the 3D UI  USELESSSSS!!!!!!!!!
	CDXUTDialog                         g_SharedUI;             // dialog for sample specific controls
	CDXUTDialog							g_SpriteUI;
	CDXUTDialog							g_MainUI;

	ID3DX10Font*                        g_pFont;         // Font for drawing text
	ID3DX10Sprite*                      g_pSprite;       // Sprite for batching text drawing
	CDXUTTextHelper*                    g_pTxtHelper;

	int gameMode;
	int WindowHeight;
	int WindowWidth;
};

#endif