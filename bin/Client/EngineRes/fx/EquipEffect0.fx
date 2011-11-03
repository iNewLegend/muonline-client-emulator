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
    float4  Color   : COLOR;
};

float4x4 WorldXViewXProj: register(c0);
VS_MODEL_OUTPUT VS(VS_MODEL_INPUT i)
{
	VS_MODEL_OUTPUT o;
	o.UV0 = i.UV0;
	o.Pos = mul(i.Pos,WorldXViewXProj);
	o.Color = float4(0.7,1,0.7,1)*(0.8-sin(g_fTime*2)*0.2f);
	return o;
}

sampler s0: register(s0);
float4 PS(VS_MODEL_OUTPUT i) : COLOR0
{
	float4 color	= tex2D(s0, i.UV0);
	return color*i.Color;
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

        VertexShader		= compile vs_1_0 VS();
        PixelShader			= compile ps_2_0 PS();
    }
}