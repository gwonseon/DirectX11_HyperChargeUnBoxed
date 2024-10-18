#include "..\Public\Bone.h"

CBone::CBone()
{
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (-1 == m_iParentBoneIndex)
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);

		return;
	}	

	XMStoreFloat4x4(&m_CombinedTransformationMatrix,
		XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix());
}

void CBone::Update_CombinedTransformationMatrix(const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix, _float fTimeDelta)
{
	if (-1 == m_iParentBoneIndex)
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);

		return;
	}
	string strName(m_szName);
	if (strName == "prop1" || strName == "prop2" || strName == "prop3" || strName == "prop4")
	{
		XMMATRIX rotationMatrix = XMMatrixRotationZ(fTimeDelta * 50.f);
		_float4x4 floatMatrix{};
		XMStoreFloat4x4(&floatMatrix, rotationMatrix);
		XMMATRIX transformationMatrix = XMLoadFloat4x4(&m_TransformationMatrix);
		XMMATRIX resultMatrix = transformationMatrix * rotationMatrix;
		XMStoreFloat4x4(&m_TransformationMatrix, resultMatrix);
	}

	XMStoreFloat4x4(&m_CombinedTransformationMatrix,
		XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix());
}

void CBone::Update_CombinedTransformationMatrix(const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix, _float fRotation_Angle, _uint iUpperMotion)
{
	if (-1 == m_iParentBoneIndex)
	{

		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);

		return;
	}

	string strName(m_szName);
	/*if (GetAsyncKeyState(VK_UP))
	{
		fChest += 0.1f;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		fChest -= 0.1f;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		fLowerBody += 0.1f;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		fLowerBody -= 0.1f;
	}

	if (GetAsyncKeyState('P'))
	{
		cout << "fChest		: " << fChest << endl;
		cout << "fLowerBody : " << fLowerBody << endl;

	}*/

	if (iUpperMotion == ATTACK_KATANA_MOTION)
	{
		if (strName == "upperarm_R_SKEL" || strName == "upperarm_L_SKEL" )
		{
			// 뼈 위치 위로 올려주기
			XMMATRIX positionMatrix = XMMatrixTranslation(9.f, 0.f, 0.f);
			_float4x4 floatMatrix{};
			XMStoreFloat4x4(&floatMatrix, positionMatrix);
			XMMATRIX transformationMatrix = XMLoadFloat4x4(&m_TransformationMatrix);
			XMMATRIX resultMatrix = transformationMatrix * positionMatrix;
			XMStoreFloat4x4(&m_TransformationMatrix, resultMatrix);
		}
	}


	

	XMStoreFloat4x4(&m_CombinedTransformationMatrix,
		XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix());

	


}

HRESULT CBone::Initialize(_uint iParentBoneIndex, HANDLE hFileRead)
{
	_uint iBoneNameLen = 0;
	ReadFile(hFileRead, &iBoneNameLen, sizeof(_uint), &dwByte, nullptr);

	char* szName = new char[iBoneNameLen + 1];
	ReadFile(hFileRead, szName, iBoneNameLen * sizeof(_char), &dwByte, nullptr);
	szName[iBoneNameLen] = '\0'; 
	// cout << szName << endl;
	strcpy_s(m_szName, szName);  
	delete[] szName;

	
	ReadFile(hFileRead, &m_TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);	
	
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	
	//cout << m_TransformationMatrix._11 << "    " << m_TransformationMatrix._12 << "    " <<m_TransformationMatrix._13 << "     " << m_TransformationMatrix._14 << endl;
	//cout << m_TransformationMatrix._21 << "    " << m_TransformationMatrix._22 << "    " <<m_TransformationMatrix._23 << "     " << m_TransformationMatrix._24 << endl;
	//cout << m_TransformationMatrix._31 << "    " << m_TransformationMatrix._32 << "    " <<m_TransformationMatrix._33 << "     " << m_TransformationMatrix._34 << endl;
	//cout << m_TransformationMatrix._41 << "    " << m_TransformationMatrix._42 << "    " <<m_TransformationMatrix._43 << "     " << m_TransformationMatrix._44 << endl;
	//cout << "---------------------------------------------------------------------------------------------------------" << endl;
	ReadFile(hFileRead, &m_iParentBoneIndex, sizeof(_uint), &dwByte, nullptr);
	// cout << "부모 뼈 : " <<m_iParentBoneIndex << endl;
	
	return S_OK;
}



CBone* CBone::Create(_uint iParentBoneIndex, HANDLE hFileRead)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(iParentBoneIndex, hFileRead)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();

}

