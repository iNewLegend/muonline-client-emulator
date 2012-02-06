#include "shared.fx"

sampler s0: register(s0);
sampler s1: register(s1);
float4 PS( in float2 uv : TEXCOORD0 ) : COLOR0
{
	float4 color = tex2D(s0, uv);
	float3 diffuse = tex2D(s1, uv);

	color.rgb*=diffuse;
	color.rgb += color.a;
    return color;
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
