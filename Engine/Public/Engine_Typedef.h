#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{
	typedef		bool						_bool;

	typedef		signed char					_byte;
	typedef		unsigned char				_ubyte;
	typedef		char						_char;
	typedef		wchar_t						_tchar;

	typedef		wstring						_wstring;

	typedef		signed short				_short;
	typedef		unsigned short				_ushort;

	typedef		signed int					_int;
	typedef		unsigned int				_uint;

	typedef		signed long					_long;
	typedef		unsigned long				_ulong;

	typedef		float						_float;
	typedef		double						_double;

	// 단순 저장용 벡터와 행렬 타입 ( 연산용이 아니다)
	typedef		XMFLOAT2					_float2;
	typedef		XMFLOAT3					_float3;
	typedef		XMFLOAT4					_float4;
	typedef		XMFLOAT4X4					_float4x4;



	// 저장이 가능하긴 하지만 연산용 벡터와 행렬
	typedef		XMVECTOR					_vector;
	typedef		FXMVECTOR					_fvector;
	typedef		GXMVECTOR					_gvector;
	typedef		HXMVECTOR					_hvector;
	typedef		CXMVECTOR					_cvector;
	typedef		XMMATRIX					_matrix;
	typedef		FXMMATRIX					_fmatrix;
	typedef		CXMMATRIX					_cmatrix;

	/*
	연산의 최적화를 위해 저장용과 연산용을 따로 한다.
	연산을 할 때는 저장용타입을 연산용 타입으로 치환해서 사용할덧



	현재 대부분의 CPU는 멀티 코어를 지원한다.
	그러한 특성을 이용하여 연산의 병렬화를 통해 빠른 연산을 수행할 수 있다.
	병렬화를 위해 데이터를 조금 다른 방식으로 저장해야 하는데 
	그래서 XMVECTOR, XMMATRIX에 데이터를 저장하고 SIMD 가속을 받는 병렬 연산을 수행하는 것이다.
	XMVECTOR 를 이용한 연산을 수행할 때 데이터들이 안정적인 공간에 존재하는 것을 권장하낟,
	따라서 스택 메모리에 XMVECTOR를 두고 사용하는 것이 좋다. ( 함수의 지역변수로서 사용한다는 의미)
	스택 메모리는 전역적으로 접근하는 공간이 아니기에 비교적 안정적이기 때문이다.

	또한 데이터 영역을 함수의 파라미터로 넘길 때에도 파라미터 넘버에 따라 넘겨야 하는 타입이 정해져있다.
	1 ~ 3번 : FXMVECTOR
	4번 : GXMVECTOR
	5 ~ 6번 : HXMVECTOR
	7번 이상 : CXMVECTOR

	1번 : FXMMATRIX
	2번 이상 : CXMMATRIX

	
	*/

}

#endif // Engine_Typedef_h__
