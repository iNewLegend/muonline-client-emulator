#include "shared.fx"
#include "terrainShared.fx"

struct VS_OUTPUT
{
    float4  Pos		: POSITION;
    float2  UV0		: TEXCOORD0;
    float2  UV1		: TEXCOORD1;
    float3  LightDir: TEXCOORD2;
	float3  ViewDir	: TEXCOORD3;
    float4  Color	: COLOR;
};

VS_OUTPUT VS(VS_TERRAIN_INPUT i)
{
	VS_OUTPUT o;
	o.UV0 = i.UV0*g_fScaleUV;
	o.UV1 = i.UV1;
	float3 vBinormal	= normalize(cross(float3(0,0,1),i.Normal));//normalize(cross(i.Normal,float3(0,0,1)));
	float3 vTangent		= normalize(cross(i.Normal,float3(1,0,0)));//normalize(cross(float3(1,0,0),i.Normal));
	float3x3 mTangent = transpose(float3x3(vBinormal,i.Normal,vTangent));
	o.LightDir = normalize(mul(mTangent,-g_vLightDir));
	o.ViewDir= normalize(mul(mTangent,i.Pos.xyz-g_vEyePot));
	o.Pos = mul(i.Pos,g_mViewProj);
	o.Color = i.Color;
	return o;
}

float4 PS(VS_OUTPUT i) : COLOR0
{
	float4 cDiffuse	= tex2D(g_samDiffuse, i.UV0);
	float3 cLight	= i.Color.xyz;//tex2D(g_samLight, i.UV1);
	float3 vNormal	= normalize(tex2D(g_samNormal, i.UV0).xzy-0.5);
	//vNormal.z=-vNormal.z;
	float4 color;
	color.a = i.Color.a;
	color.xyz = saturate(dot(vNormal, i.LightDir))*0.8+0.2;
	color.xyz*=cLight;
		//return color;
	color *= cDiffuse;

	{
		float3 cSpecular= tex2D(g_samSpecular, i.UV0).xyz;
		float3 reflVec = normalize(reflect(normalize(i.ViewDir), vNormal));
		float fSpec = pow(saturate(dot(reflVec, normalize(i.LightDir))),32)*cLight;
		//reflVec = normalize(reflect(-reflVec, normalize(i.LightDir)));
		//float3 cEnvironment = texCUBE(g_samEnvironment, reflVec-i.LightDir).xyz;
		//color.xyz +=cEnvironment*cSpecular;
		color.xyz +=cSpecular*fSpec;
	}
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