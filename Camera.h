//=============================================================================
//
// カメラ処理 [Camera.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _CAMERA_H_// このマクロ定義がされていなかったら
#define _CAMERA_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// カメラクラス
//*****************************************************************************
class CCamera
{
public:
	CCamera();
	~CCamera();

	// カメラの種類
	typedef enum
	{
		MODE_EDIT = 0,	// エディターカメラ
		MODE_MAX
	}MODE;

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void SetCamera(void);
	void EditCamera(void);

	//*****************************************************************************
	// setter関数
	//*****************************************************************************
	void SetPosV(D3DXVECTOR3 posV) { m_posV = posV; }
	void SetPosR(D3DXVECTOR3 posR) { m_posR = posR; }
	void SetRot(D3DXVECTOR3 rot) { m_rot = rot; }
	void SetDis(float fDistance) { m_fDistance = fDistance; }

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	D3DXVECTOR3 GetRot(void) { return m_rot; }			// カメラの角度の取得
	MODE GetMode(void) { return m_Mode; }
	D3DXVECTOR3 GetPosV(void) { return m_posV; }
	D3DXVECTOR3 GetPosR(void) { return m_posR; }
	D3DXMATRIX GetViewMatrix(void) const { return m_mtxView; }
	D3DXMATRIX GetProjMatrix(void) const { return m_mtxProjection; }

private:
	static constexpr float PI_HALF				= 1.57f;	// 円周率の半分
	static constexpr float DOUBLE				= 2.0f;		// 二倍
	static constexpr float MOUSE_SENSITIVITY	= 0.004f;	// マウス感度
	static constexpr float ZOOM_SPEED			= 15.0f;	// ズーム速度
	static constexpr float CAM_MIN_DISTANCE		= 100.0;	// カメラの最低距離
	static constexpr float CAM_MAX_DISTANCE		= 800.0;	// カメラの最高距離
	static constexpr float FOV					= 80.0f;	// 視野角
	static constexpr float RENDER_DISTANCE		= 2500.0f;	// 遠クリップ面

	D3DXMATRIX	m_mtxProjection;	// プロジェクションマトリックス
	D3DXMATRIX	m_mtxView;			// ビューマトリックス
	D3DXVECTOR3 m_posV;				// 視点
	D3DXVECTOR3 m_posVDest;			// 目的の視点
	D3DXVECTOR3 m_posR;				// 注視点
	D3DXVECTOR3 m_posRDest;			// 目的の注視点
	D3DXVECTOR3 m_vecU;				// 上方向ベクトル
	D3DXVECTOR3 m_rot;				// 向き
	MODE		m_Mode;				// カメラのモード
	float		m_fDistance;		// 視点から注視点の距離
};

#endif