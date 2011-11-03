#include "shared.fx"

struct VS_MODEL_INPUT
{
    float4  Pos     : POSITION;
	float3  Normal  : NORMAL;
    float2  UV0     : TEXCOORD0;
};

struct VS_MODEL_OUTPUT
{
    float4  Pos     : POSITION;
    float2  UV0     : TEXCOORD0;
    float2  UV1     : TEXCOORD1;
    float4  Color   : COLOR;
};

VS_MODEL_OUTPUT VS(VS_MODEL_INPUT i)
{
	VS_MODEL_OUTPUT o;
	o.UV0 = i.UV0;
	//o.UV1 = mul(i.Normal,g_mView).xz+i.Pos.xz+g_fTime*0.5f;
	o.UV1 = i.Normal.xz+i.Pos.xz+g_fTime*0.3f;
	o.Pos = mul(i.Pos,wvpm);
	o.Color = 1;
	return o;
}

sampler s0: register(s0);
float4 PS(VS_MODEL_OUTPUT i) : COLOR0
{
	float4 color	= tex2D(s0, i.UV0)+(float4(0.5,0.5,0.1,1)*((sin(i.UV1.x*10)+sin(i.UV1.y*10)+2.0f)*0.5f));
	return color*i.Color;
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

        VertexShader		= compile vs_1_0 VS();
        PixelShader			= compile ps_2_0 PS();
    }
}