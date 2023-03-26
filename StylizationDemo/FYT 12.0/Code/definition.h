#pragma once
#ifndef DEFINITION_H
#define DEFINITION_H

#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "DXUTcamera.h"
#include "SDKmisc.h"
#include "resource.h"
#include <vector>
#include "Renderer.h"
#include "Shaders.h"
#include "StaticMesh.h"
#include "AnimatedMesh.h"
#include "Stroke.h"
#include "Viewer.h"
#include "Video.h"
#include "TextureRT.h"
#include "Camera.h"
#include "Stylization.h"
#include "meshloader10.h"
#include "ObjMesh.h"
using namespace std;

extern Renderer* renderer;
extern Shader* shader;
extern InputLayout* inputLayout;
extern StaticMesh* staticMesh;
extern AnimatedMesh* animatedMesh;
extern Stroke* stroke;
extern Viewer* viewer;
extern Video* video;
extern TextureRT* textureRT;
extern Camera* camera;
extern Stylization* stylization;
extern ObjMesh* objMesh;

#ifndef SAFE_CHECK
#define SAFE_CHECK(x)        { hr = (x); if( FAILED(hr) ) { return hr; } } 
#endif

extern float								ClearColor[4]; // red, green, blue, alpha
extern float								ClearCanvas[4]; // red, green, blue, alpha
extern float								InitialOpticalFlowValues[2]; 

extern CMeshLoader10								g_MeshLoader;

#endif