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
    float3  normal  : TEXCOORD1;
};

VS_OBJECT_OUTPUT VS(VS_OBJECT_INPUT i)
{
	VS_OBJECT_OUTPUT o;
	o.pos = mul(i.pos,wvpm);
	o.uv = i.uv;
	float3 normal = normalize(mul(i.normal,wvm));
	o.normal = normal;
	o.pos.xy+=normal.xy*o.pos.w*0.005;
	return o;
}

sampler s0: register(s0);
float4 PS(VS_OBJECT_OUTPUT i) : COLOR0
{
	float4 o;
	o = tex2D(s0, i.uv);
	float shine  = 1 - abs( dot(i.normal,float3(0,0,-1)) );
	shine = pow(shine, 4);
	o.xyz = gColor*shine;
	return o;
}

technique Render
{
    pass P0
    {
		CullMode			= None;

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