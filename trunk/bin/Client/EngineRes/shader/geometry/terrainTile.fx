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
		
        VertexShader = compile vs_2_0 TERRAIN_VS();
        PixelShader  = compile ps_2_0 TERRAIN_PS();
    }
}