#include "shared.fx"
float4 g_vColorFocus;

struct VS_OBJECT_INPUT
{
    float4  Pos     : POSITION;
	float3  Normal  : NORMAL;
};

struct VS_OBJECT_OUTPUT
{
    float4  Pos     : POSITION;
};

VS_OBJECT_OUTPUT VS(VS_OBJECT_INPUT i)
{
	VS_OBJECT_OUTPUT o;
	i.Pos = mul(i.Pos,g_mWorld);
	o.Pos = mul(i.Pos,g_mViewProj);
	
	float4x4 mViewProj = g_mViewProj;
	float4x4 mView = g_mView;
	mView._41=0;mView._42=0;mView._43=0;
	mViewProj._41=0;mViewProj._42=0;mViewProj._43=0;
	float3 normal = mul(i.Normal,mViewProj);
	normal = normalize(mul(normal,mView));

	o.Pos.xy+=normal.xy*o.Pos.w*0.005;

	return o;
}

float4 vColorFocus: register(c0);
float4 PS(VS_OBJECT_OUTPUT i) : COLOR0
{
	return vColorFocus;
}

technique Render
{
    pass P0
    {
		Lighting			= False;
		CullMode			= CW;

		AlphaTestEnable		= False;

		AlphaBlendEnable	= True;
		BlendOp				= Add;
		SrcBlend			= SrcAlpha;
		DestBlend			= InvSrcAlpha;
		
		ZEnable				= True;
		ZFunc				= LessEqual;
		ZWriteEnable		= False;
		
		VertexShader		= compile vs_2_0 VS();
        PixelShader			= compile ps_2_0 PS();
    }
}