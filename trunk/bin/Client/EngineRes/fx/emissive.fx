technique Render
{
    pass P0
    {
		Lighting			= False;
		CullMode			= None;

		AlphaFunc			= Never;

		AlphaBlendEnable	= True;
		BlendOp				= Add;
		SrcBlend			= One;
		DestBlend			= One;

		ZEnable				= True;
		ZFunc				= GREATEREQUAL;
		ZWriteEnable		= True;

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