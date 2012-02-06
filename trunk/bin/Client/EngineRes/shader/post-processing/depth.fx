#include "shared.fx"

struct VS_INPUT
{
    float4  pos    : POSITION;
    float2  uv     : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4  postion : POSITION;
    float2  uv      : TEXCOORD0;
    float4  pos     : TEXCOORD1;
};

VS_OUTPUT VS(VS_INPUT i)
{
	VS_OUTPUT o;
	o.uv = i.uv;
	o.postion = mul(i.pos,g_mWorld*lvm);
	o.pos = o.postion;
	return o;
}

sampler s0: register(s0);
float4 PS(VS_OUTPUT i):COLOR0
{
    return float4(i.pos.z/i.pos.w,0,0,0);
}

technique Render
{
    pass P0
    {
	CullMode		= None;
	AlphaTestEnable		= False;
	AlphaBlendEnable	= False;

	ZEnable			= True;
	ZFunc			= LessEqual;
	ZWriteEnable		= False;

        VertexShader = compile vs_2_0 VS();
        PixelShader = compile ps_2_0 PS();
    }
}