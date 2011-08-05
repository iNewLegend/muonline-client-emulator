technique Render
{
    pass P0
    {
		Lighting			= False;
		CullMode			= None;

		AlphaFunc			= Never;

		AlphaBlendEnable	= False;

		ZEnable				= False;
		ZFunc				= Never;
		ZWriteEnable		= False;

		ColorOp[0]			= Modulate;
		ColorArg1[0]		= Diffuse;
		ColorArg2[0]		= Texture;

		AlphaOp[0]			= SelectArg1;
		AlphaArg1[0]		= Diffuse;

		ColorOp[1]			= Disable;
		AlphaOP[1]			= Disable;

        VertexShader		= NULL;
        PixelShader			= NULL;
    }
}