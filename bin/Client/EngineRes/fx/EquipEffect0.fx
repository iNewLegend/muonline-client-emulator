#include "shared.fx"
#include "modelShared.fx"

PS_OUTPUT PS(VS_MODEL_OUTPUT i)
{
	PS_OUTPUT o;
	o.color	= tex2D(s0, i.uv)*float4(0.7,1,0.7,1)*(0.8-sin(g_fTime*2)*0.2f);
	o.normal = float4(i.normal,1);
	o.pos = i.pos;
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