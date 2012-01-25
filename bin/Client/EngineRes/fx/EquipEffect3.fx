#include "shared.fx"
#include "modelShared.fx"

PS_OUTPUT PS(VS_MODEL_OUTPUT i)
{
	PS_OUTPUT o;
	o.color	= tex2D(s0, i.uv);
	o.color.rgb	+= float3(1.8,1.8,0.1)*(sin(i.normal.x*8+i.pos.x*10+g_fTime*3)+cos(i.normal.z*10+i.pos.z*7+g_fTime*3));
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