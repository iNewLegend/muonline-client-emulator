#include "shared.fx"

struct VS_MODEL_INPUT
{
    float4  Pos     : POSITION;
    float3  Normal  : NORMAL;
    float2  UV0     : TEXCOORD0;
};

struct VS_MODEL_OUTPUT
{
    float4  Pos     : POSITION;
    float2  UV0     : TEXCOORD0;
    float4  pos     : TEXCOORD1;
    float3  Normal  : TEXCOORD2;
};

struct PS_OUTPUT
{
    float4  color   : COLOR0;
    float4  pos     : COLOR1;
    float4  normal  : COLOR2;
};

VS_MODEL_OUTPUT VS(VS_MODEL_INPUT i)
{
	VS_MODEL_OUTPUT o;
	o.UV0 = i.UV0;
	o.Pos = mul(i.Pos,wvpm);
	o.Normal = (mul(i.Normal,g_mWorld)+1)*0.5f;
	o.pos = mul(i.Pos,g_mWorld);
	return o;
}

sampler s0: register(s0);
PS_OUTPUT PS(VS_MODEL_OUTPUT i)
{
	PS_OUTPUT o;
	o.color	= tex2D(s0, i.UV0);
	o.normal = float4(i.Normal,1);
	o.pos = i.pos;
	return o;
}

technique Render
{
    pass P0
    {
		Lighting			= True;
		CullMode			= None;

		AlphaTestEnable		= True;
		AlphaFunc			= GreaterEqual;
		AlphaRef			= 128;

		AlphaBlendEnable	= False;

		ZEnable				= True;
		ZFunc				= LessEqual;
		ZWriteEnable		= True;

		ColorOp[0]			= Modulate;
		ColorArg1[0]		= Texture;
		ColorArg2[0]		= Diffuse;

		AlphaOp[0]			= SelectArg1;
		AlphaArg1[0]		= Texture;

		ColorOp[1]			= Disable;
		AlphaOP[1]			= Disable;
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}