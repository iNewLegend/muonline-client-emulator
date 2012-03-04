#include "shared.fx"

struct VS_OBJECT_INPUT
{
    float4  pos     : POSITION;
	float3  normal  : NORMAL;
    float2  uv      : TEXCOORD0;
};

struct VS_OBJECT_OUTPUT
{
    float4  pos     : POSITION;
    float2  uv      : TEXCOORD0;
};

VS_OBJECT_OUTPUT VS(VS_OBJECT_INPUT i)
{
	VS_OBJECT_OUTPUT o;
	o.pos = mul(i.pos,wvpm);
	o.uv = i.uv;
	float3 normal = normalize(mul(i.normal,wvpm));
	//normal = normalize(mul(normal,g_mView));
	o.pos.xy+=normal.xy*o.pos.w*0.005;
	return o;
}

sampler s0: register(s0);
float4 PS(VS_OBJECT_OUTPUT i) : COLOR0
{
	float4 o;
	o = tex2D(s0, i.uv);
	o.xyz = gColor;
	return o;
}

technique Render
{
    pass P0
    {
		CullMode			= CW;

		AlphaTestEnable		= True;
		AlphaFunc			= GreaterEqual;
		AlphaRef			= 128;

		AlphaBlendEnable	= True;
		BlendOp				= Add;
		SrcBlend			= One;
		DestBlend			= One;
		
		ZEnable				= False;
		ZWriteEnable		= False;
		
		VertexShader		= compile vs_2_0 VS();
        PixelShader			= compile ps_2_0 PS();
    }
}