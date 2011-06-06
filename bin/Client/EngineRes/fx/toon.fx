shared float4x4 g_mViewProj;
shared float4x4 g_mWorldViewProj;
shared float4x4 g_mLightView;
shared float3 g_vEyePot;
shared float3 g_vViewDir;
shared float3 g_vLightDir;
shared float3 g_vDiffuse;
shared float3 g_vSpecular;
shared float g_fPower;

// Ä¬ÈÏÎÆÀí
texture		g_tx1;
texture		g_tx2;

sampler g_sam1=
sampler_state
{
    Texture = <g_tx1>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler g_sam2=
sampler_state
{
    Texture = <g_tx2>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

struct VS_INPUT
{
    float3  Pos     : POSITION;
    float2  UV      : TEXCOORD;
    float3  Normal  : NORMAL;
};

struct VS_OUTPUT
{
    float4  Pos     : POSITION;
    float2  UV0     : TEXCOORD0;
    float2  UV1     : TEXCOORD1;
   // float3  Normal  : TEXCOORD2;
    float4  Diffuse : COLOR;
};

VS_OUTPUT VS(VS_INPUT i)
{
	VS_OUTPUT o;
	o.UV0 = i.UV;
	o.Pos = mul( float4(i.Pos, 1),g_mViewProj);
	float u = dot(-g_vLightDir, i.Normal);
	o.UV1.x = 1;//max(u,0.1);
	//float3 v = mul( float4(i.Pos, 1), g_mLightView );
	o.UV1.y = o.Pos.y;
	//o.Normal = i.Normal;
	float3 vReflect = normalize(reflect(-g_vLightDir,i.Normal));
	float s = 0;//pow( max(dot(normalize(g_vViewDir), vReflect ),0), g_fPower);
	o.Diffuse = float4(g_vDiffuse+g_vSpecular*s,1);
	return o;
}
float4 PS( in float2 Tex0 : TEXCOORD0, in float2 Tex1 : TEXCOORD1, in float3 normal : TEXCOORD2, in float4 diffuse : COLOR ) : COLOR0
{
	float4 color1 = tex2D( g_sam1, Tex1);
	float4 color2 = tex2D( g_sam2, Tex0);

	float u = saturate( dot(-g_vLightDir, normalize(normal) ) );
	u = u<0.5?0.5:u<0.985?0.75:1;
	u=0;

	return diffuse;//diffuse*0.6+diffuse*u*0.6;//diffuse*(u<0.5?0.5:u<0.985?0.75:1);//color1;//*color2;
}

technique Render
{
    pass P0
    {
        VertexShader = compile vs_1_0 VS();
        PixelShader  = null;//compile ps_2_0 PS();
    }
}