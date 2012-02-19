#include "shared.fx"

sampler s0: register(s0);
float4 PS(float2 uv : TEXCOORD0, float4 color : COLOR):COLOR0
{
    return tex2D(s0, uv)*color;
}

technique Render
{
    pass P0
    {
		CullMode			= None;

		AlphaTestEnable		= False;

		AlphaBlendEnable	= True;
		BlendOp				= Add;
		SrcBlend			= SrcAlpha;
		DestBlend			= InvSrcAlpha;
	
		ZEnable				= True;
		ZFunc				= LessEqual;
		ZWriteEnable		= False;

        VertexShader		= NULL;
        PixelShader = compile ps_2_0 PS();
    }
}