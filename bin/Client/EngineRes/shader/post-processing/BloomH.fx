#include "shared.fx"

static const int g_cKernelSize = 13;

float2 PixelKernel[g_cKernelSize] =
{
    { -6, 0 },
    { -5, 0 },
    { -4, 0 },
    { -3, 0 },
    { -2, 0 },
    { -1, 0 },
    {  0, 0 },
    {  1, 0 },
    {  2, 0 },
    {  3, 0 },
    {  4, 0 },
    {  5, 0 },
    {  6, 0 },
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
	Tex.x-=inv_width_height.x*7;
    for (int i = 0; i < g_cKernelSize; i++)
    {
		Tex.x+=inv_width_height.x;
        Color += tex2D( s0, Tex ) * BlurWeights[i];
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

		AlphaBlendEnable	= False;

		ZEnable				= False;
		ZFunc				= LessEqual;
		ZWriteEnable		= False;

		VertexShader		= NULL;
        PixelShader			= compile ps_2_0 PS();
    }
}