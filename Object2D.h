//=============================================================================
//
// 2Dオブジェクト処理 [Object2D.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _OBJECT2D_H_// このマクロ定義がされていなかったら
#define _OBJECT2D_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Object.h"

//*****************************************************************************
// 2Dオブジェクトクラス
//*****************************************************************************
class CObject2D : public CObject
{
public:
	CObject2D(int nPriority = 5);
	~CObject2D();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static CObject2D* Create(D3DXVECTOR3 pos, D3DXCOLOR col, float fWidth, float fHeight);

	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRot(D3DXVECTOR3 rot) { m_rot = rot; }
	void SetCol(D3DXCOLOR color) { m_col = color; }
	void SetSize(float fWidth, float fHeight);
	void SetScrollSpeed(float fUSpeed) { m_fUSpeed = fUSpeed; }
	void SetUV(int nTexU, int nTexV);

	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXVECTOR3 GetMove(void) { return m_move; }
	D3DXVECTOR3 GetRot(void) { return m_rot; }
	D3DCOLOR GetCol(void) { return m_col; }
	float GetWidth(void) { return m_fWidth; }
	float GetHeight(void) { return m_fHeight; }

	void MoveTexUV(float fLeft, float fTop, float fWidth, float fHeight);
	void TextureAnim(int nTexPosX, int nTexPosY, int nAnimSpeed);
	void ScrollTexture(void);

private:
	static constexpr int VERTEX = 4;			// 頂点数

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファへのポインタ
	D3DXVECTOR3				m_pos;				// 位置
	D3DXVECTOR3				m_rot;				// 向き
	D3DXVECTOR3				m_move;				// 移動量
	D3DCOLOR				m_col;				// 色
	int						m_nCounterAnim;		// アニメーションカウンター
	int						m_nPatternAnim;		// アニメーションパターンNo.
	float					m_fTexU;			// テクスチャUV
	float					m_fTexV;			// テクスチャUV
	float					m_fWidth;			// 幅
	float					m_fHeight;			// 高さ
	float					m_fLength;			// 長さ
	float					m_fAngle;			// 角度
	float					m_fUSpeed;			// 横のスクロールスピード
	float					m_fVSpeed;			// 縦のスクロールスピード
	float					m_aPosTexU;			// 横のスクロール用変数
};

#endif