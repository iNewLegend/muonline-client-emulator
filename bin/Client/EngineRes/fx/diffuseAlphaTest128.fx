#include "shared.fx"
#include "modelShared.fx"

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

        VertexShader = compile vs_2_0 MODEL_VS();
        PixelShader  = compile ps_2_0 MODEL_PS();
    }
}