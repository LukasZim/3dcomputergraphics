#ifndef STATICMESH_H
#define STATICMESH_H
#include "DXUT.h"
#include "SDKmesh.h" 
#include "Style.h"

extern Style* style;

class StaticMesh{
public:
		StaticMesh();
		~StaticMesh();
		void DeleteMesh();
		void LoadStaticMesh();		
		CDXUTSDKMesh GetObjectMesh(){return g_ObjectMesh;}
		CDXUTSDKMesh GetObjectMesh1(){return g_ObjectMesh1;}
		CDXUTSDKMesh GetSkyMesh(){return g_SkyMesh;}
		void RenderModelIntoTexture(int RenderTargetIndex);
		void RenderOpticalFlow();
		D3DXVECTOR3 GetLightDir1(){return g_vLightDir1;}
		D3DXVECTOR3 GetLightDir2(){return g_vLightDir2;}
private:
		CDXUTSDKMesh						g_ObjectMesh;
		CDXUTSDKMesh						g_ObjectMesh1;
		CDXUTSDKMesh						g_ObjectMesh2;
		CDXUTSDKMesh						g_SkyMesh;
		D3DXVECTOR3							g_vLightDir1;
		D3DXVECTOR3							g_vLightDir2;
};


#endif