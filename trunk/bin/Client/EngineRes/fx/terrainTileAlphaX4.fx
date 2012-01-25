#include "shared.fx"
#include "terrainShared.fx"

technique Render
{
    pass P0
    {
		Lighting			= False;
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

		ColorOp[0]			= Modulate;
		ColorArg1[0]		= Texture;
		ColorArg2[0]		= Diffuse;

		AlphaOp[0]			= SelectArg1;
		AlphaArg1[0]		= Diffuse;

		ColorOp[1]			= Disable;
		AlphaOP[1]			= Disable;
		
        VertexShader = compile vs_1_1 TERRAIN_VS_X4();
        PixelShader  = compile ps_2_0 TERRAIN_PS();
    }
}