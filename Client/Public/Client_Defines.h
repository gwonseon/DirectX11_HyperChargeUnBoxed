#pragma once

#include <process.h>

#define BUILD_EA	159
#define ENVIRONMENT_EA 165
#define WEAPON_EA		13
#define MONSTER_EA		6
#define OBJ_DEAD 1
#define TRAP_EA 17
#define EFFECT_EA 10
#define	BULLET_EA 6
#define MISSILEROUND 3

namespace Client
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_YARD, LEVEL_IMGUI, LEVEL_NAVIGATION, LEVEL_MONSTERSPAWN,  LEVEL_END };
	enum UIID	 { UI_MACHINE_HP, UI_ARMCANNON, UI_END};
	enum ANIMMODEL_INDEX { ANIM_HELICOPTER, ANIM_EVILDAMAGE, ANIM_TANK, ANIM_BLIMP , ANIM_MEATBAG , ANIM_MEATBAGDEMOLISHER, ANIM_PLAYERFPS, ANIM_PLAYERTPS, ANIM_ALIEN, ANIM_CHOPPER, ANIM_PONY, ANIM_RIFLEMAN, ANIM_TERRAINCRUSHED, ANIM_END};
	enum PLAYERVIEWSTATE	{ PLAYER_TPS_VIEW, PLAYER_FPS_VIEW, PLAYER_END_VIEW	};
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace std;
using namespace Client;

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <ImGuiFileDialog.h>


#pragma comment(lib, "ImGui.lib")

