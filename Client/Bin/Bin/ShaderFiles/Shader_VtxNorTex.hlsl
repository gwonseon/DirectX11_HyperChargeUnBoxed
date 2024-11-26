#include "Engine_Shader_Defines.hlsli"

matrix              g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4 g_vLightDir;
float4 g_vLightDiffuse;
float4 g_vLightAmbient;
float4 g_vLightSpecular;

texture2D g_DiffuseTexture;
float4 g_vMtrlAmbient = float4(0.4f, 0.4f, 0.4f, 1.f);
float4 g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4 g_vCamPosition;


//m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

// 샘플러 선언과 동시에 초기화하기, Min Mag Mip 모두 Linear(선형으)로 초기화
sampler LinearSampler = sampler_state
{ 
// 텍스처 좌표가 0에서 1을 벗어날 때 텍스처가 반복되도록 처리한다.
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP; 
    AddressV = WRAP;
};


sampler PointSampler = sampler_state
{
    filter = MIN_MAG_MIP_POINT;

};


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{ 
    float4 vPosition : SV_POSITION;  
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};



VS_OUT VS_MAIN(VS_IN In) // 진입점 함수 ( 내가 원하는 이름으로 만들 수 있다)
{
    VS_OUT Out = (VS_OUT) 0;
    // VS_IN 타입의 포지션 벡터는 float3로 정의되어있지만 변환 행렬은 4x4 로 이루어져있따.
    // 그래서 mul 함수의 조건을 충족하지 못해 1x4로 확장하고 변환(행렬곱)을 수행해야 한다.
    //  1.f 는 w를 의미한다. 줄이면 원근 효과를 줄 수 있으나 0이되면 원근 효과 적용 안된다
    vector vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix); 
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    
    Out.vPosition = vPosition;
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    return Out;
    
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};


struct PS_OUT
{
	/* 변수에 대한 시멘틱을 정의한다. */
    vector vColor : SV_TARGET0;

};

/*
버텍스 셰이더에서 픽셀 셰이더로 넘어갈 때, SV_POSITION 과 TEXCOORD의 차이
SV_POSITION은 버텍스 셰이더에서 나오고 픽셀 셰이더로 들어갈 때 Z나누기와 레스터화를 거쳐 픽셀 좌표( 뷰 포트상의 좌표 )로 변한다.
TEXCOORD는 버텍스 사이의 보간만 발생하고, 픽셀 좌표계로 변환되지 않는다. 즉, 뷰스페이스로 변환되지 않은 상태이다.

*/
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0; // 0초기화
    // In.vTexcoord : 텍스처 좌표
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * 30.f); // g_DiffuseTexture 에서  In.vTexcoord * 30.f에 해다하는 텍스처 좌표 샘플링한다.
    
    // 조명을 계산한다. In.vNormal : 픽셀의 법선 벡터,  g_vLightDir : 빛의 방향 
    // dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)) : 광원과 픽셀 표면의 각도를 계산해 Diffuse 값을 구한다.
    // (g_vLightAmbient * g_vMtrlAmbient): 주변광을 계산하는데 사용된다.
    // max : 음수 방지
    float4 vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient);
    
    // 빛의 반사 벡터를 계산한다. In.vNormal에 대해 광원의 방향을 반사한다
    float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    float4 vLook = In.vWorldPos - g_vCamPosition;
    
    // 반사광을 계산한다.
    // 반사된 빛의 방향(vReflect) 와 카메라를 향하는 방향(vLook) 사이의 각도를 구한다.
    // 이를 기반으로 스페큘러를 구한다.
    // pos를 통해 강도결정한다.
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 20.f);
    
    // 최종 픽셀 색을 계산한다.
    // (g_vLightDiffuse * vMtrlDiffuse) * saturate(vShade): 디퓨즈 색상
    //  (g_vLightSpecular * g_vMtrlSpecular) * fSpecular : 스페큘러 색상
    // 둘을 합하여 최종 색 결정한다.+
    
    
    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(vShade) + (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
    
    return Out;

}


technique11 DefaultTechnique // Technique : 어떤 버전으로 적혔는지 구분한다.
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();

    }
   
}