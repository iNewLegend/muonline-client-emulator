#include "shared.fx"

float Luminance = 0.08f;
static const float fMiddleGray = 0.18f;
static const float fWhiteCutoff = 0.8f;

float fBrightPassThreshold = 0.8;             // Values greater than this are accepted for the bright pass

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


sampler s0: register(s0);
float4 PS( in float2 Tex : TEXCOORD0 ) : COLOR0
{
    float3 color = 0;

    for (int i = 0; i < 16; i++)
    {
        color += tex2D( s0, Tex + PixelCoordsDownFilter[i].xy*inv_width_height );
    }

    color *= ( 1.0f / 16.0f );

    float luminance = max( color.r, max( color.g, color.b ) );
    if( luminance < fBrightPassThreshold )
        color = float3(0.0f, 0.0f, 0.0f);

    return float4( color, 1.0f );
}

technique Render
{
    pass P0
    {
		Lighting			= False;
		CullMode			= None;
		
		AlphaTestEnable		= False;

		AlphaBlendEnable	= False;

		ZEnable				= False;
		ZFunc				= LessEqual;
		ZWriteEnable		= False;
		
        VertexShader = null;
        PixelShader = compile ps_2_0 PS();
    }
}
