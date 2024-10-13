
//      전역변수들 : 컨스턴트 테이블
//      같은 파일 내에 존재하는 모든 함수에서 전역변수를 사용할 수 있다. 대입은 불가하다.
//      외부프로젝트에서 쉐이더 전역으로 특정 데이터를 던지고 받기 위한 메모리 공간을 의미한다.
//      전역변수는 다른 쉐이더파일에 같은 타입과 이름으로 선언된 변수가 있다라면 메모리 공간을 공유한다.

float2              g_Index;

float               g_Percent, g_ImgSize, g_fGageAmount;
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
// 그냥 출력
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
 //   Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
    // 색으로 채우는 것이 이미지를 가져와서 색을 채워줌
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
 //   Out.vColor.gb = Out.vColor.r; // 같으면 회색, 죽었을 때를 표현하면 좋을듯
    
    // 알파테스트 : 알파값을 기준으로 그린다 안그린다를 고려한다. Dx11 에선 알파테스트가 사라지고 셰이파파일에서 비교하는 방식으로 바뀜
    // 깊이 테스트를 통과하여 레스터라이즈를 거쳤으나 알파 테스트 통과 못한 값은 파괴한다. 따라서 깊이 값 기록 안한다!
    //if(Out.vColor.a == 0.f) 
    //    discard;// 파괴한다. 

    //// 투영변환은 x,y를 변환전의 z값인 w로 나눔으로써 완성된다.
    //float2 vProjPos = In.vProjPos.xy / In.vProjPos.w;
    
    // 데미지를 입었을 때 이기능을 통해 빨간색으로 바꿔서 피격효과를 줄 수 있다.
    //if(vProjPos.x <= 1.f)
    //    Out.vColor.r = 0.1;
    
    return Out;

}

// 빨간색으로 변경
PS_OUT PS_MAIN2(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
 //   Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
    // 색으로 채우는 것이 이미지를 가져와서 색을 채워줌
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
 //   Out.vColor.gb = Out.vColor.r; // 같으면 회색, 죽었을 때를 표현하면 좋을듯
    
    
    // 알파테스트 : 알파값을 기준으로 그린다 안그린다를 고려한다. Dx11 에선 알파테스트가 사라지고 셰이파파일에서 비교하는 방식으로 바뀜
    // 깊이 테스트를 통과하여 레스터라이즈를 거쳤으나 알파 테스트 통과 못한 값은 파괴한다. 따라서 깊이 값 기록 안한다!
    if (Out.vColor.a == 0.f) 
        discard; // 파괴한다. 
 
    if (Out.vColor.r == 1.f && Out.vColor.g == 1.f && Out.vColor.b == 1.f)
        Out.vColor.gb = 0.f;
    
    // 투영변환은 x,y를 변환전의 z값인 w로 나눔으로써 완성된다.
//    float2 vProjPos = In.vProjPos.xy / In.vProjPos.w;
    
    return Out;

}


// API식 애니메이션
PS_OUT PS_MAIN3(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
  
    Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);
// 애니메이션의 총 아이콘 수
// 시작점 계산
    float2 fSize = float2(0.108, 0.108);
    float2 fStart = { g_Index.x * fSize.x + 0.216, g_Index.y * fSize.y + 0.216};

    float2 UV = fStart + fSize * In.vTexcoord;
    
    Out.vColor = g_Texture.Sample(PointSampler, UV);
 
    if (Out.vColor.a == 0.f) 
        discard;  

    
    
    return Out;

}

// 점점 투명해지는 박스
PS_OUT PS_MAIN4(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
 //   Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
    // 색으로 채우는 것이 이미지를 가져와서 색을 채워줌
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
 //   Out.vColor.gb = Out.vColor.r; // 같으면 회색, 죽었을 때를 표현하면 좋을듯
    
    // 알파테스트 : 알파값을 기준으로 그린다 안그린다를 고려한다. Dx11 에선 알파테스트가 사라지고 셰이파파일에서 비교하는 방식으로 바뀜
    // 깊이 테스트를 통과하여 레스터라이즈를 거쳤으나 알파 테스트 통과 못한 값은 파괴한다. 따라서 깊이 값 기록 안한다!
    Out.vColor.r = 0.f;
    Out.vColor.g = 0.f;
    Out.vColor.b = 0.f;
    Out.vColor.a = In.vTexcoord.x;
    return Out;
}

// 에너지 게이지 바 ( 중간 중간 잘림 )
PS_OUT PS_MAIN5(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
 //   Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
    // 색으로 채우는 것이 이미지를 가져와서 색을 채워줌
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    //   Out.vColor.gb = Out.vColor.r; // 같으면 회색, 죽었을 때를 표현하면 좋을듯
    // 알파테스트 : 알파값을 기준으로 그린다 안그린다를 고려한다. Dx11 에선 알파테스트가 사라지고 셰이파파일에서 비교하는 방식으로 바뀜
    // 깊이 테스트를 통과하여 레스터라이즈를 거쳤으나 알파 테스트 통과 못한 값은 파괴한다. 따라서 깊이 값 기록 안한다!
  
    float fXPos = In.vTexcoord.x;
    if (frac(fXPos * 10.f) < 0.4f)
    {
        Out.vColor.a = 1.0f; // 점이 그려질 부분
    }
    else
    {
        Out.vColor.a = 0.0f; // 점이 그려지지 않을 부분
    }
    
    float fCurrentGage = g_fGageAmount * 1.25f * 0.01;
    if (fCurrentGage <= In.vTexcoord.x)
        Out.vColor.a = 0.0f;
    
    return Out;
}
  
// 바 줄어들게
PS_OUT PS_MAIN6(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
 //   Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
    // 색으로 채우는 것이 이미지를 가져와서 색을 채워줌
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    //   Out.vColor.gb = Out.vColor.r; // 같으면 회색, 죽었을 때를 표현하면 좋을듯
    // 알파테스트 : 알파값을 기준으로 그린다 안그린다를 고려한다. Dx11 에선 알파테스트가 사라지고 셰이파파일에서 비교하는 방식으로 바뀜
    // 깊이 테스트를 통과하여 레스터라이즈를 거쳤으나 알파 테스트 통과 못한 값은 파괴한다. 따라서 깊이 값 기록 안한다!
  
  
    float fCurrentGage = g_fGageAmount   * 0.01;
    if (fCurrentGage <= In.vTexcoord.x)
        Out.vColor.a = 0.0f;
    
    return Out;
}

// Pass 는 그래픽 파이프라인 상태를 설정한다
// Pass는 여러개 할 수 있음 , 지금은 1개뿐, 그래서 Begin 함수 매개변수가 0이었음
// VertexShader 와 PixelShader 는 각각의 패스에서 사용할 셰이더의 프로그램을 지정한다.  위의 VS_MAIN 을 지정함


// 같은 픽셀에 대한 쉐이딩 방식을 여러 개 두기 위해 Pass를 여러 개 둔다.

//  compile vs_5_0 은 셰이더 모델 5.0을 사용하여 셰이더를 컴파일하도록 하는 명령어
technique11 DefaultTechnique // Technique : 어떤 버전으로 적혔는지 구분한다.
{
    pass DefaultPass
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();

    }
    pass DefaultPass1
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN2();

    }
    pass DefaultPass2
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN3();

    }
    pass DefaultPass3
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN4();
    }
    pass DefaultPass4
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN5();
    }
    pass DefaultPass5
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN6();
    }
	//pass DefaultPass1
	//{
	//	VertexShader = compile vs_5_0 VS_MAIN();
	//	PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	//}

	//pass DefaultPass2
	//{
	//	VertexShader = compile vs_5_0 VS_MAIN_DIS();
	//	PixelShader = compile ps_5_0 PS_MAIN_DIS();
	//}
}