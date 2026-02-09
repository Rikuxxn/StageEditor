//=============================================================================
//
// 入力処理 [Input.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _INPUT_H_// このマクロ定義がされていなかったら
#define _INPUT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NUM_KEY_MAX (256)	// キーの最大数


//*****************************************************************************
// 入力クラス
//*****************************************************************************
class CInput
{
public:
	CInput();
	virtual ~CInput();

	virtual HRESULT Init(HINSTANCE hInstance);
	virtual void Uninit(void);
	virtual void Update(void) = 0;
	LPDIRECTINPUTDEVICE8 GetDevice(void) { return m_pDevice; }

protected:
	static LPDIRECTINPUT8 m_pInput;		// DirectInputオブジェクトへのポインタ
	LPDIRECTINPUTDEVICE8 m_pDevice;		// 入力デバイスへのポインタ

private:
};

//*****************************************************************************
// キーボードクラス
//*****************************************************************************
class CInputKeyboard : public CInput
{
public:
	CInputKeyboard();
	~CInputKeyboard();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey);
	bool GetTrigger(int nKey);
	bool GetAnyKeyTrigger(void);
	bool GetRelease(int nKey);
	bool GetRepeat(int nKey);
private:
	BYTE m_aKeyState[NUM_KEY_MAX];				// キーボードのプレス情報
	BYTE m_aOldState[NUM_KEY_MAX];				// キーボードの前回のプレス情報
};

//*****************************************************************************
// ジョイパッドクラス
//*****************************************************************************
class CInputJoypad
{
public:
	CInputJoypad();
	~CInputJoypad();

	static constexpr float	DEADZONE = 10922.0f;	// デッドゾーン

	//キーの種類
	typedef enum
	{
		JOYKEY_UP = 0,	// 十字キー上
		JOYKEY_DOWN,	// 十字キー下
		JOYKEY_LEFT,	// 十字キー左
		JOYKEY_RIGHT,	// 十字キー右
		JOYKEY_START,	// スタートボタン
		JOYKEY_BACK,	// バックボタン
		JOYKEY_LS,		// 左スティック押し込み
		JOYKEY_RS,		// 右スティック押し込み
		JOYKEY_LB,		// 左ボタン
		JOYKEY_RB,		// 右ボタン
		JOYKEY_LT,		// 左トリガー
		JOYKEY_RT,		// 右トリガー
		JOYKEY_A,		// Aボタン
		JOYKEY_B,		// Bボタン
		JOYKEY_X,		// Xボタン
		JOYKEY_Y,		// Yボタン
		JOYKEY_MAX
	}JOYKEY;

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	bool GetTrigger(JOYKEY Key);
	bool GetPress(JOYKEY Key);
	bool GetTriggerL2(void);
	bool GetTriggerR2(void);
	bool GetPressL2(void);
	bool GetPressR2(void);
	bool GetAnyPress(void);
	bool GetAnyTrigger(void);
	bool GetStick(void);
	static XINPUT_STATE* GetStickAngle(void);
private:
	static XINPUT_STATE m_joyKeyState;			// ジョイパッドのプレス情報
	XINPUT_STATE m_joyKeyStateTrigger;			// ジョイパッドのトリガー情報
	XINPUT_STATE m_joyKeyStateRelease;			// ジョイパッドのリリース情報
	XINPUT_STATE m_aOldJoyKeyState;				// ジョイパッドの前回の情報
	bool m_joyKeyFlag[JOYKEY_MAX];
};

//*****************************************************************************
// マウスクラス
//*****************************************************************************
class CInputMouse : public CInput
{
public:
	CInputMouse();
	~CInputMouse();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int button);
	bool GetTrigger(int button);
	bool GetRelease(int button);
	bool GetMouseState(DIMOUSESTATE* mouseState);
	static int GetWheel(void);
	static void SetCursorVisibility(bool visible);
	D3DXVECTOR3 GetGroundHitPosition(void); // 地面Y=0との交差点を取得

private:
	static DIMOUSESTATE m_mouseState;			// マウスの状態
};

#endif