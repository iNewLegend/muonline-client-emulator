#include "shared.fx"

struct VS_INPUT
{
    float4  Pos     : POSITION;
    float2  UV      : TEXCOORD;
};

struct VS_OUTPUT
{
    float4  Pos    : POSITION;
    float2  UV     : TEXCOORD0;
    float4  UV2    : TEXCOORD1;
};

VS_OUTPUT VS(VS_INPUT i)
{
	VS_OUTPUT o;
	o.UV = i.UV;
	o.Pos = mul(i.Pos,vpm);
	o.UV2 = o.Pos.xyzw;

	return o;
}

sampler s0: register(s0);
sampler s1: register(s1);
float4 PS( in float2 Tex0 : TEXCOORD0, in float4 UV2 : TEXCOORD1) : COLOR0
{
	float3 vNormal	= normalize(tex2D(s0,Tex0).xyz-0.5f);
	//vNormal.y
	UV2.xy = (UV2.xy/UV2.w)*0.5f;
	UV2.y=-UV2.y;
	UV2+=0.5f;
	
	float4 color = tex2D( s1, (UV2.xyz+vNormal*0.02f).xy);//*1.3;float4(0.8,0.8,0.9,0);

	return color;
}

technique Render
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}