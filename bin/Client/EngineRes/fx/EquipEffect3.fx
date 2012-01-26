#include "shared.fx"
#include "modelShared.fx"

PS_OUTPUT PS(VS_MODEL_OUTPUT i)
{
	PS_OUTPUT o;
	o.color	= tex2D(s0, i.uv);
	o.color.rgb	+= float3(0.8,0.8,0.1)*pow(sin(i.normal.x*12+i.pos.x*10+g_fTime*3)+sin(i.normal.z*10+i.pos.z*13+g_fTime*3),4);
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