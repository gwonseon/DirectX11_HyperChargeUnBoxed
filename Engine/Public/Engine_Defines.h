#pragma once

#pragma warning (disable : 4251)
#pragma warning (disable : 5208)

#include <d3d11.h>
#include <d3dcompiler.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <DirectXCollision.h>


#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"
#include "DirectXTK\Effects.h"
#include "Effects11\d3dx11effect.h"


using namespace std;
using namespace DirectX;



#include <string>
#include <vector>
#include <list>	
#include <queue>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <iostream>

#include <DirectXMath.h>


#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"



namespace Engine
{
	const _wstring g_strTransformTag = TEXT("Com_Transform"); // Transform 과 Component 두 맵에 넣어줘야 하니 전역적인 이름으로 만들어줌

	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };


	enum aiTextureType
	{
		aiTextureType_NONE = 0,
		aiTextureType_DIFFUSE = 1,
		aiTextureType_SPECULAR = 2,
		aiTextureType_AMBIENT = 3,
		aiTextureType_EMISSIVE = 4,
		aiTextureType_HEIGHT = 5,
		aiTextureType_NORMALS = 6,
		aiTextureType_SHININESS = 7,
		aiTextureType_OPACITY = 8,
		aiTextureType_DISPLACEMENT = 9,
		aiTextureType_LIGHTMAP = 10,
		aiTextureType_REFLECTION = 11,
		aiTextureType_BASE_COLOR = 12,
		aiTextureType_NORMAL_CAMERA = 13,
		aiTextureType_EMISSION_COLOR = 14,
		aiTextureType_METALNESS = 15,
		aiTextureType_DIFFUSE_ROUGHNESS = 16,
		aiTextureType_AMBIENT_OCCLUSION = 17,

		aiTextureType_UNKNOWN = 18,

#ifndef SWIG
		_aiTextureType_Force32Bit = INT_MAX
#endif
	};


}

using namespace Engine;

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG
