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
		Lighting			= False;
		CullMode			= None;
		
		AlphaTestEnable		= False;

		AlphaBlendEnable	= False;

		ZEnable				= False;
		ZFunc				= LessEqual;
		ZWriteEnable		= False;
		
        VertexShader = null;
        PixelShader = compile ps_2_0 PS();
    }
}
