#ifndef SHADERS_H
#define SHADERS_H

#include "dxut.h"
//#include "definition.h"
#include "InputLayout.h"
#include "Renderer.h"

class Shader{
	friend Renderer;
public:
	Shader(int, int);
	~Shader();
	HRESULT				LoadShaders();
	void				SetupTechniques();
	void				SetupShaderVariables();
	void				SetShaderVariable();

	//Preprocessing
	void 				GaussianSmoothPicture(LPCSTR PassName);
	void 				AddSobelFilter(); //not focus
	void				AddSobelFilterWithFocus(); //with focus
	void 				ConstructStrokeDetails();


	//Optical Flow
	void				PreOpticalFlow();
	void				OpticalFlow(int IterIndex);


	//Stroke Processing
	void				DetermineSprites();
	void 				DetermineLargeSprites();
	void 				DetermineMediumSprites();
	void 				DetermineSmallSprites();

	void				UpdateSprites();
	void 				UpdateSpritesLarge();
	void 				UpdateSpritesMedium();
	void 				UpdateSpritesSmall();

	void				AppendSpritesModel();
	void				AppendSprites();
	void				AppendParticlesLargeForVideo();
	void 				AppendSpritesLarge();
	void 				AppendSpritesMedium();
	void 				AppendSpritesSmall();

	void				DeleteSprites();
	void 				DeleteSpritesLarge();
	void 				DeleteSpritesMedium();
	void 				DeleteSpritesSmall();

	//Stroke Rendering

	void				RenderSprites();
	void 				RenderLargeSprites(string passName = "RenderSprites");
	void 				RenderMediumSprites(string passName = "RenderSprites");
	void 				RenderSmallSprites(string passName = "RenderSprites");

	//Helper
	void				RenderOriginal();
	void 				RenderEmptyPosition();
	void 				RenderDeletePosition();
	void 				RenderScene();
	void 				RenderOpticalFlow();
	void 				RenderOpticalFlowTest();
	void 				RenderGradient();
	void 				RenderStrokePosition();
	void 				RenderTest();
	void 				RenderModelOnVideoFrame();
	void 				RenderModelMatte();
	void				RenderResult();
	void				RenderPaperTexture();
	void				SetBackBuffer();

	ID3D10Effect*		GetAnimatedModelEffect(){return g_pAnimatedModelEffect;}
	ID3D10Effect*		GetImageEffect(){return g_pImageEffect;}
	ID3D10Effect*		GetStaticModelEffect(){return g_pStaticModelEffect;}
	ID3D10Effect*		GetObjModelEffect(){return g_pObjModelEffect;}

	ID3D10EffectTechnique* GetRenderSpritesTechnique(){return pRenderSpritesTechnique;}
	ID3D10EffectTechnique* GetSimulateParticleTechnique(){return pSimulateParticleTechnique;}
	ID3D10EffectTechnique* GetPreprocessingTechnique(){return pPreprocessingTechnique;}
	ID3D10EffectTechnique* GetOpticalFlowTechnique(){return pOpticalFlowTechnique;}
	ID3D10EffectTechnique* GetOtherTechnique(){return pOtherTechnique;}
	ID3D10EffectTechnique* GetRenderMeshTechnique(){return pRenderMeshTechnique;}
	ID3D10EffectTechnique* GetRenderOpticalFlow(){return pRenderOpticalFlow;}

	ID3D10EffectTechnique* GetRenderPostTransformed(){return g_pRenderPostTransformed;}
	ID3D10EffectTechnique* GetComputeOpticalFlow1(){return g_pComputeOpticalFlow1;}
	ID3D10EffectTechnique* GetRenderConstantBuffer(){return g_pRenderConstantBuffer;}
	ID3D10EffectTechnique* GetRenderScene(){return g_pRenderScene;}
	ID3D10EffectTechnique* GetRenderModelMatte(){return g_pRenderModelMatte;}
	ID3D10EffectTechnique* GetRenderConstantBuffer_SO(){return g_pRenderConstantBuffer_SO;}
	ID3D10EffectTechnique* GetRenderSky(){return g_pRenderSky;}
	ID3D10EffectTechnique* GetRenderSceneOpticalFlow(){return g_pRenderSceneOpticalFlow;}
	ID3D10EffectTechnique* GetRenderSkyOpticalFlow(){return g_pRenderSkyOpticalFlow;}
	


private:

	ID3D10Query							*ppQuery;

	//Effect
	ID3D10Effect*                       g_pAnimatedModelEffect;
	ID3D10Effect*                       g_pImageEffect;
	ID3D10Effect*						g_pStaticModelEffect;
	ID3D10Effect*						g_pObjModelEffect;

	//Techniques
	ID3D10EffectTechnique*              pRenderSpritesTechnique;
	ID3D10EffectTechnique*              pSimulateParticleTechnique;
	ID3D10EffectTechnique*              pPreprocessingTechnique;
	ID3D10EffectTechnique*				pOpticalFlowTechnique;
	ID3D10EffectTechnique*				pOtherTechnique;

	ID3D10EffectTechnique*              pRenderMeshTechnique; 
	ID3D10EffectTechnique*              pRenderOpticalFlow;

	ID3D10EffectTechnique*				g_pRenderScene;
	ID3D10EffectTechnique*				g_pRenderSky;
	ID3D10EffectTechnique*				g_pRenderSceneOpticalFlow;
	ID3D10EffectTechnique*				g_pRenderSkyOpticalFlow;
	ID3D10EffectTechnique*              g_pRenderConstantBuffer;
	ID3D10EffectTechnique*              g_pRenderTextureBuffer;
	ID3D10EffectTechnique*              g_pRenderTexture;
	ID3D10EffectTechnique*              g_pRenderBuffer;

	ID3D10EffectTechnique*              g_pRenderConstantBuffer_SO;
	ID3D10EffectTechnique*              g_pRenderTextureBuffer_SO;
	ID3D10EffectTechnique*              g_pRenderTexture_SO;
	ID3D10EffectTechnique*              g_pRenderBuffer_SO;

	ID3D10EffectTechnique*              g_pRenderPostTransformed;
	ID3D10EffectTechnique*				g_pComputeOpticalFlow1;
	ID3D10EffectTechnique*				g_pComputeOpticalFlow;
	ID3D10EffectTechnique*				g_pRenderModelMatte;

	//Shader Variables

	public:
	ID3D10EffectShaderResourceVariable* g_pStrokeTexVariable; //stroke texture
	ID3D10EffectShaderResourceVariable* g_pAlphaMaskVariable; //alpha mask
	ID3D10EffectShaderResourceVariable* g_pNormalMapVariable; //normal map
	ID3D10EffectShaderResourceVariable* g_pProbabilityVariable; //probability texture
	ID3D10EffectShaderResourceVariable* g_pOpticalFlowVariable;

	ID3D10EffectScalarVariable          *DeltaXShaderVariable;
	ID3D10EffectScalarVariable          *DeltaYShaderVariable;
	ID3D10EffectScalarVariable          *WindowWidthVariable;
	ID3D10EffectScalarVariable          *WindowHeightVariable;
	ID3D10EffectScalarVariable			*NumParticleXVariable;
	ID3D10EffectScalarVariable			*NumParticleYVariable;
	ID3D10EffectScalarVariable			*StrokeTexWidthVariable;
	ID3D10EffectScalarVariable			*StrokeTexLengthVariable;
	ID3D10EffectScalarVariable			*MouseXVariable;
	ID3D10EffectScalarVariable			*MouseYVariable;
	ID3D10EffectScalarVariable          *GradientThresholdShaderVariable;
	ID3D10EffectScalarVariable          *ProbabilityThresholdShaderVariable;
	ID3D10EffectScalarVariable          *StrokeLengthShaderVariable;
	ID3D10EffectScalarVariable          *StrokeWidthShaderVariable;
	ID3D10EffectScalarVariable          *GradientAngleShaderVariable;
	ID3D10EffectScalarVariable          *ToggleAngleShaderVariable;
	ID3D10EffectScalarVariable          *ToggleClippingShaderVariable;
	ID3D10EffectScalarVariable			*LambdaShaderVariable;
	ID3D10EffectScalarVariable			*RenderingStyleShaderVariable;
	ID3D10EffectScalarVariable			*ProbabilityLargeShaderVariable;
	ID3D10EffectScalarVariable			*ProbabilityMediumShaderVariable;
	ID3D10EffectScalarVariable			*ProbabilitySmallShaderVariable;
	ID3D10EffectScalarVariable			*GradientLargeShaderVariable;
	ID3D10EffectScalarVariable			*GradientMediumShaderVariable;
	ID3D10EffectScalarVariable			*SizeMediumShaderVariable;
	ID3D10EffectScalarVariable			*SizeSmallShaderVariable;
	ID3D10EffectScalarVariable			*ContrastShaderVariable;
	ID3D10EffectScalarVariable			*FadeOutSpeedShaderVariable;
	ID3D10EffectScalarVariable			*FadeInSpeedShaderVariable;
	ID3D10EffectScalarVariable			*AlphaLargeShaderVariable;
	ID3D10EffectScalarVariable			*AlphaMediumShaderVariable;
	ID3D10EffectScalarVariable			*AlphaSmallShaderVariable;
	ID3D10EffectScalarVariable			*ParallelReductionIndexVariable;
	ID3D10EffectScalarVariable			*CurentTimeShaderVariable;
	ID3D10EffectScalarVariable			*MipMapLevelVariable;
	ID3D10EffectScalarVariable			*FocusfVariable;
	ID3D10EffectScalarVariable			*Focusr1Variable;
	ID3D10EffectScalarVariable			*Focusr2Variable;
	ID3D10EffectScalarVariable			*FocusPointMethodShaderVariable;

	
	ID3D10EffectMatrixVariable*         g_pWorld0;
	ID3D10EffectMatrixVariable*         g_pWorldViewProjection;
	ID3D10EffectShaderResourceVariable* g_pMeshTextureVariable;
	ID3D10EffectMatrixVariable*         g_pPreWorldViewProjection;
	ID3D10EffectShaderResourceVariable* g_pDiffuseTex0;
	ID3D10EffectShaderResourceVariable* g_pNormalTex0;
	ID3D10EffectVectorVariable*			g_pvWorldLightDir1;
	ID3D10EffectVectorVariable*			g_pvWorldLightDir2;
	ID3D10EffectVectorVariable*			g_pvEyePt0; 
	ID3D10EffectMatrixVariable*         g_pmWorldViewProj1;
	ID3D10EffectMatrixVariable*         g_pmPreWorldViewProj1;
	ID3D10EffectMatrixVariable*         g_pmWorld1;
	ID3D10EffectVectorVariable*         g_pvLightPos;
	ID3D10EffectVectorVariable*         g_pvEyePt1;
	ID3D10EffectMatrixVariable*         g_pmConstBoneWorld;
	ID3D10EffectMatrixVariable*         g_pmTexBoneWorld;
	ID3D10EffectShaderResourceVariable* g_pBoneBufferVar;
	ID3D10EffectShaderResourceVariable* g_ptxBoneTexture;
	ID3D10EffectShaderResourceVariable* g_ptxDiffuse1;
	ID3D10EffectShaderResourceVariable* g_ptxNormal1;
	ID3D10EffectMatrixVariable*         g_pmWorldViewProj0;
	ID3D10EffectMatrixVariable*         g_pmInvView;


	ID3D10EffectVectorVariable*         g_pAmbient;
	ID3D10EffectVectorVariable*         g_pDiffuse;
	ID3D10EffectVectorVariable*         g_pSpecular;
	ID3D10EffectScalarVariable*         g_pOpacity;
	ID3D10EffectScalarVariable*         g_pSpecularPower;
	ID3D10EffectVectorVariable*         g_pLightColor;
	ID3D10EffectVectorVariable*         g_pCameraPosition;
	ID3D10EffectVectorVariable*         g_pLightPosition;	
	ID3D10EffectMatrixVariable*         g_pWorld1;
	ID3D10EffectMatrixVariable*         g_pWorldViewProjection1;
	ID3D10EffectMatrixVariable*			g_pPreWorldViewProjection1;

	int									WindowWidth;
	int									WindowHeight;
};

void				SetBackBuffer();

















#endif