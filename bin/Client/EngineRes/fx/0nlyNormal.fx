#include "shared.fx"

struct VS_OUTPUT
{
    float2  UV0     : TEXCOORD0;
    float2  UV1     : TEXCOORD1;
    float3  Normal  : TEXCOORD2;
    float4  Color   : COLOR;
};
sampler s0: register(s0);
sampler s1: register(s1);
sampler s2: register(s2);
float4 PS(VS_OUTPUT i) : COLOR0
{
	float4 cDiffuse	= tex2D(s0, i.UV0);
	float3 cLight	= tex2D(s1, i.UV1).xyz;
	float3 cNormal	= tex2D(s2, i.UV0).xyz;
	
	float3 vNormal	= cNormal*2-1;
	float4 color;
	color.a = i.Color.a;
	color.xyz = saturate(dot(vNormal, -i.Normal))*0.5+0.6;
	color.xyz*=cLight;
	//float3 reflVec = reflect(normView, bump);
	//float4 cEnvironment = texCUBE(g_samEnvironment,  reflVec);
	color *= cDiffuse;//+cEnvironment;

	return color;
}

technique Render
{
    pass P0
    {
        VertexShader = NULL;
        PixelShader  = compile ps_2_0 PS();
    }
}