#include "shared.fx"
#include "terrainShared.fx"

VS_TERRAIN_OUTPUT VS(VS_TERRAIN_INPUT i)
{
	VS_TERRAIN_OUTPUT o;
	o.UV0 = i.UV0*0.25f;
	o.UV1 = i.UV1;
	o.Pos = mul(i.Pos,g_mViewProj);
	o.Pos.z+=0.001f;
	o.Color = i.Color;
	return o;
}

sampler s0: register(s0);
float4 PS(VS_TERRAIN_OUTPUT i) : COLOR0
{
	i.UV0.x+=g_fTime*0.1f;
	i.UV0.y+=sin(g_fTime*2+i.UV0.x*5)*0.02f;
	float4 color	= tex2D(s0, i.UV0);
	return color*i.Color;
}

technique Render
{
    pass P0
    {
		Lighting			= False;
		CullMode			= None;

		AlphaTestEnable		= True;
		AlphaFunc			= GreaterEqual;
		AlphaRef			= 64;

		AlphaBlendEnable	= True;
		BlendOp				= Add;
		SrcBlend			= SrcAlpha;
		DestBlend			= InvSrcAlpha;

		ZEnable				= True;
		ZFunc				= LessEqual;
		ZWriteEnable		= True;

        VertexShader		= compile vs_1_0 VS();
        PixelShader			= compile ps_2_0 PS();
    }
}