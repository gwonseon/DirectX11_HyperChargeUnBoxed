#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"
#include "GameInstance.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)
class CPlayer2 final: public CContainerObject
{
public:
	typedef struct: public CGameObject::GAMEOBJ_DESC
	{
		LEVELID m_eLevelID{};
		_vector* vCameraAt = {};
		_vector* vCameraPos = {};
		_uint* iRound = {};
		_uint iCellIdx{};
	}PLAYER_DESC;



};

END
