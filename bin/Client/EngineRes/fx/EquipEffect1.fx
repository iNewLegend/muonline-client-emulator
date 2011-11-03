#include "shared.fx"

struct VS_MODEL_INPUT
{
    float4  Pos     : POSITION;
	float3  Normal  : NORMAL;
    float2  UV     : TEXCOORD0;
};

struct VS_MODEL_OUTPUT
{
    float4  Pos     : POSITION;
    float2  UV      : TEXCOORD0;
    float3  normal  : TEXCOORD1;
    float3  viewVec : TEXCOORD2;
};

VS_MODEL_OUTPUT VS(VS_MODEL_INPUT i)
{
	VS_MODEL_OUTPUT o;
	o.Pos = mul(i.Pos,wvpm);
	o.UV = i.UV;
	// Eye-soace lighting
	o.normal = normalize(mul(i.Normal,wvm));
	o.viewVec = -mul(i.Pos,wvm);
	return o;
}
//float4 lightDir: register(c2);
float furriness = 25.0f;
float sheen = 0.68f;
sampler s0: register(s0);
float4 PS(VS_MODEL_OUTPUT i) : COLOR0
{
	float4 color	= tex2D(s0, i.UV);
	// Apply some fabric style lighting
	float diffuse = 0.5 * (1 + dot(i.normal, -g_vLightDir));
	float cosView = clamp(dot(normalize(i.viewVec), i.normal), 0.0, 1.0);
	float shine = pow(1.0 - cosView * cosView, furriness);

	color.xyz+=sheen * shine;
	//color.xyz*=diffuse;
	//return float4(i.viewVec,1);
	return color;
}

technique Render
{
    pass P0
    {
		Lighting			= True;
		CullMode			= None;

		AlphaTestEnable		= True;
		AlphaFunc			= GreaterEqual;
		AlphaRef			= 128;

		AlphaBlendEnable	= False;

		ZEnable				= True;
		ZFunc				= LessEqual;
		ZWriteEnable		= True;

        VertexShader		= compile vs_2_0 VS();
        PixelShader			= compile ps_2_0 PS();
    }
}