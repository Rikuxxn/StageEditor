//=============================================================================
//
// カメラ処理 [Camera.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Camera.h"
#include "Renderer.h"
#include "Manager.h"
#include "Game.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CCamera::CCamera()
{
	// 値のクリア
	m_posV					= INIT_VEC3;// 視点
	m_posVDest				= INIT_VEC3;// 目的の視点
	m_posR					= INIT_VEC3;// 注視点
	m_posRDest				= INIT_VEC3;// 目的の注視点
	m_vecU					= INIT_VEC3;// 上方向ベクトル
	m_mtxProjection			= {};		// プロジェクションマトリックス
	m_mtxView				= {};		// ビューマトリックス
	m_rot					= INIT_VEC3;// 向き
	m_Mode = MODE_EDIT;					// カメラのモード
}
//=============================================================================
// デストラクタ
//=============================================================================
CCamera::~CCamera()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CCamera::Init(void)
{	
	m_posV = D3DXVECTOR3(0.0f, 80.0f, -240.0f);
	m_posR = D3DXVECTOR3(0.0f, 80.0f, 0.0f);
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);// 固定でいい
	m_rot = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);
	
	m_Mode = MODE_EDIT;									// カメラのモード

	m_fDistance = sqrtf(
		((m_posV.x - m_posR.x) * (m_posV.x - m_posR.x)) +
		((m_posV.y - m_posR.y) * (m_posV.y - m_posR.y)) +
		((m_posV.z - m_posR.z) * (m_posV.z - m_posR.z)));

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CCamera::Uninit(void)
{

}
//=============================================================================
// 更新処理
//=============================================================================
void CCamera::Update(void)
{
	switch (m_Mode)
	{
	case MODE_EDIT:

		// エディターカメラの処理
		EditCamera();
		break;
	}
}
//=============================================================================
// カメラの設定処理
//=============================================================================
void CCamera::SetCamera(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&m_mtxView,
		&m_posV,
		&m_posR,
		&m_vecU);

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
		D3DXToRadian(FOV),							// 視野角
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, // アスペクト比
		1.0f,										// 近クリップ面
		RENDER_DISTANCE);							// 遠クリップ面

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);
}
//=============================================================================
// エディターカメラの処理
//=============================================================================
void CCamera::EditCamera(void)
{
	// キーボードの取得
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	// マウスの取得
	CInputMouse* pInputMouse = CManager::GetInputMouse();

	// マウスカーソルを表示する
	pInputMouse->SetCursorVisibility(true);

	// 現在のカーソル位置を取得
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	// 前フレームからのマウス移動量を取得
	static POINT prevCursorPos = { cursorPos.x, cursorPos.y };
	float deltaX = (float)(cursorPos.x - prevCursorPos.x);
	float deltaY = (float)(cursorPos.y - prevCursorPos.y);

	// 現在のカーソル位置を保存（次のフレームでの比較用）
	prevCursorPos = cursorPos;

	// マウス感度

	deltaX *= MOUSE_SENSITIVITY;
	deltaY *= MOUSE_SENSITIVITY;

	//====================================
	// マウスホイールでズームイン・アウト
	//====================================
	int wheel = pInputMouse->GetWheel();

	if (wheel != 0)
	{
		m_fDistance -= wheel * ZOOM_SPEED;

		// カメラ距離制限
		if (m_fDistance < CAM_MIN_DISTANCE)
		{
			m_fDistance = CAM_MIN_DISTANCE;
		}
		if (m_fDistance > CAM_MAX_DISTANCE)
		{
			m_fDistance = CAM_MAX_DISTANCE;
		}
	}

	if (pInputKeyboard->GetPress(DIK_LALT) && pInputMouse->GetPress(0)) // 左クリック押しながらマウス移動 → 視点回転
	{
		m_rot.y += deltaX; // 水平回転
		m_rot.x += deltaY; // 垂直回転

		//角度の正規化
		if (m_rot.y > D3DX_PI)
		{
			m_rot.y -= D3DX_PI * DOUBLE;
		}
		else if (m_rot.y < -D3DX_PI)
		{
			m_rot.y += D3DX_PI * DOUBLE;
		}

		// 垂直回転の制限
		if (m_rot.x > PI_HALF)
		{
			m_rot.x = PI_HALF;
		}

		if (m_rot.x < -PI_HALF)
		{
			m_rot.x = -PI_HALF;
		}

		// 視点の更新（カメラの方向を適用）
		m_posV.x = m_posR.x + sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posV.y = m_posR.y + sinf(m_rot.x) * m_fDistance;
		m_posV.z = m_posR.z + cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;

	}
	else if (pInputMouse->GetPress(1)) // 右クリック押しながらマウス移動 → 注視点回転
	{
		m_rot.y += deltaX; // 水平回転
		m_rot.x += deltaY; // 垂直回転

		//角度の正規化
		if (m_rot.y > D3DX_PI)
		{
			m_rot.y -= D3DX_PI * DOUBLE;
		}
		else if (m_rot.y < -D3DX_PI)
		{
			m_rot.y += D3DX_PI * DOUBLE;
		}

		// 垂直回転の制限
		if (m_rot.x > PI_HALF)
		{
			m_rot.x = PI_HALF;
		}
		if (m_rot.x < -PI_HALF)
		{
			m_rot.x = -PI_HALF;
		}

		// 注視点の更新
		m_posR.x = m_posV.x - sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posR.y = m_posV.y - sinf(m_rot.x) * m_fDistance;
		m_posR.z = m_posV.z - cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	}
	else
	{
		// 入力がない場合でもズーム反映のために視点を更新
		m_posV.x = m_posR.x + sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
		m_posV.y = m_posR.y + sinf(m_rot.x) * m_fDistance;
		m_posV.z = m_posR.z + cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	}

	// 注視点の更新
	m_posR.x = m_posV.x - sinf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
	m_posR.y = m_posV.y - sinf(m_rot.x) * m_fDistance;
	m_posR.z = m_posV.z - cosf(m_rot.y) * cosf(m_rot.x) * m_fDistance;
}
