#include "shared.fx"

sampler s0: register(s0);
sampler s1: register(s1);
float4 PS( in float2 Tex : TEXCOORD0 ) : COLOR0
{
    float3 ColorOrig = tex2D( s0, Tex );

    ColorOrig += tex2D( s1, Tex );

    return float4( ColorOrig, 1.0f );
}

technique Render
{
    pass P0
    {
		CullMode		= None;
		AlphaTestEnable		= False;
		AlphaBlendEnable	= False;
		ZEnable			= False;
		ZWriteEnable		= False;
        VertexShader = null;
        PixelShader = compile ps_2_0 PS();
    }
}
