#ifndef INPUTLAYOUT_H
#define INPUTLAYOUT_H

#include "dxut.h"
#include <vector>

using namespace std;



class InputLayout{
public:
	InputLayout();
	~InputLayout();
	HRESULT			SetupParticleVertexLayout();
	HRESULT			SetupQuadInputLayout();
	HRESULT			SetupObjMeshInputLayout();
	HRESULT			SetupSDKSceneInputLayout();
	HRESULT			SetupSkinnedMeshInputLayout();
	HRESULT			SetupTransformedMeshInputLayout();
	HRESULT			SetupForOpticalFlowMeshInputLayout();

	void			EnableQuadVertexLayout();

	ID3D10InputLayout* GetParticleVertexLayout(){return g_pParticleVertexLayout;}
	ID3D10InputLayout* GetQuadVertexLayout(){return g_pQuadVertexLayout;}
	ID3D10InputLayout* GetObjMeshVertexlayout(){return g_pObjMeshVertexlayout;}
	ID3D10InputLayout* GetSDKSceneVertexLayout(){return g_pSDKSceneVertexLayout;}
	ID3D10InputLayout* GetSkinnedVertexLayout(){return g_pSkinnedVertexLayout;}
	ID3D10InputLayout* GetTransformedVertexLayout(){return g_pTransformedVertexLayout;}
	ID3D10InputLayout* GetOpticalFlowVertexLayout(){return g_pOpticalFlowVertexLayout;}

	HRESULT			ConstructParticleBuffer();

private:
	//Input Layout
	ID3D10InputLayout*                  g_pParticleVertexLayout;
	ID3D10InputLayout*                  g_pQuadVertexLayout;
	ID3D10InputLayout*					g_pObjMeshVertexlayout;
	ID3D10InputLayout*					g_pSDKSceneVertexLayout;
	ID3D10InputLayout*                  g_pSkinnedVertexLayout;
	ID3D10InputLayout*                  g_pTransformedVertexLayout;
	ID3D10InputLayout*					g_pOpticalFlowVertexLayout;

	struct SimpleVertex
	{
		D3DXVECTOR3 Pos;
		D3DXVECTOR2 Tex;
	};

public:
	//Input Buffers
	ID3D10Buffer*                       g_pQuadVertexBuffer;
	ID3D10Buffer*                       g_pQuadIndexBuffer;
	ID3D10Buffer*                       g_pPointSprite;
	ID3D10Buffer*                       g_pParticleStreamTo;
	ID3D10Buffer*                       g_pParticleDrawFrom;
	ID3D10Buffer*                       g_pParticleStreamToSmall;
	ID3D10Buffer*						g_pParticleDrawFromSmall; 
	ID3D10Buffer*                       g_pParticleStreamToLarge; 
	ID3D10Buffer*                       g_pParticleDrawFromLarge;
	ID3D10Buffer*                       g_pParticleStreamToMedium;
	ID3D10Buffer*                       g_pParticleDrawFromMedium;

	struct PARTICLE_VERTEX
	{
		D3DXVECTOR2 pos;
		D3DXVECTOR4 color;
		D3DXVECTOR4 info;
	};

};

#endif