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
	
	float4x4 mViewProj = g_mViewProj;
	float4x4 mView = g_mView;
	mView._41=0;mView._42=0;mView._43=0;
	mViewProj._41=0;mViewProj._42=0;mViewProj._43=0;
	float3 normal = mul(i.Normal,mViewProj);
	normal = normalize(mul(normal,mView));

	o.Pos.xy+=normal.xy*o.Pos.w*0.005;
	
	o.UV0 = i.UV0;
	return o;
}

float4 PS(VS_OBJECT_OUTPUT i) : COLOR0
{
	float4 color = 0xFFFF4000;//g_vColorFocus;
	//color.a	*= tex2D(g_samDiffuse, i.UV0).a;
	return color;
}

technique Render
{
    pass P0
    {
		Lighting			= False;
		CullMode			= CW;

		AlphaFunc			= NEVER;

		AlphaBlendEnable	= False;

		ZEnable				= True;
		ZFunc				= GREATEREQUAL;
		ZWriteEnable		= False

		ColorOp[0]			= Modulate;
		ColorArg1[0]		= Diffuse;
		ColorArg2[0]		= Texture;

		AlphaOp[0]			= SelectArg1;
		AlphaArg1[0]		= Diffuse;

		ColorOp[1]			= Disable;
		AlphaOP[1]			= Disable;
		
		VertexShader		= compile vs_2_0 VS();
        PixelShader			= compile ps_2_0 PS();
    }
}