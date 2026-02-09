//=============================================================
//
//　Imguimaneger[Imguimaneger.cpp]
//　Author : TANEKAWA RIKU
//
//=============================================================

//*************************************************************
//インクルードファイル
//*************************************************************
#include "imguimaneger.h"
#include "imgui_internal.h"

// 名前空間の使用
using namespace std;

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
std::unique_ptr<CImGuiManager> CImGuiManager::m_Instance = nullptr;

//=============================================================
// コンストラクタ
//=============================================================
CImGuiManager::CImGuiManager()
{
	// 値のクリア

}
//=============================================================
// デストラクタ
//=============================================================
CImGuiManager::~CImGuiManager()
{
	Uninit();
}
//=============================================================
// ImGuiのインスタンス生成処理
//=============================================================
CImGuiManager& CImGuiManager::Instance(void)
{
	if (!m_Instance) // まだ作られていなければ作る
	{
		m_Instance = std::make_unique<CImGuiManager>();
	}
	return *m_Instance;
}
//=============================================================
// ImGuiの初期化処理
//=============================================================
void CImGuiManager::Init(HWND hWnd, LPDIRECT3DDEVICE9 pDevice)
{

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight(); // 他のスタイルも選べます

	// フォント設定（デフォルトのフォントとして Consolas を使用）
	ImFont* myFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/meiryo.ttc", 23.0f,NULL,io.Fonts->GetGlyphRangesJapanese());

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(pDevice);
}

//=============================================================
// ImGuiの終了処理
//=============================================================
void CImGuiManager::Uninit()
{
	// Cleanup
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
//=============================================================
// ImGuiのウィンドウの位置を設定
//=============================================================
void CImGuiManager::SetPosImgui(ImVec2 Pos)
{
	ImGui::SetNextWindowPos(Pos, ImGuiCond_Once);
}
//=============================================================
// ImGuiのウィンドウの大きさを設定
//=============================================================
void CImGuiManager::SetSizeImgui(ImVec2 Size)
{
	ImGui::SetNextWindowSize(Size, ImGuiCond_Once);
}
//=============================================================
// ImGuiのウィンドウの描画開始
//=============================================================
void CImGuiManager::StartImgui(const char* ImguiName, IMGUITYPE nType)
{
	if (ImguiName != NULL)
	{
		switch (nType)
		{
		case IMGUITYPE_DEFOULT:
			ImGui::Begin(ImguiName);
			break;
		case IMGUITYPE_NOMOVE:
			ImGui::Begin(ImguiName, NULL, ImGuiWindowFlags_NoMove);
			break;
		case IMGUITYPE_NOMOVESIZE:
			ImGui::Begin(ImguiName, NULL, ImGuiWindowFlags_NoResize);
			break;
		case IMGUITYPE_NOMOVEANDSIZE:
			ImGui::Begin(ImguiName, NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			break;
		default:
			break;
		}
	}
}
//=============================================================
// ImGuiのウィンドウの描画を終了
//=============================================================
void CImGuiManager::EndImgui()
{
	ImGui::End();
}
//=============================================================
// ImGuiのウィンドウのフレームを設定
//=============================================================
void CImGuiManager::NewFlameImgui()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
//=============================================================
// ImGuiのウィンドウのレンダーを設定
//=============================================================
void CImGuiManager::EndImguiRender()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
