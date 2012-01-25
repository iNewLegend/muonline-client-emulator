#include "shared.fx"

float Luminance = 0.08f;
static const float fMiddleGray = 0.18f;
static const float fWhiteCutoff = 0.8f;

sampler s0: register(s0);
float4 PS( in float2 Tex : TEXCOORD0 ) : COLOR0
{
    float3 ColorOut = tex2D( s0, Tex );

   // ColorOut *= fMiddleGray / ( Luminance + 0.001f );
   // ColorOut *= ( 1.0f + ( ColorOut / ( fWhiteCutoff * fWhiteCutoff ) ) );
    ColorOut -= 1.0f;

    ColorOut = max( ColorOut, 0.0f );

   // ColorOut /= ( 10.0f + ColorOut );

    return float4( ColorOut, 1.0f );
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
