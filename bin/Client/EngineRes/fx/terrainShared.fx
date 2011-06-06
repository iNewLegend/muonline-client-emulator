#ifndef __TERRAIN_SHARED_FX
#define __TERRAIN_SHARED_FX

struct VS_TERRAIN_INPUT
{
    float4  Pos     : POSITION;
	float3  Normal  : NORMAL;
    float4  Color   : COLOR;
    float2  UV0     : TEXCOORD0;
    float2  UV1     : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
    float4  Pos     : POSITION;
    float2  UV0     : TEXCOORD0;
    float2  UV1     : TEXCOORD1;
    float4  Color   : COLOR;
};

VS_TERRAIN_OUTPUT TERRAIN_VS(VS_TERRAIN_INPUT i)
{
	VS_TERRAIN_OUTPUT o;
	o.UV0 = i.UV0;
	o.UV1 = i.UV1;
	o.Pos = mul(i.Pos,g_mViewProj);
	o.Pos.z+=0.001f;
	o.Color = i.Color;
	return o;
}

#endif