#include "shared.fx"
#include "terrainShared.fx"

technique Render
{
    pass P0
    {
		CullMode			= None;

		AlphaTestEnable		= False;
		AlphaBlendEnable	= False;

		ZEnable				= True;
		ZFunc				= LessEqual;
		ZWriteEnable		= True;
		
        VertexShader = compile vs_1_1 TERRAIN_VS_X2();
        PixelShader  = compile ps_2_0 TERRAIN_PS();
    }
}