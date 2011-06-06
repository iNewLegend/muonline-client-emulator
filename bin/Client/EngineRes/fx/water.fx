#include "shared.fx"

struct VS_INPUT
{
    float4  Pos     : POSITION;
    float2  UV0     : TEXCOORD0;
    float2  UV2     : TEXCOORD2;
    float3  Normal  : NORMAL;
    float4  Color   : COLOR;
};

struct VS_OUTPUT
{
    float4  Pos     : POSITION;
	float3  Normal  : TEXCOORD3;
    float3  UVBump0 : TEXCOORD7;
	float2  UVBump1 : TEXCOORD4;
    float2  UVLight : TEXCOORD6;
    float3  View    : TEXCOORD5;
    float4  Color   : COLOR;
};

VS_OUTPUT VS(VS_INPUT i)
{
	VS_OUTPUT o;
	o.UVBump0 = i.Pos.xyz * 0.06;
   o.UVBump0.xz += 0.01f * g_fTime;
   o.UVBump0.y += o.UVBump0.z + 0.01f * g_fTime;
   

	o.UVBump1 = i.UV0*0.8;
	o.UVBump1.x += 	g_fTime*0.06;
	//o.UVBump1.y += 	g_fTime*0.04;
	o.Normal = i.Normal;
	o.UVLight = i.UV2;
//i.Pos.y+=sin(i.Pos.x+i.Pos.z+g_fTime)*0.01;
	o.Pos = mul(i.Pos,g_mViewProj);
	o.View = i.Pos.xyz-g_vEyePot;
	
	//o.UV1 =  mul(i.Pos,g_mWorldViewProj).xz;//+o.Normal.xy+0.5;
	o.Color = i.Color;
	return o;
}
float4 PS(VS_OUTPUT i) : COLOR0
{
	float4 color;
	float3 vBump0	= tex3D(g_samDiffuse, i.UVBump0).xzy;
	float4 cLight	= tex2D(g_samLight, i.UVLight);
	float3 vBump1	= tex2D(g_samNormal, i.UVBump1).xyz;
	//cLight.xy =i.UV0;
	float3 bump = (2.0 * (vBump0) - 1.0);
	bump.xz *= 0.15;
	//bump.z=-bump.z;
	bump.y = 0.8 * abs(bump.y) + 0.2;
	//bump*=0.00001;
	//bump+=float3(-1,0,0);
	bump = normalize(i.Normal+bump);
	color.xyz=cLight.xyz;
	float3 normView = normalize(i.View);
	float3 reflVec = reflect(normView, bump);//normalize((vBump0+vBump1)*2-2));
	reflVec.z = -reflVec.z;
	float4 cEnvironment = texCUBE(g_samEnvironment,  reflVec);//+sin(g_fTime)*0.1

	color.xyz *= cEnvironment.xyz;//*float3(0.8,0.7,1);//+cEnvironment;
	color.a = i.Color.a;

	return color;
}

technique Render
{
    pass P0
    {
        VertexShader = compile vs_1_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}