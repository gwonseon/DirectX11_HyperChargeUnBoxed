#pragma once

#include "Base.h"

BEGIN(Engine)



class CSoundMgr final : public CBase
{
private:
	CSoundMgr(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSoundMgr() = default;

public:
	HRESULT Ready_Sound();
public:
	void PlaySoundW(const wstring pSoundKey, CHANNELID eID, float fVolume = 0);
	void PlayBGM(const wstring pSoundKey, float fVolume = 0);
	void StopSound(CHANNELID eID);
	void StopAll();

	void SetChannelVolume(CHANNELID eID, float fVolume);
	void VolumeFade(bool _bOnOff, float _fMinusVolume = 0.03f, float _fPlusVolume = 0.05f);
	void VolumeFade_boss();
	void  Set_BGMVolume(float fVolume) { m_fVolume = fVolume; }
	float Get_BGMVolume() { return m_fVolume; };

	wstring Get_NowBGM() { return nowBGM; };

private:
	void LoadSoundFile(const wstring soundFile);


private:
	// 사운드 리소스를 저장할 맵
 // 사운드 리소스 정보를 갖는 객체 
	map<wstring, FMOD::Sound*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD::Channel* m_pChannelArr[MAXCHANNEL]{};

	FMOD::ChannelGroup* channelGroup;
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD::System* m_pSystem = nullptr;

	FMOD_RESULT result{};
	float m_fVolume = 0.f;
	wstring nowBGM;




private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CSoundMgr* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END