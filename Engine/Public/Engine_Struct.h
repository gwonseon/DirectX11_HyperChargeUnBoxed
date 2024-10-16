#pragma once

namespace Engine
{

	typedef struct
	{
		HINSTANCE		hInstance;
		HWND			hWnd;
		unsigned int	iWinSizeX, iWinSizeY;
		bool			isWindowed;
		unsigned int	iNumLevels;
	}ENGINE_DESC;



	typedef struct
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOT };

		TYPE		eType;
		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;

	}LIGHT_DESC;

	typedef struct
	{
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
		XMFLOAT4	vEmissive;

	}MATERIAL;

	//0 1 0 1		
	//1 0 0 1 
 // = 0 0 0 1
	//Light.Diffuse * Mtrl.Diffuse



	typedef struct
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vPosition;
		float			fTrackPosition;
	}KEYFRAME;

	typedef struct ENGINE_DLL
	{
		/* 정점의 위치 (Position)*/
		XMFLOAT3		vPosition;

		static const unsigned int		iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[1];
	}VTXPOS;

	typedef struct ENGINE_DLL
	{
		/* 정점의 위치 (Position)*/
		XMFLOAT3		vPosition;
		/* 텍스쳐의 색을 가져오기위한 좌표.(Texcoord) */
		XMFLOAT2		vTexcoord;

		static const unsigned int		iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXPOSTEX;



	typedef struct ENGINE_DLL
	{
		/* 정점의 위치 (Position)*/
		XMFLOAT3		vPosition;

		XMFLOAT3		vNormal;
		/* 텍스쳐의 색을 가져오기위한 좌표.(Texcoord) */
		XMFLOAT2		vTexcoord;

		static const unsigned int		iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXNORTEX;


	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		// XMFLOAT2		vTexcoord1;

		XMFLOAT3		vTangent;

		static const unsigned int		iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMESH;


	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		/* 이 정점이 영향을 받아야할 뼈들의 인덱스 */
		XMUINT4			vBlendIndex;

		/* 영향을 받아야하는 뼈들의 가중치. */
		XMFLOAT4		vBlendWeight;

		static const unsigned int		iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXANIMMESH;





}


// XMFLOAT3  : 저장용 벡터 타입, 연산자 오버로딩이 안되어 있다.
// 
// 구조체가 Elements와 정점의 개수까지 가지고 있게 만들었다
// 구조체가 부가 정보인 num과 Elements에 대한 용량까지 차지하면 안되기에 static으로 선언한다.
// 정점변수의 경우 헤더를 Include 할 때마다 재정의 하니까 const로 방지하고
// cpp에서 다시 구현한다.
// 이제 정적 멤버에 대한 선언부는 Engine_Struct.h 에 
// 구현부는 Engine_Struct.cpp에 존재한다.
// 하지만 cpp에 대한 정보를 클라이언트가 모르니 버텍스 구조체를 ENGINE_DLL로 만들어서 라이브러리 파일로 들어가게 만든다.
// 라이브러리 파일로 들어가게 만들면 클라이언트에서도 엔진에 정의된 cpp 파일에서 elements 를 초기화하는 부분에 접근할 수 있게 된다.
