//#include "Common.fxh"

//for sdkmesh rendering
Texture2D g_txDiffuse0;
Texture2D g_txNormal0;
       
BlendState NoBlending
{
	AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
	DepthFunc = LESS_EQUAL;
};

cbuffer cbChange{
	matrix		g_mWorld0 ;               // World matrix
	matrix		g_mWorldViewProjection ; // World * View * Projection matrix , for rendering models
	matrix		g_mPreWorldViewProjection;
	float4		g_vEyePt0; //for sdk model
};

cbuffer cbImmutable{
	float4 g_directional1 = float4( 0.992, 1.0, 0.880, 0.0 );
    float4 g_directional2 = float4( 0.595, 0.6, 0.528, 0.0 );
	float4 g_ambient0 = float4(0.525,0.474,0.474,0.0);
	float3 g_vLightPosition     = float3( 50.0f, 10.0f, 0.0f );   // Light position
};

DepthStencilState DisableDepthTest
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = ALWAYS;
};

SamplerState g_samLinearWrap
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VSSDKSceneIn
{
    float3 Pos            : POSITION; //3D position in local space
    float3 Norm           : NORMAL;  
    float2 Tex            : TEXCOORD;
    float3 Tan            : TANGENT;
};

struct PSSDKSceneIn
{
    float4 Pos			  : SV_POSITION;
    float3 Norm			  : NORMAL;
    float2 Tex			  : TEXCOORD0;
    float3 Tan			  : TEXCOORD1;
    float3 vPos			  : TEXCOORD2;
};

float4 g_vWorldLightDir1;
float4 g_vWorldLightDir2;

PSSDKSceneIn VSScenemain(VSSDKSceneIn input)
{
    PSSDKSceneIn output;
    
    output.vPos = mul( float4(input.Pos,1), g_mWorld0 ); //get the 3D location in World space
    output.Pos = mul( float4(input.Pos,1), g_mWorldViewProjection ); //get the 3D location in projection space
    output.Norm = normalize( mul( input.Norm, (float3x3)g_mWorld0 ) );
    output.Tan = normalize( mul( input.Tan, (float3x3)g_mWorld0 ) );
    output.Tex = input.Tex;
    
    return output;
}

//the sky is only a sphere with texture mapped onto it
float4 PSSkymain(PSSDKSceneIn input) : SV_Target
{   
    return g_txDiffuse0.Sample( g_samLinearWrap, input.Tex );
}

//do diffuse and specular lighting on the scene models
float4 PSScenemain(PSSDKSceneIn input) : SV_Target
{   
    float4 diffuse = g_txDiffuse0.Sample( g_samLinearWrap, input.Tex );
    float specMask = diffuse.a;
    float3 norm = g_txNormal0.Sample( g_samLinearWrap, input.Tex );
    norm *= 2;
    norm -= float3(1,1,1);
    
    float3 binorm = normalize( cross( input.Norm, input.Tan ) );
    float3x3 BTNMatrix = float3x3( binorm, input.Tan, input.Norm );
    norm = normalize(mul( norm, BTNMatrix ));
    
    // diffuse lighting
    float4 lighting = saturate( dot( norm, g_vWorldLightDir1.xyz ) )*g_directional1;
    lighting += saturate( dot( norm, g_vWorldLightDir2.xyz ) )*g_directional2;
    lighting += g_ambient0;
    
    // Calculate specular power
	//use the eye position in world space substract the point position in world space to get the view direction
    float3 viewDir = normalize( g_vEyePt0 - input.vPos );  
    float3 halfAngle = normalize( viewDir + g_vWorldLightDir1.xyz );
    float4 specPower1 = pow( saturate(dot( halfAngle, norm )), 32 ) * g_directional1;
    
    halfAngle = normalize( viewDir + g_vWorldLightDir2.xyz );
    float4 specPower2 = pow( saturate(dot( halfAngle, norm )), 32 ) * g_directional2;
    
    return lighting*diffuse + (specPower1+specPower2)*specMask;
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

//the same with RenderSceneOpticalFlow, except the depth stencil state
technique10 RenderSkyOpticalFlow
{
	pass p0
	{
		SetVertexShader( CompileShader( vs_4_0, VSScenemain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSComputeOpticalFlow() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepthTest, 0 );
	}
}


technique10 RenderSDKScene
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VSScenemain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScenemain() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
    }  
}

technique10 RenderSky
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VSScenemain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSSkymain() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepthTest, 0 );
    }  
}




