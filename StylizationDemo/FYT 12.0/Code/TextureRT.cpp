#include "dxut.h"
#include "TextureRT.h"
#include "Stylization.h"
#include "definition.h"


char *initScreenShotPathO		 =  "ScreenShot\\Original\\";
char *initScreenShotPathR		 =  "ScreenShot\\Rendering\\";


TextureRT::TextureRT(int w, int h)
{
	VideoResource			= NULL;
	g_pDepthStencilTexture	= NULL;
	g_pDepthStencilDSV		= NULL;
	WindowWidth				 = w;
	WindowHeight			 = h;
	OpticalFlowMip			 = 5;
	ScreenShotIndex			 = 0;
	CreateRenderTargets(WindowWidth, WindowHeight);
	//LoadTexture(RENDER_TARGET_COLOR2, RENDER_TARGET_COLOR2);
	LoadTexture1(stylization->StrokeTexture[1], g_pStrokeTex, shader->g_pStrokeTexVariable); 		
	GetStrokeTextureInfo(1); //get the stroke texture dimention and set them to shader variables
	LoadTexture1(stylization->AlphaMaskTexture[0], g_pAlphaMask, shader->g_pAlphaMaskVariable);
	LoadProbability();
}

TextureRT::~TextureRT()
{
	SAFE_RELEASE( g_pTextureRV );
	SAFE_RELEASE( g_pAlphaMask );
	SAFE_RELEASE( g_pStrokeTex );
	SAFE_RELEASE( g_pNormalMap );

	for(int i=0;i<NUM_RENDER_TARGETS;i++)
	{
		SAFE_RELEASE( pRenderTargets2D[i]);
		SAFE_RELEASE( pRenderTargetShaderViews[i]);
		SAFE_RELEASE( pRenderTargetViews[i]);
	}

	if (VideoResource)
	{
		for(int i=0;i < video->GetNumFrame();i++)
		{
			SAFE_RELEASE( VideoResource[i]);
		}
		delete VideoResource;
	}
}

void TextureRT::SetAlphaTexture(int index)
{
	LoadTexture1(stylization->AlphaMaskTexture[index], g_pAlphaMask, shader->g_pAlphaMaskVariable);
}

void TextureRT::SetStrokeTexture(int index)
{
	LoadTexture1(stylization->StrokeTexture[index], g_pStrokeTex, shader->g_pStrokeTexVariable);
}

void TextureRT::ClearRenderTarget(int RenderTargetIndex, float* clearcolor)
{
	DXUTGetD3D10Device()->ClearRenderTargetView( pRenderTargetViews[RenderTargetIndex], clearcolor );
}

void TextureRT::SetRenderTargetStencil(int RenderTargetIndex)
{
	DXUTGetD3D10Device()->OMSetRenderTargets( 1, &pRenderTargetViews[RenderTargetIndex], g_pDepthStencilDSV );
}

void TextureRT::SetRenderTarget(int RenderTargetIndex)
{
	DXUTGetD3D10Device()->OMSetRenderTargets( 1, &pRenderTargetViews[RenderTargetIndex], NULL );
}

void TextureRT::NullShaderResource(int ShaderResourceIndex)
{
	pShaderResourceVariables[ShaderResourceIndex]->SetResource( NULL );
}

void TextureRT::SetRTShaderView(int ShaderResourceIndex)
{
	 pShaderResourceVariables[ShaderResourceIndex]->SetResource( pRenderTargetShaderViews[ShaderResourceIndex] );
}

void TextureRT::SetVideoResource(int RenderTargetIndex, int FrameIndex)
{
	pShaderResourceVariables[RenderTargetIndex]->SetResource(VideoResource[FrameIndex]);
}

HRESULT TextureRT::CreateRenderTargetTextureAndView(int rtIndex, D3D10_TEXTURE2D_DESC TexDesc, const D3D10_SUBRESOURCE_DATA *pInitialData = NULL)
{

    // Release resources in case they exist
    SAFE_RELEASE( pRenderTargets2D[rtIndex] );
    SAFE_RELEASE( pRenderTargetViews[rtIndex] );

    // Create the texture
    DXUTGetD3D10Device()->CreateTexture2D(&TexDesc,pInitialData,&pRenderTargets2D[rtIndex]);	
	
    // Create the render target view
    D3D10_RENDER_TARGET_VIEW_DESC DescRT;
    DescRT.Format = TexDesc.Format;
    DescRT.ViewDimension =  D3D10_RTV_DIMENSION_TEXTURE2D;

	DescRT.Texture2D.MipSlice = 0;

    DXUTGetD3D10Device()->CreateRenderTargetView( pRenderTargets2D[rtIndex], &DescRT, &pRenderTargetViews[rtIndex]);

    return S_OK;
}


HRESULT TextureRT::GetStrokeTextureInfo(int index)
{
	HRESULT hr = S_OK;
		
	D3DX10_IMAGE_INFO info;
	D3DX10GetImageInfoFromFile(stylization->StrokeTexture[index], NULL, &info, NULL);
	//int a = info.Height;
	//int b = info.Width;
	shader->StrokeTexWidthVariable->SetInt(info.Width);
	shader->StrokeTexLengthVariable->SetInt(info.Height);

	return hr;
}

HRESULT TextureRT::LoadTexture1( LPCWSTR ImageName, ID3D10ShaderResourceView* resource, ID3D10EffectShaderResourceVariable* resourceVar)
{
	// Load the Texture
	HRESULT hr = S_OK;
	//SAFE_RELEASE(resource);
	if (resourceVar)
	{
		ID3D10ShaderResourceView* OriginalResource;
		resourceVar->GetResource( &OriginalResource );
		SAFE_RELEASE(OriginalResource);
	}		

	SAFE_CHECK(D3DX10CreateShaderResourceViewFromFile( DXUTGetD3D10Device(), ImageName, NULL, NULL, &resource, NULL ));	
	resourceVar->SetResource(resource );
	renderer->OnInitSpriteList = true;
	renderer->ResetStrokeCount();

	return hr;
}

HRESULT TextureRT::LoadTexture(int resource, int resourceVar)
{
	// Load the Texture
	HRESULT hr = S_OK;
	//SAFE_RELEASE(resource);
	if (pShaderResourceVariables[resourceVar])
	{
		ID3D10ShaderResourceView* OriginalResource;
		pShaderResourceVariables[resourceVar]->GetResource( &OriginalResource );
		SAFE_RELEASE(OriginalResource);
	}		

	D3DX10CreateShaderResourceViewFromFile( DXUTGetD3D10Device(), stylization->ImageName[stylization->SelectedImage], NULL, NULL, &pRenderTargetShaderViews[resource], NULL );	
	pShaderResourceVariables[resourceVar]->SetResource( pRenderTargetShaderViews[resource] );
	renderer->OnInitSpriteList = true;
	renderer->ResetStrokeCount();

	return hr;
}

void TextureRT::ClearDepthStencil()
{
	DXUTGetD3D10Device()->ClearDepthStencilView( g_pDepthStencilDSV, D3D10_CLEAR_DEPTH, 1.0, 0);
}

HRESULT TextureRT::CreateRTTextureAsShaderResource(RENDER_TARGET rtIndex, LPCSTR shaderImageName,ID3D10Effect* pEffect,D3D10_SHADER_RESOURCE_VIEW_DESC *SRVDesc )
{

    // Create the "shader resource view" (SRView) and "shader resource variable" (SRVar) for the given texture 
    SAFE_RELEASE(pRenderTargetShaderViews[rtIndex]);
    DXUTGetD3D10Device()->CreateShaderResourceView( pRenderTargets2D[rtIndex], SRVDesc, &pRenderTargetShaderViews[rtIndex]);

    pShaderResourceVariables[rtIndex] = pEffect->GetVariableByName( shaderImageName )->AsShaderResource();
    assert(pShaderResourceVariables[rtIndex]->IsValid());

    // Then we bind the texture SRView to the SRVar
    pShaderResourceVariables[rtIndex]->SetResource( pRenderTargetShaderViews[rtIndex] );
    
    return S_OK;
}

HRESULT TextureRT::CreateRenderTargets(UINT width, UINT height)
{
	
	HRESULT hr = S_OK;

	InitRenderTarget();

	D3D10_TEXTURE2D_DESC desc;
    desc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
	desc.ArraySize = NUM_RENDER_TARGETS;	
    desc.CPUAccessFlags = 0; 
    desc.MipLevels = 1;
    desc.MiscFlags = 0;
    desc.Usage = D3D10_USAGE_DEFAULT;
    desc.Width =  width;
    desc.Height = height;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;


	D3D10_TEXTURE2D_DESC descMip;
	descMip.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET; //
	descMip.ArraySize = NUM_RENDER_TARGETS;	//
	descMip.CPUAccessFlags = 0; //
	descMip.MipLevels = 0; //3
	descMip.MiscFlags = D3D10_RESOURCE_MISC_GENERATE_MIPS;
	descMip.Usage = D3D10_USAGE_DEFAULT;//
	descMip.Width =  width;//
	descMip.Height = height;//
	descMip.SampleDesc.Count = 1;//
	descMip.SampleDesc.Quality = 0;//

    for(int rtIndex=0; rtIndex<NUM_RENDER_TARGETS; rtIndex++)
    {	
		if (rtIndex != RENDER_TARGET_MIPRENDERING && rtIndex != RENDER_TARGET_INFO) //CHANGE
		{
			desc.Format = RenderTargetFormats[rtIndex];		
			CreateRenderTargetTextureAndView( rtIndex, desc );
		}
		else 
		{
			descMip.Format = RenderTargetFormats[rtIndex];	
			CreateRenderTargetTextureAndView( rtIndex, descMip );
		}
		
    }	

	D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
    ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
    SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = 1;
    SRVDesc.Texture2D.MostDetailedMip = 0;

	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_COLOR1];
    CreateRTTextureAsShaderResource(RENDER_TARGET_COLOR1,"ColorBuffer1",shader->GetImageEffect(),&SRVDesc);

	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_COLOR0];
    CreateRTTextureAsShaderResource(RENDER_TARGET_COLOR0,"ColorBuffer0",shader->GetImageEffect(),&SRVDesc);


	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_COLOR2];
    CreateRTTextureAsShaderResource(RENDER_TARGET_COLOR2,"ColorBuffer2",shader->GetImageEffect(),&SRVDesc);	

	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_COLOR3];
	CreateRTTextureAsShaderResource(RENDER_TARGET_COLOR3,"ColorBuffer3",shader->GetImageEffect(),&SRVDesc);

	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_FlOW];
    CreateRTTextureAsShaderResource(RENDER_TARGET_FlOW,"FlowBuffer0",shader->GetImageEffect(),&SRVDesc);

	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_FlOW1];
    CreateRTTextureAsShaderResource(RENDER_TARGET_FlOW1,"FlowBuffer1",shader->GetImageEffect(),&SRVDesc);

	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_PREFLOW0];
	CreateRTTextureAsShaderResource(RENDER_TARGET_PREFLOW0,"PreviousVector0",shader->GetImageEffect(),&SRVDesc);	
	
	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_TEMP];
    CreateRTTextureAsShaderResource(RENDER_TARGET_TEMP,"TempImage",shader->GetImageEffect(),&SRVDesc);

	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_TEMP1];
    CreateRTTextureAsShaderResource(RENDER_TARGET_TEMP1,"TempImage1",shader->GetImageEffect(),&SRVDesc);

	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_MODELMATTE];
    CreateRTTextureAsShaderResource(RENDER_TARGET_MODELMATTE,"ModelMatte",shader->GetImageEffect(),&SRVDesc);

	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_STROKE];
    CreateRTTextureAsShaderResource(RENDER_TARGET_STROKE,"StrokeBuffer",shader->GetImageEffect(),&SRVDesc);
	
	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_PREFLOW];
    CreateRTTextureAsShaderResource(RENDER_TARGET_PREFLOW,"PreviousVector",shader->GetImageEffect(),&SRVDesc);

    SRVDesc.Texture2D.MipLevels = OpticalFlowMip;
    SRVDesc.Texture2D.MostDetailedMip = 0;

	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_INFO];
    CreateRTTextureAsShaderResource(RENDER_TARGET_INFO,"RenderInfo",shader->GetImageEffect(),&SRVDesc);

	SRVDesc.Texture2D.MipLevels = stylization->MipMapLevel;
	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_MIPRENDERING];
    CreateRTTextureAsShaderResource(RENDER_TARGET_MIPRENDERING,"MipRendering",shader->GetImageEffect(),&SRVDesc);

	reset();
	return hr;
}

HRESULT TextureRT::LoadOpticalFlow()
{
	OpticalFlowTextureSRV = new ID3D10ShaderResourceView*[video->GetNumFrame()];

	ifstream fp_in1;  // declarations of streams fp_in and fp_out
	ifstream fp_in2;

	char *filename1 = new char[35];
	char *filename2 = new char[35];
	int filenum = video->GetInitialFrameIndex();

	//for (; filenum < NumFrame + IntialFrameIndex; filenum++){

	for (; filenum < 5; filenum++){
	
		int r = filenum;
		int num = 0;
		do{
			r = r / 10;
			num++;
		}while (r!= 0);

		char suffix [4];
		_itoa_s (filenum,suffix,10);

		char   *zero   =   "0";   
		strcpy(filename1,"result/");   	   
		
		for (int i = 0; i< 6 - num; i++)
			strcat(filename1, zero);
		strcat(filename1, suffix); 

		strcpy(filename2,filename1);
		strcat(filename1, "u.txt");	
		strcat(filename2, "v.txt");	
		
		

		//delete filename;

		fp_in1.open(filename1, ios::in);
		fp_in2.open(filename2, ios::in);
		//fp_in1.open("result/000000u.txt", ios::in);    // open the streams
		//fp_in2.open("result/000000v.txt", ios::in);


		D3D10_TEXTURE2D_DESC desc;
		desc.Width = 356;
		desc.Height = 272;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R16G16_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D10_USAGE_IMMUTABLE;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3DXFLOAT16 *data = new D3DXFLOAT16[WindowWidth * WindowHeight * 2];
		//BYTE data[WindowWidth * WindowHeight];
		float u,v;
		for (int i = 0; i < WindowWidth * WindowHeight * 2; i+=2)	
		{
			fp_in1>>u;
			fp_in2>>v;
			u = (u/4.0f + 1) * 0.5f;
			v = (v/4.0f + 1) * 0.5f;
			data[i] = (D3DXFLOAT16) (u);	
			data[i+1] = (D3DXFLOAT16) (v);
		}

		D3D10_SUBRESOURCE_DATA dataDesc;
		dataDesc.pSysMem = data;
		dataDesc.SysMemPitch = 256;

		ID3D10Texture2D                     *OpticalFlowTexture = NULL;
		//ID3D10ShaderResourceView            *OpticalFlowTextureSRV = NULL;
		HRESULT hr = S_OK;
		DXUTGetD3D10Device()->CreateTexture2D(&desc, &dataDesc, &OpticalFlowTexture );

		D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
		SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Format = DXGI_FORMAT_R16G16_FLOAT;

		DXUTGetD3D10Device()->CreateShaderResourceView( OpticalFlowTexture, &SRVDesc, &OpticalFlowTextureSRV[filenum - video->GetInitialFrameIndex()]);
		//shader->g_pOpticalFlowVariable->SetResource( OpticalFlowTextureSRV );

		SAFE_RELEASE(OpticalFlowTexture);
		//SAFE_RELEASE(OpticalFlowTextureSRV);
		delete data;

		fp_in1.close();
		fp_in2.close();   // close the streams
	}

	renderer->OnOpticalFlowLoaded = true;

	delete filename1;
	delete filename2;
	return S_OK;
}

HRESULT TextureRT::LoadProbability()
{
	
	D3D10_TEXTURE2D_DESC desc;
	desc.Width = WindowWidth;
	desc.Height = WindowHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D10_USAGE_IMMUTABLE;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	BYTE *data = new BYTE[WindowWidth * WindowHeight];
	//BYTE data[WindowWidth * WindowHeight];
	for (int i = 0; i < WindowWidth * WindowHeight; i++)			
		data[i] = (unsigned char) (rand()%256);		

	D3D10_SUBRESOURCE_DATA dataDesc;
	dataDesc.pSysMem = data;
	dataDesc.SysMemPitch = 256;

	ID3D10Texture2D                     *ProbabilityTexture = NULL;
	ID3D10ShaderResourceView            *ProbabilityTextureSRV = NULL;
	HRESULT hr = S_OK;
	DXUTGetD3D10Device()->CreateTexture2D(&desc, &dataDesc, &ProbabilityTexture );

	D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
    ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
    SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = 1;
    SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Format = DXGI_FORMAT_R8_UNORM;

    DXUTGetD3D10Device()->CreateShaderResourceView( ProbabilityTexture, &SRVDesc, &ProbabilityTextureSRV);
    shader->g_pProbabilityVariable->SetResource( ProbabilityTextureSRV );

	SAFE_RELEASE(ProbabilityTexture);
    SAFE_RELEASE(ProbabilityTextureSRV);
	delete data;
	return S_OK;
}

void TextureRT::reset()
{
	float color[4] = {0, 0, 0, 1 };
	for (int i = 0; i < NUM_RENDER_TARGETS; i++)	
		DXUTGetD3D10Device()->ClearRenderTargetView( pRenderTargetViews[i], color );		
}

void TextureRT::InitRenderTarget()
{
	RenderTargetFormats [RENDER_TARGET_COLOR1]   = DXGI_FORMAT_R16G16B16A16_FLOAT;
	RenderTargetFormats [RENDER_TARGET_COLOR0]   = DXGI_FORMAT_R16G16B16A16_FLOAT;
	RenderTargetFormats [RENDER_TARGET_COLOR2]   = DXGI_FORMAT_R16G16B16A16_FLOAT;
	RenderTargetFormats [RENDER_TARGET_COLOR3]   = DXGI_FORMAT_R16G16B16A16_FLOAT;
	//RenderTargetFormats [RENDER_TARGET_COLOR4]   = DXGI_FORMAT_R16G16B16A16_FLOAT;
	/*RenderTargetFormats [RENDER_TARGET_COLOR5]   = DXGI_FORMAT_R16G16B16A16_FLOAT;*/
	/*RenderTargetFormats [RENDER_TARGET_GRADIENTIMAGE1]   = DXGI_FORMAT_R16G16B16A16_FLOAT;
	RenderTargetFormats [RENDER_TARGET_GRADIENTIMAGE2]   = DXGI_FORMAT_R16G16B16A16_FLOAT;
	RenderTargetFormats [RENDER_TARGET_GRADIENTIMAGE3]   = DXGI_FORMAT_R16G16B16A16_FLOAT;
	RenderTargetFormats [RENDER_TARGET_TENSOR1]   = DXGI_FORMAT_R16_FLOAT;
	RenderTargetFormats [RENDER_TARGET_TENSOR2]   = DXGI_FORMAT_R16_FLOAT;
	RenderTargetFormats [RENDER_TARGET_TENSOR3]   = DXGI_FORMAT_R16_FLOAT;
	RenderTargetFormats [RENDER_TARGET_TENSOR4]   = DXGI_FORMAT_R16_FLOAT;*/
	//RenderTargetFormats [RENDER_TARGET_TENSOR5]   = DXGI_FORMAT_R16_FLOAT;
	RenderTargetFormats [RENDER_TARGET_FlOW1]    = DXGI_FORMAT_R32G32_FLOAT;	
	RenderTargetFormats [RENDER_TARGET_TEMP]     = DXGI_FORMAT_R16_FLOAT;
	RenderTargetFormats [RENDER_TARGET_TEMP1]	 = DXGI_FORMAT_R16_FLOAT;
	RenderTargetFormats [RENDER_TARGET_STROKE]   = DXGI_FORMAT_R16G16B16A16_FLOAT; //stroke surface details
	RenderTargetFormats [RENDER_TARGET_INFO]     = DXGI_FORMAT_R16G16B16A16_FLOAT;
	RenderTargetFormats [RENDER_TARGET_FlOW]    = DXGI_FORMAT_R32G32_FLOAT;
	RenderTargetFormats [RENDER_TARGET_PREFLOW0] = DXGI_FORMAT_R32G32_FLOAT;
	RenderTargetFormats [RENDER_TARGET_PREFLOW]  = DXGI_FORMAT_R32G32_FLOAT;
	RenderTargetFormats [RENDER_TARGET_MIPRENDERING] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	RenderTargetFormats [RENDER_TARGET_MODELMATTE] = DXGI_FORMAT_R16_FLOAT;
	memset(pRenderTargets2D, 0, sizeof(pRenderTargets2D));
    memset(pShaderResourceVariables, 0, sizeof(pShaderResourceVariables));
    memset(pRenderTargetShaderViews, 0, sizeof(pRenderTargetShaderViews));
    memset(pRenderTargetViews, 0, sizeof(pRenderTargetViews));
}






HRESULT TextureRT::CreateDepthStencilTextureAndView(int width, int height, DXGI_SAMPLE_DESC Dest)
{
	HRESULT hr = S_OK;

	// Create a new Depth-Stencil texture to replace the DXUT created one
    D3D10_TEXTURE2D_DESC descDepth;
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_R32_TYPELESS; // Use a typeless type here so that it can be both depth-stencil and shader resource.
                                                 // This will fail if we try a format like D32_FLOAT
    descDepth.SampleDesc = Dest;
    descDepth.Usage = D3D10_USAGE_DEFAULT;
    descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
	V_RETURN( DXUTGetD3D10Device()->CreateTexture2D( &descDepth, NULL, &g_pDepthStencilTexture ) );
    //V_RETURN( DXUTGetD3D10Device()->CreateTexture2D( &descDepth, NULL, &g_pDepthStencilTexture1 ) );


	// Create the depth stencil view
    D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
    if( 1 == descDepth.SampleDesc.Count ) {
        descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    } else {
        descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DMS;
    }

    descDSV.Format = DXGI_FORMAT_D32_FLOAT;	// Make the view see this as D32_FLOAT instead of typeless
    descDSV.Texture2D.MipSlice = 0;
	V_RETURN( DXUTGetD3D10Device()->CreateDepthStencilView( g_pDepthStencilTexture, &descDSV, &g_pDepthStencilDSV ) );
    //V_RETURN( DXUTGetD3D10Device()->CreateDepthStencilView( g_pDepthStencilTexture1, &descDSV, &g_pDepthStencilDSV1 ) );
	return hr;
}

DXGI_FORMAT TextureRT::GetRenderTargetFormats(int index)
{
	return RenderTargetFormats[index];
}

HRESULT TextureRT::LoadVideoResource()
{
	HRESULT hr = S_OK;
	VideoResource = new ID3D10ShaderResourceView*[video->GetNumFrame()];

	char   *filename   =   new   char[35];
	int filenum = video->GetInitialFrameIndex();
	for (; filenum < video->GetNumFrame() + video->GetInitialFrameIndex(); filenum++){
		int r = filenum;
		int index = 0;
		do{
			r = r / 10;
			index++;
		}while (r!= 0);

		char suffix [4];
	    _itoa_s (filenum,suffix,10);
 
		char   *zero   =   "0";   
		strcpy(filename,stylization->initVideoPath[stylization->SelectedVideo]);   	   
		
		for (int i = 0; i< 6 - index; i++)
			strcat(filename, zero);
		strcat(filename, suffix); 
		strcat(filename, ".jpg");		

		//Convert from char* to LPCTSTR
        int size = strlen(filename);
        TCHAR unicode_string[35];
		size_t convertedChars = 0;
        mbstowcs_s(&convertedChars, unicode_string, size+1,filename, _TRUNCATE);
	    LPCTSTR name = (LPCTSTR) unicode_string;			
		
		if ((hr = D3DX10CreateShaderResourceViewFromFile( DXUTGetD3D10Device(),name, NULL, NULL, 
			&VideoResource[filenum - video->GetInitialFrameIndex()], NULL )) != S_OK)
		{
			video->SetNumFrame(filenum - video->GetInitialFrameIndex());
			break;
		}
	}

	delete filename;
	return hr; 	
}

HRESULT TextureRT::DeleteVideoResource()
{
	HRESULT hr = S_OK;
	if (VideoResource)
	{
		for (int i = 0; i < video->GetNumFrame(); i++)
			SAFE_RELEASE(VideoResource[i]);
		delete VideoResource;
	}
	VideoResource = NULL;
	return hr;		
}


HRESULT TextureRT::ScreenCapture()
{	

	//Get the back buffer as a texture, captured everything including the UI and text on the screen
	
	HRESULT  hr;
	ID3D10Texture2D* pBackBuffer;
    hr = DXUTGetDXGISwapChain()->GetBuffer( 0, __uuidof( *pBackBuffer ), ( LPVOID* )&pBackBuffer );
    if( FAILED( hr ) )
        return hr;
    D3D10_TEXTURE2D_DESC backBufferSurfaceDesc;
    pBackBuffer->GetDesc( &backBufferSurfaceDesc );

	D3D10_TEXTURE2D_DESC Desc2;
	ZeroMemory( &Desc2, sizeof( D3D10_TEXTURE2D_DESC ) );
	Desc2.ArraySize = 1;
	Desc2.Usage = D3D10_USAGE_STAGING;
	Desc2.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Desc2.Width =  backBufferSurfaceDesc.Width;
	Desc2.Height = backBufferSurfaceDesc.Height;
	Desc2.MipLevels = 1;
	Desc2.SampleDesc.Count = 1;
	Desc2.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
	
	ID3D10Texture2D						*pStagingResource;
	V_RETURN(DXUTGetD3D10Device()->CreateTexture2D(&Desc2,NULL,&pStagingResource));    
	DXUTGetD3D10Device()->CopyResource( pStagingResource, pBackBuffer); //change

	char suffix [4];
	_itoa_s (ScreenShotIndex,suffix,10);

	//ScreenShotIndex++;

	char   *filename   =   new   char[35];
	strcpy(filename,initScreenShotPathR);
	strcat(filename, suffix); 
	strcat(filename, ".bmp");		

	//Convert from char* to LPCTSTR
    int size = strlen(filename);
    TCHAR unicode_string[35];
	size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, unicode_string, size+1,filename, _TRUNCATE);
    LPCTSTR name = (LPCTSTR) unicode_string;	

	delete filename;
	//save texture to a image file
	D3DX10SaveTextureToFile(pStagingResource,D3DX10_IFF_BMP, name);

	//**********************************************
	//copy resource back from a texture
	//saving the original image to the disk
	//**********************************************
	
	//Get the result after any passes
	
	D3D10_TEXTURE2D_DESC dstex;
	pRenderTargets2D[RENDER_TARGET_COLOR2]->GetDesc(&dstex);	
	dstex.Usage = D3D10_USAGE_STAGING;                      // Staging allows us to copy to and from the GPU
    dstex.BindFlags = 0;                                    // Staging resources cannot be bound to ANY part of the pipeline
    dstex.CPUAccessFlags = D3D10_CPU_ACCESS_READ;           // We want to read from this resource

	//HRESULT  hr;
	ID3D10Texture2D* pStagingResource0;
	V_RETURN(DXUTGetD3D10Device()->CreateTexture2D(&dstex,NULL,&pStagingResource0));    
	DXUTGetD3D10Device()->CopyResource( pStagingResource0, pRenderTargets2D[RENDER_TARGET_COLOR2]);
	
	
	//not useful for saving a screen shot
	//D3D10_MAPPED_TEXTURE2D map;
    //V_RETURN(pStagingResource->Map( 0, D3D10_MAP_READ, NULL, &map ));    
    //float* pTexels = ( float* )map.pData;	

	//**********************************************
	//Determine the file name
	//**********************************************
	char suffix1 [4];
	_itoa_s (ScreenShotIndex,suffix1,10);

	ScreenShotIndex++;

	char   *filename1   =   new   char[35];
	strcpy(filename1,initScreenShotPathO);
	strcat(filename1, suffix1); 
	strcat(filename1, ".bmp");		

	//Convert from char* to LPCTSTR
    size = strlen(filename1);
    TCHAR unicode_string1[35];
	convertedChars = 0;
    mbstowcs_s(&convertedChars, unicode_string1, size+1,filename1, _TRUNCATE);
    LPCTSTR name1 = (LPCTSTR) unicode_string1;	

	delete filename1;
	//save texture to a image file
	D3DX10SaveTextureToFile(pStagingResource0,D3DX10_IFF_BMP, name1);

	SAFE_RELEASE (pStagingResource);
	SAFE_RELEASE (pStagingResource0);
	return S_OK;
	
}