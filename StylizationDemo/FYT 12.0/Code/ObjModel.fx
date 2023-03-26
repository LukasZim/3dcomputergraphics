//--------------------------------------------------------------------------------------
// File: MeshFromOBJ10.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
float3 g_vMaterialAmbient   = float3( 0.2f, 0.2f, 0.2f );   // Material's ambient color
float3 g_vMaterialDiffuse   = float3( 0.8f, 0.8f, 0.8f );   // Material's diffuse color
float3 g_vMaterialSpecular  = float3( 1.0f, 1.0f, 1.0f );   // Material's specular color
float  g_fMaterialAlpha     = 1.0f;
int    g_nMaterialShininess = 32;

float3 g_vLightColor        = float3( 1.0f, 1.0f, 1.0f );     // Light color
float3 g_vLightPosition     = float3( 50.0f, 10.0f, 0.0f );   // Light position
float3 g_vCameraPosition ;

Texture2D g_MeshTexture;        // Color texture for mesh

matrix g_mWorld ;               // World matrix
matrix g_mWorldViewProjection ; // World * View * Projection matrix
matrix g_mPreWorldViewProjection;

//--------------------------------------------------------------------------------------
// Texture sampler
//--------------------------------------------------------------------------------------
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//--------------------------------------------------------------------------------------
// Vertex shader input structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 vPosObject   : POSITION;
    float3 vNormalObject: NORMAL;
    float2 vTexUV       : TEXCOORD0;
};

struct PSSDKSceneIn
{
    float4 Pos			  : SV_POSITION;
    float2 Tex			  : TEXCOORD0;
    float3 vPos			  : TEXCOORD2;
};

//--------------------------------------------------------------------------------------
// Pixel shader input structure
//--------------------------------------------------------------------------------------
struct PS_INPUT
{
    float4 vPosProj : SV_POSITION;
    float4 vColor   : COLOR0;
    float2 vTexUV   : TEXCOORD0;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
	DepthFunc = LESS_EQUAL;
};

BlendState NoBlending
{
	AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
};

PS_INPUT VS( VS_INPUT input, uniform bool bSpecular )
{
    PS_INPUT output;
    
    // Transform the position into world space for lighting, and projected space
    // for display
    float4 vPosWorld = mul( float4(input.vPosObject,1), g_mWorld );
    output.vPosProj = mul( float4(input.vPosObject,1), g_mWorldViewProjection );

    // Transform the normal into world space for lighting
    float3 vNormalWorld = mul( input.vNormalObject, (float3x3)g_mWorld );

    // Pass the texture coordinate
    output.vTexUV = input.vTexUV;

    // Compute the light vector
    float3 vLight = normalize( g_vLightPosition - vPosWorld.xyz );

    // Compute the ambient and diffuse components of illumination
    output.vColor.rgb = g_vLightColor * g_vMaterialAmbient;
    output.vColor.rgb += g_vLightColor * g_vMaterialDiffuse * saturate( dot( vLight, vNormalWorld ) );

    // If enabled, calculate the specular term
    if ( bSpecular )
    {
        float3 vCamera = normalize( vPosWorld.xyz - g_vCameraPosition );
        float3 vReflection = reflect( vLight, vNormalWorld );
        float  fPhoneValue = saturate( dot(vReflection, vCamera) );

        output.vColor.rgb += g_vMaterialSpecular * pow( fPhoneValue, g_nMaterialShininess );
    }

    output.vColor.a = g_fMaterialAlpha;

    return output;
}

PSSDKSceneIn VSScenemain(VS_INPUT input)
{
    PSSDKSceneIn output;
    
    output.vPos = mul( float4(input.vPosObject,1), g_mWorld ); //get the 3D location in World space
    output.Pos = mul( float4(input.vPosObject,1), g_mWorldViewProjection ); //get the 3D location in projection space
    output.Tex = input.vTexUV;
    
    return output;
}

float4 PS( PS_INPUT input, uniform bool bTexture ) : SV_Target
{
    float4 output = input.vColor;

    // Sample and modulate the texture
    if ( bTexture )
    {
        output.rgb *= g_MeshTexture.Sample( samLinear, input.vTexUV );
    }

    return output;
}

float2 PSComputeOpticalFlow(PSSDKSceneIn input) : SV_Target
{
	//The screen space position in previous frame
	float4 PrePos = mul( float4(input.vPos,1), g_mPreWorldViewProjection ); 
	//Prospective projection, do homonization
	float4 HPrePos = PrePos / PrePos.a; //screen space coordinate from -1 to 1
	
	//The screen space position in the current frame
	float4 Pos = mul( float4(input.vPos,1), g_mWorldViewProjection );
	//Prospective projection
	float4 HPos = Pos / Pos.a;
	
	//return float4(abs(HPrePos.r - HPos.r), abs(HPrePos.g - HPos.g),0,1);
	//calculate the optical flow value, and scale them from the range (-2, 2) to the range (0,1)
	float2 opticalFlow = ((HPos.rg - HPrePos.rg) * 0.5 + float2(1,1)) * 0.5;
	return opticalFlow;
}


//other pixel shaders 

technique10 RenderSceneOpticalFlow
{
	pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VSScenemain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSComputeOpticalFlow() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
    } 
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 NoSpecular
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS(false) ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(false) ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
    }
}
technique10 Specular
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS(true) ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(false) ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
    }
}
technique10 TexturedNoSpecular
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS(false) ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(true) ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
    }
}
technique10 TexturedSpecular
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS(true) ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(true) ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
    }
}

