#include "dxut.h"
#include "definition.h"


using namespace std;

StaticMesh* staticMesh;
AnimatedMesh* animatedMesh;
ObjMesh* objMesh;
Video* video;
Stroke* stroke;

Viewer* viewer;
Renderer* renderer;
Shader* shader;
TextureRT* textureRT;
InputLayout* inputLayout;

Camera* camera;
Stylization* stylization;

float								ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // red, green, blue, alpha
float								ClearCanvas[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // red, green, blue, alpha
float								InitialOpticalFlowValues[2] = { 0.5f, 0.5f }; 







