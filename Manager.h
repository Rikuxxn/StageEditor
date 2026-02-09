//=============================================================================
//
// マネージャー処理 [Manager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MANAGER_H_// このマクロ定義がされていなかったら
#define _MANAGER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Renderer.h"
#include "Input.h"
#include "Texture.h"
#include "Camera.h"
#include "Light.h"
#include "Scene.h"
#include "Fade.h"
#include "PhysicsWorld.h"

//*****************************************************************************
// マネージャークラス
//*****************************************************************************
class CManager
{
public:
	CManager();
	~CManager();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static void OnDeviceReset(void);
	static void ReleaseThumbnail(void);
	static void ResetThumbnail(void);

	static CRenderer* GetRenderer(void) { return m_pRenderer; }
	static CInputKeyboard* GetInputKeyboard(void) { return m_pInputKeyboard; }
	static CInputJoypad* GetInputJoypad(void) { return m_pInputJoypad; }
	static CInputMouse* GetInputMouse(void) { return m_pInputMouse; }
	static CTexture* GetTexture(void) { return m_pTexture; }
	static CCamera* GetCamera(void) { return m_pCamera; }
	static CLight* GetLight(void) { return m_pLight; }
	static CFade* GetFade(void) { return m_pFade; }
	int GetFPS(int fps) { return m_fps = fps; };
	int GetFPSCnt(void) { return m_fps; }
	static PhysicsWorld* GetPhysicsWorld(void) { return m_pPhysicsWorld.get(); }
	static void SetMode(CScene::MODE mode);
	static CScene::MODE GetMode(void);

private:
	static CRenderer*						m_pRenderer;		// レンダラーへのポインタ
	static CInputKeyboard*					m_pInputKeyboard;	// キーボードへのポインタ
	static CInputJoypad*					m_pInputJoypad;		// ジョイパッドへのポインタ
	static CInputMouse*						m_pInputMouse;		// マウスへのポインタ
	static CTexture*						m_pTexture;			// テクスチャへのポインタ
	static CCamera*							m_pCamera;			// カメラへのポインタ
	static CLight*							m_pLight;			// ライトへのポインタ
	static std::unique_ptr<PhysicsWorld>	m_pPhysicsWorld;	// 物理世界へのポインタ
	static CFade*							m_pFade;			// フェードへのポインタ
	static CScene*							m_pScene;			// シーンへのポインタ
	int										m_fps;				// FPS値
};

#endif