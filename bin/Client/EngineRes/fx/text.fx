#include "shared.fx"

struct PS_TEXT_INPUT
{
    float4  Diffuse : COLOR;
    float2  UV      : TEXCOORD0;
};

sampler s0: register(s0);
float4 c0: register(c0);
float4 PS(PS_TEXT_INPUT i) : COLOR0
{
	//F*(1-T)+T*D
	float4 color	= tex2D(s0, i.UV);
	color.xyz = c0*(1-color.x)+color.x*i.Diffuse.xyz;
	color.w*=i.Diffuse.w;
	return color;
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
		SrcBlend			= SrcAlpha;
		DestBlend			= InvSrcAlpha;

		ZEnable				= False;
		ZFunc				= LessEqual;
		ZWriteEnable		= False;

		VertexShader = NULL;
        PixelShader  = compile ps_2_0 PS();
    }
}