#ifndef __TERRAIN_SHARED_FX
#define __TERRAIN_SHARED_FX
#include "shared.fx"

struct VS_TERRAIN_INPUT
{
    float4  pos     : POSITION;
    float3  normal  : NORMAL;
    float4  color   : COLOR;
    float2  uv      : TEXCOORD0;
};

struct VS_TERRAIN_OUTPUT
{
    float4  postion : POSITION;
    float2  uv      : TEXCOORD0;
    float4  pos     : TEXCOORD1;
    float3  normal  : TEXCOORD2;
    float4  color   : COLOR;
};

struct PS_OUTPUT
{
    float4  color   : COLOR0;
    float4  pos     : COLOR1;
    float4  normal  : COLOR2;
};

VS_TERRAIN_OUTPUT TERRAIN_VS(VS_TERRAIN_INPUT i)
{
	VS_TERRAIN_OUTPUT o;
	o.postion = mul(i.pos,vpm);
	o.uv = i.uv;
	o.pos = i.pos;
	o.normal = i.normal;
	o.color = i.color;
	return o;
}

VS_TERRAIN_OUTPUT TERRAIN_GRASS_VS(VS_TERRAIN_INPUT i)
{
	VS_TERRAIN_OUTPUT o;
	o.uv = i.uv;
	o.pos = i.pos;
	float offset = sin((1-i.uv.y)*(g_fTime+i.pos.x*0.5f+i.pos.y)*2)*0.2f-0.5f*(1-i.uv.y);
	o.pos.x+=offset;
	o.postion = mul(o.pos,vpm);
	o.normal = i.normal;
	o.normal.x+=offset;
	o.color = 1;
	return o;
}

VS_TERRAIN_OUTPUT TERRAIN_VS_X2(VS_TERRAIN_INPUT i)
{
	VS_TERRAIN_OUTPUT o;
	o.postion = mul(i.pos,vpm);
	o.uv = i.uv*0.5f;
	o.pos = i.pos;
	o.normal = i.normal;
	o.color = i.color;
	return o;
}

VS_TERRAIN_OUTPUT TERRAIN_VS_X4(VS_TERRAIN_INPUT i)
{
	VS_TERRAIN_OUTPUT o;
	o.postion = mul(i.pos,vpm);
	o.uv = i.uv*0.25f;
	o.pos = i.pos;
	o.normal = i.normal;
	o.color = i.color;
	return o;
}

sampler s0: register(s0);
PS_OUTPUT TERRAIN_PS(VS_TERRAIN_OUTPUT i)
{
	PS_OUTPUT o;
	o.color	= tex2D(s0, i.uv);
	o.color.a *= i.color.a;
	o.pos = i.pos;
	o.normal = float4(i.normal,1);
	return o;
}

PS_OUTPUT TERRAIN_WATER_PS(VS_TERRAIN_OUTPUT i)
{
	PS_OUTPUT o;
	i.uv.x+=g_fTime*0.1f;
	i.uv.y+=sin(g_fTime*2+i.uv.x*5)*0.02f;
	o.color	= tex2D(s0, i.uv);
	o.color.a *= i.color.a;
	o.pos = i.pos;
	o.normal = float4(i.normal,1);
	return o;
}

struct _VS_OUTPUT
{
    float4  pos		: POSITION;
    float2  uv		: TEXCOORD0;
    float3  LightDir: TEXCOORD2;
	float3  ViewDir	: TEXCOORD3;
    float4  color	: COLOR;
};

_VS_OUTPUT _VS(VS_TERRAIN_INPUT i)
{
	_VS_OUTPUT o;
	o.uv = i.uv;
	float3 vBinormal	= normalize(cross(float3(0,0,1),i.normal));//normalize(cross(i.normal,float3(0,0,1)));
	float3 vTangent		= normalize(cross(i.normal,float3(1,0,0)));//normalize(cross(float3(1,0,0),i.normal));
	float3x3 mTangent = transpose(float3x3(vBinormal,i.normal,vTangent));
	o.LightDir = normalize(mul(mTangent,-g_vLightDir));
	o.ViewDir= normalize(mul(mTangent,i.pos.xyz-g_vEyePot));
	o.pos = mul(i.pos,vpm);
	o.color = i.color;
	return o;
}
sampler s1: register(s1);
sampler s2: register(s2);
float4 _PS(_VS_OUTPUT i) : COLOR0
{
	float4 cDiffuse	= tex2D(s0, i.uv);
	float3 cLight	= i.color.rgb;//tex2D(g_samLight, i.UV1);
	float3 vNormal	= normalize(tex2D(s1, i.uv).xzy-0.5);
	//vNormal.z=-vNormal.z;
	float4 color;
	color.a = i.color.a;
	color.xyz = saturate(dot(vNormal, i.LightDir))*0.8+0.2;
	color.xyz*=cLight;
		//return color;
	color *= cDiffuse;

	{
		float3 cSpecular= tex2D(s2, i.uv).xyz;
		float3 reflVec = normalize(reflect(normalize(i.ViewDir), vNormal));
		float fSpec = pow(saturate(dot(reflVec, normalize(i.LightDir))),32)*cLight;
		//reflVec = normalize(reflect(-reflVec, normalize(i.LightDir)));
		//float3 cEnvironment = texCUBE(g_samEnvironment, reflVec-i.LightDir).xyz;
		//color.xyz +=cEnvironment*cSpecular;
		color.xyz +=cSpecular*fSpec;
	}
	return color;
}
#endif