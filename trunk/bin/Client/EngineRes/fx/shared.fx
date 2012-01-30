#ifndef __SHARED_FX
#define __SHARED_FX

shared float4x4 wvpm;
shared float4x4 wvm;
shared float4x4 vpm;
shared float4x4 g_mView;
shared float4x4 g_mViewProj;
shared float4x4 g_mWorld;
shared float4x4 g_mWorldViewProj;
shared float4x4 g_mLightView;
shared float3 g_vPointLight;
shared float3 g_vEyePot;
shared float3 g_vViewDir;
shared float3 g_vLightDir;
shared float3 g_vDiffuse;
shared float3 g_vSpecular;
shared float2 g_fScaleUV;
shared float2 inv_width_height;
shared float g_fPower;
shared float g_fTime;

technique Shared
{
    pass P0
    {
        VertexShader = NULL;
        PixelShader  = NULL;
    }
}
#endif