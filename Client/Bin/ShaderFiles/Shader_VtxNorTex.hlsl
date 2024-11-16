#include "Engine_Shader_Defines.hlsli"

matrix              g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
float4 g_vCamPosition;


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
    float4 vProjPos : TEXCOORD2;
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
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;

    return Out;
    
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};


struct PS_OUT
{
	/* 변수에 대한 시멘틱을 정의한다. */
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;

};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0; // 0초기화
    // In.vTexcoord : 텍스처 좌표
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vDiffuse = vector(vMtrlDiffuse.rgb, 1.f);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
	
    return Out;

}


technique11 DefaultTechnique // Technique : 어떤 버전으로 적혔는지 구분한다.
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();

    }
   
}