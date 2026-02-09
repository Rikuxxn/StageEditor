//=============================================================================
//
// スカイキューブ処理 [SkyCube.h]
// Author: RIKU TANEKAWA
//
//=============================================================================
#ifndef _SKYCUBE_H_
#define _SKYCUBE_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Object.h"


//*****************************************************************************
// スカイキューブクラス
//*****************************************************************************
class CSkyCube : public CObject
{
public:
	CSkyCube(int nPriority = 2);
	~CSkyCube();

	static CSkyCube* Create(void);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Scroll(void);
	void Draw(void);

private:
	static constexpr float VERTEX	= 4;		// 頂点数
	static constexpr float ADD_TIME = 1.0f;		// 時間加算量

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファへのポインタ
	int						m_nIdxTexture;		// テクスチャのインデックス
	float					m_fTimer;			// タイマー
};

#endif

