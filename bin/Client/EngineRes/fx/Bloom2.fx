#include "shared.fx"

static const int g_cKernelSize = 13;

float2 PixelKernel[g_cKernelSize] =
{
    { 0, -6 },
    { 0, -5 },
    { 0, -4 },
    { 0, -3 },
    { 0, -2 },
    { 0, -1 },
    { 0,  0 },
    { 0,  1 },
    { 0,  2 },
    { 0,  3 },
    { 0,  4 },
    { 0,  5 },
    { 0,  6 },
};

float2 TexelKernel[g_cKernelSize]
<
    string ConvertPixelsToTexels = "PixelKernel";
>;

static const float BlurWeights[g_cKernelSize] = 
{
    0.002216,
    0.008764,
    0.026995,
    0.064759,
    0.120985,
    0.176033,
    0.199471,
    0.176033,
    0.120985,
    0.064759,
    0.026995,
    0.008764,
    0.002216,
};

struct VS_OUTPUT
{
    float4  Pos     : POSITION;
	float3  UV0		: TEXCOORD0;
};

float BloomScale = 1.0f;

sampler s0: register(s0);
float4 PS(float2 Tex : TEXCOORD0) : COLOR0
{
    float4 Color = 0;
    for (int i = 0; i < g_cKernelSize; i++)
    {    
        Color += tex2D( s0, Tex + PixelKernel[i].xy*1/600.0f ) * BlurWeights[i];
    }
	return Color * BloomScale;
}

technique Render
{
    pass P0
    {
		Lighting			= False;
		CullMode			= None;
		
		AlphaTestEnable		= False;

		AlphaBlendEnable	= True;
		BlendOp				= Add;
		SrcBlend			= One;
		DestBlend			= One;

		ZEnable				= False;
		ZFunc				= LessEqual;
		ZWriteEnable		= False;

		ColorOp[0]			= Modulate;
		ColorArg1[0]		= Texture;
		ColorArg2[0]		= Diffuse;

		AlphaOp[0]			= Modulate;
		AlphaArg1[0]		= Texture;
		AlphaArg2[0]		= Diffuse;

		ColorOp[1]			= Disable;
		AlphaOP[1]			= Disable;
		 
		VertexShader		= NULL;
        PixelShader			= compile ps_2_0 PS();
    }
}