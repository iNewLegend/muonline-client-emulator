#include "shared.fx"
#include "terrainShared.fx"

struct VS_TERRAIN_GRASS_INPUT
{
    float4  Pos     : POSITION;
	float3  Normal  : NORMAL;
    float4  Color   : COLOR;
    float2  UV0     : TEXCOORD0;
};

struct VS_TERRAIN_GRASS_OUTPUT
{
    float4  Pos     : POSITION;
    float2  UV0     : TEXCOORD0;

    float4  Color   : COLOR;
};

VS_TERRAIN_GRASS_OUTPUT VS(VS_TERRAIN_GRASS_INPUT i)
{
	VS_TERRAIN_GRASS_OUTPUT o;
	o.UV0 = i.UV0;
	i.Pos.x+=sin((1-i.UV0.y)*(g_fTime+i.Pos.x*0.5f+i.Pos.y)*2)*0.2f-0.5f*(1-i.UV0.y);
	o.Pos = mul(i.Pos,g_mViewProj);
	o.Color.rgb = i.Color.rgb;
	o.Color.a=1.0f;
	return o;
}

sampler s0: register(s0);
float4 PS(VS_TERRAIN_GRASS_OUTPUT i) : COLOR0
{
	float4 cDiffuse	= tex2D(s0, i.UV0);
	float4 color;
	//float4 cLight	= tex2D(g_samLight, i.UV1);
	//color.xyz=cLight.xyz;
	color = i.Color;
	color *= cDiffuse;
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