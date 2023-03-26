#ifndef TEXTURERT_H
#define TEXTURERT_H

#include "dxut.h"
//#include "definition.h"
#include <fstream>
#include "Shaders.h"
#include "Video.h"

extern Shader* shader;
extern Video* video;

enum RENDER_TARGET
{
	RENDER_TARGET_COLOR0, 
    RENDER_TARGET_COLOR1, //Previous Video Frame    
	RENDER_TARGET_COLOR2, //Current Video Frame
	RENDER_TARGET_COLOR3, //Next Video Frame
	RENDER_TARGET_FlOW,
	RENDER_TARGET_FlOW1,	
	RENDER_TARGET_TEMP, //Store the result of sobel filter for the current frame
	RENDER_TARGET_TEMP1,
	RENDER_TARGET_STROKE,
	RENDER_TARGET_INFO, 
	RENDER_TARGET_PREFLOW0,
	RENDER_TARGET_PREFLOW,
	RENDER_TARGET_MIPRENDERING,
	RENDER_TARGET_MODELMATTE,
    NUM_RENDER_TARGETS		
};

class TextureRT{
public:
	TextureRT(int, int);
	~TextureRT();

	void            reset();
	void			InitRenderTarget();
	HRESULT         CreateRenderTargetTextureAndView( int rtIndex, D3D10_TEXTURE2D_DESC TexDesc, const D3D10_SUBRESOURCE_DATA * pInitialData);
	HRESULT			CreateDepthStencilTextureAndView(int width, int height, DXGI_SAMPLE_DESC Dest);
	HRESULT         CreateRTTextureAsShaderResource( RENDER_TARGET rtIndex, LPCSTR shaderImageName,ID3D10Effect* pEffect,D3D10_SHADER_RESOURCE_VIEW_DESC *SRVDesc );
	HRESULT         CreateRenderTargets(UINT width, UINT height);
	HRESULT			GetStrokeTextureInfo(int index);
	HRESULT         LoadTexture(int resource, int resourceVar);
	HRESULT			LoadTexture1( LPCWSTR ImageName, ID3D10ShaderResourceView* resource, ID3D10EffectShaderResourceVariable* resourceVar);
	HRESULT			LoadOpticalFlow();
	HRESULT			LoadProbability();
	void			SetAlphaTexture(int index);
	void			SetStrokeTexture(int index);	
	ID3D10ShaderResourceView** GetVideoResource(){return VideoResource;}
	void SetVideoResource(int RenderTargetIndex, int FrameIndex);
	void SetRTShaderView(int ShaderResourceIndex);
	void NullShaderResource(int ShaderResourceIndex);
	void SetRenderTarget(int RenderTargetIndex);
	void SetRenderTargetStencil(int RenderTargetIndex);
	void ClearRenderTarget(int RenderTargetIndex, float*);
	HRESULT ScreenCapture();
	HRESULT DeleteVideoResource();
	HRESULT LoadVideoResource();
	DXGI_FORMAT GetRenderTargetFormats(int index);
	void ClearDepthStencil();
	//int GetSelectedImage(){return SelectedImage;}
	//void SetSelectedImage(int index){SelectedImage = index;}
	void SetScreenShotIndex(int index){ScreenShotIndex = index;}
	//void SetMipMapLevel(int m){MipMapLevel = m;}
	//int GetMipMapLevel(){return MipMapLevel;}

//private: //test
public:

	ID3D10ShaderResourceView*           g_pStrokeTex;
	ID3D10ShaderResourceView*           g_pAlphaMask;
	ID3D10ShaderResourceView*			g_pNormalMap;
	ID3D10ShaderResourceView*           g_pTextureRV;
	ID3D10ShaderResourceView**			VideoResource;
	ID3D10ShaderResourceView**          OpticalFlowTextureSRV;


	ID3D10Texture2D                     *pRenderTargets2D				[ NUM_RENDER_TARGETS ];
	ID3D10ShaderResourceView            *pRenderTargetShaderViews		[ NUM_RENDER_TARGETS ];
	ID3D10RenderTargetView              *pRenderTargetViews				[ NUM_RENDER_TARGETS ];
	ID3D10EffectShaderResourceVariable  *pShaderResourceVariables		[ NUM_RENDER_TARGETS ];
	DXGI_FORMAT                         RenderTargetFormats				[ NUM_RENDER_TARGETS ];

	ID3D10Texture2D*					g_pDepthStencilTexture;
	ID3D10DepthStencilView*				g_pDepthStencilDSV;


	int									OpticalFlowMip;
	int									ScreenShotIndex;
	int									WindowWidth;
	int									WindowHeight;
};


#endif