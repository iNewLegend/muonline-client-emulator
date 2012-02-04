#include "shared.fx"
#include "terrainShared.fx"

technique Render
{
    pass P0
    {
		Lighting			= False;
		CullMode			= None;

		AlphaTestEnable		= False;
		AlphaBlendEnable	= False;

		ZEnable				= True;
		ZFunc				= LessEqual;
		ZWriteEnable		= True;

        VertexShader		= compile vs_1_0 TERRAIN_VS_X4();
        PixelShader			= compile ps_2_0 TERRAIN_WATER_PS();
    }
}