#include "shared.fx"

float2 PixelCoordsDownFilter[16] =
{
    { 1.5,  -1.5 },
    { 1.5,  -0.5 },
    { 1.5,   0.5 },
    { 1.5,   1.5 },

    { 0.5,  -1.5 },
    { 0.5,  -0.5 },
    { 0.5,   0.5 },
    { 0.5,   1.5 },

    {-0.5,  -1.5 },
    {-0.5,  -0.5 },
    {-0.5,   0.5 },
    {-0.5,   1.5 },

    {-1.5,  -1.5 },
    {-1.5,  -0.5 },
    {-1.5,   0.5 },
    {-1.5,   1.5 },
};

float2 TexelCoordsDownFilter[16]
<
    string ConvertPixelsToTexels = "PixelCoordsDownFilter";
>;

sampler s0: register(s0);
float4 DownFilter( in float2 Tex : TEXCOORD0 ) : COLOR0
{
    float4 color = 0;
    for (int i = 0; i < 16; i++)
    {
        color += tex2D( s0, Tex + PixelCoords[i]/*PixelCoordsDownFilter[i].xy*inv_width_height*/ );
    }
	color *= (1.0f / 16.0f);
    return color;
}

technique Render
{
    pass P0
    {
		CullMode		= None;
		AlphaTestEnable		= False;
		AlphaBlendEnable	= False;
		ZEnable			= False;
		ZWriteEnable		= False;
		
        VertexShader = null;
        PixelShader = compile ps_2_0 DownFilter();
    }
}
