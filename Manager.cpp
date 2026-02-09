//=============================================================================
//
// マネージャー処理 [Manager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Manager.h"
#include "Renderer.h"
#include "Edit.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
CRenderer* CManager::m_pRenderer = nullptr;
CInputKeyboard* CManager::m_pInputKeyboard = nullptr;
CInputJoypad* CManager::m_pInputJoypad = nullptr;
CInputMouse* CManager::m_pInputMouse = nullptr;
CTexture* CManager::m_pTexture = nullptr;
CCamera* CManager::m_pCamera = nullptr;
CLight* CManager::m_pLight = nullptr;
CScene* CManager::m_pScene = nullptr;
CFade* CManager::m_pFade = nullptr;
std::unique_ptr<PhysicsWorld> CManager::m_pPhysicsWorld = nullptr;

//=============================================================================
// コンストラクタ
//=============================================================================
CManager::CManager()
{
	// 値のクリア
	m_fps = 0;
}
//=============================================================================
// デストラクタ
//=============================================================================
CManager::~CManager()
{

}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd)
{
	// レンダラーの生成
	m_pRenderer = new CRenderer;

	// レンダラーの初期化処理
	if (FAILED(m_pRenderer->Init(hWnd, TRUE)))
	{
		return -1;
	}

	// キーボードの生成
	m_pInputKeyboard = new CInputKeyboard;

	// キーボードの初期化処理
	if (FAILED(m_pInputKeyboard->Init(hInstance, hWnd)))
	{
		return -1;
	}

	// ジョイパッドの生成
	m_pInputJoypad = new CInputJoypad;

	// ジョイパッドの初期化処理
	if (FAILED(m_pInputJoypad->Init()))
	{
		return E_FAIL;
	}

	// マウスの生成
	m_pInputMouse = new CInputMouse;

	// マウスの初期化処理
	if (FAILED(m_pInputMouse->Init(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	// 物理ワールドの生成
	m_pPhysicsWorld = std::make_unique <PhysicsWorld>();

	// 重力の設定
	m_pPhysicsWorld->SetGravity(D3DXVECTOR3(0.0f, -320.0f, 0.0f));

	// カメラの生成
	m_pCamera = new CCamera;

	// カメラの初期化処理
	m_pCamera->Init();

	// ライトの生成
	m_pLight = new CLight;

	// ライトの初期化処理
	m_pLight->Init();

	// テクスチャの生成
	m_pTexture = new CTexture;

	// テクスチャの読み込み
	m_pTexture->Load();

	// エディター画面
	m_pFade = CFade::Create(CScene::MODE_EDIT);

	// エディター画面
	m_pScene = CScene::Create(CScene::MODE_EDIT);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CManager::Uninit(void)
{
	// すべてのオブジェクトの破棄
	CObject::ReleaseAll();

	// テクスチャの破棄
	if (m_pTexture != nullptr)
	{
		// 全てのテクスチャの破棄
		m_pTexture->Unload();

		delete m_pTexture;
		m_pTexture = nullptr;
	}

	// キーボードの終了処理
	m_pInputKeyboard->Uninit();

	// ジョイパッドの終了処理
	m_pInputJoypad->Uninit();

	// マウスの終了処理
	m_pInputMouse->Uninit();

	// キーボードの破棄
	if (m_pInputKeyboard != nullptr)
	{
		// レンダラーの終了処理
		m_pInputKeyboard->Uninit();

		delete m_pInputKeyboard;
		m_pInputKeyboard = nullptr;
	}

	// ジョイパッドの破棄
	if (m_pInputJoypad != nullptr)
	{
		// ジョイパッドの終了処理
		m_pInputJoypad->Uninit();

		delete m_pInputJoypad;
		m_pInputJoypad = nullptr;
	}

	// マウスの破棄
	if (m_pInputMouse != nullptr)
	{
		// マウスの終了処理
		m_pInputMouse->Uninit();

		delete m_pInputMouse;
		m_pInputMouse = nullptr;
	}

	// カメラの破棄
	if (m_pCamera != nullptr)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
	}

	// ライトの破棄
	if (m_pLight != nullptr)
	{
		delete m_pLight;
		m_pLight = nullptr;
	}

	// フェードの破棄
	if (m_pFade != nullptr)
	{
		// フェードの終了処理
		m_pFade->Uninit();

		delete m_pFade;
		m_pFade = nullptr;
	}

	// レンダラーの破棄
	if (m_pRenderer != nullptr)
	{
		// レンダラーの終了処理
		m_pRenderer->Uninit();

		delete m_pRenderer;
		m_pRenderer = nullptr;
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void CManager::Update(void)
{
	// キーボードの更新
	m_pInputKeyboard->Update();

	// ジョイパッドの更新
	m_pInputJoypad->Update();

	// マウスの更新
	m_pInputMouse->Update();

	// フェードの更新処理
	if (m_pFade != nullptr)
	{
		// フェードの更新処理
		m_pFade->Update();
	}

	float dt = 1.0f / 60.0f;

	// 物理シミュレーション
	m_pPhysicsWorld->StepSimulation(dt);// 引き数にFPS,1フレームに付き何回衝突するかデフォ1,デルタタイム

	// カメラの更新
	m_pCamera->Update();

	// ライトの更新
	m_pLight->Update();

	// レンダラーの更新
	m_pRenderer->Update();
}
//=============================================================================
// 描画処理
//=============================================================================
void CManager::Draw(void)
{
	// レンダラーの描画
	m_pRenderer->Draw(m_fps);
}
//=============================================================================
// モードの設定
//=============================================================================
void CManager::SetMode(CScene::MODE mode)
{
	// カメラの初期化処理
	m_pCamera->Init();

	if (m_pScene != nullptr)
	{
		// 現在のモード破棄
		m_pScene->Uninit();
	}

	// 全てのオブジェクトを破棄
	CObject::ReleaseAll();

	// 新しいモードの生成
	m_pScene = CScene::Create(mode);
}
//=============================================================================
// 現在のモードの取得
//=============================================================================
CScene::MODE CManager::GetMode(void)
{
	return m_pScene->GetMode();
}
//=============================================================================
// デバイスリセット通知
//=============================================================================
void CManager::OnDeviceReset(void)
{
	if (m_pScene)
	{
		m_pScene->OnDeviceReset();
	}
}
//=============================================================================
// サムネイルリリース通知
//=============================================================================
void CManager::ReleaseThumbnail(void)
{
	if (m_pScene)
	{
		m_pScene->ReleaseThumbnail();
	}
}
//=============================================================================
// サムネイルリセット通知
//=============================================================================
void CManager::ResetThumbnail(void)
{
	if (m_pScene)
	{
		m_pScene->ResetThumbnail();
	}
}