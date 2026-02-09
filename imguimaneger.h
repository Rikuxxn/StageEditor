//=============================================================
//
// ImGuiの制御処理 [ImguiManager.h]
// Author : TANEKAWA RIKU
//
//=============================================================
#ifndef _IMGUIMANEGER_H_// このマクロ定義がされていなかったら
#define _IMGUIMANEGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"


//*****************************************************************************
// ImGuiマネージャークラス
//*****************************************************************************
class CImGuiManager
{
public :
	CImGuiManager();
	~CImGuiManager();

	typedef enum
	{
		IMGUITYPE_DEFOULT = 0,
		IMGUITYPE_NOMOVE,
		IMGUITYPE_NOMOVESIZE,
		IMGUITYPE_NOMOVEANDSIZE,
		IMGUITYPE_MAX
	}IMGUITYPE;

	static CImGuiManager& Instance(void);
	void Init(HWND hWnd, LPDIRECT3DDEVICE9 pDevice);
	void Uninit();
	void SetPosImgui(ImVec2 Pos);
	void SetSizeImgui(ImVec2 Size);
	void StartImgui(const char* ImguiName, IMGUITYPE nType);
	void EndImgui();
	void EndImguiRender();
	void NewFlameImgui();

private:
	static std::unique_ptr<CImGuiManager> m_Instance;

};

#endif // !_IMGUIMANEGER_H_