#include "shared.fx"
#include "terrainShared.fx"

technique Render
{
    pass P0
    {
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
		
        VertexShader = compile vs_1_1 TERRAIN_VS_X4();
        PixelShader  = compile ps_2_0 TERRAIN_PS();
    }
}