//#include "Common.fxh"

//*************************************************************************************//
//								Frame buffer
//*************************************************************************************//
Texture2D   ColorBuffer0; //content of current frame
Texture2D   ColorBuffer1; //content of previous frame
Texture2D   ColorBuffer2; //content of pre pre frame
Texture2D   ColorBuffer3; //content of pre pre pre frame
Texture2D   FlowBuffer0; //content of current frame
Texture2D   FlowBuffer1; //content of previous frame
Texture2D   PreviousVector0; //content of pre pre frame
Texture2D   StrokeBuffer;
Texture2D   RenderInfo;
Texture2D   PreviousVector;
Texture2D	TempImage;
Texture2D	TempImage1;
Texture2D	MipRendering;
Texture2D	ModelMatte;

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

//*************************************************************************************//
//								Textures
//*************************************************************************************//
//never change
Texture2D   StrokeTex;
Texture2D   AlphaMask;
Texture2D	PaperTex;
Texture2D   Probability;
Texture2D	OpticalFlowTexture;

//*************************************************************************************//
//								Buffers
//*************************************************************************************//
//never change
cbuffer cbImmutable
{
    float2 g_positions[4] =
    {
        float2( -1, 1),
        float2( 1, 1),
        float2( -1, -1),
        float2( 1, -1),
    };
	float2 g_texcoords[4] = 
    { 
        float2(0,1), 
        float2(1,1),
        float2(0,0),
        float2(1,0),
    };
	float PI = 3.1416;

	float ScaleMap = 0.32; //0.3125

	float4x4	g_mWorldViewProj0; //for rendering image frame
	float4x4	g_mInvView;	//for rendering image frame
};

//constants that may change every frame
cbuffer cbChange{
	float       gradientThreshold;
	float       angleThreshold;
	float		strokeLength;
	float       strokeWidth;
	float       Lambda;
	int			renderStyle;
	int			index;
	bool        OnRealGradientAngle;
	bool		OnClipping;

	float		ProbabilityLargeThreshold;
	float		ProbabilityMediumThreshold;
	float		ProbabilitySmallThreshold;
	float		GradientLargeThreshold;
	float		GradientMediumThreshold;
	//float		GradientSmallThreshold;
	float		SizeMediumThreshold;
	float		SizeSmallThreshold;
	float		ContrastThreshold;
	float		FadeOutSpeedThreshold;
	float		FadeInSpeedThreshold;
	float		AlphaLargeThreshold;
	float		AlphaMediumThreshold;
	float		AlphaSmallThreshold;
	int			MipMapLevel;
	int			StrokeTexLength;
	int			StrokeTexWidth;	

	float		MouseX;
	float		MouseY;
	float		focusf;
	float		focusr1;
	float		focusr2;
	bool		OnFocusPoint;
};

//when user adjust the parameters, they will change
cbuffer cbSomeChange{
	float       deltaY;
	float       deltaX;
	int			windowWidth;
	int			windowHeight;
	int			numParticleX;
	int			numParticleY;	
};

cbuffer cbAlwaysChange{
	float CurTime;
};

//*************************************************************************************//
//								Input layout
//*************************************************************************************//
//used as input to vertex and geometry shader
//storing the information about particle position, color and other properties
struct VSParticle
{
    float2 pos              : POSITION; //position of particle in screen space 
	float4 color			: COLOR;
	float4 info				: INFO;
};

//used as input to pixel shader to render the actual sprites
struct PSSceneIn
{
    float4 pos : SV_Position; 
    float2 tex : TEXTURE0;
	float2 CGtex : TEXTURE1;
	float2 Gtex : TEXTURE2;
    float4 color : COLOR0;	
	float  type : TYPE;
};

//used as input to vertex shader for passes that do image processing
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

//used as input to pixel shader for passes that do image processing
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

struct PSBLUR3x3_INPUT
{
    float4 Pos   : SV_POSITION;
    float2 Tex   : TEXCOORD0;
	float2 TexL  : TEXCOORD1;
	float2 TexR  : TEXCOORD2;
	float2 TexU  : TEXCOORD3;
	float2 TexB  : TEXCOORD4;
	float2 TexUL : TEXCOORD5;
	float2 TexUR : TEXCOORD6;
	float2 TexBL : TEXCOORD7;
	float2 TexBR : TEXCOORD8;
};

struct PSBLUR5x5_INPUT
{
	float4 Pos   : SV_POSITION;
    float2 Tex11 : TEXCOORD0;
	float2 Tex12 : TEXCOORD1;
	float2 Tex13 : TEXCOORD2;
	float2 Tex14 : TEXCOORD3;
	float2 Tex15 : TEXCOORD4;
	float2 Tex21 : TEXCOORD5;
	float2 Tex22 : TEXCOORD6;
	float2 Tex23 : TEXCOORD7;
	float2 Tex24 : TEXCOORD8;
	float2 Tex25 : TEXCOORD9;
	float2 Tex31 : TEXCOORD10;
	float2 Tex32 : TEXCOORD11;
	float2 Tex33 : TEXCOORD12;
	float2 Tex34 : TEXCOORD13;
	float2 Tex35 : TEXCOORD14;
	float2 Tex41 : TEXCOORD15;
	float2 Tex42 : TEXCOORD16;
	float2 Tex43 : TEXCOORD17;
	float2 Tex44 : TEXCOORD18;
	float2 Tex45 : TEXCOORD19;
	float2 Tex51 : TEXCOORD20;
	float2 Tex52 : TEXCOORD21;
	float2 Tex53 : TEXCOORD22;
	float2 Tex54 : TEXCOORD23;
	float2 Tex55 : TEXCOORD24;
};

//multiple render target, currently not used
struct PTEXTURE_OUTPUT
{
	float4 color0 : SV_TARGET0;
	float4 color1 : SV_TARGET1;
	float4 color2 : SV_TARGET2;
};

//*************************************************************************************//
//								Sampler States
//*************************************************************************************//

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

//sampler states
SamplerState samPointWrap
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

//*************************************************************************************//
//								Blend States
//*************************************************************************************//

//this blending will let the alpha get smaller and smaller
BlendState CompositeBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = SRC_ALPHA;
    DestBlendAlpha = INV_SRC_ALPHA;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState AdditiveBlending
{
	AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = ONE;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ONE;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

//*************************************************************************************//
//								Depth Stencil States
//*************************************************************************************//

DepthStencilState DisableDepth  
{ 
	DepthEnable = FALSE; 
};

//*************************************************************************************//
//Helper Functions
//*************************************************************************************//

float4 RGB_to_HSV (float4 color)
{
	float r, g, b, delta;
	float colorMax, colorMin;
	float h=0, s=0, v=0;
	float4 hsv=0;
	r = color[0];
	g = color[1];
	b = color[2];
	colorMax = max (r,g);
	colorMax = max (colorMax,b);
	colorMin = min (r,g);
	colorMin = min (colorMin,b);
	v = colorMax; // this is value
	if (colorMax != 0)
	{
		s = (colorMax - colorMin) / colorMax;
	}
	if (s != 0) // if not achromatic
	{
		delta = colorMax - colorMin;
		if (r == colorMax)
		{
			h = (g-b)/delta;
		}
		else if (g == colorMax)
		{
			h = 2.0 + (b-r) / delta;
		}
		else // b is max
		{
			h = 4.0 + (r-g)/delta;
		}
		h *= 60;
		if( h < 0)
		{
			h +=360;
		}
		hsv[0] = h / 360.0; // moving h to be between 0 and 1.
		hsv[1] = s;
		hsv[2] = v;
	}
	return hsv;
}


//*************************************************************************************//
//								Vertex shaders
//*************************************************************************************//
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos      = input.Pos;
    output.Tex      = input.Tex;
    
    return output;
}

PSBLUR3x3_INPUT VSBlur3x3( VS_INPUT input )
{
	PSBLUR3x3_INPUT output = (PSBLUR3x3_INPUT)0;
    output.Pos      = input.Pos;
    output.Tex      = input.Tex;
	output.TexL		= input.Tex - float2( deltaX,   0);
	output.TexR		= input.Tex - float2(-deltaX,   0);
	output.TexU		= input.Tex - float2( 0,        deltaY);
	output.TexB		= input.Tex - float2( 0,       -deltaY);
	output.TexUR    = input.Tex - float2(-deltaX,   deltaY);
	output.TexUL	= input.Tex - float2( deltaX,   deltaY);
	output.TexBL	= input.Tex - float2( deltaX,  -deltaY);
	output.TexBR	= input.Tex - float2(-deltaX,  -deltaY);

	return output;
}

PSBLUR5x5_INPUT VSBlur5x5( VS_INPUT input )
{
	PSBLUR5x5_INPUT output = (PSBLUR5x5_INPUT)0;
    output.Pos      = input.Pos;
	output.Tex11		= input.Tex - float2( deltaX * 2, deltaY * 2);
	output.Tex12		= input.Tex - float2( deltaX,     deltaY * 2);
	output.Tex13		= input.Tex - float2( 0,          deltaY * 2);
	output.Tex14		= input.Tex - float2(-deltaX,     deltaY * 2);
	output.Tex15		= input.Tex - float2(-deltaX * 2, deltaY * 2);
	output.Tex21		= input.Tex - float2( deltaX * 2, deltaY);
	output.Tex22		= input.Tex - float2( deltaX,     deltaY);
	output.Tex23		= input.Tex - float2( 0,          deltaY);
	output.Tex24		= input.Tex - float2(-deltaX,     deltaY);
	output.Tex25		= input.Tex - float2(-deltaX * 2, deltaY);
	output.Tex31		= input.Tex - float2( deltaX * 2, 0);
	output.Tex32		= input.Tex - float2( deltaX,     0);
	output.Tex33		= input.Tex;
	output.Tex34		= input.Tex - float2(-deltaX,     0);
	output.Tex35		= input.Tex - float2(-deltaX * 2, 0);
	output.Tex41		= input.Tex - float2( deltaX * 2, -deltaY);
	output.Tex42		= input.Tex - float2( deltaX,     -deltaY);
	output.Tex43		= input.Tex - float2( 0,          -deltaY);
	output.Tex44		= input.Tex - float2(-deltaX,     -deltaY);
	output.Tex45		= input.Tex - float2(-deltaX * 2, -deltaY);
	output.Tex51		= input.Tex - float2( deltaX * 2, -deltaY * 2);
	output.Tex52		= input.Tex - float2( deltaX,     -deltaY * 2);
	output.Tex53		= input.Tex - float2( 0,          -deltaY * 2);
	output.Tex54		= input.Tex - float2(-deltaX,     -deltaY * 2);
	output.Tex55		= input.Tex - float2(-deltaX * 2, -deltaY * 2);

	return output;
}


//For Generating particles
VSParticle VSPassThroughParticle(VSParticle input)
{
	VSParticle output = (VSParticle) 0;
	output.pos = input.pos; //the position ranges from 0 to 1
	output.color = input.color;
	output.info = input.info;
    return output;
}

//*************************************************************************************//
//								Geometry shaders
//*************************************************************************************//

[maxvertexcount(4)]
void GSRenderOpticalFlow(point VSParticle input[1], inout TriangleStream<PSSceneIn> SpriteStream)
{
	PSSceneIn output = (PSSceneIn)0; 
	float2 index;
	index.x = (input[0].pos.x * numParticleX - numParticleX * 0.5) * windowWidth * ScaleMap / numParticleX;
	index.y = (input[0].pos.y * numParticleY - numParticleY * 0.5) * windowHeight * ScaleMap / numParticleY;

	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);
	float2 opticalFlowValue = FlowBuffer0.SampleLevel( samLinear, texcoord, 0 ).rg * 2 - float2(1, 1);
	float randomNumber = Probability.SampleLevel( samLinear, texcoord, 0);

	bool bDraw = true;
	float angle;
	if (abs(opticalFlowValue.x) < 0.0001 && abs(opticalFlowValue.y) > 0.0001) 
	{
		angle = PI * 0.5;
		if (opticalFlowValue.y <= 0)
			angle = -angle;		
	}
	else if (abs(opticalFlowValue.x) < 0.0001 && abs(opticalFlowValue.y) < 0.0001)
		bDraw = false;
	else
		angle = atan2(-opticalFlowValue.y, opticalFlowValue.x);
	float sine, cosine;
	sincos(angle, sine, cosine);
	float3 position;
	

	//output.color = float4(0,1,0,1);
	output.type = 1;
	output.CGtex = texcoord;

	float ofLength = clamp(length(opticalFlowValue), 0.3, 1);

	float randomNumber0 = Probability.SampleLevel( samLinear, texcoord - float2( 0 , deltaX * 2), 0);
	float randomNumber1 = Probability.SampleLevel( samLinear, texcoord - float2( deltaX * 2, 0), 0);
	float randomNumber2 = Probability.SampleLevel( samLinear, texcoord - float2( - deltaX * 2, 0), 0);
	output.color = float4(randomNumber0, randomNumber1, randomNumber2, 1);

	if (randomNumber < ProbabilityMediumThreshold * 0.7 && bDraw)
	{
		[unroll]for(int i=0; i<4; i++)
		{	
			float x = g_positions[i].r * 4 * ofLength;
			float y = g_positions[i].g * 0.3;
			if ( i == 0 || i == 2 )
				y = y * 3;		
			
					
			//orient the stroke according to the gradient angle
			position.r = x * cosine - y * sine;
			position.g = x * sine + y * cosine;
			position.b = 0;

			float2 pos = input[0].pos + float2(position.r * deltaX, position.g * deltaY);
			output.Gtex = float2(pos.r, 1 - pos.g);

			position = mul( position, (float3x3)g_mInvView ) + float3(index, 0); 
			output.pos = mul( float4(position,1.0), g_mWorldViewProj0 );		

			output.tex = g_texcoords[i];
			SpriteStream.Append(output);
			
		}	
		SpriteStream.RestartStrip();
	}
}

[maxvertexcount(4)]
void GSRenderParticlesOnlyModel(point VSParticle input[1], inout TriangleStream<PSSceneIn> SpriteStream)
{
    PSSceneIn output = (PSSceneIn)0;  	
	float2 StrokeSize = input[0].info.rg;
	float4 StrokeColor = input[0].color;
	float StrokeAngle = input[0].info.b;

	float2 index;
	index.x = (input[0].pos.x * numParticleX - numParticleX * 0.5) * windowWidth * ScaleMap / numParticleX;
	index.y = (input[0].pos.y * numParticleY - numParticleY * 0.5) * windowHeight * ScaleMap / numParticleY;
	float3 position;
	float angle = (StrokeAngle * 2 - 0.5) * PI;
	float sine, cosine;
	sincos(angle, sine, cosine);

	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);
	if (ModelMatte.SampleLevel( samLinear, texcoord, 0).r < 0.5)
		return;

	//float OldGrad = RenderInfo.SampleLevel( samPoint, texcoord, 0).b;
	float PLength = StrokeSize.r, NLength = StrokeSize.r;

	output.color = StrokeColor;
	output.CGtex = texcoord;
	output.type = 0;
	if (abs(strokeLength - input[0].info.r) < 0.01) //the stroke is large
		output.type = 1;
	else if (abs(strokeLength * SizeMediumThreshold - input[0].info.r) < 0.01) //the stroke is medium
		output.type = 0.5;
	else 
		output.type = 0;

	[unroll]for(int i=0; i<4; i++)
	{	
		float x;
		float y = g_positions[i].g * StrokeSize.g;
		if (g_positions[i].r > 0)
			x = g_positions[i].r * PLength;
		else
			x = g_positions[i].r * NLength;
				
		
		//orient the stroke according to the gradient angle
		position.r = x * cosine - y * sine;
		position.g = x * sine + y * cosine;
		position.b = 0;

		float2 pos = input[0].pos + float2(position.r * deltaX, position.g * deltaY);
		output.Gtex = float2(pos.r, 1 - pos.g);

		position = mul( position, (float3x3)g_mInvView ) + float3(index, 0); 
		output.pos = mul( float4(position,1.0), g_mWorldViewProj0 );

		output.tex = g_texcoords[i];
		SpriteStream.Append(output);
		
	}	
    SpriteStream.RestartStrip();	
}


[maxvertexcount(4)]
void GSRenderParticles(point VSParticle input[1], inout TriangleStream<PSSceneIn> SpriteStream)
{
    PSSceneIn output = (PSSceneIn)0;  	
	float2 StrokeSize = input[0].info.rg;
	float4 StrokeColor = input[0].color;
	float StrokeAngle = input[0].info.b;

	float2 index;
	index.x = (input[0].pos.x * numParticleX - numParticleX * 0.5) * windowWidth * ScaleMap / numParticleX; //160
	index.y = (input[0].pos.y * numParticleY - numParticleY * 0.5) * windowHeight * ScaleMap / numParticleY; //160
	float3 position;
	float angle = (StrokeAngle * 2 - 0.5) * PI;
	float sine, cosine;
	sincos(angle, sine, cosine);

	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);
	/*if (ModelMatte.SampleLevel( samLinear, texcoord, 0) < 0.5)
		return;*/

	//float OldGrad = RenderInfo.SampleLevel( samPoint, texcoord, 0).b;
	float PLength = StrokeSize.r, NLength = StrokeSize.r;

	output.color = StrokeColor;
	output.CGtex = texcoord;
	output.type = 0;
	if (abs(strokeLength - input[0].info.r) < 0.01) //the stroke is large
		output.type = 1;
	else if (abs(strokeLength * SizeMediumThreshold - input[0].info.r) < 0.01) //the stroke is medium
		output.type = 0.5;
	else 
		output.type = 0;
	
	
	[unroll]for(int i=0; i<4; i++)
	{	
		float x;
		float y = g_positions[i].g * StrokeSize.g;
		if (g_positions[i].r > 0)
			x = g_positions[i].r * PLength;
		else
			x = g_positions[i].r * NLength;
				
		
		//orient the stroke according to the gradient angle
		position.r = x * cosine - y * sine;
		position.g = x * sine + y * cosine;
		position.b = 0;

		float2 pos = input[0].pos + float2(position.r * deltaX, position.g * deltaY);
		output.Gtex = float2(pos.r, 1 - pos.g);

		position = mul( position, (float3x3)g_mInvView ) + float3(index, 0); 
		output.pos = mul( float4(position,1.0), g_mWorldViewProj0 );

		output.tex = g_texcoords[i];
		SpriteStream.Append(output);
		
	}	
    SpriteStream.RestartStrip();	
}


//[maxvertexcount(1)]
//void GSConstructParticlesToBuffers(point VSParticle input[1], inout PointStream<VSParticle> ParticleOutputStream[3])
//{	
//	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);
//	float4 RenderInformation = RenderInfo.SampleLevel( samPoint, texcoord, 0 );
//	float size = RenderInformation.r;
//	float StrokeAngle = RenderInformation.a;
//	float alpha = RenderInformation.g;
//	if (alpha > AlphaLargeThreshold - 0.1) //the alpha value indicate the existence of stroke
//	{
//		VSParticle output = (VSParticle) 0.0;
//		//float2 Displacement = RenderInfo.SampleLevel( samPoint, texcoord, 0 );
//		//input[0].pos += Displacement;
//		output.pos = input[0].pos;
//		output.color = float4(ColorBuffer2.SampleLevel( samPoint, texcoord, 0).rgb, alpha)  * ContrastThreshold - (ContrastThreshold - 1);			
//		output.info = float4(size * strokeLength, size * strokeWidth, StrokeAngle, 1);  //1 denotes existing stroke, 0 denotes the stroke to be deleted, 0.5 denotes newly constructed particle
//		ParticleOutputStream[0].Append(output);
//	}
//}


[maxvertexcount(1)]
void GSConstructParticlesSmall(point VSParticle input[1], inout PointStream<VSParticle> ParticleOutputStream)
{	
	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);
	float4 RenderInformation = RenderInfo.SampleLevel( samPoint, texcoord, 0 );
	float size = RenderInformation.r;
	float StrokeAngle = RenderInformation.a;
	float alpha = RenderInformation.g;
	if (abs(alpha - AlphaSmallThreshold) < 0.01) //the alpha value indicate the existence of stroke
	{
		VSParticle output = (VSParticle) 0.0;
		//float2 Displacement = RenderInfo.SampleLevel( samPoint, texcoord, 0 );
		//input[0].pos += Displacement;
		output.pos = input[0].pos;
		output.color = float4(ColorBuffer2.SampleLevel( samPoint, texcoord, 0).rgb, alpha)  * ContrastThreshold - (ContrastThreshold - 1);			
		output.info = float4(size * strokeLength, size * strokeWidth, StrokeAngle, 1);  //1 denotes existing stroke, 0 denotes the stroke to be deleted, 0.5 denotes newly constructed particle
		ParticleOutputStream.Append(output);
	}
}

[maxvertexcount(1)]
void GSConstructParticlesLarge(point VSParticle input[1], inout PointStream<VSParticle> ParticleOutputStream)
{	
	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);
	float4 RenderInformation = RenderInfo.SampleLevel( samPoint, texcoord, 0 );
	float size = RenderInformation.r;
	float StrokeAngle = RenderInformation.a;
	float alpha = RenderInformation.g;
	if (abs(alpha - AlphaLargeThreshold) < 0.01) 
	{
		VSParticle output = (VSParticle) 0.0;
		//float2 Displacement = RenderInfo.SampleLevel( samPoint, texcoord, 0 );
		//input[0].pos += Displacement;
		output.pos = input[0].pos;
		output.color = float4(ColorBuffer2.SampleLevel( samPoint, texcoord, 0).rgb, alpha)  * ContrastThreshold - (ContrastThreshold - 1);
			
		output.info = float4(size * strokeLength, size * strokeWidth, StrokeAngle, 1);  //1 denotes existing stroke, 0 denotes the stroke to be deleted, 0.5 denotes newly constructed particle
		ParticleOutputStream.Append(output);
	}
}

[maxvertexcount(1)]
void GSConstructParticlesMedium(point VSParticle input[1], inout PointStream<VSParticle> ParticleOutputStream)
{	
	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);
	float4 RenderInformation = RenderInfo.SampleLevel( samPoint, texcoord, 0 );
	float size = RenderInformation.r;
	float StrokeAngle = RenderInformation.a;
	float alpha = RenderInformation.g;
	if (abs(alpha - AlphaMediumThreshold) < 0.01) //the alpha value indicate the existence of stroke
	{
		VSParticle output = (VSParticle) 0.0;
		//float2 Displacement = RenderInfo.SampleLevel( samPoint, texcoord, 0 );
		//input[0].pos += Displacement;
		output.pos = input[0].pos;
		output.color = float4(ColorBuffer2.SampleLevel( samPoint, texcoord, 0).rgb, alpha)  * ContrastThreshold - (ContrastThreshold - 1);
			
		output.info = float4(size * strokeLength, size * strokeWidth, StrokeAngle, 1);  //1 denotes existing stroke, 0 denotes the stroke to be deleted, 0.5 denotes newly constructed particle
		ParticleOutputStream.Append(output);
	}
}


[maxvertexcount(1)]
void GSAppendParticlesLargeForVideo(point VSParticle input[1], inout PointStream<VSParticle> ParticleOutputStream)
{	
	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);
	float4 RenderInformation = RenderInfo.SampleLevel( samPoint, texcoord, 0 );
	float size = RenderInformation.r;
	float StrokeAngle = RenderInformation.a;
	//float alpha = RenderInformation.g;

	if (size > 0.9) //large stroke
	//if (abs(alpha - AlphaLargeThreshold) < 0.01) //large stroke
	{
		float Prob = Probability.SampleLevel( samPointWrap, texcoord + float2(CurTime * 0.071, CurTime * 0.041), 0);		
		float Prob1 = Probability.SampleLevel( samPointWrap, texcoord + float2(CurTime * 0.082, CurTime *0.03), 0);

		VSParticle output = (VSParticle) 0.0;
		//Prob is from 0 to 1, jitter within two pixels width
		
		//jitter the position a little bit in order to fill in the blank, do not insert particle at exactly the same location each time
		output.pos.x = input[0].pos.x + Prob * deltaX * 3; 
		output.pos.y = input[0].pos.y + Prob1 * deltaY * 3; 

		float3 c1 = ColorBuffer1.SampleLevel( samPoint, texcoord, 0).rgb;
		float3 c2 = ColorBuffer2.SampleLevel( samPoint, texcoord, 0).rgb;
		float3 c3 = ColorBuffer3.SampleLevel( samPoint, texcoord, 0).rgb;		

		//average the color a little bit over color values of the previous frames
		//output.color.rgb = (c1 * 0.2 + c2 * 0.6 + c3 * 0.2) * ContrastThreshold - (ContrastThreshold - 1);
		output.color.rgb = c2 * ContrastThreshold - (ContrastThreshold - 1);	
		output.color.a = AlphaLargeThreshold * 0.5;
		//output.color.rgb = float3(0,0,1); //testing
		
		output.info = float4(size * strokeLength, size * strokeWidth, StrokeAngle, 0.5);  //1 denotes existing stroke, 0 denotes the stroke to be deleted, 0.5 denotes newly constructed particle
		
		float Emptyness = TempImage.SampleLevel( samPoint, texcoord, 0 );
		
		//when there are absolute emptiness, definitely add a stroke here
			if (Emptyness < ProbabilityLargeThreshold * 0.54) //0.07
			{
				if (texcoord.r < 0.9 && texcoord.g < 0.9 && texcoord.r > 0.1 && texcoord.g > 0.1)
				{
					//output.color.rgb = float3(0,0,1);
					//if(Prob < ProbabilityLargeThreshold * 1.5)
						//output.color.rgb = float3(0,0,1); //blue
						ParticleOutputStream.Append(output);
				}
			}
			else if (Emptyness < ProbabilityLargeThreshold * 1.08) //when it is not really empty, add a new stroke probablisitcally
			{
				
				//output.color.rgb = float3(0,1,0);
				//output.color.a = AlphaLargeThreshold - 0.1; //float4(0,0,1,1);
				//output.color.rgb = float3(0,0,1);

				//if it is at the image boundary
				if (texcoord.r > 0.1 && texcoord.r < 0.9 && texcoord.g > 0.1 && texcoord.g < 0.9)
				{
					if(Prob < ProbabilityLargeThreshold )
						ParticleOutputStream.Append(output);				
				}
				else
				{
					//if not very empty, then add new strokes conditionally
					if(Prob < ProbabilityLargeThreshold * 0.3)
						ParticleOutputStream.Append(output);	
				}			
			}
		//}
	}	
}

[maxvertexcount(1)]
void GSAppendParticlesLarge(point VSParticle input[1], inout PointStream<VSParticle> ParticleOutputStream)
{	
	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);
	float4 RenderInformation = RenderInfo.SampleLevel( samPoint, texcoord, 0 );
	float size = RenderInformation.r;
	float StrokeAngle = RenderInformation.a;
	//float alpha = RenderInformation.g;

	if (size > 0.9) //large stroke
	{
		float Prob = Probability.SampleLevel( samPointWrap, texcoord + float2(CurTime * 0.071, CurTime * 0.041), 0);		
		float Prob1 = Probability.SampleLevel( samPointWrap, texcoord + float2(CurTime * 0.082, CurTime *0.03), 0);

		VSParticle output = (VSParticle) 0.0;
		//Prob is from 0 to 1, jitter within two pixels width
		
		//jitter the position a little bit in order to fill in the blank, do not insert particle at exactly the same location each time
		output.pos.x = input[0].pos.x + Prob * deltaX * 3; 
		output.pos.y = input[0].pos.y + Prob1 * deltaY * 3; 
		//output.pos = input[0].pos;

		float3 c1 = ColorBuffer1.SampleLevel( samPoint, texcoord, 0).rgb;
		float3 c2 = ColorBuffer2.SampleLevel( samPoint, texcoord, 0).rgb;
		float3 c3 = ColorBuffer3.SampleLevel( samPoint, texcoord, 0).rgb;		

		//average the color a little bit over color values of the previous frames
		//output.color.rgb = (c1 * 0.2 + c2 * 0.6 + c3 * 0.2) * ContrastThreshold - (ContrastThreshold - 1);
		output.color.rgb = c2 * ContrastThreshold - (ContrastThreshold - 1);	
		output.color.a = AlphaLargeThreshold * 0.5;
		//output.color.rgb = float3(0,0,1); //testing
		
		output.info = float4(size * strokeLength, size * strokeWidth, StrokeAngle, 0.5);  //1 denotes existing stroke, 0 denotes the stroke to be deleted, 0.5 denotes newly constructed particle
		
		float Emptyness = TempImage.SampleLevel( samPoint, texcoord, 0 );
		
		//when there are absolute emptiness, definitely add a stroke here
			if (Emptyness < ProbabilityLargeThreshold * 0.38)
			{
				//if (texcoord.r < 0.95 && texcoord.g < 0.95 && texcoord.r > 0.15 && texcoord.g > 0.15)
				//{
				//output.color.rgb = float3(0,0,1);
				if(Prob < ProbabilityLargeThreshold * 2) //2
				{
					//output.color.rgb = float3(0,1,0); //blue
					ParticleOutputStream.Append(output);
				}
			}
			else if (Emptyness < ProbabilityLargeThreshold) //when it is not really empty, add a new stroke probablisitcally
			{
				
				//output.color.rgb = float3(1,0,0);
				
				//if not very empty, then add new strokes conditionally
				if(Prob < ProbabilityMediumThreshold * 0.6) // ProbabilityMediumThreshold
					ParticleOutputStream.Append(output);	
						
			}
		//}
	}	
}

[maxvertexcount(1)]
void GSAppendParticlesMedium(point VSParticle input[1], inout PointStream<VSParticle> ParticleOutputStream)
{	
	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);
	float4 RenderInformation = RenderInfo.SampleLevel( samPoint, texcoord, 0 );
	float size = RenderInformation.r;
	float StrokeAngle = RenderInformation.a;
	//float alpha = RenderInformation.g;
	
	if (abs(size - SizeMediumThreshold) < 0.1)
	//if (abs(alpha - AlphaMediumThreshold) < 0.01)
	{
		float Prob = Probability.SampleLevel( samPointWrap, texcoord + float2(CurTime * 0.056, CurTime * 0.036) , 0);
		float Prob1 = Probability.SampleLevel( samPointWrap, texcoord + float2(CurTime * 0.082, CurTime *0.03), 0);

		VSParticle output = (VSParticle) 0.0;
		output.pos.x = input[0].pos.x + Prob * deltaX * 3; 
		output.pos.y = input[0].pos.y + Prob1 * deltaY * 3; 

		float3 c1 = ColorBuffer1.SampleLevel( samPoint, texcoord, 0).rgb;
		float3 c2 = ColorBuffer2.SampleLevel( samPoint, texcoord, 0).rgb;
		float3 c3 = ColorBuffer3.SampleLevel( samPoint, texcoord, 0).rgb;		

		//average the color a little bit over color values of the previous frames
		output.color.rgb = c2 * ContrastThreshold - (ContrastThreshold - 1);
		output.color.a = AlphaMediumThreshold * 0.6;
		output.info = float4( size * strokeLength, size * strokeWidth, StrokeAngle, 0.5);  //1 denotes existing stroke, 0 denotes the stroke to be deleted, 0.5 denotes newly constructed particle
		
		//if(Prob < ProbabilityMediumThreshold * 0.1)
		//{
			//output.color.rgb = float3(1,0,0);
			//ParticleOutputStream.Append(output);
		//}
		//else{
			float Emptyness = TempImage.SampleLevel( samPoint, texcoord, 0 );
			if (Emptyness < ProbabilityMediumThreshold * 0.2) //0.07
			{
				//output.color.rgb = float3(1,0,0);
				ParticleOutputStream.Append(output);
			}			
			if (Emptyness < ProbabilityMediumThreshold * 0.25) //0.13
			{		
				//output.color.rgb = float3(0,0,1);
				if(Prob < ProbabilityMediumThreshold * 0.5)
					ParticleOutputStream.Append(output);			
			}
		//}
	}
}

[maxvertexcount(1)]
void GSAppendParticlesSmall(point VSParticle input[1], inout PointStream<VSParticle> ParticleOutputStream)
{	
	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);
	float4 RenderInformation = RenderInfo.SampleLevel( samPoint, texcoord, 0 );
	float size = RenderInformation.r;
	float StrokeAngle = RenderInformation.a;
	//float alpha = RenderInformation.g;
	
	if (abs(size - SizeSmallThreshold) < 0.1)
	//if (abs(alpha - AlphaSmallThreshold) < 0.01) //small stroke
	{
		//float StrokeAngle = RenderInfo.SampleLevel( samPoint, texcoord, 0 ).a;
		float Prob = Probability.SampleLevel( samPointWrap, texcoord + float2(CurTime * 0.051, CurTime * 0.031) , 0);
		float Prob1 = Probability.SampleLevel( samPointWrap, texcoord + float2(CurTime * 0.082, CurTime *0.03), 0);

		VSParticle output = (VSParticle) 0.0;
		output.pos.x = input[0].pos.x + Prob * deltaX * 3; 
		output.pos.y = input[0].pos.y + Prob1 * deltaY * 3; 

		float3 c1 = ColorBuffer1.SampleLevel( samPoint, texcoord, 0).rgb;
		float3 c2 = ColorBuffer2.SampleLevel( samPoint, texcoord, 0).rgb;
		float3 c3 = ColorBuffer3.SampleLevel( samPoint, texcoord, 0).rgb;		

		//average the color a little bit over color values of the previous frames
		output.color.rgb = c2 * ContrastThreshold - (ContrastThreshold - 1);
		output.color.a = AlphaSmallThreshold * 0.7;
		output.info = float4( size * strokeLength, size * strokeWidth, StrokeAngle, 0.5);  //1 denotes existing stroke, 0 denotes the stroke to be deleted, 0.5 denotes newly constructed particle
		
		if (Prob < ProbabilitySmallThreshold * 0.03) //the alpha value indicate the existence of stroke	
		{
			//output.color.rgb = float3(0,1,0);
			ParticleOutputStream.Append(output);
		}
		else{
			//float alphaValue = MipRendering.SampleLevel( samLinear, texcoord, MipMapLevel ).a;
			float Emptyness = TempImage.SampleLevel( samPoint, texcoord, 0 );
			//float Emptyness = TempImage.SampleLevel( samPoint, texcoord, 0 ).a;
			if (Emptyness < ProbabilitySmallThreshold * 0.12) //0.04
			{
				//output.color.rgb = float3(1,0,0);
				ParticleOutputStream.Append(output);
			}
			if (Emptyness < ProbabilitySmallThreshold * 0.2) //0.1
			{
				//output.color.rgb = float3(0,0,1);
				//output.color.a = 0.05; //float4(0,0,1,1);
				//output.color.rgb = float3(0,1,0);
				//if (texcoord.r > 0.1 && texcoord.r < 0.9 && texcoord.g > 0.1 && texcoord.g < 0.9)
				//{
					//output.color.rgb = float3(1,0,0);
					//if(Prob < ProbabilitySmallThreshold * 1.3)
						//ParticleOutputStream.Append(output);				
				//}
				//else
				//{
					if(Prob < ProbabilitySmallThreshold * 0.2) //0.05
						ParticleOutputStream.Append(output);	
				//}			
			}
		}
	}
}


[maxvertexcount(1)]
void GSDeleteParticlesLarge(point VSParticle input[1], inout PointStream<VSParticle> ParticleOutputStream)
{	
	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);
	float Prob = Probability.SampleLevel( samPointWrap, texcoord + float2(CurTime * 0.06, CurTime * 0.04) , 0);

	float overlap = TempImage.SampleLevel( samLinear, texcoord, 0);


	//if there are many strokes overlapped on each other, append the paricle conditionally
	if (overlap >= ProbabilityLargeThreshold * 3.07) //there was && Prob < (1 - ProbabilityLargeThreshold)
	{
		//in highly overlapped region, delete the stroke conditionally
		if (Prob < 1 - overlap * (1 - ProbabilityLargeThreshold) * 0.6 )
		{
			VSParticle output = input[0];		
			//output.color.rgb = float3(1,0,0);
			ParticleOutputStream.Append(output);
		}
	
		//VSParticle output = input[0];		
		//output.color.rgb = float3(1,0,0);
		//ParticleOutputStream.Append(output);
	}
	else
		ParticleOutputStream.Append(input[0]);
}

[maxvertexcount(1)]
void GSDeleteParticlesMedium(point VSParticle input[1], inout PointStream<VSParticle> ParticleOutputStream)
{	
	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);
	float Prob = Probability.SampleLevel( samPointWrap, texcoord + float2(CurTime * 0.055, CurTime * 0.035) , 0);
	float overlap = TempImage.SampleLevel( samPoint, texcoord, 0 );
	//float overlap = TempImage.SampleLevel( samPoint, texcoord, 0 ).a;

	
	if (overlap >= ProbabilityMediumThreshold) //was  && Prob < (1 - ProbabilityMediumThreshold)
	{
		
		if (Prob < 1 - overlap * (1 - ProbabilityMediumThreshold) * 1.67)
		{
			VSParticle output = input[0];
			output.color.a = input[0].color.a - FadeOutSpeedThreshold;
			output.info.a = 0;
			ParticleOutputStream.Append(output);
		}
		//VSParticle output = input[0];
		//output.color.rgb = float3(1,0,0);
		//ParticleOutputStream.Append(output);
	}
	else
		ParticleOutputStream.Append(input[0]);
}

[maxvertexcount(1)]
void GSDeleteParticlesSmall(point VSParticle input[1], inout PointStream<VSParticle> ParticleOutputStream)
{	
	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);
	float Prob = Probability.SampleLevel( samPointWrap, texcoord + float2(CurTime * 0.05, CurTime * 0.03) , 0);
	float overlap = TempImage.SampleLevel( samPoint, texcoord, 0 );
	//float overlap = TempImage.SampleLevel( samPoint, texcoord, 0 ).a;

	if (overlap >= ProbabilitySmallThreshold * 0.7 && Prob < (1 - ProbabilitySmallThreshold * 1.2))
	{
		//VSParticle output = input[0];
		//output.color.rgb = float3(1,1,0);
		//ParticleOutputStream.Append(output);
	}
	else
		ParticleOutputStream.Append(input[0]);
}

[maxvertexcount(1)]
void GSUpdateParticlesSmall(point VSParticle input[1], inout PointStream<VSParticle> ParticleOutputStream)
{	
	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);

	float2 Displacement = FlowBuffer0.SampleLevel( samLinear, texcoord, 0 ).rg * 2 - float2(1, 1);	 //the optical flow vector at the current pixel location	
	float2 DisplacedPos = input[0].pos + float2(Displacement.x, Displacement.y); //the new location of the updated stroke

	//discard the particle if it is out of the image boundary
	//discard the particle if the alpha value is too small
	if (min(DisplacedPos.r, DisplacedPos.g) >=0 && max(DisplacedPos.r, DisplacedPos.g) <= 1 && input[0].color.a > FadeOutSpeedThreshold)
	{	
		float2 newTexcoord = float2(DisplacedPos.r, 1 - DisplacedPos.g); //the texture coordinate of the new stroke location

		float4 RenderInformation = RenderInfo.SampleLevel( samPoint, newTexcoord, 0 );
		float size = RenderInformation.r;
		float StrokeGrad = RenderInformation.b;
		float StrokeAngle = RenderInformation.a;
		float alpha = RenderInformation.g;

		VSParticle output = (VSParticle) 0;

		output.color.a = input[0].color.a;
		output.info = input[0].info;
		//update the position, size, angle, color (as well as alpha value) of the particle
		output.pos = DisplacedPos;
		

		//the stroke is normal
		if (input[0].info.a > 0.6)
		{
			if (abs(size * strokeLength - input[0].info.r) > 0.01) //the stroke size has changed
			{	
				
				output.info.a = 0; //indicate the stroke is going to be deleted gradually
				output.color.a -= FadeOutSpeedThreshold; //gradually fade the stroke out				
			}
		}
		//the stroke is newly constructed
		else if (input[0].info.a > 0.4)
		{
			//the upper bound is subject to change
			if (input[0].color.a > alpha - FadeInSpeedThreshold)
				output.info.a = 1;
			else
				output.color.a += FadeInSpeedThreshold;				
		}
		//the stroke is to be deleted, decrease the alpha value
		else
		{
			//if the stroke has come back to the correct frequency, then treat it as a new stroke again
			if ( StrokeGrad >= GradientMediumThreshold - 0.001 )
				output.info.a = 0.5;
			else
			
				output.color.a -= FadeOutSpeedThreshold; //gradually fade the stroke out
		}

		//if the new location has a small stroke
		//Displacement = DisplacedPos - input[0].pos;
		if ( StrokeGrad >= GradientMediumThreshold - 0.001 && Displacement.r * Displacement.r + Displacement.g * Displacement.g >= 0.02 )
			output.info.b = StrokeAngle;
		else
		{
			if (abs(StrokeAngle - output.info.b) < 0.002)
				output.info.b = StrokeAngle;
			else
			{
				if (StrokeAngle > output.info.b)
					output.info.b = output.info.b + 0.001;
				else
					output.info.b = output.info.b - 0.001;
			}
		}
		

		float3 c1 = ColorBuffer1.SampleLevel( samPoint, newTexcoord, 0).rgb;
		float3 c2 = ColorBuffer2.SampleLevel( samPoint, newTexcoord, 0).rgb;
		float3 c3 = ColorBuffer3.SampleLevel( samPoint, newTexcoord, 0).rgb;

		//average the previous colors and the current color
		//output.color.rgb = (c1 * 0.2 + c2 * 0.6 + c3 * 0.2) * ContrastThreshold - (ContrastThreshold - 1);	
		output.color.rgb = (0.3 * input[0].color.rgb + 0.7 * c2 ) * ContrastThreshold - (ContrastThreshold - 1);
		ParticleOutputStream.Append(output);	
	}
}

[maxvertexcount(1)]
void GSUpdateParticlesMedium(point VSParticle input[1], inout PointStream<VSParticle> ParticleOutputStream)
{	
	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);

	float2 Displacement = FlowBuffer0.SampleLevel( samLinear, texcoord, 0 ).rg * 2 - float2(1, 1);	

	float2 DisplacedPos = input[0].pos + float2(Displacement.x, Displacement.y); //the new location of the updated stroke
	
	//discard the particle if it is out of the image boundary
	//discard the particle if the alpha value is too small
	if (min(DisplacedPos.r, DisplacedPos.g) >=0 && max(DisplacedPos.r, DisplacedPos.g) <= 1 && input[0].color.a > FadeOutSpeedThreshold)
	{
		float2 newTexcoord = float2(DisplacedPos.r, 1 - DisplacedPos.g); //the texture coordinate of the new stroke location
	
		float4 RenderInformation = RenderInfo.SampleLevel( samPoint, newTexcoord, 0 );
		float size = RenderInformation.r;
		float StrokeGrad = RenderInformation.b;
		float StrokeAngle = RenderInformation.a;
		float alpha = RenderInformation.g;


		VSParticle output = (VSParticle) 0;
		output.color.a = input[0].color.a;
		output.info = input[0].info;
		//update the position, size, angle, color (as well as alpha value) of the particle
		output.pos = DisplacedPos;
		

		//the stroke is normal
		if (input[0].info.a > 0.6)
		{
			if (abs(size * strokeLength - input[0].info.r) > 0.01) //the stroke size has changed
			{		
				output.info.a = 0; //indicate the stroke is going to be deleted gradually
				output.color.a -= FadeOutSpeedThreshold; //gradually fade the stroke out
			}
		}
		//the stroke is newly constructed
		else if (input[0].info.a > 0.4)
		{
			//the upper bound is subject to change
			if (input[0].color.a > alpha - FadeInSpeedThreshold)
				output.info.a = 1;
			else
				output.color.a += FadeInSpeedThreshold;
				
		}
		//the stroke is to be deleted, decrease the alpha value
		else
		{
			//if the stroke has come back to the correct frequency, then treat it as a new stroke again
			if (StrokeGrad >= GradientLargeThreshold - 0.001 && StrokeGrad <= GradientMediumThreshold + 0.01)
				output.info.a = 0.5;
			else
				output.color.a -= FadeOutSpeedThreshold; //gradually fade the stroke out
		}


		Displacement = DisplacedPos - input[0].pos;
		//if (strokeGrad.r >= GradientMediumThreshold || Displacement.r * Displacement.r + Displacement.g * Displacement.g >= 0.01)
		//if the stroke size hasn't changed, stay in the right frequency
		if (abs(size * strokeLength - input[0].info.r) <= 0.01 && Displacement.r * Displacement.r + Displacement.g * Displacement.g >= 0.04)
		//if (Displacement.r * Displacement.r + Displacement.g * Displacement.g >= 0.01 && )
			output.info.b = StrokeAngle;
		else
		{
			if (abs(StrokeAngle - output.info.b) < 0.002)
				output.info.b = StrokeAngle;
			else
			{
				if (StrokeAngle > output.info.b)
					output.info.b = output.info.b + 0.001;
				else
					output.info.b = output.info.b - 0.001;
			}
		}

		float3 c1 = ColorBuffer1.SampleLevel( samPoint, newTexcoord, 0).rgb;
		float3 c2 = ColorBuffer2.SampleLevel( samPoint, newTexcoord, 0).rgb;
		float3 c3 = ColorBuffer3.SampleLevel( samPoint, newTexcoord, 0).rgb;		

		//average the color a little bit over color values of the previous frames		
		//output.color.rgb = (c1 * 0.2 + c2 * 0.6 + c3 * 0.2) * ContrastThreshold - (ContrastThreshold - 1);
		output.color.rgb = (0.3 * input[0].color.rgb + 0.7 * c2 ) * ContrastThreshold - (ContrastThreshold - 1);
		ParticleOutputStream.Append(output);	
	}
}


[maxvertexcount(1)]
void GSUpdateParticlesLarge(point VSParticle input[1], inout PointStream<VSParticle> ParticleOutputStream)
{	
	float2 texcoord = float2(input[0].pos.r, 1 - input[0].pos.g);

	//if background of a model, do not update the particle
	/*if (ModelMatte.SampleLevel( samLinear, texcoord, 0 ).r < 0.5)
	{
		ParticleOutputStream.Append(input[0]);
		return;
	}*/

	//float4 CurrentDifference = ColorBuffer0.SampleLevel( samLinear, texcoord, 0 ) - ColorBuffer1.SampleLevel( samLinear, texcoord, 0 );
	//float4 GradientX = (ColorBuffer0.SampleLevel( samLinear, texcoord + float2( deltaX,   0), 0) 
	//	              - ColorBuffer0.g fSampleLevel( samLinear, texcoord - float2( deltaX,   0), 0)
	//				  + ColorBuffer1.SampleLevel( samLinear, texcoord + float2( deltaX,   0), 0) 
	//	              - ColorBuffer1.SampleLevel( samLinear, texcoord - float2( deltaX,   0), 0)) * 0.5;

	//float4 GradientY = (ColorBuffer0.SampleLevel( samLinear, texcoord + float2( 0,   deltaY), 0) 
	//	              - ColorBuffer0.SampleLevel( samLinear, texcoord - float2( 0,   deltaY), 0)
	//				  + ColorBuffer1.SampleLevel( samLinear, texcoord + float2( 0,   deltaY), 0) 
	//	              - ColorBuffer1.SampleLevel( samLinear, texcoord - float2( 0,   deltaY), 0)) * 0.5;

	////float GX = (GradientX.r + GradientX.g + GradientX.b ) * 0.33;
	////float GY = (GradientY.r + GradientY.g + GradientY.b ) * 0.33;
	//float GX = 0.299 * GradientX.r + 0.557 * GradientX.g + 0.144 * GradientX.b;
	//float GY = 0.299 * GradientY.r + 0.557 * GradientY.g + 0.144 * GradientY.b;

	//float GradientMagnitude = sqrt(GX * GX + GY * GY + Lambda);

	////since the value stored is not the actual optical, therefore need to retrieve the actual value
	////float2 PreVectorBlured = blur(input.Tex).rg * 2 - float2(1,1);
	//float2 PreVectorBlured = float2(0,0);


	//float PreDotGradient = PreVectorBlured.r * GX + PreVectorBlured.g * GY;

	//float2 Displacement;
	////float CD = (CurrentDifference.r + CurrentDifference.g + CurrentDifference.b) / 3.0f;
	//float CD = CurrentDifference.r * 0.299 + CurrentDifference.g * 0.557 + CurrentDifference.b * 0.144;
	//Displacement.r = PreVectorBlured.r + (CD + PreDotGradient) * (GX / GradientMagnitude);
	//Displacement.g = PreVectorBlured.g + (CD + PreDotGradient) * (GY / GradientMagnitude);

	float2 Displacement = FlowBuffer0.SampleLevel( samLinear, texcoord, 0 ).rg * 2 - float2(1, 1);
	/*if (Displacement.r > 0.01)
		Displacement.r = 0.01;
	else if(Displacement.r < -0.01)
		Displacement.r = -0.01;

	if (Displacement.g > 0.01)
		Displacement.g = 0.01;
	else if(Displacement.g < -0.01)
		Displacement.g = -0.01;*/
	//loading the optical flow value from disk files
	//float2 Displacement = OpticalFlowTexture.SampleLevel( samLinear, texcoord, 0).rg * 2 - float2(1, 1);
	//Displacement = float2(0.0, 0.0); //testing
	float2 DisplacedPos = input[0].pos + float2(Displacement.x, Displacement.y); //the new location of the updated stroke

	if (min(DisplacedPos.r, DisplacedPos.g) >=0 && max(DisplacedPos.r, DisplacedPos.g) <= 1 && input[0].color.a > FadeOutSpeedThreshold)
	{
		float2 newTexcoord = float2(DisplacedPos.r, 1 - DisplacedPos.g); //the texture coordinate of the new stroke location
	
		float4 RenderInformation = RenderInfo.SampleLevel( samPoint, newTexcoord, 0 );
		float size = RenderInformation.r;
		float StrokeGrad = RenderInformation.b;
		float StrokeAngle = RenderInformation.a;
		//float alpha = RenderInformation.g;

		//10/17
		//float RenderAlpha = StrokeBuffer.SampleLevel( samLinear, newTexcoord, 0 );

		//if the alpha at the target position is very large, do not put new stroke there.
		//if (RenderAlpha < 0.8)
		//{

			VSParticle output = (VSParticle) 0;
			output.color.a = input[0].color.a;
			output.info = input[0].info;
			//update the position, size, angle, color (as well as alpha value) of the particle
			output.pos = DisplacedPos;
			

			//the stroke is normal
			if (input[0].info.a > 0.6)
			{
				if (abs(size * strokeLength - input[0].info.r) > 0.01) //if the stroke size has changed
				{		
					output.info.a = 0; //indicate the stroke is going to be deleted gradually
					output.color.a -= FadeOutSpeedThreshold; //gradually fade the stroke out
				}
			}
			//the stroke is newly constructed
			else if (input[0].info.a > 0.4)
			{
				//the upper bound is subject to change
				if (input[0].color.a > 0.6)//alpha - FadeInSpeedThreshold
					output.info.a = 1;
				else
					output.color.a += FadeInSpeedThreshold;
					
			}
			//if the stroke is determined to be deleted
			else
			{
				//if the stroke has come back to the correct frequency, then treat it as a new stroke again
				if (StrokeGrad < GradientLargeThreshold)
					output.info.a = 0.5;
				else
					output.color.a -= FadeOutSpeedThreshold; //gradually fade the stroke out
			}

			Displacement = DisplacedPos - input[0].pos;
			//if (strokeGrad.r >= GradientMediumThreshold || Displacement.r * Displacement.r + Displacement.g * Displacement.g >= 0.01)
			if (Displacement.r * Displacement.r + Displacement.g * Displacement.g >= 0.06)
				output.info.b = StrokeAngle;
			else
			{
				if (abs(StrokeAngle - output.info.b) < 0.002)
					output.info.b = StrokeAngle;
				else
				{
					if (StrokeAngle > output.info.b)
						output.info.b = output.info.b + 0.001;
					else
						output.info.b = output.info.b - 0.001;
				}
			}

			float3 c1 = ColorBuffer1.SampleLevel( samPoint, newTexcoord, 0).rgb;
			float3 c2 = ColorBuffer2.SampleLevel( samPoint, newTexcoord, 0).rgb;
			float3 c3 = ColorBuffer3.SampleLevel( samPoint, newTexcoord, 0).rgb;		

			//average the color a little bit over color values of the previous frames
			//output.color.rgb = (c1 * 0.2 + c2 * 0.6 + c3 * 0.2) * ContrastThreshold - (ContrastThreshold - 1);
			output.color.rgb = (0.3 * input[0].color.rgb + 0.7 * c2 ) * ContrastThreshold - (ContrastThreshold - 1);
			//output.color.rgb = c2  * ContrastThreshold - (ContrastThreshold - 1);
			ParticleOutputStream.Append(output);	
		//}
	}	
}

float4 RenderPaperTexture( PS_INPUT input ) : SV_Target
{
	return float4(1,1,0.9,PaperTex.SampleLevel( samLinear, input.Tex, 0).r * 0.2);
	//return float4(PaperTex.SampleLevel( samLinear, input.Tex, 0).rgb, 0.2);
}


//*************************************************************************************//
//								Pixel shaders
//*************************************************************************************//

float4 Original( PS_INPUT input) : SV_Target
{	
	/*if (OpticalFlowTexture.SampleLevel(samLinear, input.Tex, 0).r > 0.5001 || OpticalFlowTexture.SampleLevel(samLinear, input.Tex, 0).g > 0.5001)
		return float4(1,0,0,1);
	else return float4(0,0,0,1);*/
	//return float4(OpticalFlowTexture.SampleLevel(samLinear, input.Tex, 0).rg, 0, 1);
	return ColorBuffer2.SampleLevel( samLinear, input.Tex, 0 ) * ContrastThreshold - (ContrastThreshold - 1);
	//return PaperTex.SampleLevel( samLinear, input.Tex, 0);
	//return float4(Probability.SampleLevel( samLinear, input.Tex, 0));
}

float4 RenderModelOnVideoFrame( PS_INPUT input) : SV_Target
{
	float RenderChoices = ModelMatte.SampleLevel( samLinear, input.Tex, 0);
	if (RenderChoices > 0.5)
		return ColorBuffer0.SampleLevel( samLinear, input.Tex, 0 ) + (ContrastThreshold - 1) / ContrastThreshold;
	else
		return ColorBuffer3.SampleLevel( samLinear, input.Tex, 0 );
}

float4 RenderModelMatte( PS_INPUT input) : SV_Target
{
	float RenderChoices = ModelMatte.SampleLevel( samLinear, input.Tex, 0);
	if (RenderChoices > 0.5)
		//return float4(1,0,0,1);
		return float4(1,0,0,0);
	else return float4(0,0,0,1);
}

float4 RenderGradient( PS_INPUT input) : SV_Target
{
	float g = RenderInfo.SampleLevel( samLinear, input.Tex, 0).b;

	/*if (0.5 > RenderInfo.SampleLevel( samLinear, input.Tex, 0).r && 0.5 - RenderInfo.SampleLevel( samLinear, input.Tex, 0).r < 0.0001 )
		return float4(0,0,1,1);
	else if (RenderInfo.SampleLevel( samLinear, input.Tex, 0).r> 0.5 && RenderInfo.SampleLevel( samLinear, input.Tex, 0).r - 0.5 < 0.0001 )
		return float4(0,1,0,1);
	else
		return float4(0,0,0,1);*/


	//return RenderInfo.SampleLevel( samLinear, input.Tex, 0);
	return float4(g,g,g,1);
	//return float4( 1, 0,0, 1);
}

float4 RenderMipMap( PS_INPUT input) : SV_Target
{
	float4 color = MipRendering.SampleLevel( samLinear, input.Tex, 0 );
	if (color.a > 0.7 ) return float4(1,0,0,1);
	else return float4(0,0,0,1);
	//return MipRendering.SampleLevel( samLinear, input.Tex, 0 );
}

float4 RenderEmptyPosition( PS_INPUT input ) : SV_Target
{
	float emptiness = TempImage.SampleLevel( samPoint, input.Tex, 0);
	if (emptiness < 0.14)
		return float4(1,0,0,1);
	else
		return float4(0,0,0,1);
}

float4 RenderDeletingPosition( PS_INPUT input ) : SV_Target
{
	//float overlap = TempImage.SampleLevel( samPoint, input.Tex, 0 ).a;
	float overlap = TempImage.SampleLevel( samPoint, input.Tex, 0 );
	//return float4(overlap, 0,0,1);
	if (overlap >= 0.4)
		return float4(1,0,0,1);
	else
		return float4(0,0,0,1);
	//return float4(1,0,0,1);
}

float4 RenderOpticalFlow( PS_INPUT input ) : SV_Target
{
	float2 Velocity = FlowBuffer0.SampleLevel( samPoint, input.Tex, 0 );
	/*if (Velocity.r < 0.5)
		return float4(1,0,0,1);
	else if (Velocity.r > 0.5)
		return float4(0,1,0,1);
	return float4(0,0,0,1);*/
	//if (abs(Velocity.r - 0.3) < 0.01)
		//return float4(1,0,0,1);
	//else
		//return float4(0,1,0,1);

	return float4(Velocity, 0, 1);
	//if (abs(Velocity.r * Velocity.r + Velocity.g * Velocity.g - 0.5) < 0.00001)
		//return float4(0,0,0,1);
	//else
		//return float4(Velocity, 0, 1);
}

float4 RenderStrokePosition( PS_INPUT input ) : SV_Target
{
	float Stroke = RenderInfo.SampleLevel( samPoint, input.Tex, 0).g;
	//float Stroke = StrokeBuffer.SampleLevel( samPoint, input.Tex, 0 ).a;
	if ( Stroke > 0.001)
		return float4(1, 1, 1, 1);
	else
		return float4(0, 0, 0, 1);
}

//propagate the overlapping value to the neighborhood
float SmoothOverlappingIndication( PSBLUR3x3_INPUT input ): SV_Target
{
	
	float c  = TempImage.SampleLevel( samLinear, input.Tex, 0 );
	float ul = TempImage.SampleLevel( samLinear, input.TexUL, 0 );
	float u  = TempImage.SampleLevel( samLinear, input.TexU, 0 );
	float ur = TempImage.SampleLevel( samLinear, input.TexUR, 0 );
	float l  = TempImage.SampleLevel( samLinear, input.TexL, 0 );
	float r  = TempImage.SampleLevel( samLinear, input.TexR, 0 );
	float bl = TempImage.SampleLevel( samLinear, input.TexBL, 0 );
	float b  = TempImage.SampleLevel( samLinear, input.TexB, 0 );
	float br = TempImage.SampleLevel( samLinear, input.TexBR, 0 );	
	
	return (c + ul + u + ur + l + r + bl + b + br)/9.0f;
}



//--------------------------------------------------------------------------------------
// GaussianSmooth
//--------------------------------------------------------------------------------------
float4 GaussianSmooth3x3( PSBLUR3x3_INPUT input) : SV_Target
{

	float4 c  = ColorBuffer2.SampleLevel( samLinear, input.Tex, 0 );
	float4 ul = ColorBuffer2.SampleLevel( samLinear, input.TexUL, 0 );
	float4 u  = ColorBuffer2.SampleLevel( samLinear, input.TexU, 0 );
	float4 ur = ColorBuffer2.SampleLevel( samLinear, input.TexUR, 0 );
	float4 l  = ColorBuffer2.SampleLevel( samLinear, input.TexL, 0 );
	float4 r  = ColorBuffer2.SampleLevel( samLinear, input.TexR, 0 );
	float4 bl = ColorBuffer2.SampleLevel( samLinear, input.TexBL, 0 );
	float4 b  = ColorBuffer2.SampleLevel( samLinear, input.TexB, 0 );
	float4 br = ColorBuffer2.SampleLevel( samLinear, input.TexBR, 0 );	
	
	return (c + ul + u + ur + l + r + bl + b + br) / 9.0f // divide 9
			 * ContrastThreshold - (ContrastThreshold - 1); //enhance contrast		
	//return float4(0.5, 0.4, 0.8, 1);
}

float4 GaussianSmooth5x5( PSBLUR5x5_INPUT input) : SV_Target
{

	float4 C11  = ColorBuffer2.SampleLevel( samLinear, input.Tex11, 0 );
	float4 C12  = ColorBuffer2.SampleLevel( samLinear, input.Tex12, 0 );
	float4 C13  = ColorBuffer2.SampleLevel( samLinear, input.Tex13, 0 );
	float4 C14  = ColorBuffer2.SampleLevel( samLinear, input.Tex14, 0 );
	float4 C15  = ColorBuffer2.SampleLevel( samLinear, input.Tex15, 0 );
	float4 C21  = ColorBuffer2.SampleLevel( samLinear, input.Tex21, 0 );
	float4 C22  = ColorBuffer2.SampleLevel( samLinear, input.Tex22, 0 );
	float4 C23  = ColorBuffer2.SampleLevel( samLinear, input.Tex23, 0 );
	float4 C24  = ColorBuffer2.SampleLevel( samLinear, input.Tex24, 0 );
	float4 C25  = ColorBuffer2.SampleLevel( samLinear, input.Tex25, 0 );
	float4 C31  = ColorBuffer2.SampleLevel( samLinear, input.Tex31, 0 );
	float4 C32  = ColorBuffer2.SampleLevel( samLinear, input.Tex32, 0 );
	float4 C33  = ColorBuffer2.SampleLevel( samLinear, input.Tex33, 0 );
	float4 C34  = ColorBuffer2.SampleLevel( samLinear, input.Tex34, 0 );
	float4 C35  = ColorBuffer2.SampleLevel( samLinear, input.Tex35, 0 );
	float4 C41  = ColorBuffer2.SampleLevel( samLinear, input.Tex41, 0 );
	float4 C42  = ColorBuffer2.SampleLevel( samLinear, input.Tex42, 0 );
	float4 C43  = ColorBuffer2.SampleLevel( samLinear, input.Tex43, 0 );
	float4 C44  = ColorBuffer2.SampleLevel( samLinear, input.Tex44, 0 );
	float4 C45  = ColorBuffer2.SampleLevel( samLinear, input.Tex45, 0 );
	float4 C51  = ColorBuffer2.SampleLevel( samLinear, input.Tex51, 0 );
	float4 C52  = ColorBuffer2.SampleLevel( samLinear, input.Tex52, 0 );
	float4 C53  = ColorBuffer2.SampleLevel( samLinear, input.Tex53, 0 );
	float4 C54  = ColorBuffer2.SampleLevel( samLinear, input.Tex54, 0 );
	float4 C55  = ColorBuffer2.SampleLevel( samLinear, input.Tex55, 0 );

	float4 CRow1 = 2 * C11 + 4 * C12 + 5 * C13 + 4 * C14 + 2 * C15;
	float4 CRow2 = 4 * C21 + 9 * C22 + 12 * C23 + 9 * C24 + 4 *C25;
	float4 CRow3 = 5 * C31 + 12 * C32 + 15 * C33 + 12 * C34 + 5 * C35;
	float4 CRow4 = 4 * C41 + 9 * C42 + 12 * C43 + 9 * C44 + 4 *C45;
	float4 CRow5 = 2 * C51 + 4 * C52 + 5 * C53 + 4 * C54 + 2 * C55;
	return (CRow1 + CRow2 + CRow3 + CRow4 + CRow5) / 159.0f * ContrastThreshold - (ContrastThreshold - 1);	
}

float2 DetermineStrokeDetail(float2 texcoord)
{
	float2 Result;
	//Calculate lighting factor and alpha mask value
	//TODO
	//float dx = 0.0149; //1 / stroke texture width
	//float dy = 0.0036; //1 / stroke texture height
	float dx = 1.0f / StrokeTexLength;
	float dy = 1.0f / StrokeTexWidth;
	float3 v1, v2, v3, v4;
	float rr = StrokeTex.SampleLevel( samLinear, float2(texcoord.r + dx, texcoord.g     ), 0 ).r;
	float ll = StrokeTex.SampleLevel( samLinear, float2(texcoord.r - dx, texcoord.g     ), 0 ).r;
	float uu = StrokeTex.SampleLevel( samLinear, float2(texcoord.r,      texcoord.g + dy), 0 ).r;
	float bb = StrokeTex.SampleLevel( samLinear, float2(texcoord.r,      texcoord.g - dy), 0 ).r;
	
	Result.r = AlphaMask.SampleLevel( samLinear, texcoord, 0 ).r;

	v1 = float3( 4 * dx, 0,      rr - ll);
	v2 = float3( 0     , 4 * dy, uu - bb);
	float3 normal = normalize(cross(v1,v2));
	float lightingFactor = abs(dot(normal, float3(0,0,1)));

	if (renderStyle == 0)
		lightingFactor = 1;
	Result.g = lightingFactor;
	return Result;
}

//--------------------------------------------------------------------------------------
// Sobel Filter
//--------------------------------------------------------------------------------------
//the modified sobel filter not only compute the gradient magnitude and the angle but also compute the lighting factor
float4 SobelFilter( PSBLUR3x3_INPUT input) : SV_Target
{

	//ColorBuffer store the smoothed result after gaussian smooth
	
	float3 c  = ColorBuffer1.SampleLevel( samLinear, input.Tex, 0 ).rgb;
	float3 ul = ColorBuffer1.SampleLevel( samLinear, input.TexUL, 0 ).rgb;
	float3 u  = ColorBuffer1.SampleLevel( samLinear, input.TexU, 0 ).rgb;
	float3 ur = ColorBuffer1.SampleLevel( samLinear, input.TexUR, 0 ).rgb;
	float3 l  = ColorBuffer1.SampleLevel( samLinear, input.TexL, 0 ).rgb;
	float3 r  = ColorBuffer1.SampleLevel( samLinear, input.TexR, 0 ).rgb;
	float3 bl = ColorBuffer1.SampleLevel( samLinear, input.TexBL, 0 ).rgb;
	float3 b  = ColorBuffer1.SampleLevel( samLinear, input.TexB, 0 ).rgb;
	float3 br = ColorBuffer1.SampleLevel( samLinear, input.TexBR, 0 ).rgb;	

	float3 Gx = ul - ur + 2 * l - 2 * r + bl - br;
	float3 Gy = ul - bl + 2 * u - 2 * b + ur - br;
	float Gx1 = 0.299 * Gx.r + 0.557 * Gx.g + 0.144 * Gx.b;
	float Gy1 = 0.299 * Gy.r + 0.557 * Gy.g + 0.144 * Gy.b;
	float Gradient = sqrt(Gx1 * Gx1 + Gy1 * Gy1);
	//float3 G = sqrt(Gx * Gx + Gy * Gy);
	//float Gradient = (G.r + G.g + G.b) * 0.33;


	float GradientAngle = 0;
	if (!OnRealGradientAngle && Gradient< angleThreshold)	
	{	
		//GradientAngle = 0;
		//GradientAngle = RGB_to_HSV(ColorBuffer1.SampleLevel( samLinear, input.Tex, 0 )).r;
		GradientAngle = ColorBuffer1.SampleLevel( samLinear, input.Tex, 0 ).r;//CHANGE			
	}
	else
	{
		//finalColor.b = Gradient.y;	

		float Y = sqrt(Gy * Gy);
		float X = sqrt(Gx * Gx);

		if (Gx.r < 0) X = -X;
		if (Gy.r > 0) Y = -Y;
		
		float angle;
		if (abs(X) < 0.001)
			angle = 0;
		else
			angle= atan2(Y, X);
		
		GradientAngle = ( angle / PI + 1 ) * 0.5;
	}

	float2 Detail = DetermineStrokeDetail(input.Tex);
	//return float4(Gradient, Gradient, Gradient, GradientAngle);
	return float4(Detail, Gradient, GradientAngle);

	//return float4(0.6, 0.5, 0.2, 1);
	//return ColorBuffer1.SampleLevel( samLinear, input.Tex, 0 );
}

float4 AddSobelFilterWithFocus( PSBLUR3x3_INPUT input) : SV_Target
{
	//calculate the gradient magnitude and angle	
	float3 c  = ColorBuffer0.SampleLevel( samLinear, input.Tex, 0 ).rgb;
	float3 ul = ColorBuffer0.SampleLevel( samLinear, input.TexUL, 0 ).rgb;
	float3 u  = ColorBuffer0.SampleLevel( samLinear, input.TexU, 0 ).rgb;
	float3 ur = ColorBuffer0.SampleLevel( samLinear, input.TexUR, 0 ).rgb;
	float3 l  = ColorBuffer0.SampleLevel( samLinear, input.TexL, 0 ).rgb;
	float3 r  = ColorBuffer0.SampleLevel( samLinear, input.TexR, 0 ).rgb;
	float3 bl = ColorBuffer0.SampleLevel( samLinear, input.TexBL, 0 ).rgb;
	float3 b  = ColorBuffer0.SampleLevel( samLinear, input.TexB, 0 ).rgb;
	float3 br = ColorBuffer0.SampleLevel( samLinear, input.TexBR, 0 ).rgb;	

	float3 Gx = ul - ur + 2 * l - 2 * r + bl - br;
	float3 Gy = ul - bl + 2 * u - 2 * b + ur - br;
	float Gx1 = 0.299 * Gx.r + 0.557 * Gx.g + 0.144 * Gx.b;
	float Gy1 = 0.299 * Gy.r + 0.557 * Gy.g + 0.144 * Gy.b;

	//float f = 0.1;
	//float r1 = 0.1;
	//float r2 = 0.2;
	float2 fxy = float2(MouseX, MouseY);
	float rad = length(fxy - input.Tex);// /sqrt(2);
	float scale;

	if (OnFocusPoint)
		scale = focusf + (1 - focusf) * smoothstep(1 - focusr2, 1 - focusr1, 1 - rad);
	else
		scale = focusf + ModelMatte.SampleLevel( samLinear, input.Tex, 0 ).r * (1 - focusf);
	float Gradient = sqrt(Gx1 * Gx1 + Gy1 * Gy1) * scale;

	float GradientAngle = 0;
	//if (!OnRealGradientAngle && Gradient< angleThreshold)	
	if (Gradient< angleThreshold) //in low frequency area
	{
		if (OnRealGradientAngle)
			GradientAngle = ColorBuffer0.SampleLevel( samLinear, input.Tex, 0 ).r;//CHANGE
		else
		{
			float3 hsv = RGB_to_HSV(ColorBuffer0.SampleLevel( samLinear, input.Tex, 0 )).rgb;
			GradientAngle = fmod(hsv.r * smoothstep(0, 0.1, hsv.g * hsv.b) + 0.25, 1.0);
		}
	}
	else
	{
		float Y = sqrt(Gy * Gy);
		float X = sqrt(Gx * Gx);

		//float Y = Gy1;
		//float X = Gx1;

		//if (X < 0) X = -X;
		//if (Y > 0) Y = -Y;
		if (Gx.r < 0) X = -X;
		if (Gy.r > 0) Y = -Y;
		
		float angle;
		if (abs(X) < 0.01)		
			;
			//angle = PI * 0.5;
		else
			angle= atan2(Y, X);
		
		GradientAngle = ( angle / PI + 1 ) * 0.5;
	}

	//return float4(Gradient, Gradient, Gradient, GradientAngle);
	//float2 opticalFlow = FlowBuffer0.SampleLevel( samLinear, input.Tex, 0 ).rg;
	//return float4(opticalFlow, Gradient, GradientAngle);

	//calculate the stroke properties
	float4 finalColor;
	float prob = Probability.SampleLevel( samLinear, input.Tex, 0 ); //probability from 0 - 1
	finalColor = float4(0,0,Gradient,GradientAngle);

	//for the large stroke
	if (Gradient < GradientLargeThreshold)
	{	
		finalColor.r = 1; //size
		if (prob < ProbabilityLargeThreshold && gradientThreshold < GradientLargeThreshold) //prob 0.05
			finalColor.g = AlphaLargeThreshold;
	}	
	
	//for the medium stroke
	else if (Gradient >= GradientLargeThreshold && Gradient < GradientMediumThreshold)
	{
		finalColor.r = SizeMediumThreshold;
		if (prob < ProbabilityMediumThreshold && gradientThreshold < GradientMediumThreshold) //prob 0.15
			finalColor.g = AlphaMediumThreshold;
	}
	
	//for the small stroke
	else if (Gradient >= GradientMediumThreshold) // 0.3
	{		
		finalColor.r = SizeSmallThreshold;
		if (prob < ProbabilitySmallThreshold) //prob 0.2
			finalColor.g = AlphaSmallThreshold;
	}
	return finalColor;
}

float4 AddSobelFilter( PSBLUR3x3_INPUT input) : SV_Target
{
	//calculate the gradient magnitude and angle	
	float3 c  = ColorBuffer0.SampleLevel( samLinear, input.Tex, 0 ).rgb;
	float3 ul = ColorBuffer0.SampleLevel( samLinear, input.TexUL, 0 ).rgb;
	float3 u  = ColorBuffer0.SampleLevel( samLinear, input.TexU, 0 ).rgb;
	float3 ur = ColorBuffer0.SampleLevel( samLinear, input.TexUR, 0 ).rgb;
	float3 l  = ColorBuffer0.SampleLevel( samLinear, input.TexL, 0 ).rgb;
	float3 r  = ColorBuffer0.SampleLevel( samLinear, input.TexR, 0 ).rgb;
	float3 bl = ColorBuffer0.SampleLevel( samLinear, input.TexBL, 0 ).rgb;
	float3 b  = ColorBuffer0.SampleLevel( samLinear, input.TexB, 0 ).rgb;
	float3 br = ColorBuffer0.SampleLevel( samLinear, input.TexBR, 0 ).rgb;	

	float3 Gx = ul - ur + 2 * l - 2 * r + bl - br;
	float3 Gy = ul - bl + 2 * u - 2 * b + ur - br;
	float Gx1 = 0.299 * Gx.r + 0.557 * Gx.g + 0.144 * Gx.b;
	float Gy1 = 0.299 * Gy.r + 0.557 * Gy.g + 0.144 * Gy.b;
	float Gradient = sqrt(Gx1 * Gx1 + Gy1 * Gy1);

	float GradientAngle = 0;
	//if (!OnRealGradientAngle && Gradient< angleThreshold)	
	if (Gradient< angleThreshold) //in low frequency area
	{
		if (OnRealGradientAngle)
			GradientAngle = ColorBuffer0.SampleLevel( samLinear, input.Tex, 0 ).r;//CHANGE
		else
		{
			float3 hsv = RGB_to_HSV(ColorBuffer0.SampleLevel( samLinear, input.Tex, 0 )).rgb;
			GradientAngle = fmod(hsv.r * smoothstep(0, 0.1, hsv.g * hsv.b) + 0.25, 1.0);
			//jitter the gradient angle a bit
			GradientAngle = fmod(GradientAngle + Probability.SampleLevel(samLinear, input.Tex, 0) * 0.1, 1.0);
		}
	}
	else
	{
		float Y = sqrt(Gy * Gy);
		float X = sqrt(Gx * Gx);

		//float Y = Gy1;
		//float X = Gx1;

		//if (X < 0) X = -X;
		//if (Y > 0) Y = -Y;
		if (Gx.r < 0) X = -X;
		if (Gy.r > 0) Y = -Y;
		
		float angle;
		if (abs(X) < 0.01)		
			;
			//angle = PI * 0.5;
		else
			angle= atan2(Y, X);
		
		GradientAngle = ( angle / PI + 1 ) * 0.5;
	}

	//return float4(Gradient, Gradient, Gradient, GradientAngle);
	//float2 opticalFlow = FlowBuffer0.SampleLevel( samLinear, input.Tex, 0 ).rg;
	//return float4(opticalFlow, Gradient, GradientAngle);

	//calculate the stroke properties
	float4 finalColor;
	float prob = Probability.SampleLevel( samLinear, input.Tex, 0 ); //probability from 0 - 1
	finalColor = float4(0,0,Gradient,GradientAngle);

	//for the large stroke
	if (Gradient < GradientLargeThreshold)
	{	
		finalColor.r = 1; //size
		if (prob < ProbabilityLargeThreshold && gradientThreshold < GradientLargeThreshold) //prob 0.05
			finalColor.g = AlphaLargeThreshold;
	}	
	
	//for the medium stroke
	else if (Gradient >= GradientLargeThreshold && Gradient < GradientMediumThreshold)
	{
		finalColor.r = SizeMediumThreshold;
		if (prob < ProbabilityMediumThreshold && gradientThreshold < GradientMediumThreshold) //prob 0.15
			finalColor.g = AlphaMediumThreshold;
	}
	
	//for the small stroke
	else if (Gradient >= GradientMediumThreshold) // 0.3
	{		
		finalColor.r = SizeSmallThreshold;
		if (prob < ProbabilitySmallThreshold) //prob 0.2
			finalColor.g = AlphaSmallThreshold;
	}
	return finalColor;
}


float4 ConstructStrokesDetails( PS_INPUT input) : SV_Target
{
	float4 finalColor;

	finalColor.rg = DetermineStrokeDetail(input.Tex);
	finalColor.b = RenderInfo.SampleLevel( samPoint, input.Tex, 0).b;
	finalColor.a = MipRendering.SampleLevel( samLinear, input.Tex, 0).a; //the alpha value of the rendering of previous frame

	return finalColor;
}

float4 blur(float2 texcoord)
{
	//Blur the previous result
	float4 c  = PreviousVector.SampleLevel( samLinear, texcoord, 0 );
	float4 ul = PreviousVector.SampleLevel( samLinear, texcoord - float2( deltaX,   deltaY), 0 );
	float4 u  = PreviousVector.SampleLevel( samLinear, texcoord - float2( 0,        deltaY), 0 );
	float4 ur = PreviousVector.SampleLevel( samLinear, texcoord - float2(-deltaX,   deltaY), 0 );
	float4 l  = PreviousVector.SampleLevel( samLinear, texcoord - float2( deltaX,   0), 0 );
	float4 r  = PreviousVector.SampleLevel( samLinear, texcoord - float2(-deltaX,   0), 0 );
	float4 bl = PreviousVector.SampleLevel( samLinear, texcoord - float2( deltaX,  -deltaY), 0 );
	float4 b  = PreviousVector.SampleLevel( samLinear, texcoord - float2( 0,       -deltaY), 0 );
	float4 br = PreviousVector.SampleLevel( samLinear, texcoord - float2(-deltaX,  -deltaY), 0 );	
	
	return (c + ul + u + ur + l + r + bl + b + br) / 9.0f;
}



float2 Gradient(PSBLUR3x3_INPUT input/*float2 texcoord*/, Texture2D BufferName)
{
	float3 c  = BufferName.SampleLevel( samLinear, input.Tex, 0 ).rgb;
	float3 ul = BufferName.SampleLevel( samLinear, input.TexUL, 0 ).rgb;
	float3 u  = BufferName.SampleLevel( samLinear, input.TexU, 0 ).rgb;
	float3 ur = BufferName.SampleLevel( samLinear, input.TexUR, 0 ).rgb;
	float3 l  = BufferName.SampleLevel( samLinear, input.TexL, 0 ).rgb;
	float3 r  = BufferName.SampleLevel( samLinear, input.TexR, 0 ).rgb;
	float3 bl = BufferName.SampleLevel( samLinear, input.TexBL, 0 ).rgb;
	float3 b  = BufferName.SampleLevel( samLinear, input.TexB, 0 ).rgb;
	float3 br = BufferName.SampleLevel( samLinear, input.TexBR, 0 ).rgb;
	
	/*float3 Gx = ul - ur + 2 * l - 2 * r + bl - br;
	float3 Gy = ul - bl + 2 * u - 2 * b + ur - br;
	float3 G = sqrt(Gx * Gx + Gy * Gy);
	float Gradient = (G.r + G.g + G.b) * 0.33;*/

	float3 Gx = ul - ur + 2 * l - 2 * r + bl - br;
	float3 Gy = ul - bl + 2 * u - 2 * b + ur - br;
	float Gx1 = 0.299 * Gx.r + 0.557 * Gx.g + 0.144 * Gx.b;
	float Gy1 = 0.299 * Gy.r + 0.557 * Gy.g + 0.144 * Gy.b;
	float Gradient = sqrt(Gx1 * Gx1 + Gy1 * Gy1);

	float GradientAngle = 0;
	if (!OnRealGradientAngle && Gradient< angleThreshold)		
		GradientAngle = BufferName.SampleLevel( samLinear, input.Tex, 0 ).r;//CHANGE	
	else
	{
		float Y = sqrt(Gy * Gy);
		float X = sqrt(Gx * Gx);

		if (Gx.r < 0) X = -X;
		if (Gy.r > 0) Y = -Y;
		
		float angle;
		if (abs(X) < 0.001)
			angle = 0;
		else
			angle= atan2(Y, X);
		
		GradientAngle = ( angle / PI + 1 ) * 0.5;
	}
	return float2(Gradient, GradientAngle);
}

float4 RenderTest( PS_INPUT input) : SV_Target
{
	return ColorBuffer0.SampleLevel( samLinear, input.Tex, 0 );
}

float4 PreOpticalFlow( PS_INPUT input) : SV_Target
{
	float4 CurrentDifference = ColorBuffer2.SampleLevel( samLinear, input.Tex, 0 ) - ColorBuffer3.SampleLevel( samLinear, input.Tex, 0 );
	float4 GradientX = (ColorBuffer2.SampleLevel( samLinear, input.Tex + float2( deltaX,   0), 0) 
		              - ColorBuffer2.SampleLevel( samLinear, input.Tex - float2( deltaX,   0), 0)
					  + ColorBuffer3.SampleLevel( samLinear, input.Tex + float2( deltaX,   0), 0) 
		              - ColorBuffer3.SampleLevel( samLinear, input.Tex - float2( deltaX,   0), 0)) * 0.5;

	float4 GradientY = (ColorBuffer2.SampleLevel( samLinear, input.Tex + float2( 0,   deltaY), 0) 
		              - ColorBuffer2.SampleLevel( samLinear, input.Tex - float2( 0,   deltaY), 0)
					  + ColorBuffer3.SampleLevel( samLinear, input.Tex + float2( 0,   deltaY), 0) 
		              - ColorBuffer3.SampleLevel( samLinear, input.Tex - float2( 0,   deltaY), 0)) * 0.5;

	float CD = (CurrentDifference.r + CurrentDifference.g + CurrentDifference.b) / 3.0f;

	float GX = 0.33 * GradientX.r + 0.34 * GradientX.g + 0.33 * GradientX.b;
	float GY = 0.33 * GradientY.r + 0.34 * GradientY.g + 0.33 * GradientY.b;

	CD = (CD + 1) * 0.5;
	GX = (GX + 1) * 0.5;
	GY = (GY + 1) * 0.5;

	//float GradientMagnitude = sqrt(GX * GX + GY * GY + Lambda);
	//float GradientMagnitude = GX * GX + GY * GY + Lambda;

	/*if (GX > 1)
		return float4(1,0,0,1);
	else
		return float4(0,1,0,1);*/

	return float4(CD, GX, GY, 1);
	//return float4(1,0,0,1);
}

//the positive optical flow value means upwared and rightward movement
//called at odd iterations
float2 OpticalFlowI1( PSBLUR3x3_INPUT input) : SV_Target
{
	float4 ImageInfo = ColorBuffer0.SampleLevel(samLinear, input.Tex, 0);
	float CD = ImageInfo.r * 2 - 1;
	float GX = ImageInfo.g * 2 - 1;
	float GY = ImageInfo.b * 2 - 1;
	//float GradientMagnitude = sqrt(GX * GX + GY * GY + Lambda);
	float GradientMagnitude = GX * GX + GY * GY + Lambda;
	//float GradientMagnitude = ImageInfo.a;
	
	float2 c  = PreviousVector.SampleLevel( samLinear, input.Tex, 0 );
	float2 ul = PreviousVector.SampleLevel( samLinear, input.TexUL, 0 );
	float2 u  = PreviousVector.SampleLevel( samLinear, input.TexU, 0 );
	float2 ur = PreviousVector.SampleLevel( samLinear, input.TexUR, 0 );
	float2 l  = PreviousVector.SampleLevel( samLinear, input.TexL, 0 );
	float2 r  = PreviousVector.SampleLevel( samLinear, input.TexR, 0 );
	float2 bl = PreviousVector.SampleLevel( samLinear, input.TexBL, 0 );
	float2 b  = PreviousVector.SampleLevel( samLinear, input.TexB, 0 );
	float2 br = PreviousVector.SampleLevel( samLinear, input.TexBR, 0 );

	float2 result = (c + ul + u + ur + l + r + bl + b + br) / 9.0f;
	float2 PreVectorBlured = result.rg * 2 - float2(1,1);

	float PreDotGradient = PreVectorBlured.r * GX + PreVectorBlured.g * GY;

	float2 Velocity;
	Velocity.r = PreVectorBlured.r - (CD + PreDotGradient) * (GX / GradientMagnitude);
	Velocity.g = PreVectorBlured.g - (CD + PreDotGradient) * (GY / GradientMagnitude);
	
	Velocity.r = (Velocity.r * 0.5 + 1) * 0.5;
	Velocity.g = (Velocity.g * 0.5 + 1) * 0.5;

	return Velocity;
	//return float4(CD, GX, GY, GradientMagnitude);
	//return float4(0,0,0,1);
}

float2 OpticalFlowI2( PSBLUR3x3_INPUT input) : SV_Target
{
	float4 ImageInfo = ColorBuffer0.SampleLevel(samLinear, input.Tex, 0);
	float CD = ImageInfo.r * 2 - 1;
	float GX = ImageInfo.g * 2 - 1;
	float GY = ImageInfo.b * 2 - 1;
	//float GradientMagnitude = sqrt(GX * GX + GY * GY + Lambda);
	float GradientMagnitude = GX * GX + GY * GY + Lambda;
	//float GradientMagnitude = ImageInfo.a;
	
	
	float2 c  = PreviousVector0.SampleLevel( samLinear, input.Tex, 0 );
	float2 ul = PreviousVector0.SampleLevel( samLinear, input.TexUL, 0 );
	float2 u  = PreviousVector0.SampleLevel( samLinear, input.TexU, 0 );
	float2 ur = PreviousVector0.SampleLevel( samLinear, input.TexUR, 0 );
	float2 l  = PreviousVector0.SampleLevel( samLinear, input.TexL, 0 );
	float2 r  = PreviousVector0.SampleLevel( samLinear, input.TexR, 0 );
	float2 bl = PreviousVector0.SampleLevel( samLinear, input.TexBL, 0 );
	float2 b  = PreviousVector0.SampleLevel( samLinear, input.TexB, 0 );
	float2 br = PreviousVector0.SampleLevel( samLinear, input.TexBR, 0 );

	float2 result = (c + ul + u + ur + l + r + bl + b + br) / 9.0f;
	float2 PreVectorBlured = result.rg * 2 - float2(1,1);

	float PreDotGradient = PreVectorBlured.r * GX + PreVectorBlured.g * GY;

	float2 Velocity;
	Velocity.r = PreVectorBlured.r - (CD + PreDotGradient) * (GX / GradientMagnitude);
	Velocity.g = PreVectorBlured.g - (CD + PreDotGradient) * (GY / GradientMagnitude);
	
	Velocity.r = (Velocity.r * 0.5 + 1) * 0.5;
	Velocity.g = (Velocity.g * 0.5 + 1) * 0.5;

	return Velocity;
	//return float4(CD, GX, GY, GradientMagnitude);
	//return float4(0,0,0,1);
}

float PSRenderScene(PSSceneIn input) : SV_Target
{
	return 0.01;
}

float4 PSRenderOpticalFlow(PSSceneIn input) : SV_Target
{
	return input.color;
}


float4 PSRenderParticles(PSSceneIn input) : SV_Target
{ 	
	float3 Mask = StrokeBuffer.SampleLevel( samLinear, input.tex, 0 ).rgb;

	float4 color = input.color * Mask.y * float4(1,1,1,Mask.x);
	if (input.type > 0.6 && OnClipping) //large stroke
	{
		//color = float4(1,0,1,1);
		float2 step = (input.CGtex - input.Gtex) * 0.5 / strokeLength;
		int i = 0;
		for (i = 0; i <= 2 * strokeLength; i++)
		{
			//float size = StrokeBuffer.SampleLevel( samLinear, input.Gtex + step * i, 0 ).b;
			float2 texcoord = input.Gtex + step * i;
			float gradient = RenderInfo.SampleLevel( samLinear, texcoord, 0 ).b;

			//if encounter both the medium and small gradient during the searching
			if (gradient >= GradientLargeThreshold)
			{
				color = float4(0,0,0,0);
				//color = float4(1,0,0,1);
				break;
			}	
		}
	}
	else if (input.type > 0.4 && OnClipping) //medium stroke
	{
		//color = float4(1,0,0,1);
		float2 step = (input.CGtex - input.Gtex) * 0.5 / strokeLength;
		int i = 0;
		for (i = 0; i <= 2 * strokeLength; i++)
		{
			float2 texcoord = input.Gtex + step * i;
			float gradient = RenderInfo.SampleLevel( samLinear, texcoord, 0 ).b;
		
			//if encounter small gradient during the searching
			if (gradient >= GradientMediumThreshold)
			{
				color = float4(0,0,0,0);
				//color = float4(0,0,1,1);
				break;
			}
		}
		//color = float4(0,0,1,1);
	}

	return color;

	//return float4(1,0,0,1);
}

//--------------------------------------------------------------------------------------
// GaussianSmooth
//--------------------------------------------------------------------------------------
float4 EnlargeStroke( PSBLUR3x3_INPUT input) : SV_Target
{
	float count = 0.0f;
	float4 result = 0.0f;
	float4 c  = RenderInfo.SampleLevel( samLinear, input.Tex, 0 );
	float4 ul = RenderInfo.SampleLevel( samLinear, input.TexUL, 0 );
	float4 u  = RenderInfo.SampleLevel( samLinear, input.TexU, 0 );
	float4 ur = RenderInfo.SampleLevel( samLinear, input.TexUR, 0 );
	float4 l  = RenderInfo.SampleLevel( samLinear, input.TexL, 0 );
	float4 r  = RenderInfo.SampleLevel( samLinear, input.TexR, 0 );
	float4 bl = RenderInfo.SampleLevel( samLinear, input.TexBL, 0 );
	float4 b  = RenderInfo.SampleLevel( samLinear, input.TexB, 0 );
	float4 br = RenderInfo.SampleLevel( samLinear, input.TexBR, 0 );	

	if (max(max(c.r, c.g), c.b) < 0.9){
		result = result + c;
		count = count + 1.0f;
	}

	if (max(max(ul.r, ul.g), ul.b) < 0.9){
		result = result + ul;
		count = count + 1.0f;
	}

	if (max(max(u.r, u.g), u.b) < 0.9){
		result = result + u;
		count = count + 1.0f;
	}

	if (max(max(ur.r, ur.g), ur.b) < 0.9){
		result = result + ur;
		count = count + 1.0f;
	}

	if (max(max(l.r, l.g), l.b) < 0.9){
		result = result + l;
		count = count + 1.0f;
	}

	if (max(max(r.r, r.g), r.b) < 0.9){
		result = result + r;
		count = count + 1.0f;
	}
	
	if (max(max(bl.r, bl.g), bl.b) < 0.9){
		result = result + bl;
		count = count + 1.0f;
	}

	if (max(max(b.r, b.g), b.b) < 0.9){
		result = result + b;
		count = count + 1.0f;
	}
	
	if (max(max(br.r, br.g), br.b) < 0.9){
		result = result + br;
		count = count + 1.0f;
	}

	if (count > 0.0)
		return result / count ;// * 1.1 - 0.1;
	else
		return float4(1,1,1,1);
}

float FindMedian(float a, float b, float c)
{
	float median;
	if( a < b )
	{
		if( b < c)
		{
			median = b;
		}
		else
		{
			median = max(a,c);
		}
	}
	else
	{
		if( a < c)
		{
			median = a;
		}
		else
		{
			median = max(b,c);
		}
	}
	return median;
}

float4 MedianFilter( PSBLUR3x3_INPUT input) : SV_Target
{
	float4 c  = ColorBuffer2.SampleLevel( samLinear, input.Tex, 0 );
	float4 ul = ColorBuffer2.SampleLevel( samLinear, input.TexUL, 0 );
	float4 u  = ColorBuffer2.SampleLevel( samLinear, input.TexU, 0 );
	float4 ur = ColorBuffer2.SampleLevel( samLinear, input.TexUR, 0 );
	float4 l  = ColorBuffer2.SampleLevel( samLinear, input.TexL, 0 );
	float4 r  = ColorBuffer2.SampleLevel( samLinear, input.TexR, 0 );
	float4 bl = ColorBuffer2.SampleLevel( samLinear, input.TexBL, 0 );
	float4 b  = ColorBuffer2.SampleLevel( samLinear, input.TexB, 0 );
	float4 br = ColorBuffer2.SampleLevel( samLinear, input.TexBR, 0 );	

	float Ur = FindMedian(ul.r, u.r, ur.r);
	float Mr = FindMedian(c.r, l.r, r.r);
	float Br = FindMedian(bl.r, b.r, br.r);
	float Red = FindMedian(Ur, Mr, Br);

	float Ug = FindMedian(ul.g, u.g, ur.g);
	float Mg = FindMedian(c.g, l.g, r.g);
	float Bg = FindMedian(bl.g, b.g, br.g);
	float Green = FindMedian(Ug, Mg, Bg);

	float Ub = FindMedian(ul.b, u.b, ur.b);
	float Mb = FindMedian(c.b, l.b, r.b);
	float Bb = FindMedian(bl.b, b.b, br.b);
	float Blue = FindMedian(Ub, Mb, Bb);

	return float4(Red, Green, Blue, c.a);	
}

/*
float4 GaussianSmoothGradient( PSBLUR3x3_INPUT input) : SV_Target
{
	float2 c  = TempImage.SampleLevel( samLinear, input.Tex, 0 ).ra;
	float2 ul = TempImage.SampleLevel( samLinear, input.TexUL, 0 ).ra;
	float2 u  = TempImage.SampleLevel( samLinear, input.TexU, 0 ).ra;
	float2 ur = TempImage.samLinear, input.TexUR, 0 ).ra;
	float2 l  = TempImage.SampleLevel( samLinear, input.TexL, 0 ).ra;
	float2 r  = TempImage.SampleLevel( samLinear, input.TexR, 0 ).ra;
	float2 bl = TempImage.SampleLevel( samLinear, input.TexBL, 0 ).ra;
	float2 b  = TempImage.SampleLevel( samLinear, input.TexB, 0 ).ra;
	float2 br = TempImage.SampleLevel( samLinear, input.TexBR, 0 ).ra;		

	//Calculate lighting factor and alpha mask value
	float dx = 0.0149; //1 / stroke texture width
	float dy = 0.0036; //1 / stroke texture height
	float3 v1, v2, v3, v4;
	float rr = StrokeTex.SampleLevel( samLinear, float2(input.Tex.r + dx, input.Tex.g     ), 0 ).r;
	float ll = StrokeTex.SampleLevel( samLinear, float2(input.Tex.r - dx, input.Tex.g     ), 0 ).r;
	float uu = StrokeTex.SampleLevel( samLinear, float2(input.Tex.r,      input.Tex.g + dy), 0 ).r;
	float bb = StrokeTex.SampleLevel( samLinear, float2(input.Tex.r,      input.Tex.g - dy), 0 ).r;
	
	
	float alpha = AlphaMask.SampleLevel( samLinear, input.Tex, 0 ).r;

	v1 = float3( 4 * dx, 0,      rr - ll);
	v2 = float3( 0     , 4 * dy, uu - bb);
	float3 normal = normalize(cross(v1,v2));
	float lightingFactor = abs(dot(normal, float3(0,0,1)));

	if (renderStyle == 0)
		lightingFactor = 1;
	
	//return float4(1,0,0,1);
	
	float2 Gradient = (c + ul + u + ur + l + r + bl + b + br) / 9.0f; // divide 9
	//return TempImage.SampleLevel( samLinear, input.Tex, 0 );
	return float4(Gradient.x, TempImage.SampleLevel( samLinear, input.Tex, 0 ).gba);
	//return (1, 1, 1, 1);
}
*/


//--------------------------------------------------------------------------------------
//GeometryShader gsStreamOut0 = ConstructGSWithSO( CompileShader( gs_4_0, GSConstructParticles() ), "POSITION.xy;COLOR.xyzw;INFO.xyzw" );
GeometryShader gsStreamOut1 = ConstructGSWithSO( CompileShader( gs_4_0, GSConstructParticlesLarge() ), "POSITION.xy;COLOR.xyzw;INFO.xyzw" );
GeometryShader gsStreamOut2 = ConstructGSWithSO( CompileShader( gs_4_0, GSConstructParticlesMedium() ), "POSITION.xy;COLOR.xyzw;INFO.xyzw" );
GeometryShader gsStreamOut3 = ConstructGSWithSO( CompileShader( gs_4_0, GSConstructParticlesSmall() ), "POSITION.xy;COLOR.xyzw;INFO.xyzw" );
GeometryShader gsStreamOut6 = ConstructGSWithSO( CompileShader( gs_4_0, GSUpdateParticlesLarge() ), "POSITION.xy;COLOR.xyzw;INFO.xyzw" );
GeometryShader gsStreamOut7 = ConstructGSWithSO( CompileShader( gs_4_0, GSUpdateParticlesMedium() ), "POSITION.xy;COLOR.xyzw;INFO.xyzw" );
GeometryShader gsStreamOut8 = ConstructGSWithSO( CompileShader( gs_4_0, GSUpdateParticlesSmall() ), "POSITION.xy;COLOR.xyzw;INFO.xyzw" );
GeometryShader gsStreamOut9 = ConstructGSWithSO( CompileShader( gs_4_0, GSAppendParticlesLarge() ), "POSITION.xy;COLOR.xyzw;INFO.xyzw" );
GeometryShader gsStreamOut10 = ConstructGSWithSO( CompileShader( gs_4_0, GSAppendParticlesMedium() ), "POSITION.xy;COLOR.xyzw;INFO.xyzw" );
GeometryShader gsStreamOut11 = ConstructGSWithSO( CompileShader( gs_4_0, GSAppendParticlesSmall() ), "POSITION.xy;COLOR.xyzw;INFO.xyzw" );
GeometryShader gsStreamOut12 = ConstructGSWithSO( CompileShader( gs_4_0, GSDeleteParticlesLarge() ), "POSITION.xy;COLOR.xyzw;INFO.xyzw" );
GeometryShader gsStreamOut13 = ConstructGSWithSO( CompileShader( gs_4_0, GSDeleteParticlesMedium() ), "POSITION.xy;COLOR.xyzw;INFO.xyzw" );
GeometryShader gsStreamOut14 = ConstructGSWithSO( CompileShader( gs_4_0, GSDeleteParticlesSmall() ), "POSITION.xy;COLOR.xyzw;INFO.xyzw" );
//GeometryShader gsStreamOut6 = ConstructGSWithSO( CompileShader( gs_4_0, GSRenderParticles() ), "POSITION.xyzw;TEXTURE0.xy;COLOR0.xyzw" ); //new
GeometryShader gsStreamOut15 = ConstructGSWithSO( CompileShader( gs_4_0, GSAppendParticlesLargeForVideo() ), "POSITION.xy;COLOR.xyzw;INFO.xyzw" );


technique10 Preprocessing
{
	pass RenderPaperTexture
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, RenderPaperTexture() ) );
		SetBlendState( CompositeBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	pass Original
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, Original() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
    }

	pass ModelMatte
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, RenderModelMatte() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	pass RenderModelOnVideoFrame
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, RenderModelOnVideoFrame() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

    pass GaussianSmooth3x3
    {
        SetVertexShader( CompileShader( vs_4_0, VSBlur3x3() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, GaussianSmooth3x3() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
    }

	/*pass GaussianSmooth5x5
	{
		SetVertexShader( CompileShader( vs_4_0, VSBlur5x5() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, GaussianSmooth5x5() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}*/

	pass SmoothOverlappingIndication
	{
		SetVertexShader( CompileShader( vs_4_0, VSBlur3x3() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, MedianFilter()));
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	pass MedianFilter
	{
		SetVertexShader( CompileShader( vs_4_0, VSBlur3x3() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, MedianFilter() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	pass AddSobelFilterWithFocus
	{
		SetVertexShader( CompileShader( vs_4_0, VSBlur3x3() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, AddSobelFilterWithFocus() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	pass AddSobelFilter
	{
		SetVertexShader( CompileShader( vs_4_0, VSBlur3x3() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, AddSobelFilter() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	pass SobelFilter
	{
		SetVertexShader( CompileShader( vs_4_0, VSBlur3x3() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, SobelFilter() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}	
	
	pass ConstructStrokesDetails
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, ConstructStrokesDetails() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF ); 
		SetDepthStencilState( DisableDepth, 0 );
	}

	pass RenderGradient
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, RenderGradient() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
    }

	pass RenderStrokePosition
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, RenderStrokePosition() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}	
}

technique10 OpticalFlow
{
	pass PreOpticalFlow
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PreOpticalFlow() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	pass OpticalFlowI1
	{
		SetVertexShader( CompileShader( vs_4_0, VSBlur3x3() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, OpticalFlowI1() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	pass OpticalFlowI2
	{
		SetVertexShader( CompileShader( vs_4_0, VSBlur3x3() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, OpticalFlowI2() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}	

	pass RenderOpticalFlow
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, RenderOpticalFlow() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	pass RenderTest
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, RenderTest() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}
}

technique10 SimulateParticle
{
	pass DetermineSpriteSmall
	{
		SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( gsStreamOut3 );
        SetPixelShader( NULL );
        
        SetDepthStencilState( DisableDepth, 0 );
	}

	pass DetermineSpriteLarge
	{
		SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( gsStreamOut1 );
        SetPixelShader( NULL );
        
        SetDepthStencilState( DisableDepth, 0 );
	}

	pass DetermineSpriteMedium
	{
		SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( gsStreamOut2 );
        SetPixelShader( NULL );
        
        SetDepthStencilState( DisableDepth, 0 );
	}

	pass AppendSpritesLarge
    {
        SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( gsStreamOut9 );
        SetPixelShader( NULL );
        
        SetDepthStencilState( DisableDepth, 0 );
    }

	pass AppendSpritesLargeForVideo
    {
        SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( gsStreamOut15 );
        SetPixelShader( NULL );
        
        SetDepthStencilState( DisableDepth, 0 );
    }

	pass AppendSpritesMedium
    {
        SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( gsStreamOut10 );
        SetPixelShader( NULL );
        
        SetDepthStencilState( DisableDepth, 0 );
    } 

	pass AppendSpritesSmall
    {
        SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( gsStreamOut11 );
        SetPixelShader( NULL );
        
        SetDepthStencilState( DisableDepth, 0 );
    } 

	pass UpdateSpritesSmall
	{
		SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( gsStreamOut8 );
        SetPixelShader( NULL );
        
        SetDepthStencilState( DisableDepth, 0 );
	}

	pass UpdateSpritesMedium
	{
		SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( gsStreamOut7 );
        SetPixelShader( NULL );
        
        SetDepthStencilState( DisableDepth, 0 );
	}

	pass UpdateSpritesLarge
	{
		SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( gsStreamOut6 );
        SetPixelShader( NULL );
        
        SetDepthStencilState( DisableDepth, 0 );
	}

	pass DeleteSpritesLarge
	{
		SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( gsStreamOut12 );
        SetPixelShader( NULL );
        
        SetDepthStencilState( DisableDepth, 0 );
	}

	pass DeleteSpritesMedium
	{
		SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( gsStreamOut13 );
        SetPixelShader( NULL );
        
        SetDepthStencilState( DisableDepth, 0 );
	}

	pass DeleteSpritesSmall
	{
		SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( gsStreamOut14 );
        SetPixelShader( NULL );
        
        SetDepthStencilState( DisableDepth, 0 );
	}
}

technique10 RenderSprites
{	
	pass RenderOpticalFlowSprites
	{
		SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( CompileShader( gs_4_0, GSRenderOpticalFlow() ) );
        SetPixelShader( CompileShader( ps_4_0, PSRenderOpticalFlow() ) );
		SetBlendState( CompositeBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	//Use point sprite to render each stroke
	pass RenderSprites
    {
        SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( CompileShader( gs_4_0, GSRenderParticles() ) );
        SetPixelShader( CompileShader( ps_4_0, PSRenderParticles() ) );
		SetBlendState( CompositeBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	pass RenderSpritesOnlyOnModel
	{		
		SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( CompileShader( gs_4_0, GSRenderParticlesOnlyModel() ) );
        SetPixelShader( CompileShader( ps_4_0, PSRenderParticles() ) );
		SetBlendState( CompositeBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	pass RenderScene
	{
		SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( CompileShader( gs_4_0, GSRenderParticles() ) );
        SetPixelShader( CompileShader( ps_4_0, PSRenderScene() ) );
		SetBlendState( AdditiveBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	//useless, cannot find an example with outputing triangles into a buffer, how to set the index buffer
	/*pass ConstructSprites
    {
        SetVertexShader( CompileShader( vs_4_0, VSPassThroughParticle() ) );
        SetGeometryShader( gsStreamOut6 );
        SetPixelShader( NULL );
        
        SetDepthStencilState( DisableDepth, 0 );
    } */

	pass RenderMipMap
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, RenderMipMap() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
    }

	pass RenderDeletingPosition
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, RenderDeletingPosition() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	pass RenderEmptyPosition
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, RenderEmptyPosition() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}	
}

technique10 OtherTechnique
{
	pass EnlargeStroke
	{
		SetVertexShader( CompileShader( vs_4_0, VSBlur3x3() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, EnlargeStroke() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}

	/*pass GaussianSmoothGradient
	{
		SetVertexShader( CompileShader( vs_4_0, VSBlur3x3() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, GaussianSmoothGradient() ) );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
	}	*/
}

