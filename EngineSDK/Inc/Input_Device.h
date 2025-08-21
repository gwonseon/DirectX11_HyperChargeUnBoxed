#ifndef InputDev_h__
#define InputDev_h__


#include "Base.h"

BEGIN(Engine)

class CInput_Device: public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;

public:
	_ubyte Get_DIKeyState(_ubyte byKeyID);            // Pressing
	_ubyte Get_DIKeyState_Pressing(_ubyte byKeyID);   // Pressing
	_ubyte Get_DIKeyState_Up(_ubyte byKeyID);
	_ubyte Get_DIKeyState_Down(_ubyte byKeyID);

	_ubyte Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_ubyte Get_DIMouseState_Down(MOUSEKEYSTATE eMouse);
	_ubyte Get_DIMouseState_Pressing(MOUSEKEYSTATE eMouse);
	_ubyte Get_DIMouseState_Up(MOUSEKEYSTATE eMouse);
	_long Get_DIMouseMove(MOUSEMOVESTATE eMouseState);


public:
	HRESULT Initialize(HINSTANCE hInst,HWND hWnd);
	void	Update_InputDev(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = {nullptr};
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = {nullptr};
	LPDIRECTINPUTDEVICE8	m_pMouse = {nullptr};

private:
	_byte					m_byKeyState[256] = {};		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	_byte                    m_byPrevKeyState[256];
	DIMOUSESTATE			m_tMouseState = {};
	DIMOUSESTATE             m_tPrevMouseState;
public:
	static CInput_Device* Create(HINSTANCE hInst,HWND hWnd);
	virtual void	Free(void);

};
END
#endif // InputDev_h__