
#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;


texture2D g_Texture;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

texture2D g_NormalTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;


texture2D g_ShadeTexture;
texture2D g_DiffuseTexture;
texture2D g_LightDepthTexture;
texture2D g_FinalTexture;
texture2D g_BlurTexture;
Texture2D g_BrightPassTexture;
bool g_bFog;
vector g_vMtrlAmbient = { 1.f, 1.f, 1.f, 1.f };
vector g_vMtrlSpecular = { 0.6f, 0.6f, 0.6f, 1.f };
vector g_vCamPosition;
float g_fCamFar;
// 텍스쳐에서 한 픽셀의 간격
float dX;
float dY;

// 안개
float g_FogStart;
float g_FogEnd;

float3 g_fDirection;
float g_fAngle;


float4 Compute_WorldPos(float2 vTexcoord)
{
    float4 vWorldPos = 0.f;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
    float fViewZ = vDepthDesc.y * g_fCamFar;
	
    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    return vWorldPos;
}


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


struct PS_OUT
{
    vector vColor : SV_TARGET0;
};


PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};


PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;

	/* 빛 정보와 노말 정보를 이용해서 명암을 계산하여 리턴하낟. */
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fCamFar;
	/* 0 ~ 1 -> -1 ~ 1 */
    vector vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    float fShade = max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f);
    Out.vShade = g_vLightDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient));
    float4 vWorldPos;
	/* 투영스페이스 상의 완벽한 픽셀의 위치를 구했다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 튜ㅜ영행렬 / w */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 완벽한 픽셀의 위치를 구했다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 튜ㅜ영행렬 / w */
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스로 이동하자. */
	/* 월드 페이스 상의 완벽한 픽셀의 위치를 구했다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(g_vLightDir), vNormal);

    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    return Out;
}



PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;

	/* 빛 정보와 노말 정보를 이용해서 명암을 계산하여 리턴하낟. */
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fCamFar;
    vector vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    float4 vWorldPos;

	/* 투영스페이스 상의 완벽한 픽셀의 위치를 구했다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 튜ㅜ영행렬 / w */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 완벽한 픽셀의 위치를 구했다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 튜ㅜ영행렬 / w */
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스로 이동하자. */
	/* 월드 페이스 상의 완벽한 픽셀의 위치를 구했다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    float fShade = max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f);
    Out.vShade = (g_vLightDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))) * fAtt;
    
    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(vLightDir), vNormal);

    float fSpecular = (pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f)) * fAtt * 3.f;

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    return Out;
}

  
PS_OUT_LIGHT PS_MAIN_LIGHT_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out;

   	/* 빛 정보와 노말 정보를 이용해서 명암을 계산하여 리턴하낟. */
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fCamFar;
    vector vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    float4 vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 완벽한 픽셀의 위치를 구했다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 튜ㅜ영행렬 / w */
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드스페이스로 이동하자. */
	/* 월드 페이스 상의 완벽한 픽셀의 위치를 구했다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    // 거리
    float fDistance = length(vWorldPos - g_vLightPos);
    // 스포트라이트 계산
    float fShade = Calc_Spot_LightPower(g_vLightDir.xyz, g_vLightPos.xyz, vNormal.xyz, vWorldPos.xyz, g_fAngle);
    // 감쇠
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    // 빛
    Out.vShade = (g_vLightDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))) * fAtt;

    // 스페큘러 계산
    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(g_vLightDir), vNormal);
    float fSpecular = (pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f)) * fAtt;
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
    return Out;
}


PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.a == 0.f)
        discard;


    vector vPosition = Compute_WorldPos(In.vTexcoord);

    vPosition = mul(vPosition, g_LightViewMatrix);
    vPosition = mul(vPosition, g_LightProjMatrix);
	

    float2 vTexcoord = 0.f;

    vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;

    vector vOldDepth = g_LightDepthTexture.Sample(LinearSampler, vTexcoord);

    Out.vColor = vDiffuse * vShade + vSpecular;

    if (vPosition.w - 0.15f > vOldDepth.y * g_fCamFar)
    {
        Out.vColor.rgb *= 0.7f;
		
    }
    return Out;
}


PS_OUT PS_BRIGHT_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vColor = g_FinalTexture.Sample(PointSampler, In.vTexcoord);
    Out.vColor = float4(vColor.rgb, 1.f);
    
    // 블룸 조건
    //float BrightColor = 1.f;
    //float redThreshold = 0.3; // 빨간색의 비율을 나타내는 기준값
    //float greenThreshold = 0.3;
    //float blueThreshold = 0.0f;
    //if (vColor.r >= redThreshold && vColor.g >= greenThreshold && vColor.b <= blueThreshold)
    //{
    //   // if (vColor.r <= redThreshold + 0.2f && vColor.g <= greenThreshold + 0.2f )
    //    {
            
    //        Out.vColor = float4(vColor.rgb, 1.f);
    //    }
    //}
    //else
    //{
    //    // 나머지 색상은 제거
    //    Out.vColor = float4(0, 0, 0, 0);
    //}
    return Out;
    
}

struct PS_OUT_BLUR
{
    vector vBlur : SV_TARGET0;
};

float g_fWeights[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.5261, 0.6231, 0.7f, 0.6231, 0.5261, 0.4868, 0.278, 0.1353, 0.0561
};
float g_fWeights2[5] =
{
    0.7261, 0.9231, 1.f, 0.9231, 0.7261
};
float Bloom_Weights[5] =
{
    0.0545, 0.2442, 1.f, 0.2442, 0.0545
};
const float Bloom_Weights2[5] = { 0.0545, 0.2442, 0.6026, 0.2442, 0.0545 };
const float Bloom_Weights3[7] =
{
    0.028, 0.100, 0.233, 0.278, 0.233, 0.100, 0.028
};
PS_OUT_BLUR PS_MAIN_BLUR_X_BLOOM(PS_IN In)
{
    PS_OUT_BLUR Out = (PS_OUT_BLUR) 0;

    float2 vBlurUV = (float2) 0.f;

    for (int i = -2; i < 3; i++)
    {
        vBlurUV = In.vTexcoord + float2(1.f / 1280.f * i, 0.f);
        Out.vBlur += g_fWeights2[i + 2] * g_FinalTexture.Sample(PointSampler, vBlurUV);
    }
 
    Out.vBlur /= 2.f;

    return Out;
}
PS_OUT_BLUR PS_MAIN_BLUR_Y_BLOOM(PS_IN In)
{
    PS_OUT_BLUR Out = (PS_OUT_BLUR) 0;

    float2 vBlurUV = (float2) 0.f;

    for (int i = -2; i < 3; i++)
    {
        vBlurUV = In.vTexcoord + float2(0.f, 1.f / 720.f * i);
        Out.vBlur += g_fWeights2[i + 2] * g_FinalTexture.Sample(PointSampler, vBlurUV);
    }

    Out.vBlur /= 2.f;

    return Out;
}
PS_OUT PS_MAIN_BLOOM_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 clampedUV = clamp(In.vTexcoord, float2(0.0f, 0.0f), float2(1.0f, 1.0f));
    vector vBlur = g_BlurTexture.Sample(PointSampler, clampedUV);
    vector vFinal = g_FinalTexture.Sample(PointSampler, clampedUV);

    float redFactor = vFinal.g > vFinal.b && vFinal.r > vFinal.b ? 1.5f : 1.f;
    Out.vColor = vBlur /** redFactor*/ + vFinal;

    return Out;
}

PS_OUT_BLUR PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT_BLUR Out = (PS_OUT_BLUR) 0;

    float2 vBlurUV = (float2) 0.f;

    for (int i = -6; i < 7; i++)
    {
        vBlurUV = In.vTexcoord + float2(1.f / 1280.f * i, 0.f);
        Out.vBlur += g_fWeights[i + 6] * g_FinalTexture.Sample(LinearSampler, vBlurUV);
    }

    Out.vBlur /= 10.f;

    return Out;
}
PS_OUT_BLUR PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT_BLUR Out = (PS_OUT_BLUR) 0;

    float2 vBlurUV = (float2) 0.f;
    for (int i = -6; i < 7; i++)
    {
        vBlurUV = In.vTexcoord + float2(0.f, 1.f / 720.f * i);
        Out.vBlur += g_fWeights[i + 6] * g_FinalTexture.Sample(LinearSampler, vBlurUV);
    }

    Out.vBlur /= 10.f;

    return Out;
}
PS_OUT PS_MAIN_BLUR_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vBlur = g_BlurTexture.Sample(LinearSampler, In.vTexcoord);
    vector vFinal = g_FinalTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = vBlur + vFinal;

    return Out;
}



// 그대로 출력 (다운 샘플링 용도로 사용)
PS_OUT PS_MAIN_DownSample(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler_Clamp, In.vTexcoord);
    
    Out.vColor = vDiffuse;
    
    return Out;
}

PS_OUT PS_MAIN_BLUR_X_DownSample(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);
    for (int i = 0; i < 5; i++)
        vDiffuse += Bloom_Weights2[i] * g_DiffuseTexture.Sample(LinearSampler_Clamp, In.vTexcoord + float2(dX, 0.0) * float(i - 2));
    Out.vColor = vDiffuse;
    return Out;
}

PS_OUT PS_MAIN_BLUR_Y_DownSample(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);
    for (int i = 0; i < 5; i++)
        vDiffuse += Bloom_Weights2[i] * g_DiffuseTexture.Sample(LinearSampler_Clamp, In.vTexcoord + float2(0.0, dY) * float(i - 2));
    Out.vColor = vDiffuse;
    return Out;
}

PS_OUT PS_MAIN_FOG(PS_IN In)
{
    // 지수 안개
    //PS_OUT Out = (PS_OUT) 0;
    //float4 pixelColor = g_FinalTexture.Sample(LinearSampler_Clamp, In.vTexcoord);
    //float pixelDepth = g_DepthTexture.Sample(PointSampler, In.vTexcoord).y;
    //float fogFactor = 1.f / 2.7182 * (pixelDepth * 0.8f);
    //float4 FogColor = { 0.7f, 0.5f, 0.f ,1.f};
    //float4 finalColor = fogFactor * pixelColor + (1 - fogFactor) * FogColor;
    //Out.vColor = finalColor;
    //return Out;
    
    // 지수 안개2
    //float g_FogDensity = 1.f; 
    //float fogFactor = exp2(pow((g_FogDensity * pixelDepth), 2));
    //fogFactor = 1.f / 2.7182 * saturate(fogFactor);
    

    //PS_OUT Out = (PS_OUT) 0;
    
    //float4 pixelColor = g_FinalTexture.Sample(LinearSampler_Clamp, In.vTexcoord);
    //float pixelDepth = g_DepthTexture.Sample(PointSampler, In.vTexcoord).y;
    
    //float fogStart = g_FogStart / g_FogEnd;
    //float fogEnd = g_FogEnd / g_FogEnd;
    //float fogFactor = saturate(( pixelDepth) / (fogEnd - fogStart));
    
    //float4 FogColor = float4(1.f, 0.7f, 0.0f, 1.0f);
    //float4 finalColor = (1 - fogFactor )* pixelColor + (fogFactor) * FogColor;

    //Out.vColor = finalColor;
    //if (g_bFog == true)
    //    Out.vColor = finalColor;
    //else
    //    Out.vColor = pixelColor;
    
    
    
    PS_OUT Out = (PS_OUT) 0;
    float4 pixelColor = g_FinalTexture.Sample(LinearSampler_Clamp, In.vTexcoord);
    float pixelDepth = g_DepthTexture.Sample(PointSampler, In.vTexcoord).y;
    float fogFactor = saturate((pixelDepth) / (g_FogEnd - g_FogStart));
    float4 FogColor = float4(1.f, 0.7f, 0.4f, 1.0f);
    float4 finalColor = lerp(pixelColor, FogColor, fogFactor);
    if (g_bFog == true)
        Out.vColor = finalColor;
    else
        Out.vColor = pixelColor;
    
    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OneByOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }

    pass Light_Point // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OneByOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
    }

    pass Final // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

    pass BlurX // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass BlurY // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

    pass Blur_Final // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_FINAL();
    }

    pass Bright_Extraction // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BRIGHT_COLOR();
    }

    pass BloomX // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X_BLOOM();
    }

    pass BloomY // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y_BLOOM();
    }

    pass Bloom_Final // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM_FINAL();
    }
// ------------------------------------------------------------------
    pass Bloom_Season2 // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DownSample();
    }

    pass Bloom_Season2_X // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X_DownSample();
    }

    pass Bloom_Season2_Y // 13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y_DownSample();
    }

    pass Pass_Fog // 14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FOG();
    }

    pass Light_Spot // 15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OneByOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_SPOT();
    }
}