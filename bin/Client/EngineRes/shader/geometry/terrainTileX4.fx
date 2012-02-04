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

		ColorOp[0]			= Modulate;
		ColorArg1[0]		= Texture;
		ColorArg2[0]		= Diffuse;

		AlphaOp[0]			= Disable;

		ColorOp[1]			= Disable;
		AlphaOP[1]			= Disable;
		
        VertexShader = compile vs_1_1 TERRAIN_VS_X4();
        PixelShader  = compile ps_2_0 TERRAIN_PS();
    }
}