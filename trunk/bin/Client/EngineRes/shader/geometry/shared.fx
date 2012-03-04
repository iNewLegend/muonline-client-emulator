#ifndef __SHARED_FX
#define __SHARED_FX

shared float4x4 lvm;
shared float4x4 wvpm;
shared float4x4 wvm;
shared float4x4 vpm;
shared float4x4 vm;
shared float4x4 g_mWorld;

shared float3 gColor;
shared float3 g_vEyePot;
shared float3 g_vViewDir;
shared float3 g_vLightDir;
shared float3 g_vDiffuse;
shared float3 g_vSpecular;
shared float2 inv_width_height;
shared float g_fPower;
shared float g_fTime;
shared float4 ld;

float2 PixelCoords[16];

technique Shared
{
    pass P0
    {
        VertexShader = NULL;
        PixelShader  = NULL;
    }
}
#endif