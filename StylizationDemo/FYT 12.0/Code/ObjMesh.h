#ifndef OBJMESH_H
#define OBJMESH_H
#include "MeshLoader10.h"

class ObjMesh{
private:
	CMeshLoader10                       g_MeshLoader;


public:
	ObjMesh();
	~ObjMesh();
	void DeleteMesh();
	HRESULT LoadObjScene();
	void RenderSubset( UINT iSubset );
	void RenderObjMesh();
	void RenderOpticalFlow( UINT iSubset );

};

#endif