#ifndef __SHARED_FX
#define __SHARED_FX

shared float4x4 g_mView;
shared float4x4 g_mViewProj;
shared float4x4 g_mWorld;
shared float4x4 g_mWorldViewProj;
shared float4x4 g_mLightView;
shared float3 g_vEyePot;
shared float3 g_vViewDir;
shared float3 g_vLightDir;
shared float3 g_vDiffuse;
shared float3 g_vSpecular;
shared float2 g_fScaleUV;
shared float g_fPower;
shared float g_fTime;

		
shared texture	g_tex0;
shared texture	g_tex1;
shared texture	g_tex2;
shared texture	g_tex3;
shared texture	g_tex4;
shared texture	g_tex5;
shared texture	g_tex6;
shared texture	g_tex7;

shared texture	g_texDiffuse;
shared texture	g_texLight;
shared texture	g_texNormal;
shared texture	g_texEnvironment;
shared texture	g_texEmissive;
shared texture	g_texSpecular;
shared texture	g_texScene;

sampler g_samDiffuse=
sampler_state
{
    Texture = <g_texDiffuse>;
    AddressU = WRAP;
    AddressV = WRAP;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler g_samLight=
sampler_state
{
    Texture = <g_texLight>;
    AddressU = WRAP;
    AddressV = WRAP;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler g_samNormal=
sampler_state
{
    Texture = <g_texNormal>;
    AddressU = WRAP;
    AddressV = WRAP;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler g_samEnvironment=
sampler_state
{
    Texture = <g_texEnvironment>;
    AddressU = WRAP;
    AddressV = WRAP;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler g_samEmissive=
sampler_state
{
    Texture = <g_texEmissive>;
    AddressU = WRAP;
    AddressV = WRAP;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler g_samSpecular=
sampler_state
{
    Texture = <g_texSpecular>;
    AddressU = WRAP;
    AddressV = WRAP;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler g_samScene=
sampler_state
{
    Texture = <g_texScene>;
    AddressU = WRAP;
    AddressV = WRAP;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = POINT;
};

/*
sampler g_samDiffuse=
sampler_state
{
    Texture = <g_tex0>;
    AddressU = BORDER;
    AddressV = BORDER;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler g_samLight=
sampler_state
{
    Texture = <g_tex1>;
    AddressU = WRAP;
    AddressV = WRAP;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler g_samNormal=
sampler_state
{
    Texture = <g_tex2>;
    AddressU = WRAP;
    AddressV = WRAP;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler g_samEnvironment=
sampler_state
{
    Texture = <g_tex3>;
    AddressU = WRAP;
    AddressV = WRAP;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler g_samEmissive=
sampler_state
{
    Texture = <g_tex4>;
    AddressU = WRAP;
    AddressV = WRAP;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler g_samSpecular=
sampler_state
{
    Texture = <g_tex4>;
    AddressU = WRAP;
    AddressV = WRAP;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};*/
technique Shared
{
    pass P0
    {
        VertexShader = NULL;
        PixelShader  = NULL;
    }
}
#endif