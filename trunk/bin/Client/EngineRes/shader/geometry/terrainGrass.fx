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
		
		VertexShader		= compile vs_2_0 TERRAIN_GRASS_VS();
		PixelShader			= compile ps_2_0 TERRAIN_PS();
    }
}