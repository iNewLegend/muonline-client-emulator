#include "shared.fx"
#include "terrainShared.fx"

float4 PS(VS_TERRAIN_OUTPUT i) : COLOR0
{
	float4 cDiffuse	= tex2D(g_samDiffuse, i.UV0);
	float4 color;
	//float4 cLight	= tex2D(g_samLight, i.UV1);
	//color.xyz=cLight.xyz;
	color = i.Color;
	color *= cDiffuse;
	return color;
}

technique Render
{
    pass P0
    {
        VertexShader = compile vs_1_1 TERRAIN_VS();
        PixelShader  = compile ps_2_0 PS();
    }
}