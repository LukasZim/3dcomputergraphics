#include "dxut.h"
#include "InputLayout.h"
#include "definition.h"
#include "Stylization.h"

extern Stylization* stylization;


InputLayout::InputLayout()
{	
	g_pParticleVertexLayout		= NULL;
	g_pQuadVertexLayout			= NULL;
	g_pObjMeshVertexlayout		= NULL;
	g_pSDKSceneVertexLayout		= NULL;
	SetupQuadInputLayout(); 
	SetupParticleVertexLayout();
	SetupSDKSceneInputLayout(); 
	SetupSkinnedMeshInputLayout();
	SetupTransformedMeshInputLayout();
	SetupForOpticalFlowMeshInputLayout(); 
	SetupObjMeshInputLayout();
	
}

InputLayout::~InputLayout()
{
	SAFE_RELEASE( g_pQuadVertexLayout );
	SAFE_RELEASE( g_pParticleVertexLayout );
	SAFE_RELEASE( g_pObjMeshVertexlayout );
	SAFE_RELEASE( g_pSDKSceneVertexLayout );
	SAFE_RELEASE( g_pSkinnedVertexLayout );
	SAFE_RELEASE( g_pTransformedVertexLayout );
	SAFE_RELEASE( g_pOpticalFlowVertexLayout );

	//release buffers
	SAFE_RELEASE( g_pQuadIndexBuffer );
	SAFE_RELEASE( g_pQuadVertexBuffer );
	SAFE_RELEASE( g_pPointSprite );
	SAFE_RELEASE( g_pParticleStreamTo );
	SAFE_RELEASE( g_pParticleDrawFrom );
	SAFE_RELEASE( g_pParticleStreamToSmall );
	SAFE_RELEASE( g_pParticleDrawFromSmall );
	SAFE_RELEASE( g_pParticleStreamToLarge );
	SAFE_RELEASE( g_pParticleStreamToLarge );
	SAFE_RELEASE( g_pParticleStreamToMedium );
	SAFE_RELEASE( g_pParticleStreamToMedium );
}

//For the current camara parameters
//The window is like 
// (-80,  80)         (80,  80)
// (-80, -80)         (80, -80)
HRESULT InputLayout::ConstructParticleBuffer()
{
	HRESULT hr = S_OK;

	D3D10_BUFFER_DESC vbdesc =
    {
        int(stylization->WindowWidth * stylization->WindowHeight * DownSampling * DownSampling) * sizeof( PARTICLE_VERTEX ), //CHANGE
        D3D10_USAGE_DEFAULT,//D3D10_USAGE_STAGING,
        D3D10_BIND_VERTEX_BUFFER,
        0,//D3D10_CPU_ACCESS_READ,
        0
    };

	D3D10_SUBRESOURCE_DATA vbInitData;
    ZeroMemory( &vbInitData, sizeof( D3D10_SUBRESOURCE_DATA ) );
	
	vector<PARTICLE_VERTEX> Vertices;
	for (int i = 0; i < stylization->WindowWidth * DownSampling; i++)
	{
		for (int j = 0; j < stylization->WindowHeight * DownSampling; j++)
		{
			PARTICLE_VERTEX vertStart =
			{
				D3DXVECTOR2( i / float(stylization->WindowWidth * DownSampling), j / float(stylization->WindowHeight * DownSampling)),
				D3DXVECTOR4( 0, 0, 0, 0),
				D3DXVECTOR4( 0, 0, 0, 0),
			};
			Vertices.push_back(vertStart);
		}
	}
	
    vbInitData.pSysMem = &Vertices[0];
    vbInitData.SysMemPitch = sizeof( PARTICLE_VERTEX );

    V_RETURN( DXUTGetD3D10Device()->CreateBuffer( &vbdesc, &vbInitData, &g_pPointSprite ) );

	vbdesc.ByteWidth = int(stylization->WindowWidth * stylization->WindowHeight * DownSampling * DownSampling) * sizeof( PARTICLE_VERTEX );
    vbdesc.BindFlags |= D3D10_BIND_STREAM_OUTPUT;
    V_RETURN( DXUTGetD3D10Device()->CreateBuffer( &vbdesc, NULL, &g_pParticleDrawFrom ) );
    V_RETURN( DXUTGetD3D10Device()->CreateBuffer( &vbdesc, NULL, &g_pParticleStreamTo ) );
	V_RETURN( DXUTGetD3D10Device()->CreateBuffer( &vbdesc, NULL, &g_pParticleDrawFromLarge ) );
    V_RETURN( DXUTGetD3D10Device()->CreateBuffer( &vbdesc, NULL, &g_pParticleStreamToLarge ) );
	V_RETURN( DXUTGetD3D10Device()->CreateBuffer( &vbdesc, NULL, &g_pParticleDrawFromMedium ) );
    V_RETURN( DXUTGetD3D10Device()->CreateBuffer( &vbdesc, NULL, &g_pParticleStreamToMedium ) );
	V_RETURN( DXUTGetD3D10Device()->CreateBuffer( &vbdesc, NULL, &g_pParticleDrawFromSmall ) );
    V_RETURN( DXUTGetD3D10Device()->CreateBuffer( &vbdesc, NULL, &g_pParticleStreamToSmall ) );

	return S_OK;
}


//HRESULT ConstructSpriteBuffer()
//{
//	HRESULT hr = S_OK;
//
//	D3D10_BUFFER_DESC vbdesc =
//    {
//        int(WindowWidth * WindowHeight * DownSampling * DownSampling) * sizeof( PARTICLE_VERTEX ), //CHANGE
//        D3D10_USAGE_DEFAULT,//D3D10_USAGE_STAGING,
//        D3D10_BIND_VERTEX_BUFFER,
//        0,//D3D10_CPU_ACCESS_READ,
//        0
//    };
//
//	vbdesc.ByteWidth = int(WindowWidth * WindowHeight * DownSampling * DownSampling) * sizeof( PARTICLE_VERTEX );
//    vbdesc.BindFlags |= D3D10_BIND_STREAM_OUTPUT;
//    V_RETURN( DXUTGetD3D10Device()->CreateBuffer( &vbdesc, NULL, &g_pParticleDrawFrom ) );
//	return S_OK;
//}

HRESULT InputLayout::SetupParticleVertexLayout()
{
	HRESULT hr = S_OK;
	V_RETURN( ConstructParticleBuffer() );
	const D3D10_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "INFO", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        //{ "ANGLE", 0, DXGI_FORMAT_R32_FLOAT, 0, 16, D3D10_INPUT_PER_VERTEX_DATA, 0 },       
    };

    D3D10_PASS_DESC PassDesc;
    shader->GetSimulateParticleTechnique()->GetPassByName( "DetermineSpriteLarge" )->GetDesc( &PassDesc );
    V_RETURN( DXUTGetD3D10Device()->CreateInputLayout( layout, sizeof( layout ) / sizeof( layout[0] ),
                                             PassDesc.pIAInputSignature,
                                             PassDesc.IAInputSignatureSize, &g_pParticleVertexLayout ) );
	return hr;
}

//HRESULT InputLayout::SetupSpriteVertexLayout()
//{
//	HRESULT hr = S_OK;
//	V_RETURN( ConstructParticleBuffer() );
//	const D3D10_INPUT_ELEMENT_DESC layout[] =
//    {
//        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
//		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D10_INPUT_PER_VERTEX_DATA, 0 },
//        { "INFO", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
//        //{ "ANGLE", 0, DXGI_FORMAT_R32_FLOAT, 0, 16, D3D10_INPUT_PER_VERTEX_DATA, 0 },       
//    };
//
//    D3D10_PASS_DESC PassDesc;
//    shader->GetSimulateParticleTechnique()->GetPassByName( "DetermineSpriteLarge" )->GetDesc( &PassDesc );
//    V_RETURN( DXUTGetD3D10Device()->CreateInputLayout( layout, sizeof( layout ) / sizeof( layout[0] ),
//                                             PassDesc.pIAInputSignature,
//                                             PassDesc.IAInputSignatureSize, &g_pParticleVertexLayout ) );
//	return hr;
//}

HRESULT InputLayout::SetupQuadInputLayout()
{
	HRESULT hr = S_OK;
	// Define the input layout
    D3D10_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = sizeof( layout ) / sizeof( layout[0] );

    // Create the input layout
    D3D10_PASS_DESC PassDesc;
    shader->GetPreprocessingTechnique()->GetPassByName( "Original" )->GetDesc( &PassDesc );
    SAFE_CHECK(DXUTGetD3D10Device()->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature,PassDesc.IAInputSignatureSize, &g_pQuadVertexLayout ));

    // Set the input layout
    DXUTGetD3D10Device()->IASetInputLayout( g_pQuadVertexLayout );	

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
		{ D3DXVECTOR3( -1.0f, -1.0f, 1.0f ), D3DXVECTOR2( 0.0f, 1.0f ) },
        { D3DXVECTOR3(  1.0f, -1.0f, 1.0f ), D3DXVECTOR2( 1.0f, 1.0f ) },
        { D3DXVECTOR3(  1.0f,  1.0f, 1.0f ), D3DXVECTOR2( 1.0f, 0.0f ) },
        { D3DXVECTOR3( -1.0f,  1.0f, 1.0f ), D3DXVECTOR2( 0.0f, 0.0f ) },
    };

    D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleVertex ) * 4; //four vertices in the vertex list
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertices;
    SAFE_CHECK(DXUTGetD3D10Device()->CreateBuffer( &bd, &InitData, &g_pQuadVertexBuffer ));
    
    // Set vertex buffer
    UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
    DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, &g_pQuadVertexBuffer, &stride, &offset );

    // Create index buffer
    DWORD indices[] =
    {
        3,1,0,
        2,1,3,
    };
    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( DWORD ) * 6; //6 numbers in the index list
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    InitData.pSysMem = indices;
    SAFE_CHECK(DXUTGetD3D10Device()->CreateBuffer( &bd, &InitData, &g_pQuadIndexBuffer ));
   
    // Set index buffer
    DXUTGetD3D10Device()->IASetIndexBuffer( g_pQuadIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

    // Set primitive topology
    DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	return hr;
}

HRESULT InputLayout::SetupObjMeshInputLayout()
{
	HRESULT hr = S_OK;
	const D3D10_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    } ;
    UINT numElements = sizeof( layout ) / sizeof( layout[0] );

    // Create the input layout
    D3D10_PASS_DESC PassDesc;
    shader->GetRenderMeshTechnique()->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    V_RETURN( DXUTGetD3D10Device()->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature,
                                             PassDesc.IAInputSignatureSize, &g_pObjMeshVertexlayout ) );

    //DXUTGetD3D10Device()->IASetInputLayout( g_pMeshVertexlayout );
	return hr;
}

//10/12
HRESULT InputLayout::SetupSDKSceneInputLayout()
{
	HRESULT hr = S_OK;
	const D3D10_INPUT_ELEMENT_DESC scenelayout[] =
    {
        { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };
    D3D10_PASS_DESC PassDesc;
    shader->GetRenderScene()->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    V_RETURN( DXUTGetD3D10Device()->CreateInputLayout( scenelayout, sizeof(scenelayout)/sizeof(scenelayout[0]), PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pSDKSceneVertexLayout ) );

	return hr;
}

HRESULT InputLayout::SetupForOpticalFlowMeshInputLayout()
{
	HRESULT hr = S_OK;
	// Define our vertex data layout for post-transformed objects
	const D3D10_INPUT_ELEMENT_DESC OpticalFlowlayout[] =
    {
       
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },       
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,      16, D3D10_INPUT_PER_VERTEX_DATA, 0 },		
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,       28, D3D10_INPUT_PER_VERTEX_DATA, 0 },		
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,     36, D3D10_INPUT_PER_VERTEX_DATA, 0 },		
		{ "POSITION", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1,      16, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 1,       28, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1,     36, D3D10_INPUT_PER_VERTEX_DATA, 0 },

    };
	D3D10_PASS_DESC PassDesc;
    int numElements = sizeof( OpticalFlowlayout ) / sizeof( OpticalFlowlayout[0] );
    shader->GetComputeOpticalFlow1()->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    V_RETURN( DXUTGetD3D10Device()->CreateInputLayout( OpticalFlowlayout, numElements, PassDesc.pIAInputSignature,
                                             PassDesc.IAInputSignatureSize, &g_pOpticalFlowVertexLayout ) );
	return hr;
}

HRESULT InputLayout::SetupTransformedMeshInputLayout()
{
	HRESULT hr = S_OK;
	// Define our vertex data layout for post-transformed objects
	const D3D10_INPUT_ELEMENT_DESC transformedlayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,      16, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,       28, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,     36, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };
	D3D10_PASS_DESC PassDesc;
    int numElements = sizeof( transformedlayout ) / sizeof( transformedlayout[0] );
    shader->GetRenderPostTransformed()->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    V_RETURN( DXUTGetD3D10Device()->CreateInputLayout( transformedlayout, numElements, PassDesc.pIAInputSignature,
                                             PassDesc.IAInputSignatureSize, &g_pTransformedVertexLayout ) );
	return hr;
}

HRESULT InputLayout::SetupSkinnedMeshInputLayout()
{
	HRESULT hr = S_OK;
	// Define our vertex data layout for skinned objects
    const D3D10_INPUT_ELEMENT_DESC skinnedlayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,    0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "WEIGHTS", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0,      12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "BONES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0,         16, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,      20, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,       32, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,     40, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };
	int numElements = sizeof( skinnedlayout ) / sizeof( skinnedlayout[0] );
    D3D10_PASS_DESC PassDesc;
    shader->GetRenderConstantBuffer()->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    V_RETURN( DXUTGetD3D10Device()->CreateInputLayout( skinnedlayout, numElements, PassDesc.pIAInputSignature,
                                             PassDesc.IAInputSignatureSize, &g_pSkinnedVertexLayout ) );    
	return hr;
}

void InputLayout::EnableQuadVertexLayout()
{
	DXUTGetD3D10Device()->IASetInputLayout( g_pQuadVertexLayout );
	UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
	DXUTGetD3D10Device()->IASetVertexBuffers( 0, 1, &g_pQuadVertexBuffer, &stride, &offset );
	DXUTGetD3D10Device()->IASetIndexBuffer( g_pQuadIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );
	DXUTGetD3D10Device()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

