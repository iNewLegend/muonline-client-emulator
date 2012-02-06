#include "shared.fx"

sampler s0: register(s0);
float4 PS(float2 uv : TEXCOORD0):COLOR0
{
    return tex2D(s0, uv)*(sin(g_fTime*20)+1);
}

technique Render
{
    pass P0
    {
	CullMode			= None;

	AlphaTestEnable		= False;

	AlphaBlendEnable	= True;
	BlendOp				= Add;
	SrcBlend			= One;
	DestBlend			= One;

	ZEnable				= True;
	ZFunc				= LessEqual;
	ZWriteEnable		= False;

        VertexShader = NULL;
        PixelShader = compile ps_2_0 PS();
    }
}