#include "HDRFormats.h"
#include "config.h"
#include "RenderSystem.h"

//{
//    CGrowableArray<D3DMULTISAMPLE_TYPE>* pMultiSampleTypeList = DXUTGetD3D9Enumeration()->GetPossibleMultisampleTypeList();
//    pMultiSampleTypeList->RemoveAll();
//    pMultiSampleTypeList->Add(D3DMULTISAMPLE_NONE);
//    DXUTGetD3D9Enumeration()->SetMultisampleQualityMax(0);
//}
double                  m_aPowsOfTwo[257]; // Lookup table for log calculations

CHDRFormats::CHDRFormats()
{
	m_pMSRT = NULL;
	m_pMSDS = NULL;

	m_bSupportsR16F = false;
	m_bSupportsR32F = false;
	m_bSupportsD16 = false;
	m_bSupportsD32 = false;
	m_bSupportsD24X8 = false;
	m_bUseMultiSample = false;
	m_MaxMultiSampleType = D3DMULTISAMPLE_NONE;
	m_dwMultiSampleQuality = 0;


    m_pEffect = NULL;
    m_pTexRender = NULL;

    m_bBloom = TRUE;
    m_eEncodingMode = RGBE8;
    m_eRenderMode = DECODED;

    m_pCurTechnique = &m_aTechHandles[ m_eEncodingMode ];

    for(int i=0; i <= 256; i++)
    {
        m_aPowsOfTwo[i] = powf(2.0f, (float)(i - 128));
    }

    ZeroMemory(m_apTexToneMap, sizeof(m_apTexToneMap));
    ZeroMemory(m_apTexBloom, sizeof(m_apTexBloom));
    ZeroMemory(m_aTechHandles, sizeof(m_aTechHandles));
}

HRESULT CHDRFormats::OnCreateDevice(LPDIRECT3DDEVICE9 pd3dDevice)
{
    HRESULT hr = S_OK;
    WCHAR str[MAX_PATH] = {0};


    DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;

    #if defined(DEBUG) || defined(_DEBUG)
    dwShaderFlags |= D3DXSHADER_DEBUG;
    #endif

    #ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUm_pS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif

    hr = DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"HDRFormats.fx");
    if(FAILED(hr))
        return hr;

    hr = D3DXCreateEffectFromFile(GetRenderSystem().GetDevice(), str, NULL, NULL, dwShaderFlags, NULL, &m_pEffect, NULL);

    // If this fails, there should be debug output
    if(FAILED(hr))
        return hr;

    RetrieveTechHandles();

    // Determine which encoding modes this device can support
//     IDirect3D9* pD3D = DXUTGetD3D9Object();
//     DXUTDeviceSettings settings = DXUTGetDeviceSettings(); 

    bool bCreatedTexture = false;
    IDirect3DCubeTexture9* pCubeTexture = NULL;
    IDirect3DCubeTexture9* pEncodedTexture = NULL;


    WCHAR strPath[MAX_PATH];
    V_RETURN(DXUTFindDXSDKMediaFileCch(strPath, MAX_PATH, L"Light Probes\\uffizi_cross.dds"));

    m_bSupportsR16F = false;
//     if(SUCCEEDED(pD3D->CheckDeviceFormat(settings.d3d9.AdapterOrdinal, settings.d3d9.DeviceType,
//                                             settings.d3d9.AdapterFormat, D3DUSAGE_RENDERTARGET, 
//                                             D3DRTYPE_TEXTURE, D3DFMT_R16F)))
//         m_bSupportsR16F = true;
//     
    m_bSupportsR32F = false;
//     if(SUCCEEDED(pD3D->CheckDeviceFormat(settings.d3d9.AdapterOrdinal, settings.d3d9.DeviceType,
//                                             settings.d3d9.AdapterFormat, D3DUSAGE_RENDERTARGET, 
//                                             D3DRTYPE_TEXTURE, D3DFMT_R32F)))
//         m_bSupportsR32F = true;  

    bool bSupports128FCube = false;
//     if(SUCCEEDED(pD3D->CheckDeviceFormat(settings.d3d9.AdapterOrdinal, settings.d3d9.DeviceType,
//                                            settings.d3d9.AdapterFormat, 0,
//                                            D3DRTYPE_CUBETEXTURE, D3DFMT_A16B16G16R16F)))
//         bSupports128FCube = true;
//         
    // FP16
    if(m_bSupportsR16F && bSupports128FCube)
    {
        //// Device supports floating-point textures. 
        //V_RETURN(D3DXCreateCubeTextureFromFileEx(GetRenderSystem().GetDevice(), strPath, D3DX_DEFAULT, 1, 0, D3DFMT_A16B16G16R16F, 
        //                                           D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &pCubeTexture));
        //  
        //V_RETURN(m_aSkybox[FP16].OnCreateDevice(GetRenderSystem().GetDevice(), 50, pCubeTexture, L"skybox.fx"));
        //
        //m_SampleUI.GetComboBox(IDC_ENCODING_MODE)->AddItem(L"FP16", (void*)FP16);
    }

    // FP32
    if(m_bSupportsR32F && bSupports128FCube)
    {
        //// Device supports floating-point textures. 
        //V_RETURN(D3DXCreateCubeTextureFromFileEx(GetRenderSystem().GetDevice(), strPath, D3DX_DEFAULT, 1, 0, D3DFMT_A16B16G16R16F, 
        //                                           D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &pCubeTexture));
        //  
        //V_RETURN(m_aSkybox[FP32].OnCreateDevice(GetRenderSystem().GetDevice(), 50, pCubeTexture, L"skybox.fx"));
        //
        //m_SampleUI.GetComboBox(IDC_ENCODING_MODE)->AddItem(L"FP32", (void*)FP32);
    }

    if((!m_bSupportsR32F && !m_bSupportsR16F) || !bSupports128FCube)
    {
        // Device doesn't support floating-point textures. Use the scratch pool to load it temporarily
        // in order to create encoded textures from it.
        bCreatedTexture = true;
        V_RETURN(D3DXCreateCubeTextureFromFileEx(GetRenderSystem().GetDevice(), strPath, D3DX_DEFAULT, 1, 0, D3DFMT_A16B16G16R16F, 
                                                   D3DPOOL_SCRATCH, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &pCubeTexture));
    }

    // RGB16
//     if(D3D_OK == pD3D->CheckDeviceFormat(settings.d3d9.AdapterOrdinal, 
//                                            settings.d3d9.DeviceType,
//                                            settings.d3d9.AdapterFormat, 0,
//                                            D3DRTYPE_CUBETEXTURE,
//                                            D3DFMT_A16B16G16R16))
//     {
//         V_RETURN(CreateEncodedTexture(pCubeTexture, &pEncodedTexture, RGB16));
//         //V_RETURN(m_aSkybox[RGB16].OnCreateDevice(GetRenderSystem().GetDevice(), 50, pEncodedTexture, L"skybox.fx"));
//     
//         //m_SampleUI.GetComboBox(IDC_ENCODING_MODE)->AddItem(L"RGB16", (void*)RGB16);
//     }


    // RGBE8
    V_RETURN(CreateEncodedTexture(pCubeTexture, &pEncodedTexture, RGBE8));
    //V_RETURN(m_aSkybox[RGBE8].OnCreateDevice(GetRenderSystem().GetDevice(), 50, pEncodedTexture, L"skybox.fx"));

    //m_SampleUI.GetComboBox(IDC_ENCODING_MODE)->AddItem(L"RGBE8", (void*)RGBE8);
    //m_SampleUI.GetComboBox(IDC_ENCODING_MODE)->SetSelectedByText(L"RGBE8");

    if(bCreatedTexture)
        S_REL(pCubeTexture);

    return S_OK;
}

HRESULT CHDRFormats::OnResetDevice()
{
    HRESULT hr = S_OK;

    if(m_pEffect)
        m_pEffect->OnResetDevice();

    D3DFORMAT fmt = D3DFMT_UNKNOWN;
    switch(m_eEncodingMode)
    {
        case FP16:  fmt = D3DFMT_A16B16G16R16F; break;
        case FP32:  fmt = D3DFMT_A16B16G16R16F; break;
        case RGBE8: fmt = D3DFMT_A8R8G8B8; break;
        case RGB16: fmt = D3DFMT_A16B16G16R16; break;
    }

    hr = GetRenderSystem().GetDevice()->CreateTexture(DXUTGetBackBufferSurfaceDesc()->Width, DXUTGetBackBufferSurfaceDesc()->Height, 
                                      1, D3DUSAGE_RENDERTARGET, fmt, 
                                      D3DPOOL_DEFAULT, &m_pTexRender, NULL);
    if(FAILED(hr))
        return hr;

    hr = GetRenderSystem().GetDevice()->CreateTexture(DXUTGetBackBufferSurfaceDesc()->Width/8, DXUTGetBackBufferSurfaceDesc()->Height/8, 
                                      1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 
                                      D3DPOOL_DEFAULT, &m_pTexBrightPass, NULL);
    if(FAILED(hr))
        return hr;

    // Determine whether we can and should support a multisampling on the HDR render target
    m_bUseMultiSample = false;
//     IDirect3D9* pD3D = DXUTGetD3D9Object();
//     if(!pD3D)
//         return E_FAIL;
// 
//     DXUTDeviceSettings settings = DXUTGetDeviceSettings();

    m_bSupportsD16 = false;
//     if(SUCCEEDED(pD3D->CheckDeviceFormat(settings.d3d9.AdapterOrdinal, settings.d3d9.DeviceType,
//                                             settings.d3d9.AdapterFormat, D3DUSAGE_DEPTHSTENCIL,
//                                             D3DRTYPE_SURFACE, D3DFMT_D16)))
//     {
//         if(SUCCEEDED(pD3D->CheckDepthStencilMatch(settings.d3d9.AdapterOrdinal, settings.d3d9.DeviceType,
//                                             settings.d3d9.AdapterFormat, fmt, 
//                                             D3DFMT_D16)))
//         {
//             m_bSupportsD16 = true;
//         }
//     }
    m_bSupportsD32 = false;
//     if(SUCCEEDED(pD3D->CheckDeviceFormat(settings.d3d9.AdapterOrdinal, settings.d3d9.DeviceType,
//                                             settings.d3d9.AdapterFormat, D3DUSAGE_DEPTHSTENCIL,
//                                             D3DRTYPE_SURFACE, D3DFMT_D32)))
//     {
//         if(SUCCEEDED(pD3D->CheckDepthStencilMatch(settings.d3d9.AdapterOrdinal, settings.d3d9.DeviceType,
//                                                 settings.d3d9.AdapterFormat, fmt, 
//                                                 D3DFMT_D32)))
//         {
//             m_bSupportsD32 = true;
//         }
//     }
    m_bSupportsD24X8 = false;
//     if(SUCCEEDED(pD3D->CheckDeviceFormat(settings.d3d9.AdapterOrdinal, settings.d3d9.DeviceType,
//                                             settings.d3d9.AdapterFormat, D3DUSAGE_DEPTHSTENCIL,
//                                             D3DRTYPE_SURFACE, D3DFMT_D24X8)))
//     {
//         if(SUCCEEDED(pD3D->CheckDepthStencilMatch(settings.d3d9.AdapterOrdinal, settings.d3d9.DeviceType,
//                                                 settings.d3d9.AdapterFormat, fmt, 
//                                                 D3DFMT_D24X8)))
//         {
//             m_bSupportsD24X8 = true;
//         }
//     }

    D3DFORMAT dfmt = D3DFMT_UNKNOWN;
    if(m_bSupportsD16)
        dfmt = D3DFMT_D16;
    else if(m_bSupportsD32)
        dfmt = D3DFMT_D32;
    else if(m_bSupportsD24X8)
        dfmt = D3DFMT_D24X8;

    if(dfmt != D3DFMT_UNKNOWN)
    {
        D3DCAPS9 Caps;
        GetRenderSystem().GetDevice()->GetDeviceCaps(&Caps);

        //m_MaxMultiSampleType = D3DMULTISAMPLE_NONE;
        //for(D3DMULTISAMPLE_TYPE imst = D3DMULTISAMPLE_2_SAMPLES; imst <= D3DMULTISAMPLE_16_SAMPLES; imst = (D3DMULTISAMPLE_TYPE)(imst + 1))
        //{
        //    DWORD msQuality = 0;
        //    if(SUCCEEDED(pD3D->CheckDeviceMultiSampleType(Caps.AdapterOrdinal, 
        //                                                    Caps.DeviceType, 
        //                                                    fmt, 
        //                                                    settings.d3d9.pp.Windowed, 
        //                                                    imst, &msQuality)))
        //    {
        //        m_bUseMultiSample = true;
        //        m_MaxMultiSampleType = imst;
        //        if(msQuality > 0)
        //            m_dwMultiSampleQuality = msQuality-1;
        //        else
        //            m_dwMultiSampleQuality = msQuality;
        //    }
        //}

        // Create the Multi-Sample floating point render target
        if(m_bUseMultiSample)
        {
            //const D3DSURFACE_DESC* pBackBufferDesc = DXUTGetD3D9BackBufferSurfaceDesc();
            //hr = GetRenderSystem().GetDevice()->CreateRenderTarget(pBackBufferDesc->Width, pBackBufferDesc->Height,
            //                                    fmt, 
            //                                    m_MaxMultiSampleType, m_dwMultiSampleQuality,
            //                                    FALSE, &m_pMSRT, NULL);
            //if(FAILED(hr))
            //    m_bUseMultiSample = false;
            //else
            //{
            //    hr = GetRenderSystem().GetDevice()->CreateDepthStencilSurface(pBackBufferDesc->Width, pBackBufferDesc->Height,
            //                                    dfmt, 
            //                                    m_MaxMultiSampleType, m_dwMultiSampleQuality,
            //                                    TRUE, &m_pMSDS, NULL);
            //    if(FAILED(hr))
            //    {
            //        m_bUseMultiSample = false;
            //        S_REL(m_pMSRT);
            //    }
            //}
        }
    }

    // For each scale stage, create a texture to hold the intermediate results
    // of the luminance calculation
    int nSampleLen = 1;
    for(int i=0; i < NUM_TONEMAP_TEXTURES; i++)
    {
        fmt = D3DFMT_UNKNOWN;
        switch(m_eEncodingMode)
        {
            case FP16: fmt = D3DFMT_R16F; break;
            case FP32: fmt = D3DFMT_R32F; break;
            case RGBE8: fmt = D3DFMT_A8R8G8B8; break;
            case RGB16: fmt = D3DFMT_A16B16G16R16; break;
        }

        hr = GetRenderSystem().GetDevice()->CreateTexture(nSampleLen, nSampleLen, 1, D3DUSAGE_RENDERTARGET, 
                                         fmt, D3DPOOL_DEFAULT, &m_apTexToneMap[i], NULL);
        if(FAILED(hr))
            return hr;

        nSampleLen *= 3;
    }

    // Create the temporary blooming effect textures
    for(int i=0; i < NUM_BLOOM_TEXTURES; i++)
    {
        hr = GetRenderSystem().GetDevice()->CreateTexture(DXUTGetBackBufferSurfaceDesc()->Width/8, DXUTGetBackBufferSurfaceDesc()->Height/8,
                                        1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 
                                        D3DPOOL_DEFAULT, &m_apTexBloom[i], NULL);
        if(FAILED(hr))
            return hr;
    }
    return S_OK;
}

void CHDRFormats::OnFrameMove(float fElapsedTime)
{
}

void CHDRFormats::OnFrameRender()
{
    //UNREFERENCED_PARAMETER(fElapsedTime);
    LPDIRECT3DSURFACE9 pSurfBackBuffer = NULL;
    PDIRECT3DSURFACE9 pSurfDS = NULL;
    GetRenderSystem().GetDevice()->GetRenderTarget(0, &pSurfBackBuffer);
    GetRenderSystem().GetDevice()->GetDepthStencilSurface(&pSurfDS);    

    LPDIRECT3DSURFACE9 pSurfRenderTarget = NULL;
    m_pTexRender->GetSurfaceLevel(0, &pSurfRenderTarget);

    // Setup the HDR render target
    if(m_bUseMultiSample)
    {
        GetRenderSystem().GetDevice()->SetRenderTarget(0, m_pMSRT);
        GetRenderSystem().GetDevice()->SetDepthStencilSurface(m_pMSDS);
    }
    else
    {
        GetRenderSystem().GetDevice()->SetRenderTarget(0, pSurfRenderTarget);
    }

    // Clear the render target
    //R.ClearBuffer(true, true, 0x000000FF);

   // if(SUCCEEDED(GetRenderSystem().GetDevice()->BeginScene()))
    { 
        // Draw the skybox
        //RenderModel();

        // If using floating point multi sampling, stretchrect to the rendertarget
        if(m_bUseMultiSample)
        {
            GetRenderSystem().GetDevice()->StretchRect(m_pMSRT, NULL, pSurfRenderTarget, NULL, D3DTEXF_NONE);
            GetRenderSystem().GetDevice()->SetRenderTarget(0, pSurfRenderTarget);
            GetRenderSystem().GetDevice()->SetDepthStencilSurface(pSurfDS);
			GetRenderSystem().ClearBuffer(true, false, 0);      
        }

        MeasureLuminance();     
        BrightPassFilter();

        if(m_bBloom) 
            RenderBloom();

        //---------------------------------------------------------------------
        // Final pass
        GetRenderSystem().GetDevice()->SetRenderTarget(0, pSurfBackBuffer);
        GetRenderSystem().GetDevice()->SetTexture(0, m_pTexRender);
        GetRenderSystem().GetDevice()->SetTexture(1, m_apTexToneMap[0]);
        GetRenderSystem().GetDevice()->SetTexture(2, m_bBloom ? m_apTexBloom[0] : NULL);

        GetRenderSystem().GetDevice()->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        GetRenderSystem().GetDevice()->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);        

        switch(m_eRenderMode)
        {
            case DECODED:       m_pEffect->SetTechnique(m_pCurTechnique->FinalPass); break;
            case RGB_ENCODED:   m_pEffect->SetTechnique("FinalPassEncoded_RGB"); break;
            case ALPHA_ENCODED: m_pEffect->SetTechnique("FinalPassEncoded_A"); break;
        }

        UINT uiPass, uiNumPasses;
        m_pEffect->Begin(&uiNumPasses, 0);

        for(uiPass=0; uiPass < uiNumPasses; uiPass++)
        {
            m_pEffect->BeginPass(uiPass);

            DrawFullScreenQuad();

            m_pEffect->EndPass();
        }

        m_pEffect->End();
        GetRenderSystem().GetDevice()->SetTexture(0, NULL);
        GetRenderSystem().GetDevice()->SetTexture(1, NULL);
        GetRenderSystem().GetDevice()->SetTexture(2, NULL);

      //  GetRenderSystem().GetDevice()->EndScene();
    }

    S_REL(pSurfRenderTarget);
    S_REL(pSurfBackBuffer);
    S_REL(pSurfDS);
}



void CHDRFormats::OnLostDevice()
{
    if(m_pEffect)
        m_pEffect->OnLostDevice();

    S_REL(m_pMSRT);
    S_REL(m_pMSDS);

    S_REL(m_pTexRender);
    S_REL(m_pTexBrightPass);

    for(int i=0; i < NUM_TONEMAP_TEXTURES; i++)
    {
        S_REL(m_apTexToneMap[i]);
    }

    for(int i=0; i < NUM_BLOOM_TEXTURES; i++)
    {
        S_REL(m_apTexBloom[i]);
    }
}

void CHDRFormats::OnDestroyDevice()
{
    S_REL(m_pEffect);
}



inline float GaussianDistribution(float x, float y, float rho)
{
    float g = 1.0f / sqrtf(2.0f * D3DX_PI * rho * rho);
    g *= expf(-(x*x + y*y)/(2*rho*rho));

    return g;
}



inline int log2_ceiling(float val)
{
    int iMax = 256;
    int iMin = 0;

	// ¶þ·Ö·¨
    while(iMax - iMin > 1)
    {
        int iMiddle = (iMax + iMin) / 2;

        if(val > m_aPowsOfTwo[iMiddle])
            iMin = iMiddle;
        else
            iMax = iMiddle;
    }

    return iMax - 128;
}



inline VOID EncodeRGBE8(D3DXFLOAT16* pSrc, BYTE** ppDest)
{
    FLOAT r, g, b;

    r = (FLOAT) *(pSrc+0);
    g = (FLOAT) *(pSrc+1);
    b = (FLOAT) *(pSrc+2);

    // Determine the largest color component
    float maxComponent = max(max(r, g), b);

    // Round to the nearest integer exponent
    int nExp = log2_ceiling(maxComponent);

    // Divide the components by the shared exponent
    FLOAT fDivisor = (FLOAT) m_aPowsOfTwo[ nExp+128 ];

    r /= fDivisor;
    g /= fDivisor;
    b /= fDivisor;

    // Constrain the color components
    r = max(0, min(1, r));
    g = max(0, min(1, g));
    b = max(0, min(1, b));

    // Store the shared exponent in the alpha channel
    D3DCOLOR* pDestColor = (D3DCOLOR*) *ppDest;
    *pDestColor = D3DCOLOR_RGBA((BYTE)(r*255), (BYTE)(g*255), (BYTE)(b*255), nExp+128);
    *ppDest += sizeof(D3DCOLOR);
}



inline VOID EncodeRGB16(D3DXFLOAT16* pSrc, BYTE** ppDest)
{
    FLOAT r, g, b;

    r = (FLOAT) *(pSrc+0);
    g = (FLOAT) *(pSrc+1);
    b = (FLOAT) *(pSrc+2);

    // Divide the components by the multiplier
    r /= RGB16_MAX;
    g /= RGB16_MAX;
    b /= RGB16_MAX;

    // Constrain the color components
    r = max(0, min(1, r));
    g = max(0, min(1, g));
    b = max(0, min(1, b));

    // Store
    USHORT* pDestColor = (USHORT*) *ppDest;
    *pDestColor++ = (USHORT)(r*65535);
    *pDestColor++ = (USHORT)(g*65535);
    *pDestColor++ = (USHORT)(b*65535);

    *ppDest += sizeof(UINT64);
}


//-----------------------------------------------------------------------------
// Name: RetrieveTechHandles()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CHDRFormats::RetrieveTechHandles()
{
    CHAR* modes[] = { "FP16", "FP16", "RGB16", "RGBE8" };
    CHAR* techniques[] = { "Scene", "DownScale2x2_Lum", "DownScale3x3", "DownScale3x3_BrightPass", "FinalPass" };
    DWORD dwNumTechniques = sizeof(TECH_HANDLES)/sizeof(D3DXHANDLE);

    CHAR strBuffer[MAX_PATH] = {0};

    D3DXHANDLE* pHandle = (D3DXHANDLE*) m_aTechHandles;

    for(UINT m=0; m < (UINT)NUM_ENCODING_MODES; m++)
    {
        for(UINT t=0; t < dwNumTechniques; t++)
        {
            StringCchPrintfA(strBuffer, MAX_PATH-1, "%s_%s", techniques[t], modes[m]);

            *pHandle++ = m_pEffect->GetTechniqueByName(strBuffer);
        }
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: MeasureLuminance()
// Desc: Measure the average log luminance in the scene.
//-----------------------------------------------------------------------------
HRESULT CHDRFormats::MeasureLuminance()
{
    HRESULT hr = S_OK;
    UINT uiNumPasses, uiPass;
    D3DXVECTOR2 avSampleOffsets[16];

    LPDIRECT3DTEXTURE9 pTexSrc = NULL;
    LPDIRECT3DTEXTURE9 pTexDest = NULL;
    LPDIRECT3DSURFACE9 pSurfDest = NULL;

    //-------------------------------------------------------------------------
    // Initial sampling pass to convert the image to the log of the grayscale
    //-------------------------------------------------------------------------
    pTexSrc = m_pTexRender;
    pTexDest = m_apTexToneMap[NUM_TONEMAP_TEXTURES-1];

    D3DSURFACE_DESC descSrc;
    pTexSrc->GetLevelDesc(0, &descSrc);

    D3DSURFACE_DESC descDest;
    pTexDest->GetLevelDesc(0, &descDest);

    GetSampleOffsets_DownScale2x2_Lum(descSrc.Width, descSrc.Height, descDest.Width, descDest.Height, avSampleOffsets);

    m_pEffect->SetValue("m_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
    m_pEffect->SetTechnique(m_pCurTechnique->DownScale2x2_Lum);

    hr = pTexDest->GetSurfaceLevel(0, &pSurfDest);
    if(FAILED(hr))
        return hr;

    GetRenderSystem().GetDevice()->SetRenderTarget(0, pSurfDest);
    GetRenderSystem().GetDevice()->SetTexture(0, pTexSrc);
    GetRenderSystem().GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    GetRenderSystem().GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

    hr = m_pEffect->Begin(&uiNumPasses, 0);
    if(FAILED(hr))
        return hr;

    for (uiPass = 0; uiPass < uiNumPasses; uiPass++)
    {
        m_pEffect->BeginPass(uiPass);

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad();

        m_pEffect->EndPass();
    }

    m_pEffect->End();
    GetRenderSystem().GetDevice()->SetTexture(0, NULL);

    S_REL(pSurfDest);

    //-------------------------------------------------------------------------
    // Iterate through the remaining tone map textures
    //------------------------------------------------------------------------- 
    for(int i = NUM_TONEMAP_TEXTURES-1; i > 0; i--)
    {
        // Cycle the textures
        pTexSrc = m_apTexToneMap[i];
        pTexDest = m_apTexToneMap[i-1];

        hr = pTexDest->GetSurfaceLevel(0, &pSurfDest);
        if(FAILED(hr))
            return hr;

        D3DSURFACE_DESC desc;
        pTexSrc->GetLevelDesc(0, &desc);
        GetSampleOffsets_DownScale3x3(desc.Width, desc.Height, avSampleOffsets);

        m_pEffect->SetValue("m_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
        m_pEffect->SetTechnique(m_pCurTechnique->DownScale3x3);

        GetRenderSystem().GetDevice()->SetRenderTarget(0, pSurfDest);
        GetRenderSystem().GetDevice()->SetTexture(0, pTexSrc);
        GetRenderSystem().GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        GetRenderSystem().GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

        hr = m_pEffect->Begin(&uiNumPasses, 0);
        if(FAILED(hr))
            return hr;

        for (uiPass = 0; uiPass < uiNumPasses; uiPass++)
        {
            m_pEffect->BeginPass(uiPass);

            // Draw a fullscreen quad to sample the RT
            DrawFullScreenQuad();

            m_pEffect->EndPass();
        }

        m_pEffect->End();
        GetRenderSystem().GetDevice()->SetTexture(0, NULL);

        S_REL(pSurfDest);
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_DownScale3x3
// Desc: Get the texture coordinate offsets to be used inside the DownScale3x3
//       pixel shader.
//-----------------------------------------------------------------------------
HRESULT CHDRFormats::GetSampleOffsets_DownScale3x3(DWORD dwWidth, DWORD dwHeight, D3DXVECTOR2 avSampleOffsets[])
{
    if(NULL == avSampleOffsets)
        return E_INVALIDARG;

    float tU = 1.0f / dwWidth;
    float tV = 1.0f / dwHeight;

    // Sample from the 9 surrounding points. 
    int index=0;
    for(int y=-1; y <= 1; y++)
    {
        for(int x=-1; x <= 1; x++)
        {
            avSampleOffsets[ index ].x = x * tU;
            avSampleOffsets[ index ].y = y * tV;

            index++;
        }
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_DownScale2x2_Lum
// Desc: Get the texture coordinate offsets to be used inside the DownScale2x2_Lum
//       pixel shader.
//-----------------------------------------------------------------------------
HRESULT CHDRFormats::GetSampleOffsets_DownScale2x2_Lum(DWORD dwSrcWidth, DWORD dwSrcHeight, DWORD dwDestWidth, DWORD dwDestHeight, D3DXVECTOR2 avSampleOffsets[])
{
    if(NULL == avSampleOffsets)
        return E_INVALIDARG;

    float tU = 1.0f / dwSrcWidth;
    float tV = 1.0f / dwSrcHeight;

    float deltaU = (float) dwSrcWidth / dwDestWidth / 2.0f;
    float deltaV = (float) dwSrcHeight / dwDestHeight / 2.0f;

    // Sample from 4 surrounding points. 
    int index=0;
    for(int y=0; y < 2; y++)
    {
        for(int x=0; x <= 2; x++)
        {
            avSampleOffsets[ index ].x = (x - 0.5f) * deltaU * tU;
            avSampleOffsets[ index ].y = (y - 0.5f) * deltaV * tV;

            index++;
        }
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_Bloom()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CHDRFormats::GetSampleOffsets_Bloom(DWORD dwD3DTexSize,
                                                float afTexCoordOffset[15],
                                                D3DXVECTOR4* avColorWeight,
                                                float fDeviation, float fMultiplier)
{
    int i=0;
    float tu = 1.0f / (float)dwD3DTexSize;

    // Fill the center texel
    float weight = 1.0f * GaussianDistribution(0, 0, fDeviation);
    avColorWeight[0] = D3DXVECTOR4(weight, weight, weight, 1.0f);

    afTexCoordOffset[0] = 0.0f;

    // Fill the right side
    for(i=1; i < 8; i++)
    {
        weight = fMultiplier * GaussianDistribution((float)i, 0, fDeviation);
        afTexCoordOffset[i] = i * tu;

        avColorWeight[i] = D3DXVECTOR4(weight, weight, weight, 1.0f);
    }

    // Copy to the left side
    for(i=8; i < 15; i++)
    {
        avColorWeight[i] = avColorWeight[i-7];
        afTexCoordOffset[i] = -afTexCoordOffset[i-7];
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: BrightPassFilter
// Desc: Prepare for the bloom pass by removing dark information from the scene
//-----------------------------------------------------------------------------
HRESULT CHDRFormats::BrightPassFilter()
{
    HRESULT hr = S_OK;

    D3DXVECTOR2 avSampleOffsets[16];
    GetSampleOffsets_DownScale3x3(DXUTGetBackBufferSurfaceDesc()->Width/2, DXUTGetBackBufferSurfaceDesc()->Height/2, avSampleOffsets);

    m_pEffect->SetValue("m_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));

    LPDIRECT3DSURFACE9 pSurfBrightPass = NULL;
    hr = m_pTexBrightPass->GetSurfaceLevel(0, &pSurfBrightPass);
    if(FAILED(hr))
        return hr;

    m_pEffect->SetTechnique(m_pCurTechnique->DownScale3x3_BrightPass);
    GetRenderSystem().GetDevice()->SetRenderTarget(0, pSurfBrightPass);
    GetRenderSystem().GetDevice()->SetTexture(0, m_pTexRender);
    GetRenderSystem().GetDevice()->SetTexture(1, m_apTexToneMap[0]);

    GetRenderSystem().GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

    UINT uiPass, uiNumPasses;
    hr = m_pEffect->Begin(&uiNumPasses, 0);
    if(FAILED(hr))
        return hr;

    for (uiPass = 0; uiPass < uiNumPasses; uiPass++)
    {
        m_pEffect->BeginPass(uiPass);

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad();

        m_pEffect->EndPass();
    }

    m_pEffect->End();
    GetRenderSystem().GetDevice()->SetTexture(0, NULL);
    GetRenderSystem().GetDevice()->SetTexture(1, NULL);

    S_REL(pSurfBrightPass);

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: RenderBloom()
// Desc: Render the blooming effect
//-----------------------------------------------------------------------------
HRESULT CHDRFormats::RenderBloom()
{
    HRESULT hr = S_OK;
    UINT uiPassCount, uiPass;
    int i=0;

    D3DXVECTOR2 avSampleOffsets[16];
    float       afSampleOffsets[16];
    D3DXVECTOR4 avSampleWeights[16];

    LPDIRECT3DSURFACE9 pSurfDest = NULL;
    hr = m_apTexBloom[1]->GetSurfaceLevel(0, &pSurfDest);
    if(FAILED(hr))
        return hr;

    D3DSURFACE_DESC desc;
    hr = m_pTexBrightPass->GetLevelDesc(0, &desc);
    if(FAILED(hr))
        return hr;

    hr = GetSampleOffsets_Bloom(desc.Width, afSampleOffsets, avSampleWeights, 3.0f, 1.25f);
    for(i=0; i < 16; i++)
    {
        avSampleOffsets[i] = D3DXVECTOR2(afSampleOffsets[i], 0.0f);
    }

    m_pEffect->SetTechnique("Bloom");
    m_pEffect->SetValue("m_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
    m_pEffect->SetValue("m_avSampleWeights", avSampleWeights, sizeof(avSampleWeights));

    GetRenderSystem().GetDevice()->SetRenderTarget(0, pSurfDest);
    GetRenderSystem().GetDevice()->SetTexture(0, m_pTexBrightPass);
    GetRenderSystem().GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    GetRenderSystem().GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);


    m_pEffect->Begin(&uiPassCount, 0);
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        m_pEffect->BeginPass(uiPass);

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad();

        m_pEffect->EndPass();
    }
    m_pEffect->End();
    GetRenderSystem().GetDevice()->SetTexture(0, NULL);

    S_REL(pSurfDest);
    hr = m_apTexBloom[0]->GetSurfaceLevel(0, &pSurfDest);
    if(FAILED(hr))
        return hr;

    hr = GetSampleOffsets_Bloom(desc.Height, afSampleOffsets, avSampleWeights, 3.0f, 1.25f);
    for(i=0; i < 16; i++)
    {
        avSampleOffsets[i] = D3DXVECTOR2(0.0f, afSampleOffsets[i]);
    }

    m_pEffect->SetTechnique("Bloom");
    m_pEffect->SetValue("m_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
    m_pEffect->SetValue("m_avSampleWeights", avSampleWeights, sizeof(avSampleWeights));

    GetRenderSystem().GetDevice()->SetRenderTarget(0, pSurfDest);
    GetRenderSystem().GetDevice()->SetTexture(0, m_apTexBloom[1]);
    GetRenderSystem().GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    GetRenderSystem().GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);


    m_pEffect->Begin(&uiPassCount, 0);

    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        m_pEffect->BeginPass(uiPass);

        // Draw a fullscreen quad to sample the RT
        DrawFullScreenQuad();

        m_pEffect->EndPass();
    }

    m_pEffect->End();
    GetRenderSystem().GetDevice()->SetTexture(0, NULL);

    S_REL(pSurfDest);

    return hr;
}


//-----------------------------------------------------------------------------
// Name: DrawFullScreenQuad
// Desc: Draw a properly aligned quad covering the entire render target
//-----------------------------------------------------------------------------
void CHDRFormats::DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV)
{
    D3DSURFACE_DESC dtdsdRT;
    PDIRECT3DSURFACE9 pSurfRT;

    // Acquire render target width and height
    GetRenderSystem().GetDevice()->GetRenderTarget(0, &pSurfRT);
    pSurfRT->GetDesc(&dtdsdRT);
    pSurfRT->Release();

    // Ensure that we're directly mapping texels to pixels by offset by 0.5
    // For more info see the doc page titled "Directly Mapping Texels to Pixels"
    FLOAT fWidth5 = (FLOAT)dtdsdRT.Width - 0.5f;
    FLOAT fHeight5 = (FLOAT)dtdsdRT.Height - 0.5f;

    // Draw the quad
    SCREEN_VERTEX svQuad[4];

    svQuad[0].pos = D3DXVECTOR4(-0.5f, -0.5f, 0.5f, 1.0f);
    svQuad[0].tex = D3DXVECTOR2(fLeftU, fTopV);

    svQuad[1].pos = D3DXVECTOR4(fWidth5, -0.5f, 0.5f, 1.0f);
    svQuad[1].tex = D3DXVECTOR2(fRightU, fTopV);

    svQuad[2].pos = D3DXVECTOR4(-0.5f, fHeight5, 0.5f, 1.0f);
    svQuad[2].tex = D3DXVECTOR2(fLeftU, fBottomV);

    svQuad[3].pos = D3DXVECTOR4(fWidth5, fHeight5, 0.5f, 1.0f);
    svQuad[3].tex = D3DXVECTOR2(fRightU, fBottomV);

    GetRenderSystem().GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
    GetRenderSystem().GetDevice()->SetFVF(SCREEN_VERTEX::FVF);
    GetRenderSystem().GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, svQuad, sizeof(SCREEN_VERTEX));
    GetRenderSystem().GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
}


//-----------------------------------------------------------------------------
// Name: CreateEncodedTexture
// Desc: Create a copy of the input floating-point texture with RGBE8 or RGB16 
//       encoding
//-----------------------------------------------------------------------------
HRESULT CHDRFormats::CreateEncodedTexture(IDirect3DCubeTexture9* pTexSrc, IDirect3DCubeTexture9** ppTexDest, ENCODING_MODE eTarget)
{
    HRESULT hr = S_OK;

    D3DSURFACE_DESC desc;
    V_RETURN(pTexSrc->GetLevelDesc(0, &desc));

    // Create a texture with equal dimensions to store the encoded texture
    D3DFORMAT fmt = D3DFMT_UNKNOWN;
    switch(eTarget)
    {
        case RGBE8: fmt = D3DFMT_A8R8G8B8; break;
        case RGB16: fmt = D3DFMT_A16B16G16R16; break;
    }

    V_RETURN(GetRenderSystem().GetDevice()->CreateCubeTexture(desc.Width, 1, 0, 
                                               fmt, D3DPOOL_MANAGED, 
                                               ppTexDest, NULL));

    for(UINT iFace=0; iFace < 6; iFace++)
    {
        // Lock the source texture for reading
        D3DLOCKED_RECT rcSrc;
        V_RETURN(pTexSrc->LockRect((D3DCUBEMAP_FACES)iFace, 0, &rcSrc, NULL, D3DLOCK_READONLY));

        // Lock the destination texture for writing
        D3DLOCKED_RECT rcDest;
        V_RETURN((*ppTexDest)->LockRect((D3DCUBEMAP_FACES)iFace, 0, &rcDest, NULL, 0));

        BYTE* pSrcBytes = (BYTE*) rcSrc.pBits;
        BYTE* pDestBytes = (BYTE*) rcDest.pBits;


        for(UINT y=0; y < desc.Height; y++)
        {
            D3DXFLOAT16* pSrc = (D3DXFLOAT16*) pSrcBytes;
            BYTE* pDest = pDestBytes;

            for(UINT x=0; x < desc.Width; x++)
            {
                switch(eTarget)
                {
                    case RGBE8: EncodeRGBE8(pSrc, &pDest); break;
                    case RGB16: EncodeRGB16(pSrc, &pDest); break;
                    default: return E_FAIL;
                }

                pSrc += 4;
            }

            pSrcBytes += rcSrc.Pitch;
            pDestBytes += rcDest.Pitch;
        }

        // Release the locks
        (*ppTexDest)->UnlockRect((D3DCUBEMAP_FACES)iFace, 0);
        pTexSrc->UnlockRect((D3DCUBEMAP_FACES)iFace, 0);
    }

    return S_OK;
}

