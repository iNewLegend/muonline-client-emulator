technique Render
{
    pass P0
    {
		Lighting			= False;
		CullMode			= None;

		AlphaTestEnable		= False;

		AlphaBlendEnable	= True;
		BlendOp				= Add;
		SrcBlend			= One;
		DestBlend			= One;

		ZEnable				= True;
		ZFunc				= LessEqual;
		ZWriteEnable		= False;

		ColorOp[0]			= Modulate;
		ColorArg1[0]		= Texture;
		ColorArg2[0]		= Diffuse;

		AlphaOp[0]			= SelectArg1;
		AlphaArg1[0]		= Diffuse;

		ColorOp[1]			= Disable;
		AlphaOP[1]			= Disable;

        VertexShader		= NULL;
        PixelShader			= NULL;
    }
}