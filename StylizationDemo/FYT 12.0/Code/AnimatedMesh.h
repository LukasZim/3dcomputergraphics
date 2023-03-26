#ifndef ANIMATEDMESH_H
#define ANIMATEDMESH_H
#include "DXUT.h"
#include "SDKmesh.h" 
#include "Style.h"

class AnimatedMesh{
public:
	AnimatedMesh();
	~AnimatedMesh();
	void DeleteMesh();
	void LoadAnimatedMesh();
	void TransformMesh(D3DXMATRIX mIdentity, double fTime);
	void SetBoneMatrices( UINT iMesh );
	void GetInstanceWorldMatrix( UINT iInstance, D3DXMATRIX* pmWorld );
	void StreamOutBones(int VBs);	
	float MatrixNorm(D3DXMATRIX m);
	void RenderModelMatte();
	void RenderOpticalFlow(bool ClearRenderTarget = true);
	void RenderModelIntoTexture(int RenderTargetIndex);
	void SwapBuffers();
	HRESULT CreateBoneTextureAndBuffer();
	D3DXVECTOR3 GetLightPos(){return g_vLightPos;}
private:
	CDXUTSDKMesh                        g_SkinnedMesh;			// The skinned mesh
	ID3D10Buffer*                       g_pBoneBuffer;
	ID3D10ShaderResourceView*           g_pBoneBufferRV;
	ID3D10Texture1D*                    g_pBoneTexture;
	ID3D10ShaderResourceView*           g_pBoneTextureRV;

	ID3D10Buffer**                      g_ppTransformedVBs;
	ID3D10Buffer**						g_ppPreTransformedVBs;

	D3DXVECTOR3                         g_vLightPos;


	struct STREAM_OUT_VERTEX
	{
		D3DXVECTOR4 Pos;
		D3DXVECTOR3 Norm;
		D3DXVECTOR2 Tex;
		D3DXVECTOR3 Tangent;
	};

	static const int MAX_BONE_MATRICES = 255;
	static const int MAX_SPRITES = 500;
	static const int MAX_INSTANCES = 500;
};

#endif