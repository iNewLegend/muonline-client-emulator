shared float4x4 g_mViewProj;
shared float4x4 g_mView;
shared float4x4 g_mWorldViewProj;
shared float3 g_vEyePot;
shared float3 g_vViewDir;
shared float3 g_vLightDir;
shared float4 g_vDiffuse;

struct VS_INPUT
{
    float3  Pos     : POSITION;
    float3  Normal  : NORMAL;
};

struct VS_OUTPUT
{
    float4  Pos     : POSITION;
};

VS_OUTPUT VS(VS_INPUT i)
{
	VS_OUTPUT o;
	o.Pos = mul( float4(i.Pos,1),g_mViewProj);
	float3 normal = normalize(mul( i.Normal, g_mViewProj));
	o.Pos.xy+=normal.xy*o.Pos.w*0.005;
	return o;
}

technique Render
{
    pass P0
    {
        VertexShader = compile vs_1_0 VS();
        PixelShader  = null;
    }
}