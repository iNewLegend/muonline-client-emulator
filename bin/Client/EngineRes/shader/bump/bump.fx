#include "shared.fx"

struct VS_INPUT
{
    float4  pos     : POSITION;
    float2  uv      : TEXCOORD;
};

struct VS_OUTPUT
{
    float4  pos    : POSITION;
    float2  uv     : TEXCOORD0;
    float2  uv2    : TEXCOORD1;
};

VS_OUTPUT VS(VS_INPUT i)
{
	VS_OUTPUT o;
	o.uv = i.uv;
	o.uv.y-=g_fTime*0.2f;
	o.pos = mul(i.pos,wvpm);
	o.uv2 = o.pos.xy/o.pos.w*0.5f;
	o.uv2.y=-o.uv2.y;
	o.uv2+=0.5f;
	return o;
}

sampler s0: register(s0);
sampler s1: register(s1);
float4 PS( in float2 uv : TEXCOORD0, in float2 uv2 : TEXCOORD1) : COLOR0
{
	float3 vNormal	= tex2D(s0,uv).xyz;
	float4 color = tex2D( s1, uv2.xy+(vNormal.xy-0.5)*0.02f);
	color.xyz+=float4(0.2,0.2,0.5,0);//vNormal;
	color.a = 1;
	return color;
}

technique Render
{
    pass P0
    {
	CullMode		= None;

	AlphaTestEnable		= False;

	AlphaBlendEnable	= True;
	BlendOp			= Add;
	SrcBlend		= SrcAlpha;
	DestBlend		= InvSrcAlpha;

	ZEnable			= True;
	ZFunc			= LessEqual;
	ZWriteEnable		= False;
	
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}