#include "shared.fx"

struct VS_OUTPUT
{
    float2  UV0     : TEXCOORD0;
    float2  UV1     : TEXCOORD1;
    float3  Normal  : TEXCOORD2;
    float4  Color   : COLOR;
};

float4 PS(VS_OUTPUT i) : COLOR0
{
	float4 cDiffuse	= tex2D(g_samDiffuse, i.UV0);
	float3 cLight	= tex2D(g_samLight, i.UV1).xyz;
	float3 cNormal	= tex2D(g_samNormal, i.UV0).xyz;
	
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