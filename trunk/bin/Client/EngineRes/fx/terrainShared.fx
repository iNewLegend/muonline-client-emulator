#ifndef __TERRAIN_SHARED_FX
#define __TERRAIN_SHARED_FX

struct VS_TERRAIN_INPUT
{
    float4  Pos     : POSITION;
    float4  Color   : COLOR;
    float2  UV0     : TEXCOORD0;
};

struct VS_TERRAIN_OUTPUT
{
    float4  Pos     : POSITION;
    float2  UV0     : TEXCOORD0;
    float4  Color   : COLOR;
};

VS_TERRAIN_OUTPUT TERRAIN_VS(VS_TERRAIN_INPUT i)
{
	VS_TERRAIN_OUTPUT o;
	o.UV0 = i.UV0;
	o.Pos = mul(i.Pos,g_mViewProj);
	o.Pos.z+=0.001f;
	o.Color = i.Color;
	return o;
}

#endif