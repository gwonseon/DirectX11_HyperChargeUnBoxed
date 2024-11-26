#include "Engine_Shader_Defines.hlsli"
//      전역변수들 : 컨스턴트 테이블
//      같은 파일 내에 존재하는 모든 함수에서 전역변수를 사용할 수 있다. 대입은 불가하다.
//      외부프로젝트에서 쉐이더 전역으로 특정 데이터를 던지고 받기 위한 메모리 공간을 의미한다.
//      전역변수는 다른 쉐이더파일에 같은 타입과 이름으로 선언된 변수가 있다라면 메모리 공간을 공유한다.


float2 g_Index, g_Winsize;
float  g_Time;
matrix              g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D           g_Texture;

//m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

// 샘플러 선언과 동시에 초기화하기, Min Mag Mip 모두 Linear(선형으)로 초기화
sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;

};


sampler PointSampler = sampler_state
{
    filter = MIN_MAG_MIP_POINT;
};


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    
};

struct VS_OUT
{ // 포지션을 1x4를 통해 4x4로 바꿨지 때문에 float4여야 한다.
    
    // 시멘틱 ( Semantic )
    // HLSL에서 변수는 시멘틱 설정이 가능하다
    // 시멘틱은 변수의 입력과 출력을 확인 또는 지정하거나
    // 데이터의 출처와 역할에 대한 분명한 의미를 부여하기 위해 함수, 변수, 인수 뒤에 선택적으로 붙여서 서술하는 것이다.
    // SV_ == 이 값에 대한 연산을 마무리 했다는 의미이다.
    float4 vPosition : SV_POSITION;   // : 뒤에 시멘틱 이름을 붙인다.
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float2 vInfo : TEXCOORD2;

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
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = vPosition; // 이 값이 픽셀 셰이더에 들어갈 때는 보간된 투영 좌표(z나누기 안된 상태) 가 들어갈 것이다.
    Out.vInfo = g_Index;
    return Out;
    
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float2 vInfo : TEXCOORD2;
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
    PS_OUT Out = (PS_OUT) 0;
 //   Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
    // 색으로 채우는 것이 이미지를 가져와서 색을 채워줌
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    float fDuration = 10.f;
    float fProgress = frac(g_Time / fDuration);
    
    float fInner_Radius = 35.f;
    float fOuter_Radius = 38.f;
    
    float fMiddle_Radius = (fInner_Radius + fOuter_Radius) * 0.5f;
    float fHalf_Width = (fOuter_Radius - fInner_Radius) * 0.5f;
    
    float2 fPos = In.vPosition.xy - (g_Winsize.xy * 0.5f);
    float fRadius = length(fPos.xy);

    float fResult_Radius = fHalf_Width - abs(fRadius - fMiddle_Radius) + 1.f;
    fResult_Radius = clamp(fResult_Radius, 0.f, 1.f);

    float fAngle = atan2(-fPos.x, fPos.y)  + 3.141532f;
    float fResult_Angle = (fAngle - fProgress * 2.f * 3.141532f) + 1.f;

    
    fResult_Angle = clamp(fResult_Angle , 0.f, 1.f);
    
    float4 color2 = float4(0.5f, 0.5f, 0.5f, 1.f);
    float4 color = float4(0.9f, 0.9f, 0.9f, 1.f);

    float4 fResult_Color = lerp(color, color2, fResult_Angle );
    fResult_Color.a *= fResult_Radius;
    
    float4 BackGround = float4(0.f, 0.f, 0.f, 0.f);
    fResult_Color = lerp(BackGround, fResult_Color, fResult_Color.a);
    

    if (fResult_Color.a < 0.001)  // 0에 가까운 값을 체크하여 불필요한 픽셀을 제거
        discard;
    Out.vColor = fResult_Color;
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