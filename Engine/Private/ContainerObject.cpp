#include "..\Public\ContainerObject.h"

#include "PartObject.h"
#include "GameInstance.h"

CContainerObject::CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CContainerObject::CContainerObject(const CContainerObject& Prototype)
	: CGameObject{ Prototype }
{
}

CComponent* CContainerObject::Find_Component(const _wstring& strComponentTag, _uint iPartObjID)
{
	return m_PartObjects[iPartObjID]->Find_Component(strComponentTag);
}

HRESULT CContainerObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CContainerObject::Initialize(void* pArg)
{
	CONTAINEROBJECT_DESC* pDesc = static_cast<CONTAINEROBJECT_DESC*>(pArg);
	m_iNumPartObjects = pDesc->iNumPartObjects;
	m_PartObjects.resize(m_iNumPartObjects);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CContainerObject::Priority_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
	{
		if (nullptr != pPartObject)
			pPartObject->Priority_Update(fTimeDelta);
	}

}

void CContainerObject::Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
	{
		if (nullptr != pPartObject)
			pPartObject->Update(fTimeDelta);
	}
}

void CContainerObject::Late_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
	{
		if (nullptr != pPartObject)
			pPartObject->Late_Update(fTimeDelta);
	}
}

HRESULT CContainerObject::Render()
{
	return S_OK;
}


HRESULT CContainerObject::Add_PartObject(const _wstring& strPrototypeTag, _uint iPartObjectIndex, void* pArg)
{
	CGameObject* pGameObject = m_pGameInstance->Clone_Prototype(strPrototypeTag, pArg);
	if (nullptr == pGameObject)
		return E_FAIL;
	
	m_PartObjects[iPartObjectIndex] = static_cast<CPartObject*>(pGameObject);

	return S_OK;
}

CPartObject* CContainerObject::Get_PartObject(_uint iPartObjectIndex)
{
	return static_cast<CPartObject*>(m_PartObjects[iPartObjectIndex]);
}

void CContainerObject::Free()
{
	__super::Free();

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);

	m_PartObjects.clear();


}
