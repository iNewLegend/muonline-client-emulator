#include "shared.fx"

struct PS_TEXT_INPUT
{
    float4  Diffuse : COLOR;
    float2  UV      : TEXCOORD0;
};

texture	g_texText;
float4  g_vShadowColor;

sampler g_samText=
sampler_state
{
    Texture = <g_texText>;
    AddressU = BORDER;
    AddressV = BORDER;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

float4 PS(PS_TEXT_INPUT i) : COLOR0
{
	//F*(1-T)+T*D
	float4 color	= tex2D(g_samDiffuse, i.UV);
	color.xyz = g_vShadowColor*(1-color.x)+color.x*i.Diffuse.xyz;
	//color.a = 1.0f;
	return color;
}

technique Render
{
    pass P0
    {
		VertexShader = NULL;
        PixelShader  = compile ps_2_0 PS();
    }
}