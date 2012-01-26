#include "shared.fx"

struct VS_INPUT
{
    float4  Pos     : POSITION;
    float2  UV0     : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4  Pos     : POSITION;
	float3  UV0		: TEXCOORD0;
};
float3 LuminanceConv = { 0.2125f, 0.7154f, 0.0721f };

sampler s0: register(s0);
sampler s1: register(s1);
float4 PS(VS_OUTPUT i) : COLOR0
{
	float3 color = tex2D(s0, i.UV0).rgb;
	float fIndex = max(dot( color, LuminanceConv ),0.08f);
	float3 color1 = tex2D(s1, float2(fIndex,0.0f)).rgb;
	
	return float4(color*color1/fIndex,1.0f);
}

technique Render
{
    pass P0
    {
		AddressU[0]			= BORDER;
		AddressV[0]			= BORDER;
		AddressW[0]			= BORDER;

		Lighting			= False;
		CullMode			= None;

		AlphaTestEnable		= False;
		AlphaBlendEnable	= False;

		ZEnable				= False;
		ZWriteEnable		= False;
		
		ColorOp[0]			= Modulate;
		ColorArg1[0]		= Texture;
		ColorArg2[0]		= Diffuse;

		AlphaOp[0]			= SelectArg1;
		AlphaArg1[0]		= Diffuse;

		ColorOp[1]			= Disable;
		AlphaOP[1]			= Disable;
		
        VertexShader		= NULL;
        PixelShader			= compile ps_2_0 PS();
    }
}