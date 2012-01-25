#include "shared.fx"

sampler s0: register(s0);
sampler s1: register(s1);
sampler s2: register(s2);
float4 PS( in float2 uv : TEXCOORD0 ) : COLOR0
{
	float3 pos = tex2D(s0, uv);
	float3 normal = tex2D(s1, uv);
	float3 lightmap = tex2D(s2, float2(pos.x/256,-pos.z/256));
	//normal = normalize(2*normal-1);
	float d = saturate(dot(normal, -g_vLightDir));
	d += 0.1f; //ambient
	float4 color = d;
	color.rgb*=lightmap;
	float3 V = normalize(pos - g_vEyePot);
	float3 R = reflect(-g_vLightDir, normal);
	
	float specular = pow(saturate(dot(R,V)),16)*d;
	color.a = 0;//specular;
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
