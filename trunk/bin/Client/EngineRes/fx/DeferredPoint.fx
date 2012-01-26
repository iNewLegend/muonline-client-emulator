#include "shared.fx"

sampler s0: register(s0);
sampler s1: register(s1);
float4 PS( in float2 uv : TEXCOORD0 ) : COLOR0
{
	float3 pos = tex2D(s0, uv);
	float3 normal = tex2D(s1, uv);
	float3 lightDir = g_vPointLight - pos;
	
	float3 L = normalize(lightDir);
	float len = length(lightDir);
	float d = saturate(dot(normal, L));
	float4 color = d*(1-smoothstep(len,0,4))*float4(1.5,0.5,0.1,0);
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
		SrcBlend			= One;
		DestBlend			= One;

		ZEnable				= False;
		ZFunc				= LessEqual;
		ZWriteEnable		= False;
		
        VertexShader = null;
        PixelShader = compile ps_2_0 PS();
    }
}
