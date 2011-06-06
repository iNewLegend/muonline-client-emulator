#include "shared.fx"
float4 g_vColorFocus;

struct VS_OBJECT_INPUT
{
    float4  Pos     : POSITION;
	float3  Normal  : NORMAL;
    float2  UV0     : TEXCOORD0;
};

struct VS_OBJECT_OUTPUT
{
    float4  Pos     : POSITION;
    float2  UV0     : TEXCOORD0;
};

VS_OBJECT_OUTPUT VS(VS_OBJECT_INPUT i)
{
	VS_OBJECT_OUTPUT o;
	i.Pos = mul(i.Pos,g_mWorld);
	o.Pos = mul(i.Pos,g_mViewProj);
	
	g_mView._41=0;g_mView._42=0;g_mView._43=0;
	g_mWorld._41=0;g_mWorld._42=0;g_mWorld._43=0;
	float3 normal = mul(i.Normal,g_mWorld);
	normal = normalize(mul(normal,g_mView));

	o.Pos.xy+=normal.xy*o.Pos.w*0.005;
	
	o.UV0 = i.UV0;
	return o;
}

float4 PS(VS_OBJECT_OUTPUT i) : COLOR0
{
	float4 color = g_vColorFocus;
	//color.a	*= tex2D(g_samDiffuse, i.UV0).a;
	return color;
}

technique Render
{
    pass P0
    {
				CullMode = CW;
				VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}