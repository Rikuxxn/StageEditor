//=============================================================================
//
// 入力処理 [Input.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Input.h"
#include "Manager.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
LPDIRECTINPUT8 CInput::m_pInput = NULL;		// DirectInputオブジェクトへのポインタ
XINPUT_STATE CInputJoypad::m_joyKeyState = {};
DIMOUSESTATE CInputMouse::m_mouseState = {};

//=============================================================================
// コンストラクタ
//=============================================================================
CInput::CInput()
{
	// 値のクリア
	m_pDevice = nullptr;
}
//=============================================================================
// デストラクタ
//=============================================================================
CInput::~CInput()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CInput::Init(HINSTANCE hInstance)
{
	// DirectInputオブジェクトの生成
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&m_pInput, NULL)))
	{
		return E_FAIL;
	}

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CInput::Uninit(void)
{
	// 入力デバイスの破棄
	if (m_pDevice != NULL)
	{
		m_pDevice->Unacquire();// キーボードへのアクセス権を破棄
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	// DirectInputオブジェクトの破棄
	if (m_pInput != NULL)
	{
		m_pInput->Release();
		m_pInput = NULL;
	}
}
//=============================================================================
// キーボードのコンストラクタ
//=============================================================================
CInputKeyboard::CInputKeyboard()
{
	// 値のクリア
	for (int nCnt = 0; nCnt < NUM_KEY_MAX; nCnt++)
	{
		m_aKeyState[nCnt] = {};				// キーボードのプレス情報
		m_aOldState[nCnt] = {};				// キーボードの前回のプレス情報
	}
}
//=============================================================================
// キーボードのデストラクタ
//=============================================================================
CInputKeyboard::~CInputKeyboard()
{
	// なし
}
//=============================================================================
// キーボードの初期化処理
//=============================================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	// 入力の初期化
	CInput::Init(hInstance);

	// 入力デバイス(キーボード)の生成
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	//データフォーマットを設定
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	//協調モードを設定
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd,
		(DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return S_OK;
	}

	// キーボードへのアクセス権
	m_pDevice->Acquire();

	return S_OK;
}
//=============================================================================
// キーボードの終了処理
//=============================================================================
void CInputKeyboard::Uninit(void)
{
	// 入力の終了処理
	CInput::Uninit();
}
//=============================================================================
// キーボードの更新処理
//=============================================================================
void CInputKeyboard::Update(void)
{
	BYTE aKeyState[NUM_KEY_MAX]; // 最新のキーボード状態
	memcpy(m_aOldState, m_aKeyState, sizeof(m_aKeyState)); // 旧状態を保存

	// キーボードの現在の状態を取得
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		memcpy(m_aKeyState, aKeyState, sizeof(m_aKeyState)); // 最新の状態を保存
	}
	else
	{
		// 入力取得失敗時、デバイスを再取得
		m_pDevice->Acquire();
		memset(m_aKeyState, 0, sizeof(m_aKeyState)); // すべてのキーを未押下状態にリセット
	}
}
//=============================================================================
// キーボードのプレス情報を取得
//=============================================================================
bool CInputKeyboard::GetPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? true : false;
}
//=============================================================================
// キーボードのトリガー情報を取得
//=============================================================================
bool CInputKeyboard::GetTrigger(int nKey)
{
	bool Trigger = false;
	if (m_aKeyState[nKey] & 0x80 && !(m_aOldState[nKey] & 0x80))
	{
		Trigger = true;
	}
	return Trigger;
}
//=============================================================================
// 何かしらのキーが新しく押されたかチェック
//=============================================================================
bool CInputKeyboard::GetAnyKeyTrigger(void)
{
	for (int nCnt = 0; nCnt < NUM_KEY_MAX; nCnt++)
	{
		if (nCnt == DIK_F11)
		{
			continue;
		}

		if ((m_aKeyState[nCnt] & 0x80) && !(m_aOldState[nCnt] & 0x80)) // 新しく押されたキーがある
		{
			return true;
		}
	}
	return false;
}
//=============================================================================
// キーボードのリリース情報を取得
//=============================================================================
bool CInputKeyboard::GetRelease(int nKey)
{
	bool Trigger = false;
	if (m_aOldState[nKey] & 0x80 && !(m_aKeyState[nKey] & 0x80))
	{
		Trigger = true;
	}
	return Trigger;
}
//=============================================================================
// キーボードのリピート情報を取得
//=============================================================================
bool CInputKeyboard::GetRepeat(int nKey)
{
	bool Trigger = false;
	if (m_aOldState[nKey] & 0x80 && !(m_aKeyState[nKey] & 0x80))
	{
		Trigger = true;
	}
	return Trigger;
}


//=============================================================================
// ジョイパッドのコンストラクタ
//=============================================================================
CInputJoypad::CInputJoypad()
{
	// 値のクリア
	m_joyKeyState = {};					// ジョイパッドのプレス情報
	m_joyKeyStateTrigger = {};			// ジョイパッドのトリガー情報
	m_joyKeyStateRelease = {};			// ジョイパッドのリリース情報
	m_aOldJoyKeyState = {};				// ジョイパッドの前回の情報

	for (int nCnt = 0; nCnt < JOYKEY_MAX; nCnt++)
	{
		m_joyKeyFlag[nCnt] = {};
	}
}
//=============================================================================
// ジョイパッドのデストラクタ
//=============================================================================
CInputJoypad::~CInputJoypad()
{
	// なし
}
//=============================================================================
// ジョイパッドの初期化処理
//=============================================================================
HRESULT CInputJoypad::Init(void)
{
	//メモリのクリア
	memset(&m_joyKeyState, 0, sizeof(XINPUT_STATE));

	//メモリのクリア
	memset(&m_joyKeyStateTrigger, 0, sizeof(XINPUT_STATE));

	//メモリのクリア
	memset(&m_aOldJoyKeyState, 0, sizeof(XINPUT_STATE));

	//XInputのステートを設定(有効にする)
	XInputEnable(true);

	return S_OK;
}
//=============================================================================
// ジョイパッドの終了処理
//=============================================================================
void CInputJoypad::Uninit(void)
{
	// XInputのステートを設定(無効にする)
	XInputEnable(false);
}
//=============================================================================
// ジョイパッドの更新処理
//=============================================================================
void CInputJoypad::Update(void)
{
	XINPUT_STATE joyKeyState;

	// 前フレームの状態を保存
	m_aOldJoyKeyState = m_joyKeyState;

	if (XInputGetState(0, &joyKeyState) == ERROR_SUCCESS)
	{
		WORD Button = joyKeyState.Gamepad.wButtons;
		WORD OldButton = m_joyKeyState.Gamepad.wButtons;

		// 新しく押されたボタンの判定
		m_joyKeyStateTrigger.Gamepad.wButtons = (Button & ~OldButton);
		m_joyKeyStateRelease.Gamepad.wButtons = (OldButton & ~Button);

		// L2・R2 のトリガー処理
		m_joyKeyStateTrigger.Gamepad.bLeftTrigger =
			(joyKeyState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) &&
			!(m_aOldJoyKeyState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

		m_joyKeyStateTrigger.Gamepad.bRightTrigger =
			(joyKeyState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) &&
			!(m_aOldJoyKeyState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

		m_joyKeyState = joyKeyState; // 最新の状態を保存
	}
	else
	{
		memset(&m_joyKeyState, 0, sizeof(XINPUT_STATE));
		memset(&m_joyKeyStateTrigger, 0, sizeof(XINPUT_STATE));
		memset(&m_joyKeyStateRelease, 0, sizeof(XINPUT_STATE));
	}
}
//=============================================================================
// ジョイパッドのトリガー処理
//=============================================================================
bool CInputJoypad::GetTrigger(JOYKEY Key)
{
	bool joykey = false;
	if (m_joyKeyState.Gamepad.wButtons & (0x01 << Key) && !(m_aOldJoyKeyState.Gamepad.wButtons & (0x01 << Key)))
	{
		joykey = true;
	}
	return joykey;
}
//=============================================================================
// ジョイパッドのプレス
//=============================================================================
bool CInputJoypad::GetPress(JOYKEY Key)
{
	return (m_joyKeyState.Gamepad.wButtons & (0x01 << Key)) ? true : false;
}
//=============================================================================
// L2トリガー（左トリガー）のトリガー判定
//=============================================================================
bool CInputJoypad::GetTriggerL2(void)
{
	return (m_joyKeyState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) &&
		!(m_aOldJoyKeyState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

//=============================================================================
// R2トリガー（右トリガー）のトリガー判定
//=============================================================================
bool CInputJoypad::GetTriggerR2(void)
{
	return (m_joyKeyState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) &&
		!(m_aOldJoyKeyState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}
//=============================================================================
// L2トリガー（左トリガー）のプレス判定
//=============================================================================
bool CInputJoypad::GetPressL2(void)
{
	return (m_joyKeyState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}
//=============================================================================
// R2トリガー（右トリガー）のプレス判定
//=============================================================================
bool CInputJoypad::GetPressR2(void)
{
	return (m_joyKeyState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}
//=============================================================================
// 何かしらのボタンが押されているか判定（プレス）
//=============================================================================
bool CInputJoypad::GetAnyPress(void)
{
	return m_joyKeyState.Gamepad.wButtons != 0;
}
//=============================================================================
// 何かしらのボタンが押されているか判定（トリガー）
//=============================================================================
bool CInputJoypad::GetAnyTrigger(void)
{
	if (m_joyKeyStateTrigger.Gamepad.wButtons != 0 ||
		m_joyKeyStateTrigger.Gamepad.bLeftTrigger ||
		m_joyKeyStateTrigger.Gamepad.bRightTrigger)
	{
		return true;
	}
	return false;
}
//=============================================================================
// スティック処理
//=============================================================================
bool CInputJoypad::GetStick(void)
{
	bool joykey = false;
	//真ん中じゃなかったら
	if (m_joyKeyState.Gamepad.sThumbLX >= 100 ||
		m_joyKeyState.Gamepad.sThumbLX <= -100 ||
		m_joyKeyState.Gamepad.sThumbLY >= 100 ||
		m_joyKeyState.Gamepad.sThumbLY <= -100)
	{
		joykey = true;
	}
	return joykey;
}
//=============================================================================
// スティック情報の取得処理
//=============================================================================
XINPUT_STATE* CInputJoypad::GetStickAngle(void)
{
	return &m_joyKeyState;
}


//=============================================================================
// マウスのコンストラクタ
//=============================================================================
CInputMouse::CInputMouse()
{
	// 値のクリア
	m_mouseState = {};
}
//=============================================================================
// マウスのデストラクタ
//=============================================================================
CInputMouse::~CInputMouse()
{
	// なし
}
//=============================================================================
// マウスの初期化処理
//=============================================================================
HRESULT CInputMouse::Init(HINSTANCE hInstance, HWND hWnd)
{
	// 入力の初期化処理
	CInput::Init(hInstance);

	// 入力デバイス(キーボード)の生成
	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	//データフォーマットを設定
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIMouse)))
	{
		return E_FAIL;
	}

	// ウィンドウが非アクティブでもマウスを使用可能にする
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd,
		DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		MessageBox(NULL, "SetCooperativeLevel に失敗しました", "エラー", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// マウスへのアクセス権
	m_pDevice->Acquire();

	return S_OK; // 初期化成功
}
//=============================================================================
// マウスの終了処理
//=============================================================================
void CInputMouse::Uninit(void)
{
	// 入力の終了処理
	CInput::Uninit();
}
//=============================================================================
// マウスの更新処理
//=============================================================================
void CInputMouse::Update(void)
{
	if (m_pDevice)
	{
		HRESULT hr = m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseState);
		if (FAILED(hr))
		{
			if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			{
				m_pDevice->Acquire();
			}
		}
	}
}
//=============================================================================
// マウスのプレス情報
//=============================================================================
bool CInputMouse::GetPress(int button)
{
	return (m_mouseState.rgbButtons[button] & 0x80) != 0;
}
//=============================================================================
// マウスのトリガー情報
//=============================================================================
bool CInputMouse::GetTrigger(int button)
{
	static DIMOUSESTATE oldState = {};

	bool trigger = (m_mouseState.rgbButtons[button] & 0x80) &&
		!(oldState.rgbButtons[button] & 0x80);

	oldState = m_mouseState;
	return trigger;
}
//=============================================================================
// マウスのリリース情報
//=============================================================================
bool CInputMouse::GetRelease(int button)
{
	static DIMOUSESTATE oldState = {};

	bool release = !(m_mouseState.rgbButtons[button] & 0x80) &&
		(oldState.rgbButtons[button] & 0x80);

	oldState = m_mouseState;  // Update previous state
	return release;
}
//=============================================================================
// マウスの状態
//=============================================================================
bool CInputMouse::GetMouseState(DIMOUSESTATE* mouseState)
{
	// マウスデバイスを取得
	LPDIRECTINPUTDEVICE8 pMouse = GetDevice();

	if (pMouse == NULL)
	{
		return false;
	}

	// マウスの状態を取得(長いから代入した)
	HRESULT hr = pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)mouseState);

	if (FAILED(hr))
	{
		// 入力デバイスがリセットされている場合、再取得を試みる
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			pMouse->Acquire();

			// 再取得を試みる
			hr = pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)mouseState);
			if (FAILED(hr))
			{
				return false;
			}
		}
		else
		{
			//エラーの場合
			return false;
		}
	}

	return true; // 正常に取得できた場合
}
//=============================================================================
// マウスホイールのスクロール量を取得する関数
//=============================================================================
int CInputMouse::GetWheel(void)
{
	const int ScrollUnit = 120; // 通常のスクロール単位

	int wheelValue = m_mouseState.lZ;

	// スクロール単位に基づいて正規化
	if (wheelValue != 0)
	{
		wheelValue /= ScrollUnit; // 正規化されたスクロール量を返す
	}

	return wheelValue; // 正（上スクロール）、負（下スクロール）、0（スクロールなし）
}
//=============================================================================
// マウスカーソル表示処理
//=============================================================================
void CInputMouse::SetCursorVisibility(bool visible)
{
	static int cursorCount = 0;

	if (visible)
	{
		// カーソルを表示
		while (cursorCount < 0)
		{
			ShowCursor(TRUE);
			cursorCount++;
		}
	}
	else
	{
		// カーソルを非表示
		while (cursorCount >= 0)
		{
			ShowCursor(FALSE);
			cursorCount--;
		}
	}
}
//=============================================================================
// 地面(Y = 0)との交差点を取得する処理
//=============================================================================
D3DXVECTOR3 CInputMouse::GetGroundHitPosition(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	if (!pDevice)
	{
		return D3DXVECTOR3(0, 0, 0);
	}

	// ビューポートと行列取得
	D3DVIEWPORT9 viewport;
	D3DXMATRIX matProj, matView, matWorld;
	pDevice->GetViewport(&viewport);
	pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matWorld);

	// マウス座標取得
	POINT mousePos;
	GetCursorPos(&mousePos);

	// ウィンドウハンドルを取得
	HWND hwnd = GetActiveWindow();
	ScreenToClient(hwnd, &mousePos);

	// スクリーン座標→ワールド座標変換
	D3DXVECTOR3 nearPoint((float)mousePos.x, (float)mousePos.y, 0.0f);
	D3DXVECTOR3 farPoint((float)mousePos.x, (float)mousePos.y, 1.0f);
	D3DXVECTOR3 rayOrigin, rayDir;

	D3DXVec3Unproject(&rayOrigin, &nearPoint, &viewport, &matProj, &matView, &matWorld);
	D3DXVec3Unproject(&rayDir, &farPoint, &viewport, &matProj, &matView, &matWorld);
	rayDir -= rayOrigin;
	D3DXVec3Normalize(&rayDir, &rayDir);

	// 地面(Y = 0)との交点
	if (fabsf(rayDir.y) < 1e-5f)
	{
		return D3DXVECTOR3(0, 0, 0); // 水平レイ対策
	}

	float t = -rayOrigin.y / rayDir.y;

	return rayOrigin + rayDir * t;
}