#include "DXUT.h"
#include "Stylization.h"
#include "Camera.h"
#include "definition.h"
//#include "Shaders.h"
//#include "Stroke.h"
//#include "Renderer.h"
//#include "TextureRT.h"
//extern Shader* shader;
//extern Stroke* stroke;
//extern Renderer* renderer;
//extern TextureRT* textureRT;

Stylization::Stylization(int w, int h)
{
	WindowWidth				= w;
	WindowHeight			= h;
	ContrastThreshold		  = 1.0f; //from 0.8 to 1.3
	FadeOutSpeedThreshold	  = 0.05f;
	FadeInSpeedThreshold	  = 0.02f;
	CurTime					  = 0.0f;
	MipMapLevel				 = 2;


	SelectedRenderingMode	 = 7;
	SelectedRenderingStyle   = 0;
	SelectedAlphaMask		 = 0;
	SelectedImage			 = 0;
	SelectedVideo			 = 0;
	SelectedAnimatedModel	 = 0;
	SelectedStaticModel      = 0;
	SelectedObjModel		 = 0;
	SelectedRenderingSession = ANIMATED_MODEL;
	FocusfThreshold			 = 1.0f;
	Focusr1Threshold		 = 0.1f;
	Focusr2Threshold		 = 0.2f;
	Init();
}

void Stylization::Init()
{
	Session.push_back(L"Image");
	Session.push_back(L"Video");
	Session.push_back(L"Static Model (optical flow)");
	Session.push_back(L"Static Model");
	Session.push_back(L"Animated Model");
	Session.push_back(L"Model on Image");
	Session.push_back(L"Model on Video");
	Session.push_back(L"Obj Model");

	renderingMode.push_back(L"Original");
	renderingMode.push_back(L"Blurred");
	renderingMode.push_back(L"Gradient");
	renderingMode.push_back(L"Stroke Position");
	renderingMode.push_back(L"Delete Position");
	renderingMode.push_back(L"Inter Rendering Result");	
	renderingMode.push_back(L"Append Position");
	renderingMode.push_back(L"Final Rendering Result");

	ImageName.push_back(L"Image/l4.jpg");
	ImageName.push_back(L"Image/nature8.jpg");
	ImageName.push_back(L"Image/nature9.jpg");
	ImageName.push_back(L"Image/nature10.jpg");	

	//ImageName.push_back(L"Image/l1.jpg");
	//ImageName.push_back(L"Image/l2.jpg");
	//ImageName.push_back(L"Image/l3.jpg");	
	//ImageName.push_back(L"Image/l5.jpg");
	//ImageName.push_back(L"Image/l6.jpg");
	//ImageName.push_back(L"Image/l7.jpg");
	//ImageName.push_back(L"Image/l8.jpg");
	//ImageName.push_back(L"Image/l9.jpg");
	//ImageName.push_back(L"Image/l10.jpg");
	//ImageName.push_back(L"Image/l11.jpg");
	//ImageName.push_back(L"Image/l12.jpg");
	//ImageName.push_back(L"Image/l13.jpg");
	//ImageName.push_back(L"Image/l14.jpg");
	//ImageName.push_back(L"Image/l15.jpg");
	//ImageName.push_back(L"Image/l16.jpg");

	////high resolution pictures
	//ImageName.push_back(L"Image/h1.jpg");
	//ImageName.push_back(L"Image/h2.jpg");
	//ImageName.push_back(L"Image/h3.jpg");
	//ImageName.push_back(L"Image/h4.jpg");
	//ImageName.push_back(L"Image/h5.jpg");
	//ImageName.push_back(L"Image/h6.jpg");
	//ImageName.push_back(L"Image/h7.jpg");
	//ImageName.push_back(L"Image/h8.jpg");
	//ImageName.push_back(L"Image/h9.jpg");
	//ImageName.push_back(L"Image/h10.jpg");
	//ImageName.push_back(L"Image/h11.jpg");
	//ImageName.push_back(L"Image/h12.jpg");
	//ImageName.push_back(L"Image/h13.jpg");
	//ImageName.push_back(L"Image/h14.jpg");
	//ImageName.push_back(L"Image/h15.jpg");
	//ImageName.push_back(L"Image/h16.jpg");

	////new pictures
	//ImageName.push_back(L"Image/n1.jpg");
	//ImageName.push_back(L"Image/n2.jpg");
	//ImageName.push_back(L"Image/n3.jpg");
	//ImageName.push_back(L"Image/n4.jpg");
	//ImageName.push_back(L"Image/n5.jpg");
	//ImageName.push_back(L"Image/n6.jpg");
	//ImageName.push_back(L"Image/n7.jpg");
	//ImageName.push_back(L"Image/n8.jpg");
	//ImageName.push_back(L"Image/n10.jpg");
	//ImageName.push_back(L"Image/n11.jpg");

	//ImageName.push_back(L"Image/1.jpg");
	//ImageName.push_back(L"Image/2.jpg");
	//ImageName.push_back(L"Image/3.jpg");
	//ImageName.push_back(L"Image/5.jpg");
	//ImageName.push_back(L"Image/10.jpg");
	//ImageName.push_back(L"Image/15.jpg");
	//ImageName.push_back(L"Image/17.jpg");
	//ImageName.push_back(L"Image/18.jpg");
	//ImageName.push_back(L"Image/19.jpg");
	//ImageName.push_back(L"Image/22.jpg");

	////public domain
	//ImageName.push_back(L"Image/p1.jpg");
	//ImageName.push_back(L"Image/p2.jpg");
	//ImageName.push_back(L"Image/p3.jpg");
	//ImageName.push_back(L"Image/p4.jpg");
	//ImageName.push_back(L"Image/p5.jpg");
	//ImageName.push_back(L"Image/p6.jpg");
	//ImageName.push_back(L"Image/p7.jpg");
	//ImageName.push_back(L"Image/p8.jpg");
	//ImageName.push_back(L"Image/p10.jpg");
	//ImageName.push_back(L"Image/p11.jpg");
	//ImageName.push_back(L"Image/p12.jpg");
	//ImageName.push_back(L"Image/p13.jpg");
	//ImageName.push_back(L"Image/p14.jpg");
	//ImageName.push_back(L"Image/p15.jpg");
	//ImageName.push_back(L"Image/p16.jpg");
	//ImageName.push_back(L"Image/p17.jpg");
	//ImageName.push_back(L"Image/p19.jpg");
	//ImageName.push_back(L"Image/p20.jpg");
	//ImageName.push_back(L"Image/Lena.png");
	//ImageName.push_back(L"Image/baboon.jpg");


	//ImageName.push_back(L"Image/Flower1.jpg");	
	//ImageName.push_back(L"Image/Flower3.jpg");
	//ImageName.push_back(L"Image/Flower4.jpg");
	//ImageName.push_back(L"Image/Flower5.jpg");
	//ImageName.push_back(L"Image/Flower6.jpg");	
	//ImageName.push_back(L"Image/lillies.jpg");
	//ImageName.push_back(L"Image/Water_lilies.jpg");
	//ImageName.push_back(L"Image/beauty1.jpg");
	//ImageName.push_back(L"Image/beauty2.jpg");
	//ImageName.push_back(L"Image/beauty3.jpg");
	//ImageName.push_back(L"Image/beauty4.jpg");
	//ImageName.push_back(L"Image/beauty5.jpg");
	//ImageName.push_back(L"Image/beauty6.jpg");
	//ImageName.push_back(L"Image/beauty7.jpg");
	//ImageName.push_back(L"Image/beauty8.jpg");
	//ImageName.push_back(L"Image/beauty9.jpg");
	//ImageName.push_back(L"Image/beauty10.jpg");
	//ImageName.push_back(L"Image/beauty11.jpg");
	//ImageName.push_back(L"Image/beauty12.jpg");
	//ImageName.push_back(L"Image/beauty13.jpg");
	//ImageName.push_back(L"Image/beauty14.jpg");
	//ImageName.push_back(L"Image/beauty15.jpg");
	//ImageName.push_back(L"Image/beauty16.jpg");
	//ImageName.push_back(L"Image/BabyGirl.jpg");
	//ImageName.push_back(L"Image/Girl.jpg");
	//ImageName.push_back(L"Image/Girl1.jpg");
	//ImageName.push_back(L"Image/People.jpg");
	//ImageName.push_back(L"Image/People1.jpg");
	//ImageName.push_back(L"Image/People2.jpg");
	//ImageName.push_back(L"Image/Lena.png");
	//ImageName.push_back(L"Image/flowerandchild.jpg");
	//ImageName.push_back(L"Image/zebras.jpg");
	//ImageName.push_back(L"Image/beagle-puppy.jpg");
	//ImageName.push_back(L"Image/puppy4.jpg");
	//ImageName.push_back(L"Image/dog.jpg");	
	//ImageName.push_back(L"Image/Bird1.jpg");
	//ImageName.push_back(L"Image/GuiLin.jpg");	
	//ImageName.push_back(L"Image/Guilin1.jpg");
	//ImageName.push_back(L"Image/shanshui.jpg");
	//ImageName.push_back(L"Image/SnowMountain.jpg");
	//ImageName.push_back(L"Image/mountain1.jpg");
	//ImageName.push_back(L"Image/Pie.jpg");
	//ImageName.push_back(L"Image/Hut.jpg");
	//ImageName.push_back(L"Image/nature1.jpg");
	//ImageName.push_back(L"Image/nature2.jpg");
	//ImageName.push_back(L"Image/nature3.jpg");
	//ImageName.push_back(L"Image/nature4.jpg");
	//ImageName.push_back(L"Image/nature5.jpg");
	//ImageName.push_back(L"Image/nature6.jpg");
	//ImageName.push_back(L"Image/nature7.jpg");
	//ImageName.push_back(L"Image/GuGong.jpg");
	//ImageName.push_back(L"Image/Building.jpg");
	//ImageName.push_back(L"Image/europe.jpg");
	//ImageName.push_back(L"Image/japan.jpg");
	//ImageName.push_back(L"Image/lasa.jpg");
	//ImageName.push_back(L"Image/room.jpg");
	//ImageName.push_back(L"Image/Rocket.bmp");
	//ImageName.push_back(L"Image/fishing.jpg");
	//ImageName.push_back(L"Image/fishing2.jpg");
	//ImageName.push_back(L"Image/fishing3.jpg");
	//ImageName.push_back(L"Image/fishing4.jpg");
	//ImageName.push_back(L"Image/fishing5.jpg");	

	StrokeTexture.push_back(L"No Texture");
	StrokeTexture.push_back(L"StrokeTex/BrushTex1.jpg");
	StrokeTexture.push_back(L"StrokeTex/BrushTex2.jpg");
	StrokeTexture.push_back(L"StrokeTex/BrushTex3.jpg");
	StrokeTexture.push_back(L"StrokeTex/BrushTex4.jpg");
	StrokeTexture.push_back(L"StrokeTex/tex1.jpg");
	StrokeTexture.push_back(L"StrokeTex/tex2.jpg");
	StrokeTexture.push_back(L"StrokeTex/tex3.jpg");
	StrokeTexture.push_back(L"StrokeTex/tex4.jpg");
	StrokeTexture.push_back(L"StrokeTex/tex5.jpg");
	StrokeTexture.push_back(L"StrokeTex/tex6.jpg");
	StrokeTexture.push_back(L"StrokeTex/tex7.jpg");
	StrokeTexture.push_back(L"StrokeTex/tex8.jpg");
	StrokeTexture.push_back(L"StrokeTex/tex10.jpg");
	StrokeTexture.push_back(L"StrokeTex/tex11.jpg");

	AlphaMaskTexture.push_back(L"AlpMask/alpha1.jpg");
	AlphaMaskTexture.push_back(L"AlpMask/alpha2.jpg");
	AlphaMaskTexture.push_back(L"AlpMask/alpha3.jpg");
	AlphaMaskTexture.push_back(L"AlpMask/alpha4.jpg");
	AlphaMaskTexture.push_back(L"AlpMask/alpha5.jpg");
	AlphaMaskTexture.push_back(L"AlpMask/alpha6.jpg");
	AlphaMaskTexture.push_back(L"AlpMask/alpha7.jpg");
	AlphaMaskTexture.push_back(L"AlpMask/alpha8.jpg");
	AlphaMaskTexture.push_back(L"AlpMask/alpha9.jpg");
	AlphaMaskTexture.push_back(L"AlpMask/alpha10.jpg");
	AlphaMaskTexture.push_back(L"AlpMask/alpha11.jpg");
	AlphaMaskTexture.push_back(L"AlpMask/alpha12.jpg");
	AlphaMaskTexture.push_back(L"AlpMask/alpha13.jpg");
	AlphaMaskTexture.push_back(L"AlpMask/alpha14.jpg");
	AlphaMaskTexture.push_back(L"AlpMask/alpha15.jpg");


	NormalMapTexture.push_back(L"paper/p-fabric-1.png");
	NormalMapTexture.push_back(L"paper/p-blobby-1.png");
	NormalMapTexture.push_back(L"paper/p-blobby-2.png");
	NormalMapTexture.push_back(L"paper/p-brushed-1.png");
	NormalMapTexture.push_back(L"paper/p-brushed-2.png");
	NormalMapTexture.push_back(L"paper/p-brushed-3.png");

	VideoName.push_back(L"Climb");
	VideoName.push_back(L"Pole");
	/*VideoName.push_back(L"Chat");
	VideoName.push_back(L"Fire");
	VideoName.push_back(L"Belgium");	
	VideoName.push_back(L"Tiger");
	VideoName.push_back(L"Bird3");
	VideoName.push_back(L"Bird4");
	VideoName.push_back(L"Bird5");
	VideoName.push_back(L"Bird6");
	VideoName.push_back(L"Sunflower");
	VideoName.push_back(L"Obama");
	VideoName.push_back(L"test3");
	VideoName.push_back(L"test1");
	VideoName.push_back(L"test2");
	VideoName.push_back(L"Butterfly1");
	VideoName.push_back(L"Cloud1");
	VideoName.push_back(L"Cloud2");
	VideoName.push_back(L"RideHorces1");
	VideoName.push_back(L"RideHorces2");
	VideoName.push_back(L"Ball");
	VideoName.push_back(L"Fish1");
	VideoName.push_back(L"Fish2");
	VideoName.push_back(L"Fish3");
	VideoName.push_back(L"Bird1");
	VideoName.push_back(L"Bird2");	
	VideoName.push_back(L"Butterfly2");
	VideoName.push_back(L"Children");
	VideoName.push_back(L"GreatWall");
	VideoName.push_back(L"Nature");
	VideoName.push_back(L"Octopus");
	VideoName.push_back(L"Blossom");	
	VideoName.push_back(L"Undersea1");
	VideoName.push_back(L"Dog&Cat");
	VideoName.push_back(L"Female");*/

	initVideoPath.push_back("Video\\Climb\\");
	initVideoPath.push_back("Video\\Pole\\");
	/*initVideoPath.push_back("Video\\Chat\\");
	initVideoPath.push_back("Video\\Fire\\");
	initVideoPath.push_back("Video\\Belgium\\");	
	initVideoPath.push_back("Video\\Tiger\\");
	initVideoPath.push_back("Video\\Bird3\\");
	initVideoPath.push_back("Video\\Bird4\\");
	initVideoPath.push_back("Video\\Bird5\\");
	initVideoPath.push_back("Video\\Bird6\\");
	initVideoPath.push_back("Video\\Sunflower\\");
	initVideoPath.push_back("Video\\Obama\\");
	initVideoPath.push_back("Video\\test3\\");
	initVideoPath.push_back("Video\\test1\\");
	initVideoPath.push_back("Video\\test2\\");
	initVideoPath.push_back("Video\\Butterfly1\\");
	initVideoPath.push_back("Video\\Cloud1\\");
	initVideoPath.push_back("Video\\Cloud2\\");
	initVideoPath.push_back("Video\\RideHorces1\\");
	initVideoPath.push_back("Video\\RideHorces2\\");
	initVideoPath.push_back("Video\\Ball\\");
	initVideoPath.push_back("Video\\Fish1\\");
	initVideoPath.push_back("Video\\Fish2\\");
	initVideoPath.push_back("Video\\Fish3\\");
	initVideoPath.push_back("Video\\Bird1\\");
	initVideoPath.push_back("Video\\Bird2\\");	
	initVideoPath.push_back("Video\\Butterfly2\\");
	initVideoPath.push_back("Video\\Children\\");
	initVideoPath.push_back("Video\\GreatWall\\");
	initVideoPath.push_back("Video\\Nature\\");
	initVideoPath.push_back("Video\\Octopus\\");
	initVideoPath.push_back("Video\\Blossom\\");	
	initVideoPath.push_back("Video\\Undersea1\\");
	initVideoPath.push_back("Video\\Dog&Cat\\");
	initVideoPath.push_back("Video\\Female\\");*/

	
	AnimatedModelName.push_back(L"Sodier");
	AnimatedModelName.push_back(L"Lizard");
	StaticModelName.push_back(L"Tank");
	StaticModelName.push_back(L"NightScene");
	StaticModelName.push_back(L"Room");
	ObjModelName.push_back(L"Cup");
	ObjModelName.push_back(L"Clock");
	ObjModelName.push_back(L"bench");
}

HRESULT Stylization::ResizeWindow(int BufferWidth, int BufferHeight)
{
	HRESULT hr = S_OK;
	WindowWidth = BufferWidth;
	WindowHeight = BufferHeight;

	//Reassign shader variables
	shader->WindowWidthVariable->SetInt(WindowWidth);
	shader->WindowHeightVariable->SetInt(WindowHeight);
	shader->DeltaXShaderVariable->SetFloat(1.0f / WindowWidth);
	shader->DeltaYShaderVariable->SetFloat(1.0f / WindowHeight);
	shader->NumParticleXVariable->SetInt(int(WindowWidth * DownSampling));
	shader->NumParticleYVariable->SetInt(int(WindowHeight * DownSampling));
	shader->StrokeLengthShaderVariable->SetFloat( stroke->strokeLength );
	shader->StrokeWidthShaderVariable->SetFloat( stroke->strokeWidth );

	renderer->OnInitSpriteList = true;
	renderer->ResetStrokeCount();
	SAFE_CHECK(textureRT->CreateRenderTargets(WindowWidth, WindowHeight));
	//SAFE_CHECK(CreateRenderTargetsInt(WindowWidth, WindowHeight));
	SAFE_CHECK(inputLayout->SetupQuadInputLayout());   
	SAFE_CHECK(inputLayout->SetupParticleVertexLayout());
	SAFE_CHECK(inputLayout->SetupSDKSceneInputLayout());
	//SAFE_CHECK(textureRT->LoadTexture(RENDER_TARGET_COLOR2, RENDER_TARGET_COLOR2));
	SAFE_CHECK(textureRT->LoadProbability());	
	camera->SetupCamaraForImage();
	//SAFE_CHECK(CreateStagingResource()); //useful for copy texture back from GPU

	return hr;
}

void Stylization::SetImageContrast()
{
	float c = 1.1f;
	stylization->ContrastThreshold = c;
	shader->ContrastShaderVariable->SetFloat(c);
	viewer->g_SharedUI.GetSlider( IDC_CONTRAST_SCALE )->SetValue(int(c * 100));
	WCHAR sz[100];
	StringCchPrintf( sz, 100, L"Contrast: %0.2f", c ); 	
	viewer->g_SharedUI.GetStatic( IDC_CONTRAST_STATIC )->SetText( sz );
}

void Stylization::SetModelContrast()
{
	float c = 1.0f;
	stylization->ContrastThreshold = c;
	shader->ContrastShaderVariable->SetFloat(c);
	viewer->g_SharedUI.GetSlider( IDC_CONTRAST_SCALE )->SetValue(int(c * 100));
	WCHAR sz[100];
	StringCchPrintf( sz, 100, L"Contrast: %0.2f", c ); 
	viewer->g_SharedUI.GetStatic( IDC_CONTRAST_STATIC )->SetText( sz );
}