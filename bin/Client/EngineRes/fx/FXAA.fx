#include "shared.fx"
//float		g_fsceneWeight;
//float		g_fGrayScale;
//float		g_fGrayBase;
sampler RT: register( s0 );
sampler s1: register(s1);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// FXAA

#ifndef FXAA_PRESET
    #define FXAA_PRESET 0
#endif
/*--------------------------------------------------------------------------*/
#if (FXAA_PRESET == 0)
    #define FXAA_EDGE_THRESHOLD      (1.0/4.0)
    #define FXAA_EDGE_THRESHOLD_MIN  (1.0/12.0)
    #define FXAA_SEARCH_STEPS        2
    #define FXAA_SEARCH_ACCELERATION 4
    #define FXAA_SEARCH_THRESHOLD    (1.0/4.0)
    #define FXAA_SUBPIX              1
    #define FXAA_SUBPIX_FASTER       0
    #define FXAA_SUBPIX_CAP          (2.0/3.0)
    #define FXAA_SUBPIX_TRIM         (1.0/4.0)
#endif
/*--------------------------------------------------------------------------*/
#if (FXAA_PRESET == 1)
    #define FXAA_EDGE_THRESHOLD      (1.0/8.0)
    #define FXAA_EDGE_THRESHOLD_MIN  (1.0/16.0)
    #define FXAA_SEARCH_STEPS        4
    #define FXAA_SEARCH_ACCELERATION 3
    #define FXAA_SEARCH_THRESHOLD    (1.0/4.0)
    #define FXAA_SUBPIX              1
    #define FXAA_SUBPIX_FASTER       0
    #define FXAA_SUBPIX_CAP          (3.0/4.0)
    #define FXAA_SUBPIX_TRIM         (1.0/4.0)
#endif
/*--------------------------------------------------------------------------*/
#if (FXAA_PRESET == 2)
    #define FXAA_EDGE_THRESHOLD      (1.0/8.0)
    #define FXAA_EDGE_THRESHOLD_MIN  (1.0/24.0)
    #define FXAA_SEARCH_STEPS        8
    #define FXAA_SEARCH_ACCELERATION 2
    #define FXAA_SEARCH_THRESHOLD    (1.0/4.0)
    #define FXAA_SUBPIX              1
    #define FXAA_SUBPIX_FASTER       0
    #define FXAA_SUBPIX_CAP          (3.0/4.0)
    #define FXAA_SUBPIX_TRIM         (1.0/4.0)
#endif
/*--------------------------------------------------------------------------*/
#if (FXAA_PRESET == 3)
    #define FXAA_EDGE_THRESHOLD      (1.0/8.0)
    #define FXAA_EDGE_THRESHOLD_MIN  (1.0/24.0)
    #define FXAA_SEARCH_STEPS        16
    #define FXAA_SEARCH_ACCELERATION 1
    #define FXAA_SEARCH_THRESHOLD    (1.0/4.0)
    #define FXAA_SUBPIX              1
    #define FXAA_SUBPIX_FASTER       0
    #define FXAA_SUBPIX_CAP          (3.0/4.0)
    #define FXAA_SUBPIX_TRIM         (1.0/4.0)
#endif
/*--------------------------------------------------------------------------*/
#if (FXAA_PRESET == 4)
    #define FXAA_EDGE_THRESHOLD      (1.0/8.0)
    #define FXAA_EDGE_THRESHOLD_MIN  (1.0/24.0)
    #define FXAA_SEARCH_STEPS        24
    #define FXAA_SEARCH_ACCELERATION 1
    #define FXAA_SEARCH_THRESHOLD    (1.0/4.0)
    #define FXAA_SUBPIX              1
    #define FXAA_SUBPIX_FASTER       0
    #define FXAA_SUBPIX_CAP          (3.0/4.0)
    #define FXAA_SUBPIX_TRIM         (1.0/4.0)
#endif
/*--------------------------------------------------------------------------*/
#if (FXAA_PRESET == 5)
    #define FXAA_EDGE_THRESHOLD      (1.0/8.0)
    #define FXAA_EDGE_THRESHOLD_MIN  (1.0/24.0)
    #define FXAA_SEARCH_STEPS        32
    #define FXAA_SEARCH_ACCELERATION 1
    #define FXAA_SEARCH_THRESHOLD    (1.0/4.0)
    #define FXAA_SUBPIX              1
    #define FXAA_SUBPIX_FASTER       0
    #define FXAA_SUBPIX_CAP          (3.0/4.0)
    #define FXAA_SUBPIX_TRIM         (1.0/4.0)
#endif
/*--------------------------------------------------------------------------*/
#define FXAA_SUBPIX_TRIM_SCALE (1.0/(1.0 - FXAA_SUBPIX_TRIM))


float3 FxaaFilterReturn(float3 rgb) {
    #if FXAA_SRGB_ROP
        // Do sRGB encoded value to linear conversion.
        return FxaaSel3(
            rgb * FxaaToFloat3(1.0/12.92), 
            FxaaPow3(
                rgb * FxaaToFloat3(1.0/1.055) + FxaaToFloat3(0.055/1.055), 
                FxaaToFloat3(2.4)),
            rgb > FxaaToFloat3(0.04045)); 
    #else
        return rgb;
    #endif
}
 
/*--------------------------------------------------------------------------*/
float4 FxaaTexGrad(sampler2D tex, float2 pos, float2 grad) {
       return tex2Dgrad(tex, pos.xy, grad, grad); 
}

//void TemporalAAVS(
//   in float4 InPosition : POSITION,
//   out float4 OutScreenPosition : TEXCOORD0,
//   out float4 OutPosition : POSITION
//   )
//{
//   OutPosition = InPosition;
//   OutScreenPosition = InPosition;
//}

// 1/src_width, 1/height


//texture color_tex;
//sampler RT = sampler_state
//{
//    Texture = <color_tex>;
//    MinFilter = anisotropic;
 //   MagFilter = Linear;
//    MipFilter = Point;
//    AddressU = CLAMP;
//    AddressV = CLAMP;
//};

// Return the luma, the estimation of luminance from rgb inputs.
// This approximates luma using one FMA instruction,
// skipping normalization and tossing out blue.
// FxaaLuma() will range 0.0 to 2.963210702.
float FxaaLuma(float3 rgb)
{
   return rgb.y * (0.587f / 0.299f) + rgb.x;
}

float3 FxaaLerp3(float3 a, float3 b, float amountOfA) {
    return ((-amountOfA) * b) + 
        ((a * (amountOfA)) + b); } 

float3 DebugFxaaPixelShader(float2 inpos)
{
   float2 pos = inpos.xy;// * ScreenPositionScaleBias.xy + ScreenPositionScaleBias.wz;


   float3 rgbN = tex2Dlod(RT, float4(pos.xy + float2(+0, -1) * inv_width_height, 0,0) ).xyz;
   float3 rgbW = tex2Dlod(RT, float4(pos.xy + float2(-1, +0) * inv_width_height, 0,0) ).xyz;
   float3 rgbM = tex2Dlod(RT, float4(pos.xy + float2(+0, +0) * inv_width_height, 0,0) ).xyz;
   float3 rgbE = tex2Dlod(RT, float4(pos.xy + float2(+1, +0) * inv_width_height, 0,0) ).xyz;
   float3 rgbS = tex2Dlod(RT, float4(pos.xy + float2(+0, +1) * inv_width_height, 0,0) ).xyz;
   float lumaN = FxaaLuma(rgbN);
   float lumaW = FxaaLuma(rgbW);
   float lumaM = FxaaLuma(rgbM);
   float lumaE = FxaaLuma(rgbE);
   float lumaS = FxaaLuma(rgbS);
   float rangeMin = min(lumaM, min(min(lumaN, lumaW), min(lumaS, lumaE)));
   float rangeMax = max(lumaM, max(max(lumaN, lumaW), max(lumaS, lumaE)));
   float range = rangeMax - rangeMin;
   float comp = max(FXAA_EDGE_THRESHOLD_MIN, rangeMax *FXAA_EDGE_THRESHOLD);
   if (range < comp)
   {
      return lumaM / (1.0 + (0.587f / 0.299f));
   }
   else
   {
   
       #if FXAA_SUBPIX > 0
         #if FXAA_SUBPIX_FASTER
            float3 rgbL = (rgbN + rgbW + rgbE + rgbS + rgbM) * 
               (1.0/5.0);
         #else
            float3 rgbL = rgbN + rgbW + rgbM + rgbE + rgbS;
         #endif
      #endif    
      ////////////////////////////////////////////////////////////////////////////
       #if FXAA_SUBPIX != 0
        float lumaL = (lumaN + lumaW + lumaE + lumaS) * 0.25;
        float rangeL = abs(lumaL - lumaM);
      #endif      
      
      #if FXAA_SUBPIX == 1
         float blendL = max(0.0, 
            (rangeL / range) - FXAA_SUBPIX_TRIM) * FXAA_SUBPIX_TRIM_SCALE; 
         blendL = min(FXAA_SUBPIX_CAP, blendL);
      #endif
      #if FXAA_SUBPIX == 2
         float blendL = rangeL / range; 
      #endif
   float3 rgbNW = tex2Dlod(RT, float4(pos.xy + float2(-1, -1) * inv_width_height, 0,0) ).xyz;
   float3 rgbNE = tex2Dlod(RT, float4(pos.xy + float2(+1, -1) * inv_width_height, 0,0) ).xyz;
   float3 rgbSW = tex2Dlod(RT, float4(pos.xy + float2(-1, +1) * inv_width_height, 0,0) ).xyz;
   float3 rgbSE = tex2Dlod(RT, float4(pos.xy + float2(+1, +1) * inv_width_height, 0,0) ).xyz;
    #if (FXAA_SUBPIX_FASTER == 0) && (FXAA_SUBPIX > 0)
        rgbL += (rgbNW + rgbNE + rgbSW + rgbSE);
        rgbL *= (1.0/9.0);
    #endif
   float lumaNW = FxaaLuma(rgbNW);
   float lumaNE = FxaaLuma(rgbNE);
   float lumaSW = FxaaLuma(rgbSW);
   float lumaSE = FxaaLuma(rgbSE);
   float edgeVert = abs((0.25f * lumaNW) + (-0.5f * lumaN) + (0.25f * lumaNE))
      + abs((0.50f * lumaW ) + (-1.0f * lumaM) + (0.50f * lumaE ))
      + abs((0.25f * lumaSW) + (-0.5f * lumaS) + (0.25f * lumaSE));
   float edgeHorz = abs((0.25f * lumaNW) + (-0.5f * lumaW) + (0.25f * lumaSW))
      + abs((0.50f * lumaN ) + (-1.0f * lumaM) + (0.50f * lumaS ))
      + abs((0.25f * lumaNE) + (-0.5f * lumaE) + (0.25f * lumaSE));
   bool horzSpan = edgeHorz >= edgeVert;
   if (horzSpan)
   {
      return float3(1.0f, 0.75f, 0.0f);
   }
   else
   {
      return float3(0.0f, 0.50f, 1.0f);
   }
   }
}

float3 FxaaPixelShader(float2 inpos)
{
   float2 pos = inpos.xy;// * ScreenPositionScaleBias.xy + ScreenPositionScaleBias.wz;

   float3 rgbN = tex2Dlod(RT, float4(pos.xy + float2(+0, -1) * inv_width_height, 0,0) ).xyz;
   float3 rgbW = tex2Dlod(RT, float4(pos.xy + float2(-1, +0) * inv_width_height, 0,0) ).xyz;
   float3 rgbM = tex2Dlod(RT, float4(pos.xy + float2(+0, +0) * inv_width_height, 0,0) ).xyz;
   float3 rgbE = tex2Dlod(RT, float4(pos.xy + float2(+1, +0) * inv_width_height, 0,0) ).xyz;
   float3 rgbS = tex2Dlod(RT, float4(pos.xy + float2(+0, +1) * inv_width_height, 0,0) ).xyz;
   float lumaN = FxaaLuma(rgbN);
   float lumaW = FxaaLuma(rgbW);
   float lumaM = FxaaLuma(rgbM);
   float lumaE = FxaaLuma(rgbE);
   float lumaS = FxaaLuma(rgbS);
   float rangeMin = min(lumaM, min(min(lumaN, lumaW), min(lumaS, lumaE)));
   float rangeMax = max(lumaM, max(max(lumaN, lumaW), max(lumaS, lumaE)));
   float range = rangeMax - rangeMin;
   float comp = max(FXAA_EDGE_THRESHOLD_MIN, rangeMax *FXAA_EDGE_THRESHOLD);
   if ( (range < comp) )
   {
      return rgbM.rgb;
   }
   else
   {
       #if FXAA_SUBPIX > 0
         #if FXAA_SUBPIX_FASTER
            float3 rgbL = (rgbN + rgbW + rgbE + rgbS + rgbM) * 
               (1.0/5.0);
         #else
            float3 rgbL = rgbN + rgbW + rgbM + rgbE + rgbS;
         #endif
      #endif    
      ////////////////////////////////////////////////////////////////////////////
       #if FXAA_SUBPIX != 0
        float lumaL = (lumaN + lumaW + lumaE + lumaS) * 0.25;
        float rangeL = abs(lumaL - lumaM);
      #endif      
      
      #if FXAA_SUBPIX == 1
         float blendL = max(0.0, 
            (rangeL / range) - FXAA_SUBPIX_TRIM) * FXAA_SUBPIX_TRIM_SCALE; 
         blendL = min(FXAA_SUBPIX_CAP, blendL);
      #endif
      #if FXAA_SUBPIX == 2
         float blendL = rangeL / range; 
      #endif
    
   /////////////////////////////////////////////////////////////////////////////////////////////////////
      float3 rgbNW = tex2Dlod(RT, float4(pos.xy + float2(-1, -1) * inv_width_height, 0,0) ).xyz;
      float3 rgbNE = tex2Dlod(RT, float4(pos.xy + float2(+1, -1) * inv_width_height, 0,0) ).xyz;
      float3 rgbSW = tex2Dlod(RT, float4(pos.xy + float2(-1, +1) * inv_width_height, 0,0) ).xyz;
      float3 rgbSE = tex2Dlod(RT, float4(pos.xy + float2(+1, +1) * inv_width_height, 0,0) ).xyz;
    #if (FXAA_SUBPIX_FASTER == 0) && (FXAA_SUBPIX > 0)
        rgbL += (rgbNW + rgbNE + rgbSW + rgbSE);
        rgbL *= (1.0/9.0);
    #endif
      float lumaNW = FxaaLuma(rgbNW);
      float lumaNE = FxaaLuma(rgbNE);
      float lumaSW = FxaaLuma(rgbSW);
      float lumaSE = FxaaLuma(rgbSE);
      float edgeVert = 
            abs((0.25f * lumaNW) + (-0.5f * lumaN) + (0.25f * lumaNE))
         + abs((0.50f * lumaW ) + (-1.0f * lumaM) + (0.50f * lumaE ))
         + abs((0.25f * lumaSW) + (-0.5f * lumaS) + (0.25f * lumaSE));
      float edgeHorz =
          abs((0.25f * lumaNW) + (-0.5f * lumaW) + (0.25f * lumaSW))
         + abs((0.50f * lumaN ) + (-1.0f * lumaM) + (0.50f * lumaS ))
         + abs((0.25f * lumaNE) + (-0.5f * lumaE) + (0.25f * lumaSE));
      bool horzSpan = edgeHorz >= edgeVert;
      float lengthSign = horzSpan ? -inv_width_height.y : -inv_width_height.x;
      if (!horzSpan)
      {
         lumaN = lumaW;
      }
      if (!horzSpan)
      {
         lumaS = lumaE;
      }
      float gradientN = abs(lumaN - lumaM);
      float gradientS = abs(lumaS - lumaM);
      lumaN = (lumaN + lumaM) * 0.5f;
      lumaS = (lumaS + lumaM) * 0.5f;
///////////////////////////////////////////////////////////////////////////////////////////
      bool pairN = gradientN >= gradientS;
      if (!pairN)
      {
         lumaN = lumaS;
         gradientN = gradientS;
         lengthSign *= -1;
      }
      float2 posN;
      posN.x = pos.x + (horzSpan ? 0 : lengthSign * 0.5f);
      posN.y = pos.y + (horzSpan ? lengthSign * 0.5f : 0);

      gradientN *= FXAA_SEARCH_THRESHOLD;
//////////////////////////////////////////////////////////////////////////////////////////
      float2 posP = posN;
      float2 offNP = horzSpan ? float2(inv_width_height.x, 0) : float2(0, inv_width_height.y); 
      float lumaEndN = lumaN;
      float lumaEndP = lumaN;
      bool doneN = false;
      bool doneP = false;
      
   #if FXAA_SEARCH_ACCELERATION == 1
        posN += offNP * float2(-1.0, -1.0);
        posP += offNP * float2( 1.0,  1.0);
    #endif
    #if FXAA_SEARCH_ACCELERATION == 2
        posN += offNP * float2(-1.5, -1.5);
        posP += offNP * float2( 1.5,  1.5);
        offNP *= float2(2.0, 2.0);
    #endif
    #if FXAA_SEARCH_ACCELERATION == 3
        posN += offNP * float2(-2.0, -2.0);
        posP += offNP * float2( 2.0,  2.0);
        offNP *= float2(3.0, 3.0);
    #endif
    #if FXAA_SEARCH_ACCELERATION == 4
        posN += offNP * float2(-2.5, -2.5);
        posP += offNP * float2( 2.5,  2.5);
        offNP *= float2(4.0, 4.0);
    #endif
    
      for (int i = 0; i < FXAA_SEARCH_STEPS; ++ i)
      {
      #if FXAA_SEARCH_ACCELERATION == 1
            if(!doneN) lumaEndN = 
                FxaaLuma(tex2Dlod(RT, float4(posN.xy,0,0) ).xyz);
            if(!doneP) lumaEndP = 
                FxaaLuma(tex2Dlod(RT, float4(posP.xy,0,0) ).xyz);
        #else
            if(!doneN) lumaEndN = 
                FxaaLuma(FxaaTexGrad(RT, posN.xy, offNP).xyz);
            if(!doneP) lumaEndP = 
                FxaaLuma(FxaaTexGrad(RT, posP.xy, offNP).xyz);
        #endif

         doneN = doneN || (abs(lumaEndN - lumaN) >= gradientN);
         doneP = doneP || (abs(lumaEndP - lumaN) >= gradientN);
         if (doneN && doneP)
         {
            i=FXAA_SEARCH_STEPS+1;
            //break;
         }
         if (!doneN)   posN -= offNP;
         if (!doneP)   posP += offNP;
      }
      
//////////////////////////////////////////////////////

      float dstN = horzSpan ? pos.x - posN.x : pos.y - posN.y;
      float dstP = horzSpan ? posP.x - pos.x : posP.y - pos.y;
      bool directionN = dstN < dstP;
      lumaEndN = directionN ? lumaEndN : lumaEndP;
//////////////////////////////////////////////////////////
      if ((lumaM - lumaN < 0) == (lumaEndN - lumaN < 0)) 
      {
         lengthSign = 0.0f;
      }

      float spanLength = (dstP + dstN);
      dstN = directionN ? dstN : dstP;
      float subPixelOffset = (0.5 + (dstN * (-1 / spanLength))) * lengthSign;
      float3 rgbF = tex2Dlod(RT, float4(
         pos.x + (horzSpan ? 0 : subPixelOffset),
         pos.y + (horzSpan ? subPixelOffset : 0), 0,0)).xyz;
         
      //return lerp(rgbF, rgbL, blendL);
   #if FXAA_SUBPIX == 0
        return FxaaFilterReturn(rgbF); 
    #else        
        return FxaaFilterReturn(FxaaLerp3(rgbL, rgbF, blendL)); 
    #endif
   }
}

//float4 FXAAPS(float2 tc0 : TEXCOORD0) : COLOR
//{
 //  return float4(FxaaPixelShader(tc0), 1.0f);
//}

//float4 DebugFXAAPS(float2 tc0 : TEXCOORD0) : COLOR
//{
//   return float4(DebugFxaaPixelShader(tc0), 1.0f);
//}
//////////////////////////////////////////////////////////////////////////////////////////////////////////


//void RenderFullSceneVS(float3 inPos: POSITION,
//float2 Tex : TEXCOORD0,
//out float4 oPos : POSITION,
//out float2 oTex : TEXCOORD0)
//{
//  oPos = float4(inPos,1.0f);
//  oTex = Tex;
//  oTex.y=1-oTex.y;
//  oTex+=inv_width_height.xy*0.5f;
//}
//void PS(float2 Tex : TEXCOORD0,out float4 oCol : COLOR0)
//{
//	float3 tempColor = FxaaPixelShader( Tex );
//	float  fPerColor = tempColor.x*0.3f+tempColor.y*0.59f+tempColor.z*0.11f;
//	float f = g_fGrayScale*fPerColor*fPerColor+g_fGrayBase;
//	f = min(f,1.15f);
//	tempColor = tempColor * f;
//	float3 cr= float3(fPerColor, fPerColor, fPerColor);
//	oCol = float4(cr*(1.0f-g_fsceneWeight)+tempColor*g_fsceneWeight,1.0f);

//	oCol = float4(tempColor,1.0f);
//}


float4 PS( in float2 Tex : TEXCOORD0 ) : COLOR0
{
    float3 ColorOrig = 1-FxaaPixelShader( Tex );

    ColorOrig += tex2D( s1, Tex );

    return float4( ColorOrig, 1.0f );
}

technique
{
  pass
  {
		Lighting			= False;
		CullMode			= None;
		
		AlphaTestEnable		= False;

		AlphaBlendEnable	= False;

		ZEnable				= False;
		ZFunc				= LessEqual;
		ZWriteEnable		= False;
		
        VertexShader = null;
        PixelShader = compile ps_3_0 PS();
  }
}