#include "dxut.h"
#include "Shaders.h"
#include "definition.h"


Shader::Shader(int w, int h)
{
	WindowWidth = w;
	WindowHeight = h;
	LoadShaders();
	SetupTechniques();
	SetupShaderVariables();
	SetShaderVariable();
}

Shader::~Shader()
{
	SAFE_RELEASE( g_pImageEffect );
	SAFE_RELEASE( g_pStaticModelEffect );
	SAFE_RELEASE( g_pAnimatedModelEffect );
	SAFE_RELEASE( g_pObjModelEffect );

	SAFE_RELEASE( ppQuery );
}

//render the result texture to the screen. 
//The rendering result is rendered to an off-screen render target to be used in the next rendering cycle
void Shader::RenderResult()
{
	inputLayout->EnableQuadVertexLayout();
	
	//DXUTGetD3D10Device()->ClearRenderTargetView( pRenderTargetViews[RENDER_TARGET_FlOW1], ClearCanvas );	
	//DXUTGetD3D10Device()->OMSetRenderTargets( 1, &pRenderTargetViews[RENDER_TARGET_FlOW1], NULL );


	//if (stylization->SelectedRenderingMode == 5)	
	//{		
		ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
		DXUTGetD3D10Device()->ClearRenderTargetView( pRTV, ClearCanvas );
		DXUTGetD3D10Device()->OMSetRenderTargets(1, &pRTV, NULL);	
	//}
	
	//DXUTGetD3D10Device()->GenerateMips( pRenderTargetShaderViews[RENDER_TARGET_MIPRENDERING] );
	textureRT->SetRTShaderView(RENDER_TARGET_MIPRENDERING);
	

	pRenderSpritesTechnique->GetPassByName("RenderMipMap")->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );
	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
}

void Shader::RenderEmptyPosition()
{
	inputLayout->EnableQuadVertexLayout();

	SetBackBuffer();
	
	//DXUTGetD3D10Device()->GenerateMips( pRenderTargetShaderViews[RENDER_TARGET_MIPRENDERING] );
	textureRT->SetRTShaderView(RENDER_TARGET_TEMP);

	pRenderSpritesTechnique->GetPassByName("RenderEmptyPosition")->Apply(0);

	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );
	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
}

void Shader::RenderDeletePosition()
{
	inputLayout->EnableQuadVertexLayout();

	SetBackBuffer();
	
	//DXUTGetD3D10Device()->GenerateMips( pRenderTargetShaderViews[RENDER_TARGET_MIPRENDERING] );

	textureRT->SetRTShaderView(RENDER_TARGET_TEMP);

	pRenderSpritesTechnique->GetPassByName("RenderDeletingPosition")->Apply(0);

	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );
	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
}

void Shader::RenderScene()
{
	//render the large strokes
    ID3D10Buffer* pBuffers[1] = { inputLayout->g_pParticleDrawFromLarge };
	UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	textureRT->NullShaderResource(RENDER_TARGET_TEMP);
	textureRT->SetRenderTarget(RENDER_TARGET_TEMP);
	textureRT->ClearRenderTarget(RENDER_TARGET_TEMP, ClearColor);

	pRenderSpritesTechnique->GetPassByName("RenderScene")->Apply( 0 );
	DXUTGetD3D10Device()->DrawAuto();

	//render the medium strokes
	pBuffers[0] = inputLayout->g_pParticleDrawFromMedium;
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	textureRT->SetRenderTarget(RENDER_TARGET_TEMP);

	pRenderSpritesTechnique->GetPassByName("RenderScene")->Apply( 0 );
	DXUTGetD3D10Device()->DrawAuto();

	//render the small strokes
	pBuffers[0] = inputLayout->g_pParticleDrawFromSmall;
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	textureRT->SetRenderTarget(RENDER_TARGET_TEMP);
  
	pRenderSpritesTechnique->GetPassByName("RenderScene")->Apply( 0 );
	DXUTGetD3D10Device()->DrawAuto();

	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
	textureRT->SetRTShaderView(RENDER_TARGET_TEMP);

}

void Shader::RenderSprites()
{
	RenderLargeSprites();
	RenderMediumSprites();
	RenderSmallSprites();
}

void Shader::RenderLargeSprites(string passName)
{
    ID3D10Buffer* pBuffers[1] = { inputLayout->g_pParticleDrawFromLarge };
    UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	textureRT->NullShaderResource(RENDER_TARGET_MIPRENDERING);
	textureRT->SetRenderTarget(RENDER_TARGET_MIPRENDERING);
	textureRT->ClearRenderTarget(RENDER_TARGET_MIPRENDERING, ClearCanvas);

	if (stylization->SelectedRenderingMode == 7)	
	{
		ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
		DXUTGetD3D10Device()->ClearRenderTargetView( pRTV, ClearCanvas );
		DXUTGetD3D10Device()->OMSetRenderTargets(1, &pRTV, NULL);	
	}
	pRenderSpritesTechnique->GetPassByName(passName.c_str())->Apply( 0 );
	//pRenderSpritesTechnique->GetPassByName("RenderSprites")->Apply( 0 );
	DXUTGetD3D10Device()->DrawAuto();
	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
	textureRT->SetRTShaderView(RENDER_TARGET_MIPRENDERING);
}

void Shader::RenderMediumSprites(string passName)
{
    ID3D10Buffer* pBuffers[1] = { inputLayout->g_pParticleDrawFromMedium };
    UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	//float ClearCanvas[4] = { 1.0f, 1.0f, 1.0f, 0.0f }; // red, green, blue, alpha
	textureRT->NullShaderResource(RENDER_TARGET_MIPRENDERING);
	textureRT->SetRenderTarget(RENDER_TARGET_MIPRENDERING);

	if (stylization->SelectedRenderingMode == 7)	
	{
		ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
		DXUTGetD3D10Device()->OMSetRenderTargets(1, &pRTV, NULL);	
	}
	pRenderSpritesTechnique->GetPassByName("RenderSprites")->Apply( 0 );
	DXUTGetD3D10Device()->DrawAuto();
	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
	textureRT->SetRTShaderView(RENDER_TARGET_MIPRENDERING);
}

void Shader::RenderSmallSprites(string passName)
{
    ID3D10Buffer* pBuffers[1] = { inputLayout->g_pParticleDrawFromSmall };
    UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	//float ClearCanvas[4] = { 1.0f, 1.0f, 1.0f, 0.0f }; // red, green, blue, alpha
	textureRT->NullShaderResource(RENDER_TARGET_MIPRENDERING);
	textureRT->SetRenderTarget(RENDER_TARGET_MIPRENDERING);

	if (stylization->SelectedRenderingMode == 7)	
	{
		ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
		DXUTGetD3D10Device()->OMSetRenderTargets(1, &pRTV, NULL);	
	}
	pRenderSpritesTechnique->GetPassByName("RenderSprites")->Apply( 0 );
	DXUTGetD3D10Device()->DrawAuto();
	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
	textureRT->SetRTShaderView(RENDER_TARGET_MIPRENDERING);
}

void Shader::DeleteSprites()
{
	DeleteSpritesLarge();
	DeleteSpritesMedium();
	DeleteSpritesSmall();
}

void Shader::DeleteSpritesLarge()
{
	//update the existing strokes
	ID3D10Buffer* pBuffers[1];
	
    pBuffers[0] = inputLayout->g_pParticleDrawFromLarge;
	
	UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	textureRT->SetRTShaderView(RENDER_TARGET_TEMP);
	// Point to the correct output buffer
    pBuffers[0] = inputLayout->g_pParticleStreamToLarge;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset ); //Input, set output target!!!!!!!!!!!!!

	pSimulateParticleTechnique->GetPassByName("DeleteSpritesLarge")->Apply( 0 );


	D3D10_QUERY_DESC QueryDesc =
	{
		D3D10_QUERY_SO_STATISTICS,
		0
	};
	
	if (DXUTGetD3D10Device()->CreateQuery(&QueryDesc, &ppQuery) != S_OK)
		return;

	ppQuery->Begin();

	DXUTGetD3D10Device()->DrawAuto(); 

	ppQuery->End();

	D3D10_QUERY_DATA_SO_STATISTICS queryData;
	ZeroMemory(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS));
	while (S_OK != ppQuery->GetData(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS), 0));	

	renderer->Num_LargePrimitives = (int)queryData.NumPrimitivesWritten;
	static bool FirstVisit = true;
	if (FirstVisit)
	{
		renderer->Num_LargeStrokePreviousFrame = renderer->Num_LargePrimitives;
		FirstVisit = false;
	}
	renderer->Num_LargeDeleted = (int)(queryData.NumPrimitivesWritten - (renderer->Num_LargeStrokePreviousFrame + renderer->Num_LargeUpdated + renderer->Num_LargeAppended));
	
	// Get back to normal
    pBuffers[0] = NULL;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

    // Swap particle buffers
    ID3D10Buffer* pTemp = inputLayout->g_pParticleDrawFromLarge;
    inputLayout->g_pParticleDrawFromLarge = inputLayout->g_pParticleStreamToLarge;
    inputLayout->g_pParticleStreamToLarge = pTemp;  
}

void Shader::DeleteSpritesMedium()
{
	//update the existing strokes
	ID3D10Buffer* pBuffers[1];
	
    pBuffers[0] = inputLayout->g_pParticleDrawFromMedium;
	
	UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	textureRT->SetRTShaderView(RENDER_TARGET_TEMP);
	// Point to the correct output buffer
    pBuffers[0] = inputLayout->g_pParticleStreamToMedium;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset ); //Input, set output target!!!!!!!!!!!!!

	pSimulateParticleTechnique->GetPassByName("DeleteSpritesMedium")->Apply( 0 );


	D3D10_QUERY_DESC QueryDesc =
	{
		D3D10_QUERY_SO_STATISTICS,
		0
	};
	
	if (DXUTGetD3D10Device()->CreateQuery(&QueryDesc, &ppQuery) != S_OK)
		return;

	ppQuery->Begin();

	DXUTGetD3D10Device()->DrawAuto(); 

	ppQuery->End();

	D3D10_QUERY_DATA_SO_STATISTICS queryData;
	ZeroMemory(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS));
	while (S_OK != ppQuery->GetData(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS), 0));
	

	renderer->Num_MediumPrimitives = (int)queryData.NumPrimitivesWritten;
	static bool FirstVisit = true;
	if (FirstVisit)
	{
		renderer->Num_MediumStrokePreviousFrame = renderer->Num_MediumPrimitives;
		FirstVisit = false;
	}
	renderer->Num_MediumDeleted = (int)(queryData.NumPrimitivesWritten - (renderer->Num_MediumStrokePreviousFrame + renderer->Num_MediumUpdated + renderer->Num_MediumAppended));


	// Get back to normal
    pBuffers[0] = NULL;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

    // Swap particle buffers
    ID3D10Buffer* pTemp = inputLayout->g_pParticleDrawFromMedium;
    inputLayout->g_pParticleDrawFromMedium = inputLayout->g_pParticleStreamToMedium;
    inputLayout->g_pParticleStreamToMedium = pTemp;  
}

void Shader::DeleteSpritesSmall()
{
	//update the existing strokes
	ID3D10Buffer* pBuffers[1];
	
    pBuffers[0] = inputLayout->g_pParticleDrawFromSmall;
	
	UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	//DXUTGetD3D10Device()->GenerateMips( pRenderTargetShaderViews[RENDER_TARGET_INFO] );
	textureRT->SetRTShaderView(RENDER_TARGET_TEMP);
	// Point to the correct output buffer
    pBuffers[0] = inputLayout->g_pParticleStreamToSmall;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset ); //Input, set output target!!!!!!!!!!!!!

	pSimulateParticleTechnique->GetPassByName("DeleteSpritesSmall")->Apply( 0 );


	D3D10_QUERY_DESC QueryDesc =
	{
		D3D10_QUERY_SO_STATISTICS,
		0
	};
	
	if (DXUTGetD3D10Device()->CreateQuery(&QueryDesc, &ppQuery) != S_OK)
		return;

	ppQuery->Begin();

	DXUTGetD3D10Device()->DrawAuto(); 

	ppQuery->End();

	D3D10_QUERY_DATA_SO_STATISTICS queryData;
	ZeroMemory(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS));
	while (S_OK != ppQuery->GetData(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS), 0));
	
	//Num_SmallDeleted = Num_SmallPrimitives + Num_SmallAppended - queryData.NumPrimitivesWritten;
	//Num_SmallPrimitives = queryData.NumPrimitivesWritten;
	renderer->Num_SmallPrimitives = (int)queryData.NumPrimitivesWritten;
	static bool FirstVisit = true;
	if (FirstVisit)
	{
		renderer->Num_SmallStrokePreviousFrame = renderer->Num_SmallPrimitives;
		FirstVisit = false;
	}
	renderer->Num_SmallDeleted = (int)(queryData.NumPrimitivesWritten - (renderer->Num_SmallStrokePreviousFrame + renderer->Num_SmallUpdated + renderer->Num_SmallAppended));
	
	// Get back to normal
    pBuffers[0] = NULL;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

    // Swap particle buffers
    ID3D10Buffer* pTemp = inputLayout->g_pParticleDrawFromSmall;
    inputLayout->g_pParticleDrawFromSmall = inputLayout->g_pParticleStreamToSmall;
    inputLayout->g_pParticleStreamToSmall = pTemp;  
}

void Shader::UpdateSprites()
{
	UpdateSpritesLarge();
	UpdateSpritesMedium();
	UpdateSpritesSmall();		
	RenderScene(); 
}

void Shader::UpdateSpritesLarge()
{
	//update the existing strokes
	ID3D10Buffer* pBuffers[1];
	
    pBuffers[0] = inputLayout->g_pParticleDrawFromLarge;
	
	UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	// Point to the correct output buffer
    pBuffers[0] = inputLayout->g_pParticleStreamToLarge;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

	pSimulateParticleTechnique->GetPassByName("UpdateSpritesLarge")->Apply( 0 );


	D3D10_QUERY_DESC QueryDesc =
	{
		D3D10_QUERY_SO_STATISTICS,
		0
	};
	
	if (DXUTGetD3D10Device()->CreateQuery(&QueryDesc, &ppQuery) != S_OK)
		return;

	ppQuery->Begin();

	if( renderer->OnInitSpriteList )
        DXUTGetD3D10Device()->Draw( int(WindowWidth * WindowHeight * DownSampling * DownSampling), 0 ); //CHANGE
    else
        DXUTGetD3D10Device()->DrawAuto(); 

	ppQuery->End();

	D3D10_QUERY_DATA_SO_STATISTICS queryData;
	ZeroMemory(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS));
	while (S_OK != ppQuery->GetData(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS), 0));	

	renderer->Num_LargeStrokePreviousFrame = renderer->Num_LargePrimitives;
	renderer->Num_LargeUpdated = (int)(queryData.NumPrimitivesWritten - renderer->Num_LargeStrokePreviousFrame);

	// Get back to normal
    pBuffers[0] = NULL;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

	ID3D10Buffer* pTemp = inputLayout->g_pParticleDrawFromLarge;
    inputLayout->g_pParticleDrawFromLarge = inputLayout->g_pParticleStreamToLarge;
    inputLayout->g_pParticleStreamToLarge = pTemp; 
	
}

void Shader::AppendSpritesModel()
{
	shader->AppendSpritesLarge();
	shader->AppendSpritesMedium();
	shader->AppendSpritesSmall();

	shader->RenderScene();	
}

void Shader::AppendSprites()
{
	AppendParticlesLargeForVideo();
	AppendSpritesMedium();
	AppendSpritesSmall();
	RenderScene();
}

void Shader::AppendParticlesLargeForVideo()
{
	ID3D10Buffer* pBuffers[1];
	//append new strokes	
    pBuffers[0] = inputLayout->g_pPointSprite;

	UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	//for the first frame, the first rendering used to determine the empty area has alpha value all 0
	//Not successful yet!!
	if (video->GetFrameIndex() == 1)
	{
		textureRT->ClearRenderTarget(RENDER_TARGET_TEMP, ClearCanvas);	
	}

	//DXUTGetD3D10Device()->GenerateMips( pRenderTargetShaderViews[RENDER_TARGET_TEMP] );
    textureRT->SetRTShaderView(RENDER_TARGET_TEMP);

	pBuffers[0] = inputLayout->g_pParticleDrawFromLarge;
	UINT AppendOffset[1] = { -1 };
	DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, AppendOffset ); //Input, set output target!!!!!!!!!!!!!
	pSimulateParticleTechnique->GetPassByName("AppendSpritesLargeForVideo")->Apply( 0 );

	D3D10_QUERY_DESC QueryDesc =
	{
		D3D10_QUERY_SO_STATISTICS,
		0
	};
	
	if (DXUTGetD3D10Device()->CreateQuery(&QueryDesc, &ppQuery) != S_OK)
		return;

	ppQuery->Begin();


    DXUTGetD3D10Device()->Draw( int(WindowWidth * WindowHeight * DownSampling * DownSampling), 0 ); //CHANGE

	ppQuery->End();

	D3D10_QUERY_DATA_SO_STATISTICS queryData;
	ZeroMemory(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS));
	while (S_OK != ppQuery->GetData(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS), 0));
	

	renderer->Num_LargeAppended = (int)queryData.NumPrimitivesWritten;

	// Get back to normal
    pBuffers[0] = NULL;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

    // Swap particle buffers
    //ID3D10Buffer* pTemp = inputLayout->g_pParticleDrawFromLarge;
    //inputLayout->g_pParticleDrawFromLarge = inputLayout->g_pParticleStreamToLarge;
    //inputLayout->g_pParticleStreamToLarge = pTemp;  
}

void Shader::AppendSpritesLarge()
{
	ID3D10Buffer* pBuffers[1];
	//append new strokes	
    pBuffers[0] = inputLayout->g_pPointSprite;

	UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	//for the first frame, the first rendering used to determine the empty area has alpha value all 0
	//Not successful yet!!
	if (video->GetFrameIndex() == 1)
	{
		textureRT->ClearRenderTarget(RENDER_TARGET_TEMP, ClearCanvas);
	}

	//DXUTGetD3D10Device()->GenerateMips( pRenderTargetShaderViews[RENDER_TARGET_TEMP] );
    textureRT->SetRTShaderView(RENDER_TARGET_TEMP);

	pBuffers[0] = inputLayout->g_pParticleDrawFromLarge;
	UINT AppendOffset[1] = { -1 };
	DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, AppendOffset ); //Input, set output target!!!!!!!!!!!!!
	pSimulateParticleTechnique->GetPassByName("AppendSpritesLarge")->Apply( 0 );

	D3D10_QUERY_DESC QueryDesc =
	{
		D3D10_QUERY_SO_STATISTICS,
		0
	};
	
	if (DXUTGetD3D10Device()->CreateQuery(&QueryDesc, &ppQuery) != S_OK)
		return;

	ppQuery->Begin();


    DXUTGetD3D10Device()->Draw( int(WindowWidth * WindowHeight * DownSampling * DownSampling), 0 ); //CHANGE

	ppQuery->End();

	D3D10_QUERY_DATA_SO_STATISTICS queryData;
	ZeroMemory(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS));
	while (S_OK != ppQuery->GetData(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS), 0));
	

	renderer->Num_LargeAppended = (int)queryData.NumPrimitivesWritten;

	// Get back to normal
    pBuffers[0] = NULL;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

    // Swap particle buffers
    //ID3D10Buffer* pTemp = inputLayout->g_pParticleDrawFromLarge;
    //inputLayout->g_pParticleDrawFromLarge = inputLayout->g_pParticleStreamToLarge;
    //inputLayout->g_pParticleStreamToLarge = pTemp;  
}

void Shader::UpdateSpritesMedium()
{
	//update the existing strokes
	ID3D10Buffer* pBuffers[1];
	
    pBuffers[0] = inputLayout->g_pParticleDrawFromMedium;
	
	UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	// Point to the correct output buffer
    pBuffers[0] = inputLayout->g_pParticleStreamToMedium;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset ); //Input, set output target!!!!!!!!!!!!!

	//DXUTGetD3D10Device()->GenerateMips( pRenderTargetShaderViews[RENDER_TARGET_INFO] );
	pSimulateParticleTechnique->GetPassByName("UpdateSpritesMedium")->Apply( 0 );


	D3D10_QUERY_DESC QueryDesc =
	{
		D3D10_QUERY_SO_STATISTICS,
		0
	};
	
	if (DXUTGetD3D10Device()->CreateQuery(&QueryDesc, &ppQuery) != S_OK)
		return;

	ppQuery->Begin();

	if( renderer->OnInitSpriteList )
        DXUTGetD3D10Device()->Draw( int(WindowWidth * WindowHeight * DownSampling * DownSampling), 0 ); //CHANGE
    else
        DXUTGetD3D10Device()->DrawAuto(); 

	ppQuery->End();

	D3D10_QUERY_DATA_SO_STATISTICS queryData;
	ZeroMemory(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS));
	while (S_OK != ppQuery->GetData(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS), 0));
	
	renderer->Num_MediumStrokePreviousFrame = renderer->Num_MediumPrimitives;
	renderer->Num_MediumUpdated = (int)(queryData.NumPrimitivesWritten - renderer->Num_MediumStrokePreviousFrame);

	// Get back to normal
    pBuffers[0] = NULL;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

	// Swap particle buffers
    ID3D10Buffer* pTemp = inputLayout->g_pParticleDrawFromMedium;
    inputLayout->g_pParticleDrawFromMedium = inputLayout->g_pParticleStreamToMedium;
    inputLayout->g_pParticleStreamToMedium = pTemp;  
}

void Shader::AppendSpritesMedium()
{
	ID3D10Buffer* pBuffers[1];
	//append new strokes	
    pBuffers[0] = inputLayout->g_pPointSprite;

	UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	//for the first frame, the first rendering used to determine the empty area has alpha value all 0
	//Not successful yet!!
	if (video->GetFrameIndex() == 1)
	{
		textureRT->ClearRenderTarget(RENDER_TARGET_TEMP, ClearCanvas);
	}

	//DXUTGetD3D10Device()->GenerateMips( pRenderTargetShaderViews[RENDER_TARGET_TEMP] );
   textureRT->SetRTShaderView(RENDER_TARGET_TEMP);

	pBuffers[0] = inputLayout->g_pParticleDrawFromMedium;
	UINT AppendOffset[1] = { -1 };
	DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, AppendOffset ); //Input, set output target!!!!!!!!!!!!!
	pSimulateParticleTechnique->GetPassByName("AppendSpritesMedium")->Apply( 0 );

	D3D10_QUERY_DESC QueryDesc =
	{
		D3D10_QUERY_SO_STATISTICS,
		0
	};
	
	if (DXUTGetD3D10Device()->CreateQuery(&QueryDesc, &ppQuery) != S_OK)
		return;

	ppQuery->Begin();


    DXUTGetD3D10Device()->Draw( int(WindowWidth * WindowHeight * DownSampling * DownSampling), 0 ); //CHANGE

	ppQuery->End();

	D3D10_QUERY_DATA_SO_STATISTICS queryData;
	ZeroMemory(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS));
	while (S_OK != ppQuery->GetData(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS), 0));
	

	renderer->Num_MediumAppended = (int)queryData.NumPrimitivesWritten;

	// Get back to normal
    pBuffers[0] = NULL;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

    // Swap particle buffers
   // ID3D10Buffer* pTemp = inputLayout->g_pParticleDrawFromMedium;
    //inputLayout->g_pParticleDrawFromMedium = inputLayout->g_pParticleStreamToMedium;
   // inputLayout->g_pParticleStreamToMedium = pTemp;  
}

void Shader::UpdateSpritesSmall()
{
	//update the existing strokes
	ID3D10Buffer* pBuffers[1];
	
    pBuffers[0] = inputLayout->g_pParticleDrawFromSmall;
	
	UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	// Point to the correct output buffer
    pBuffers[0] = inputLayout->g_pParticleStreamToSmall;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset ); //Input, set output target!!!!!!!!!!!!!

	pSimulateParticleTechnique->GetPassByName("UpdateSpritesSmall")->Apply( 0 );


	D3D10_QUERY_DESC QueryDesc =
	{
		D3D10_QUERY_SO_STATISTICS,
		0
	};
	
	if (DXUTGetD3D10Device()->CreateQuery(&QueryDesc, &ppQuery) != S_OK)
		return;

	ppQuery->Begin();

	if( renderer->OnInitSpriteList )
        DXUTGetD3D10Device()->Draw( int(WindowWidth * WindowHeight * DownSampling * DownSampling), 0 ); //CHANGE
    else
        DXUTGetD3D10Device()->DrawAuto(); 

	ppQuery->End();

	D3D10_QUERY_DATA_SO_STATISTICS queryData;
	ZeroMemory(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS));
	while (S_OK != ppQuery->GetData(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS), 0));
	
	renderer->Num_SmallStrokePreviousFrame = renderer->Num_SmallPrimitives;
	renderer->Num_SmallUpdated = (int)(queryData.NumPrimitivesWritten - renderer->Num_SmallStrokePreviousFrame);


	// Get back to normal
    pBuffers[0] = NULL;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

	// Swap particle buffers
    ID3D10Buffer* pTemp = inputLayout->g_pParticleDrawFromSmall;
    inputLayout->g_pParticleDrawFromSmall = inputLayout->g_pParticleStreamToSmall;
    inputLayout->g_pParticleStreamToSmall = pTemp;  

}

void Shader::AppendSpritesSmall()
{
	ID3D10Buffer* pBuffers[1];
	//append new strokes	
    pBuffers[0] = inputLayout->g_pPointSprite;

	UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	//for the first frame, the first rendering used to determine the empty area has alpha value all 0
	//Not successful yet!!
	if (video->GetFrameIndex() == 1)
	{		
		textureRT->ClearRenderTarget(RENDER_TARGET_TEMP, ClearCanvas);
	}

	//DXUTGetD3D10Device()->GenerateMips( pRenderTargetShaderViews[RENDER_TARGET_MIPRENDERING] );
    textureRT->SetRTShaderView(RENDER_TARGET_TEMP);

	pBuffers[0] = inputLayout->g_pParticleDrawFromSmall;
	UINT AppendOffset[1] = { -1 };
	DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, AppendOffset ); //Input, set output target!!!!!!!!!!!!!
	pSimulateParticleTechnique->GetPassByName("AppendSpritesSmall")->Apply( 0 );

	D3D10_QUERY_DESC QueryDesc =
	{
		D3D10_QUERY_SO_STATISTICS,
		0
	};
	
	if (DXUTGetD3D10Device()->CreateQuery(&QueryDesc, &ppQuery) != S_OK)
		return;

	ppQuery->Begin();


    DXUTGetD3D10Device()->Draw( int(WindowWidth * WindowHeight * DownSampling * DownSampling), 0 ); //CHANGE

	ppQuery->End();

	D3D10_QUERY_DATA_SO_STATISTICS queryData;
	ZeroMemory(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS));
	while (S_OK != ppQuery->GetData(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS), 0));
	

	renderer->Num_SmallAppended = (int)queryData.NumPrimitivesWritten;

	// Get back to normal
    pBuffers[0] = NULL;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

    // Swap particle buffers
    //ID3D10Buffer* pTemp = inputLayout->g_pParticleDrawFromSmall;
    //inputLayout->g_pParticleDrawFromSmall = inputLayout->g_pParticleStreamToSmall;
    //inputLayout->g_pParticleStreamToSmall = pTemp;  
}

void Shader::DetermineSprites()
{
	DetermineLargeSprites();
	DetermineMediumSprites();
	DetermineSmallSprites();
	RenderScene();
}

void Shader::DetermineLargeSprites()
{
	ID3D10Buffer* pBuffers[1];
	if( renderer->OnInitSpriteList )
        pBuffers[0] = inputLayout->g_pPointSprite;
    else
        pBuffers[0] = inputLayout->g_pParticleDrawFromLarge;
	
	UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };

	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	// Point to the correct output buffer
    pBuffers[0] = inputLayout->g_pParticleStreamToLarge;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset ); 
	pSimulateParticleTechnique->GetPassByName("DetermineSpriteLarge")->Apply( 0 );

	//prepare for query
	D3D10_QUERY_DESC QueryDesc =
	{
		D3D10_QUERY_SO_STATISTICS,
		0
	};
	//ID3D10Query *ppQuery = NULL;
	if (DXUTGetD3D10Device()->CreateQuery(&QueryDesc, &ppQuery) != S_OK)
		return;

	//draw the pimitives
	ppQuery->Begin();
	if( renderer->OnInitSpriteList )
        DXUTGetD3D10Device()->Draw( int(WindowWidth * WindowHeight * DownSampling * DownSampling), 0 ); //CHANGE
    else
        DXUTGetD3D10Device()->DrawAuto(); 
	ppQuery->End();

	//query the number of primitives drawn
	D3D10_QUERY_DATA_SO_STATISTICS queryData;
	ZeroMemory(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS));
	while (S_OK != ppQuery->GetData(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS), 0));

	renderer->Num_LargePrimitives = (int)(queryData.NumPrimitivesWritten);


	//ppQuery->Release();
	//stylization->Num_LargePrimitives = queryData.NumPrimitivesWritten;

	// Get back to normal
    pBuffers[0] = NULL;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

    // Swap particle buffers
    ID3D10Buffer* pTemp = inputLayout->g_pParticleDrawFromLarge;
    inputLayout->g_pParticleDrawFromLarge = inputLayout->g_pParticleStreamToLarge;
    inputLayout->g_pParticleStreamToLarge = pTemp;
}


void Shader::DetermineMediumSprites()
{
	ID3D10Buffer* pBuffers[1];
	if( renderer->OnInitSpriteList )
        pBuffers[0] = inputLayout->g_pPointSprite;
    else
        pBuffers[0] = inputLayout->g_pParticleDrawFromMedium;
	
	UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };

	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	// Point to the correct output buffer
    pBuffers[0] = inputLayout->g_pParticleStreamToMedium;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset ); 
	pSimulateParticleTechnique->GetPassByName("DetermineSpriteMedium")->Apply( 0 );

	//prepare for query
	D3D10_QUERY_DESC QueryDesc =
	{
		D3D10_QUERY_SO_STATISTICS,
		0
	};
	//ID3D10Query *ppQuery = NULL;
	if (DXUTGetD3D10Device()->CreateQuery(&QueryDesc, &ppQuery) != S_OK)
		return;

	//draw the pimitives
	ppQuery->Begin();
	if( renderer->OnInitSpriteList )
        DXUTGetD3D10Device()->Draw( int(WindowWidth * WindowHeight * DownSampling * DownSampling), 0 ); //CHANGE
    else
        DXUTGetD3D10Device()->DrawAuto(); 
	ppQuery->End();

	//query the number of primitives drawn
	D3D10_QUERY_DATA_SO_STATISTICS queryData;
	ZeroMemory(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS));
	while (S_OK != ppQuery->GetData(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS), 0));
	//ppQuery->Release();
	renderer->Num_MediumPrimitives = (int)queryData.NumPrimitivesWritten;

	// Get back to normal
    pBuffers[0] = NULL;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

    // Swap particle buffers
    ID3D10Buffer* pTemp = inputLayout->g_pParticleDrawFromMedium;
    inputLayout->g_pParticleDrawFromMedium = inputLayout->g_pParticleStreamToMedium;
    inputLayout->g_pParticleStreamToMedium = pTemp;
}


void Shader::DetermineSmallSprites()
{
	ID3D10Buffer* pBuffers[1];
	if( renderer->OnInitSpriteList )
        pBuffers[0] = inputLayout->g_pPointSprite;
    else
        pBuffers[0] = inputLayout->g_pParticleDrawFromSmall;
	
	UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };

	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	// Point to the correct output buffer
    pBuffers[0] = inputLayout->g_pParticleStreamToSmall;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset ); 
	pSimulateParticleTechnique->GetPassByName("DetermineSpriteSmall")->Apply( 0 );

	//prepare for query
	D3D10_QUERY_DESC QueryDesc =
	{
		D3D10_QUERY_SO_STATISTICS,
		0
	};
	//ID3D10Query *ppQuery = NULL;
	if (DXUTGetD3D10Device()->CreateQuery(&QueryDesc, &ppQuery) != S_OK)
		return;

	//draw the pimitives
	ppQuery->Begin();
	if( renderer->OnInitSpriteList )
        DXUTGetD3D10Device()->Draw( int(WindowWidth * WindowHeight * DownSampling * DownSampling), 0 ); //CHANGE
    else
        DXUTGetD3D10Device()->DrawAuto(); 
	ppQuery->End();

	//query the number of primitives drawn
	D3D10_QUERY_DATA_SO_STATISTICS queryData;
	ZeroMemory(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS));
	while (S_OK != ppQuery->GetData(&queryData, sizeof(D3D10_QUERY_DATA_SO_STATISTICS), 0));
	renderer->Num_SmallPrimitives = (int)queryData.NumPrimitivesWritten;

	// Get back to normal
    pBuffers[0] = NULL;
    DXUTGetD3D10Device()->SOSetTargets( 1, pBuffers, offset );

    // Swap particle buffers
    ID3D10Buffer* pTemp = inputLayout->g_pParticleDrawFromSmall;
    inputLayout->g_pParticleDrawFromSmall = inputLayout->g_pParticleStreamToSmall;
    inputLayout->g_pParticleStreamToSmall = pTemp;
	renderer->OnInitSpriteList = false;
}

void Shader::GaussianSmoothPicture(LPCSTR PassName)
{
	textureRT->NullShaderResource(RENDER_TARGET_COLOR0);
	textureRT->SetRenderTarget(RENDER_TARGET_COLOR0);

	if (stylization->SelectedRenderingMode == 1)
		SetBackBuffer();

	pPreprocessingTechnique->GetPassByName(PassName)->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );

	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
	textureRT->SetRTShaderView(RENDER_TARGET_COLOR0);
}


//not big difference from Gaussian
//void MedianFilterPicture()
//{
//	pShaderResourceVariables[RENDER_TARGET_COLOR1]->SetResource( NULL );
//	DXUTGetD3D10Device()->OMSetRenderTargets( 1, &pRenderTargetViews[RENDER_TARGET_COLOR1], NULL );
//
//	if (stylization->SelectedRenderingMode == 1)
//		SetBackBuffer();
//	pPreprocessingTechnique->GetPassByName("MedianFilter")->Apply(0);
//	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );
//
//	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
//	pShaderResourceVariables[RENDER_TARGET_COLOR1]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_COLOR1] );
//}

void Shader::RenderPaperTexture()
{
	//SetBackBuffer();
	ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
	//DXUTGetD3D10Device()->ClearRenderTargetView( pRTV, ClearColor );
	DXUTGetD3D10Device()->OMSetRenderTargets(1, &pRTV, NULL);

	pPreprocessingTechnique->GetPassByName("RenderPaperTexture")->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );
}

void Shader::AddSobelFilterWithFocus()
{
	textureRT->NullShaderResource(RENDER_TARGET_INFO);	
	textureRT->SetRenderTarget(RENDER_TARGET_INFO);
	textureRT->ClearRenderTarget(RENDER_TARGET_INFO, ClearColor);

	pPreprocessingTechnique->GetPassByName("AddSobelFilterWithFocus")->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );

	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
	
	textureRT->SetRTShaderView(RENDER_TARGET_INFO);	
}

void Shader::AddSobelFilter()
{
	textureRT->NullShaderResource(RENDER_TARGET_INFO);
	textureRT->SetRenderTarget(RENDER_TARGET_INFO);
    textureRT->ClearRenderTarget(RENDER_TARGET_INFO, ClearColor);

	pPreprocessingTechnique->GetPassByName("AddSobelFilter")->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );

	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
	
	textureRT->SetRTShaderView(RENDER_TARGET_INFO);	
}

void Shader::ConstructStrokeDetails()
{	
	textureRT->NullShaderResource(RENDER_TARGET_STROKE);
	textureRT->SetRenderTarget(RENDER_TARGET_STROKE);
    textureRT->ClearRenderTarget(RENDER_TARGET_STROKE, ClearColor);

	pPreprocessingTechnique->GetPassByName("ConstructStrokesDetails")->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );	
	
	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
	textureRT->SetRTShaderView(RENDER_TARGET_STROKE);	
}

void Shader::RenderOpticalFlow()
{
	ID3D10Buffer* pBuffers[1] = { inputLayout->g_pPointSprite };
    UINT stride[1] = { sizeof( InputLayout::PARTICLE_VERTEX ) };
    UINT offset[1] = { 0 };
	DXUTGetD3D10Device()->IASetInputLayout( inputLayout->GetParticleVertexLayout() );
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	//SetBackBuffer();
	ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
	DXUTGetD3D10Device()->ClearRenderTargetView( pRTV, ClearCanvas );
	DXUTGetD3D10Device()->OMSetRenderTargets(1, &pRTV, NULL);

	pRenderSpritesTechnique->GetPassByName("RenderOpticalFlowSprites")->Apply( 0 );
	DXUTGetD3D10Device()->Draw( int(WindowWidth * WindowHeight * DownSampling * DownSampling), 0 ); //CHANGE
	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);	
}

void Shader::RenderOpticalFlowTest()
{
	SetBackBuffer();
	textureRT->SetRTShaderView(RENDER_TARGET_FlOW);	
	pOpticalFlowTechnique->GetPassByName("RenderOpticalFlow")->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );
}

void Shader::RenderGradient()
{
	SetBackBuffer();
	//pShaderResourceVariables[RENDER_TARGET_INFO]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_INFO] );
	pPreprocessingTechnique->GetPassByName("RenderGradient")->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );
}

void Shader::RenderStrokePosition()
{
	SetBackBuffer();
	textureRT->SetRTShaderView(RENDER_TARGET_STROKE);	
	pPreprocessingTechnique->GetPassByName("RenderStrokePosition")->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );
}

void Shader::RenderTest()
{
	SetBackBuffer();
	pOpticalFlowTechnique->GetPassByName("RenderTest")->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );
}

void Shader::RenderModelOnVideoFrame()
{
	textureRT->NullShaderResource(RENDER_TARGET_COLOR2);	
	textureRT->SetRenderTarget(RENDER_TARGET_COLOR2);
    textureRT->ClearRenderTarget(RENDER_TARGET_COLOR2, ClearColor);
	pPreprocessingTechnique->GetPassByName("RenderModelOnVideoFrame")->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );
	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);	
	textureRT->SetRTShaderView(RENDER_TARGET_COLOR2);	
}

void Shader::RenderOriginal()
{
	SetBackBuffer();

	pPreprocessingTechnique->GetPassByName("Original")->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );
}

void Shader::RenderModelMatte()
{
	SetBackBuffer();
	
	pPreprocessingTechnique->GetPassByName("ModelMatte")->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );
}

void Shader::PreOpticalFlow()
{
	textureRT->NullShaderResource(RENDER_TARGET_COLOR0);
	textureRT->SetRenderTarget(RENDER_TARGET_COLOR0);
    textureRT->ClearRenderTarget(RENDER_TARGET_COLOR0, ClearColor);

	pOpticalFlowTechnique->GetPassByName("PreOpticalFlow")->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );

	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);	
	textureRT->SetRTShaderView(RENDER_TARGET_COLOR0);		

}

void Shader::OpticalFlow(int IterIndex)
{
	//float InitialOpticalFlowValues[2] = { 0.5f, 0.5f }; 	

	if (IterIndex == 0)
	{		
		textureRT->ClearRenderTarget(RENDER_TARGET_PREFLOW, InitialOpticalFlowValues);
	}

	if (IterIndex == video->GetMaxIteration() - 1)
	{		
		textureRT->NullShaderResource(RENDER_TARGET_FlOW);
		textureRT->SetRenderTarget(RENDER_TARGET_FlOW);
		textureRT->ClearRenderTarget(RENDER_TARGET_FlOW, InitialOpticalFlowValues);
		textureRT->SetRTShaderView(RENDER_TARGET_FlOW);	
	}
	else	
	{		
		textureRT->NullShaderResource(RENDER_TARGET_PREFLOW0 + IterIndex % 2);
		textureRT->SetRenderTarget(RENDER_TARGET_PREFLOW0 + IterIndex % 2);
		textureRT->ClearRenderTarget(RENDER_TARGET_PREFLOW0 + IterIndex % 2, InitialOpticalFlowValues);
	}

	textureRT->SetRTShaderView(RENDER_TARGET_PREFLOW - IterIndex % 2);	
	
	if (IterIndex % 2 == 0)
		pOpticalFlowTechnique->GetPassByName("OpticalFlowI1")->Apply(0);
	else
		pOpticalFlowTechnique->GetPassByName("OpticalFlowI2")->Apply(0);
	DXUTGetD3D10Device()->DrawIndexed( 6, 0, 0 );
	DXUTGetD3D10Device()->OMSetRenderTargets(0, NULL, NULL);
}

void Shader::SetBackBuffer()
{
	ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
	DXUTGetD3D10Device()->ClearRenderTargetView( pRTV, ClearColor );
	DXUTGetD3D10Device()->OMSetRenderTargets(1, &pRTV, NULL);
}

void Shader::SetupShaderVariables()
{
	g_pStrokeTexVariable               = g_pImageEffect->GetVariableByName( "StrokeTex" )->AsShaderResource();
	g_pAlphaMaskVariable               = g_pImageEffect->GetVariableByName( "AlphaMask" )->AsShaderResource();
	g_pNormalMapVariable			   = g_pImageEffect->GetVariableByName( "PaperTex" )->AsShaderResource();
	g_pProbabilityVariable             = g_pImageEffect->GetVariableByName( "Probability" )->AsShaderResource();
	g_pOpticalFlowVariable			   = g_pImageEffect->GetVariableByName( "OpticalFlowTexture" )->AsShaderResource();
	DeltaXShaderVariable               = g_pImageEffect->GetVariableByName( "deltaX" )->AsScalar();
    DeltaYShaderVariable               = g_pImageEffect->GetVariableByName( "deltaY" )->AsScalar();
	WindowWidthVariable                = g_pImageEffect->GetVariableByName( "windowWidth" )->AsScalar();
    WindowHeightVariable               = g_pImageEffect->GetVariableByName( "windowHeight" )->AsScalar();
	NumParticleXVariable			   = g_pImageEffect->GetVariableByName( "numParticleX" )->AsScalar();
	NumParticleYVariable			   = g_pImageEffect->GetVariableByName( "numParticleY" )->AsScalar();
	StrokeTexWidthVariable			   = g_pImageEffect->GetVariableByName( "StrokeTexWidth" )->AsScalar();
	StrokeTexLengthVariable			   = g_pImageEffect->GetVariableByName( "StrokeTexLength" )->AsScalar();
	GradientThresholdShaderVariable    = g_pImageEffect->GetVariableByName( "gradientThreshold" )->AsScalar();
	StrokeLengthShaderVariable         = g_pImageEffect->GetVariableByName( "strokeLength" )->AsScalar();
	StrokeWidthShaderVariable          = g_pImageEffect->GetVariableByName( "strokeWidth" )->AsScalar();
	GradientAngleShaderVariable        = g_pImageEffect->GetVariableByName( "angleThreshold" )->AsScalar();
	ToggleAngleShaderVariable          = g_pImageEffect->GetVariableByName( "OnRealGradientAngle" )->AsScalar();
	ToggleClippingShaderVariable       = g_pImageEffect->GetVariableByName( "OnClipping" )->AsScalar();
	LambdaShaderVariable               = g_pImageEffect->GetVariableByName( "Lambda" )->AsScalar();	
	g_pmWorldViewProj0				   = g_pImageEffect->GetVariableByName( "g_mWorldViewProj0" )->AsMatrix();
    g_pmInvView						   = g_pImageEffect->GetVariableByName( "g_mInvView" )->AsMatrix();
	RenderingStyleShaderVariable	   = g_pImageEffect->GetVariableByName( "renderStyle" )->AsScalar();

	ProbabilityLargeShaderVariable     = g_pImageEffect->GetVariableByName( "ProbabilityLargeThreshold" )->AsScalar();
	ProbabilityMediumShaderVariable    = g_pImageEffect->GetVariableByName( "ProbabilityMediumThreshold" )->AsScalar();
	ProbabilitySmallShaderVariable     = g_pImageEffect->GetVariableByName( "ProbabilitySmallThreshold" )->AsScalar();
	GradientLargeShaderVariable        = g_pImageEffect->GetVariableByName( "GradientLargeThreshold" )->AsScalar();
	GradientMediumShaderVariable       = g_pImageEffect->GetVariableByName( "GradientMediumThreshold" )->AsScalar();
	AlphaLargeShaderVariable           = g_pImageEffect->GetVariableByName( "AlphaLargeThreshold" )->AsScalar();
	AlphaMediumShaderVariable          = g_pImageEffect->GetVariableByName( "AlphaMediumThreshold" )->AsScalar();
	AlphaSmallShaderVariable           = g_pImageEffect->GetVariableByName( "AlphaSmallThreshold" )->AsScalar();
	SizeMediumShaderVariable           = g_pImageEffect->GetVariableByName( "SizeMediumThreshold" )->AsScalar();
	SizeSmallShaderVariable            = g_pImageEffect->GetVariableByName( "SizeSmallThreshold" )->AsScalar();
	ContrastShaderVariable             = g_pImageEffect->GetVariableByName( "ContrastThreshold" )->AsScalar();
	FadeOutSpeedShaderVariable         = g_pImageEffect->GetVariableByName( "FadeOutSpeedThreshold" )->AsScalar();
	FadeInSpeedShaderVariable		   = g_pImageEffect->GetVariableByName( "FadeInSpeedThreshold" )->AsScalar();
	ParallelReductionIndexVariable	   = g_pImageEffect->GetVariableByName( "index" )->AsScalar();
	CurentTimeShaderVariable		   = g_pImageEffect->GetVariableByName( "CurTime" )->AsScalar();
	MipMapLevelVariable				   = g_pImageEffect->GetVariableByName( "MipMapLevel" )->AsScalar();

	MouseXVariable					   = g_pImageEffect->GetVariableByName( "MouseX" )->AsScalar();
	MouseYVariable					   = g_pImageEffect->GetVariableByName( "MouseY" )->AsScalar();
	FocusfVariable					   = g_pImageEffect->GetVariableByName( "focusf" )->AsScalar();
	Focusr1Variable					   = g_pImageEffect->GetVariableByName( "focusr1" )->AsScalar();
	Focusr2Variable					   = g_pImageEffect->GetVariableByName( "focusr2" )->AsScalar();
	FocusPointMethodShaderVariable	   = g_pImageEffect->GetVariableByName( "OnFocusPoint" )->AsScalar();

	//10/11
	g_pMeshTextureVariable				= g_pObjModelEffect->GetVariableByName( "g_MeshTexture" )->AsShaderResource();     
    g_pAmbient							= g_pObjModelEffect->GetVariableByName( "g_vMaterialAmbient" )->AsVector();
	g_pDiffuse							= g_pObjModelEffect->GetVariableByName( "g_vMaterialDiffuse" )->AsVector();
    g_pSpecular							= g_pObjModelEffect->GetVariableByName( "g_vMaterialSpecular" )->AsVector();
    g_pOpacity							= g_pObjModelEffect->GetVariableByName( "g_fMaterialAlpha" )->AsScalar();
    g_pSpecularPower					= g_pObjModelEffect->GetVariableByName( "g_nMaterialShininess" )->AsScalar();
    g_pLightColor						= g_pObjModelEffect->GetVariableByName( "g_vLightColor" )->AsVector();
    g_pCameraPosition					= g_pObjModelEffect->GetVariableByName( "g_vCameraPosition" )->AsVector();
	g_pLightPosition					= g_pObjModelEffect->GetVariableByName( "g_vLightPosition" )->AsVector();
	g_pWorld1							= g_pObjModelEffect->GetVariableByName( "g_mWorld" )->AsMatrix();
	g_pWorldViewProjection1				= g_pObjModelEffect->GetVariableByName( "g_mWorldViewProjection" )->AsMatrix();
	g_pPreWorldViewProjection1			= g_pObjModelEffect->GetVariableByName( "g_mPreWorldViewProjection" )->AsMatrix();

    g_pWorld0							= g_pStaticModelEffect->GetVariableByName( "g_mWorld0" )->AsMatrix();
    g_pWorldViewProjection				= g_pStaticModelEffect->GetVariableByName( "g_mWorldViewProjection" )->AsMatrix();
	g_pPreWorldViewProjection			= g_pStaticModelEffect->GetVariableByName( "g_mPreWorldViewProjection" )->AsMatrix();
	g_pvWorldLightDir1					= g_pStaticModelEffect->GetVariableByName( "g_vWorldLightDir1" )->AsVector();
	g_pvWorldLightDir2					= g_pStaticModelEffect->GetVariableByName( "g_vWorldLightDir2" )->AsVector();
	g_pvEyePt0							= g_pStaticModelEffect->GetVariableByName( "g_vEyePt0" )->AsVector();
	g_pDiffuseTex0						= g_pStaticModelEffect->GetVariableByName( "g_txDiffuse0" )->AsShaderResource();
    g_pNormalTex0						= g_pStaticModelEffect->GetVariableByName( "g_txNormal0" )->AsShaderResource();


	//10/13
	    // Get effects variables
    g_pmWorldViewProj1 = g_pAnimatedModelEffect->GetVariableByName( "g_mWorldViewProj1" )->AsMatrix();
	g_pmPreWorldViewProj1 = g_pAnimatedModelEffect->GetVariableByName( "g_mPreWorldViewProj1" )->AsMatrix();
    g_pmWorld1 = g_pAnimatedModelEffect->GetVariableByName( "g_mWorld1" )->AsMatrix();
    g_pvLightPos = g_pAnimatedModelEffect->GetVariableByName( "g_vLightPos" )->AsVector();
    g_pvEyePt1 = g_pAnimatedModelEffect->GetVariableByName( "g_vEyePt1" )->AsVector();
    g_pmConstBoneWorld = g_pAnimatedModelEffect->GetVariableByName( "g_mConstBoneWorld" )->AsMatrix();
    g_pmTexBoneWorld = g_pAnimatedModelEffect->GetVariableByName( "g_mTexBoneWorld" )->AsMatrix();
    g_pBoneBufferVar = g_pAnimatedModelEffect->GetVariableByName( "g_BufferBoneWorld" )->AsShaderResource();
    g_ptxBoneTexture = g_pAnimatedModelEffect->GetVariableByName( "g_txTexBoneWorld" )->AsShaderResource();
    g_ptxDiffuse1 = g_pAnimatedModelEffect->GetVariableByName( "g_txDiffuse1" )->AsShaderResource();
    g_ptxNormal1 = g_pAnimatedModelEffect->GetVariableByName( "g_txNormal1" )->AsShaderResource();
}

void Shader::SetupTechniques()
{
	// Obtain the technique
    pRenderSpritesTechnique		= g_pImageEffect->GetTechniqueByName( "RenderSprites" );
	pSimulateParticleTechnique	= g_pImageEffect->GetTechniqueByName( "SimulateParticle" );
	pPreprocessingTechnique		= g_pImageEffect->GetTechniqueByName( "Preprocessing" );
	pOpticalFlowTechnique		= g_pImageEffect->GetTechniqueByName( "OpticalFlow" ); 
	pOtherTechnique				= g_pImageEffect->GetTechniqueByName( "OtherTechnique" ); 
		
	g_pRenderScene				= g_pStaticModelEffect->GetTechniqueByName( "RenderSDKScene" );
    g_pRenderSky				= g_pStaticModelEffect->GetTechniqueByName( "RenderSky" );
	g_pRenderSceneOpticalFlow	= g_pStaticModelEffect->GetTechniqueByName( "RenderSceneOpticalFlow" );
	g_pRenderSkyOpticalFlow		= g_pStaticModelEffect->GetTechniqueByName( "RenderSkyOpticalFlow" );

	//10/13
	g_pRenderConstantBuffer = g_pAnimatedModelEffect->GetTechniqueByName( "RenderConstantBuffer" );
    g_pRenderTextureBuffer = g_pAnimatedModelEffect->GetTechniqueByName( "RenderTextureBuffer" );
    g_pRenderTexture = g_pAnimatedModelEffect->GetTechniqueByName( "RenderTexture" );
    g_pRenderBuffer = g_pAnimatedModelEffect->GetTechniqueByName( "RenderBuffer" );

    g_pRenderConstantBuffer_SO = g_pAnimatedModelEffect->GetTechniqueByName( "RenderConstantBuffer_SO" );
    g_pRenderTextureBuffer_SO = g_pAnimatedModelEffect->GetTechniqueByName( "RenderTextureBuffer_SO" );
    g_pRenderTexture_SO = g_pAnimatedModelEffect->GetTechniqueByName( "RenderTexture_SO" );
    g_pRenderBuffer_SO = g_pAnimatedModelEffect->GetTechniqueByName( "RenderBuffer_SO" );

    g_pRenderPostTransformed = g_pAnimatedModelEffect->GetTechniqueByName( "RenderPostTransformed" );
	g_pComputeOpticalFlow = g_pAnimatedModelEffect->GetTechniqueByName( "ComputeOpticalFlow" );
	g_pComputeOpticalFlow1 = g_pAnimatedModelEffect->GetTechniqueByName( "ComputeOpticalFlow1" ); //10/20
	g_pRenderModelMatte	= g_pAnimatedModelEffect->GetTechniqueByName( "RenderModelMatte" );

	pRenderMeshTechnique		= g_pObjModelEffect->GetTechniqueByName( "NoSpecular" );
	pRenderOpticalFlow			= g_pObjModelEffect->GetTechniqueByName( "RenderSceneOpticalFlow" );
}

HRESULT Shader::LoadShaders()
{
	// Create the effect
    DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3D10_SHADER_DEBUG;
    #endif
	HRESULT hr = S_OK;


    hr = D3DX10CreateEffectFromFile( L"Code\\Images.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, DXUTGetD3D10Device(), NULL,
                                         NULL, &g_pImageEffect, NULL, NULL );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,L"The Effect FX file cannot be located.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

	hr = D3DX10CreateEffectFromFile( L"Code\\StaticModel.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, DXUTGetD3D10Device(), NULL,
                                         NULL, &g_pStaticModelEffect, NULL, NULL );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,L"The Mesh FX file cannot be located.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

	hr = D3DX10CreateEffectFromFile( L"Code\\AnimatedModel.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, DXUTGetD3D10Device(), NULL,
                                              NULL, &g_pAnimatedModelEffect, NULL, NULL );

	if( FAILED( hr ) )
    {
        MessageBox( NULL,L"The Mesh FX file cannot be located.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

	hr = D3DX10CreateEffectFromFile( L"Code\\ObjModel.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, DXUTGetD3D10Device(), NULL,
                                              NULL, &g_pObjModelEffect, NULL, NULL );

	if( FAILED( hr ) )
    {
        MessageBox( NULL,L"The Mesh FX file cannot be located.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }
	return hr;
}

void Shader::SetShaderVariable()
{
	DeltaXShaderVariable              ->SetFloat(float(1.0f / WindowWidth));
    DeltaYShaderVariable              ->SetFloat(float(1.0f / WindowHeight));
	WindowWidthVariable				  ->SetInt(WindowWidth);
	WindowHeightVariable			  ->SetInt(WindowHeight);
	NumParticleXVariable			  ->SetInt(int(WindowWidth * DownSampling));
	NumParticleYVariable			  ->SetInt(int(WindowHeight * DownSampling));
	GradientThresholdShaderVariable   ->SetFloat( stroke->gradientThreshold );
	StrokeLengthShaderVariable        ->SetFloat( WindowWidth * 0.01f );
	StrokeWidthShaderVariable         ->SetFloat( WindowWidth * 0.01f * 0.5f );
	GradientAngleShaderVariable       ->SetFloat( stroke->angleThreshold );
	ToggleAngleShaderVariable         ->SetBool ( renderer->OnRealGradientAngle );
	ToggleClippingShaderVariable	  ->SetBool	( renderer->OnClipping );
	LambdaShaderVariable			  ->SetFloat( video->GetLambda() );

	ProbabilityLargeShaderVariable	  ->SetFloat( stroke->ProbabilityLargeThreshold );
	ProbabilityMediumShaderVariable	  ->SetFloat( stroke->ProbabilityMediumThreshold );
	ProbabilitySmallShaderVariable	  ->SetFloat( stroke->ProbabilitySmallThreshold );
	GradientLargeShaderVariable	      ->SetFloat( stroke->GradientLargeThreshold );
	GradientMediumShaderVariable	  ->SetFloat( stroke->GradientMediumThreshold );
	//GradientSmallShaderVariable	      ->SetFloat( GradientSmallThreshold );
	AlphaLargeShaderVariable	      ->SetFloat( stroke->AlphaLargeThreshold );
	AlphaMediumShaderVariable	      ->SetFloat( stroke->AlphaMediumThreshold );
	AlphaSmallShaderVariable	      ->SetFloat( stroke->AlphaSmallThreshold );
	SizeMediumShaderVariable	      ->SetFloat( stroke->SizeMediumThreshold );
	SizeSmallShaderVariable	          ->SetFloat( stroke->SizeSmallThreshold );
	ContrastShaderVariable			  ->SetFloat( stylization->ContrastThreshold );
	FadeOutSpeedShaderVariable		  ->SetFloat( stylization->FadeOutSpeedThreshold );
	FadeInSpeedShaderVariable		  ->SetFloat( stylization->FadeInSpeedThreshold );
	CurentTimeShaderVariable		  ->SetFloat( stylization->CurTime );
	MipMapLevelVariable				  ->SetInt( stylization->MipMapLevel );

	FocusfVariable					  ->SetFloat( stylization->FocusfThreshold );
	Focusr1Variable					  ->SetFloat( stylization->Focusr1Threshold );
	Focusr2Variable					  ->SetFloat( stylization->Focusr2Threshold );

	MouseXVariable					  ->SetFloat(0.5f/float(WindowWidth));
	MouseYVariable					  ->SetFloat(0.5f/float(WindowHeight));
	FocusPointMethodShaderVariable	  ->SetBool ( renderer->OnFocusPoint );
}

