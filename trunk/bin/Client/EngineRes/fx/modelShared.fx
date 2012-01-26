#ifndef __MODEL_SHARED_FX
#define __MODEL_SHARED_FX
#include "shared.fx"

struct VS_MODEL_INPUT
{
    float4  pos     : POSITION;
    float3  normal  : NORMAL;
    float2  uv     : TEXCOORD0;
};

struct VS_MODEL_OUTPUT
{
    float4  postion : POSITION;
    float2  uv      : TEXCOORD0;
    float4  pos     : TEXCOORD1;
    float3  normal  : TEXCOORD2;
};

struct PS_OUTPUT
{
    float4  color   : COLOR0;
    float4  pos     : COLOR1;
    float4  normal  : COLOR2;
};

VS_MODEL_OUTPUT MODEL_VS(VS_MODEL_INPUT i)
{
	VS_MODEL_OUTPUT o;
	o.uv = i.uv;
	o.postion = mul(i.pos,wvpm);
	o.normal = mul(i.normal,g_mWorld);//+1)*0.5f;
	o.pos = mul(i.pos,g_mWorld);
	return o;
}

sampler s0: register(s0);
PS_OUTPUT MODEL_PS(VS_MODEL_OUTPUT i)
{
	PS_OUTPUT o;
	o.color	= tex2D(s0, i.uv);
	o.normal = float4(i.normal,1);
	o.pos = i.pos;
	return o;
}

#endif