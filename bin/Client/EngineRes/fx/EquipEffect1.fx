#include "shared.fx"
#include "modelShared.fx"

float furriness = 60.0f;
float sheen = 0.68f;
PS_OUTPUT PS(VS_MODEL_OUTPUT i)
{
	PS_OUTPUT o;
	o.color	= tex2D(s0, i.uv);
	o.normal = float4(i.normal,1);
	o.pos = i.pos;
	float3 V = -normalize(i.pos - g_vEyePot);
	float cosView = clamp(dot(V, i.normal), 0.0, 1.0);
	float shine = pow(1.0 - cosView * cosView, furriness);
	o.color.xyz+=sheen * shine;
	return o;
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

        VertexShader		= compile vs_2_0 MODEL_VS();
        PixelShader			= compile ps_2_0 PS();
    }
}