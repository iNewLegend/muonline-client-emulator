#include "shared.fx"

sampler s0: register(s0);
float4 PS( in float2 Tex : TEXCOORD0 ) : COLOR0
{
    return tex2D( s0, Tex );
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
