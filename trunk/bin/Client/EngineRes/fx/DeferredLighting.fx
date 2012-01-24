#include "shared.fx"

sampler s0: register(s0);
sampler s1: register(s1);
float4 PS( in float2 uv : TEXCOORD0 ) : COLOR0
{
	float3 pos = tex2D(s0, uv);
	float3 normal = tex2D(s1, uv);
	normal = normalize(2*normal-1);
	float d = saturate(dot(normal, -g_vLightDir));
	d += 0.05f; //ambient
	float4 color = d;
	
	float3 V = normalize(pos - g_vEyePot);
	float3 R = reflect(-g_vLightDir, normal);
	
	float specular = pow(saturate(dot(R,V)),8)*d;
	color.a = specular;
    return color;
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
