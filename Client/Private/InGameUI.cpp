#include "stdafx.h"
#include "..\Public\InGameUI.h"

#include "GameInstance.h"

CInGameUI::CInGameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext } 
{

}

CInGameUI::CInGameUI(const CInGameUI& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CInGameUI::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CInGameUI::Initialize(void* pArg)
{
    INGAMEUI_DESC* pDesc = (INGAMEUI_DESC*)pArg;
    m_iCount = pDesc->m_iCount; // ArmCannon Count
    m_eUIType = pDesc->eUITag;
    m_iIndex = pDesc->iIndex;
    m_fUIPosition = { pDesc->fX, pDesc->fY, 0.f};
    m_pPlayer = pDesc->pPlayer;
    m_eLevel = pDesc->eLevel;
    m_pCircle = pDesc->pCircle;
    m_fTimer = pDesc->fTimer;
    m_iRound = pDesc->iRound;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(Add_Components(pDesc->iData)))
        return E_FAIL;
    if(m_eUIType == UI_PLAYER_HP )
        m_fPlayerHp = pDesc->fPlayerHP;
    if(m_eUIType == UI_PLAYER_ENERGY)
        m_fPlayerEnergy = pDesc->fPlayerEnergy;
    if (m_eUIType == UI_MACHINE_HP)
        m_fMachineHP = pDesc->fBrainHP;
    if (m_eUIType == UI_MACHINE_ENERGY)
        m_fMachineEnergy = pDesc->fBrainEnergy;
    if (m_eUIType == UI_CONVERSATIONBOX || m_eUIType == UI_CHARACTER)
        m_bDraw = false;

    return S_OK;
}


void CInGameUI::Priority_Update(_float fTimeDelta)
{
}

void CInGameUI::Update(_float fTimeDelta)
{
    switch (m_eUIType)
    {
    case Client::CInGameUI::UI_SHIFT:
        break;
    case Client::CInGameUI::UI_RBUTTON:
        break;
    case Client::CInGameUI::UI_LBUTTON:
        break;
    case Client::CInGameUI::UI_SPACE:
        break;
    case Client::CInGameUI::UI_V:
        break;
    case Client::CInGameUI::UI_BUILDMODE_F:
    {
        if(*m_pPlayer->Get_BuildMode() == true)
            m_bDraw = true;
        else
            m_bDraw = false;
        break;
    }
    case Client::CInGameUI::UI_C:
        break;
    case Client::CInGameUI::UI_DEAD:
        if (m_pPlayer->Get_knockdown() == true)
            m_bDraw = true;
        else
            m_bDraw = false;
        break;
    case Client::CInGameUI::UI_BATTERY:
        Battery_UI(fTimeDelta);
        break;
    case Client::CInGameUI::UI_BATTERY_GAGE:
        Battery_UI(fTimeDelta);
        break;
    case Client::CInGameUI::UI_MACHINE_HP:
        Machine_HP_UI(fTimeDelta);
        break;
    case Client::CInGameUI::UI_MACHINE_ENERGY:
        Machine_UI_Energy(fTimeDelta);
        break;
    case Client::CInGameUI::UI_BULLET:
        // 칼일 때 안그리기
        if (*m_pPlayer->Get_WeaponState() == CPlayer::WEAPON_KATANA)
            m_bDraw = false;
        else
            m_bDraw = true;
        break;
    case Client::CInGameUI::UI_BUILDMODE_CONVERSATIONBOX:
    {
        if (*m_pPlayer->Get_BuildMode() == true)
            m_bDraw = true;
        else
            m_bDraw = false;
        break;
    }
    case Client::CInGameUI::UI_CONVERSATIONBOX:
        UI_Conversation(fTimeDelta);
        break;
    case Client::CInGameUI::UI_CONVERSATIONBOX_BACKGROUND:
        UI_Conversation(fTimeDelta);
        break;
        
    case Client::CInGameUI::UI_CHARACTER:
        Charater_UI(fTimeDelta);
        break;
    case Client::CInGameUI::UI_PLAYER_HP:
        Player_UI_Hp(fTimeDelta);
        break;
    case Client::CInGameUI::UI_PLAYER_ENERGY:
        Player_UI_Energy(fTimeDelta);
        break;
    case Client::CInGameUI::UI_ENERGY_ICON:
        break;
    case Client::CInGameUI::UI_HP_ICON:
        break;
    case Client::CInGameUI::UI_CREDIT_ICON:
        break;
    case Client::CInGameUI::UI_RUN_ICON:
        break;
    case Client::CInGameUI::UI_JUMP_ICON:
        break;
    case Client::CInGameUI::UI_MODECHANGE_ICON:
    {
        if (*m_pPlayer->Get_BuildMode() == true)
            m_iIndex = 1;
        else
            m_iIndex = 0;
        break;
    }
    case Client::CInGameUI::UI_VIEWCHANGE_ICON:
        break;      
    case Client::CInGameUI::UI_PUNCH_ICON:
        break;
    case Client::CInGameUI::UI_CENTERICON:
        if (*m_pPlayer->Get_Reloading() == true)
        {
           // m_pTransformCom->Set_Scaling(26.f, 26.f, 26.f);
            m_bDraw = true;
            m_iIndex = 0;
        }
        else if (m_pPlayer->Get_Build_Gauging() == true)
        {
            m_bDraw = true;
            m_iIndex = 1;
        } // 특정 조건들 가져와서 인덱스 2번으로 
        else if (m_pCircle->Get_Interaction() == true)
        {
            m_bDraw = true;
            m_iIndex = 2;
        }
        else
            m_bDraw = false;
        
        m_iIndex;// 장전이냐 건축이냐에 따라서 모양이 바뀜
        break;
    case Client::CInGameUI::UI_DAMAGED:
        if (m_pPlayer->Get_CanAttacked() == false)
            m_bDraw = true;
        else
            m_bDraw = false;
        break;
    case Client::CInGameUI::UI_SLICE:
        // 칼일 때 안그리기
        if (*m_pPlayer->Get_WeaponState() == CPlayer::WEAPON_KATANA)
            m_bDraw = false;
        else
            m_bDraw = true;
        break;
    case Client::CInGameUI::UI_MISSILE_TIMER:
        if(*m_fTimer <= 0.f)
            m_bDraw = false;
        if(*m_iRound == 1)
        {
            m_bDraw = true;
        }
        else 
        {
            m_bDraw = false;
        }
        break;
    case Client::CInGameUI::UI_END:
        break;
    default:
        break;
    }
  
 
}

void CInGameUI::Late_Update(_float fTimeDelta)
{
    if(m_eUIType == UI_DAMAGED)
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI_LAST, this)))
            return;
    }
    else
    {
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_UI, this)))
            return;
    }

   
}

HRESULT CInGameUI::Render()
{

    if(m_bDraw== true)
    {
        if (FAILED(Bind_ShaderResources()))
            return E_FAIL;

        if ((UI_CONVERSATIONBOX == m_eUIType || UI_CONVERSATIONBOX_BACKGROUND == m_eUIType) && m_iIndex == 0)
        {
            // 점점 투명해짐
            if (FAILED(m_pShaderCom->Begin(3)))
                return E_FAIL;
        }
        else if (UI_BUILDMODE_CONVERSATIONBOX == m_eUIType)
        {
            if (FAILED(m_pShaderCom->Begin(6)))
                return E_FAIL;

        }
        else if (UI_BATTERY_GAGE == m_eUIType)
        {

            if (FAILED(m_pShaderCom->Begin(4)))
                return E_FAIL;
        }
        else if (UI_CREDIT_ICON == m_eUIType)
        {
            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;
        }
        else if (UI_MISSILE_TIMER == m_eUIType || UI_MACHINE_ENERGY == m_eUIType || UI_MACHINE_HP == m_eUIType || UI_PLAYER_ENERGY == m_eUIType || UI_PLAYER_HP == m_eUIType)
        {
            if (FAILED(m_pShaderCom->Begin(5)))
                return E_FAIL;
        }
        else
        {
            // 그냥 그림
            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;
        }

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
    }
     return S_OK;
}




HRESULT CInGameUI::Add_Components(_int iNum)
{
    switch (m_eUIType)
    {
    case Client::CInGameUI::UI_SHIFT:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Shift"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_RBUTTON:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_RButton"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_LBUTTON:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_LButton"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_SPACE:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Space"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_V:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_VIcon"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_F:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_FIcon"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_BUILDMODE_F:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_FIcon"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_C:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_CIcon"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_DEAD:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Death"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_BATTERY:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Battery"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_BATTERY_GAGE:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_UIBar"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_CHARACTER:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Character"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_CONVERSATIONBOX:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_UIBackGround"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_CONVERSATIONBOX_BACKGROUND:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_UIBackGround"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
        
    case Client::CInGameUI::UI_BUILDMODE_CONVERSATIONBOX:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_UIBackGround"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_MACHINE_HP:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_UIBar"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_MACHINE_ENERGY:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_UIBar"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_DAMAGED:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_UIDamaged"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_PLAYER_ENERGY:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_UIBar"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_PLAYER_HP:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_UIBar"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_ENERGY_ICON:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_EnergyIcon"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_HP_ICON:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_HpIcon"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_CREDIT_ICON:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_CreditIcon"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_RUN_ICON:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_RunIcon"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_JUMP_ICON:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_JumpIcon"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_MODECHANGE_ICON:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_ModeChangeIcon"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_PUNCH_ICON:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_PuchIcon"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_VIEWCHANGE_ICON:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_ViewChangeIcon"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_CENTERICON:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_CenterUI"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_BULLET:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_CenterUI"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_SLICE:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Slice"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_MISSILE_TIMER:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Missile_Timer"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
    case Client::CInGameUI::UI_NUCLEAR:
        if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_Nuclear"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
        break;
        
        
    case Client::CInGameUI::UI_END:
        break;
    default:
        break;
    }

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;
    return S_OK;
}

HRESULT CInGameUI::Bind_ShaderResources()
{

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (m_eUIType == UI_BATTERY)
    {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iBattery)))
            return E_FAIL;
    }
    else if (m_eUIType == UI_CHARACTER || m_eUIType == UI_BUILDMODE_CONVERSATIONBOX || m_eUIType == UI_CONVERSATIONBOX_BACKGROUND || m_eUIType == UI_CONVERSATIONBOX || m_eUIType == UI_MODECHANGE_ICON)
    {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iIndex)))
            return E_FAIL;
    }
    else if (m_eUIType == UI_BATTERY_GAGE)
    {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iIndex)))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fGageAmount", &m_fBatteryGauge, sizeof(float))))
            return E_FAIL;
    }
    else if (m_eUIType == UI_MACHINE_HP)
    {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iIndex)))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fGageAmount", m_fMachineHP, sizeof(float))))
            return E_FAIL;
    }
    else if (m_eUIType == UI_MACHINE_ENERGY)
    {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iIndex)))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fGageAmount", m_fMachineEnergy, sizeof(float))))
            return E_FAIL;
    }
    else if (m_eUIType == UI_PLAYER_HP)
    {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iIndex)))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fGageAmount", m_fPlayerHp, sizeof(float))))
            return E_FAIL;
    }
    else if (m_eUIType == UI_PLAYER_ENERGY)
    {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iIndex)))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fGageAmount", m_fPlayerEnergy, sizeof(float))))
            return E_FAIL;

    }
    else if (m_eUIType == UI_CENTERICON)
    {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iIndex)))
            return E_FAIL;
    }
    else if (UI_MISSILE_TIMER == m_eUIType)
    {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;
        _float fMount = *m_fTimer;
        fMount *= 0.5f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fGageAmount", &(fMount), sizeof(float))))
            return E_FAIL;
    }
    else
    {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;
    }
    return S_OK;
}

CInGameUI* CInGameUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInGameUI* pInstance = new CInGameUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CInGameUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* CInGameUI::Clone(void* pArg)
{
    CInGameUI* pInstance = new CInGameUI(*this);
    
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CInGameUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CInGameUI::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}

void CInGameUI::Battery_UI(_float fTimeDelta)
{

    if (m_fBatteryGauge >= 70.f)
    {
        m_iBattery = 0;
    }
    else if (m_fBatteryGauge >= 50.f)
    {
        m_iBattery = 1;
    }
    else if (m_fBatteryGauge >= 30.f)
    {
        m_iBattery = 2;
    }
    else if (m_fBatteryGauge > 0.f)
    {
        m_iBattery = 3;
    }
    else
    {
        m_iBattery = 4;
    }
}

void CInGameUI::Machine_HP_UI(_float fTimeDelta)
{
    
   

}

void CInGameUI::Charater_UI(_float fTimeDelta)
{
#ifdef _DEBUG
    if (m_pGameInstance->Get_DIKeyState_Down(DIK_NUMPAD1))
    {
        m_iCharacter_Number++;
        if (m_iCharacter_Number == 12)
            m_iCharacter_Number = 0;
    }

#endif


}

void CInGameUI::UI_Conversation(_float fTimeDelta)
{
#ifdef _DEBUG



#endif
}



void CInGameUI::Machine_UI_Energy(_float fTimeDelta)
{
  
}

void CInGameUI::Player_UI_Hp(_float fTimeDelta)
{
    
}

void CInGameUI::Player_UI_Energy(_float fTimeDelta)
{

}
