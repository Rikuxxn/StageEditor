//=============================================================================
//
// モデル処理 [Model.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MODEL_H_// このマクロ定義がされていなかったら
#define _MODEL_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Object.h"

//*****************************************************************************
// モデルクラス
//*****************************************************************************
class CModel
{
public:
	CModel();
	~CModel();

	static CModel* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawNormal(LPDIRECT3DDEVICE9 pDevice);

	//*****************************************************************************
	// setter関数
	//*****************************************************************************
	void SetOutlineShaderConstants(LPDIRECT3DDEVICE9 pDevice);
	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRot(D3DXVECTOR3 rot) { m_rot = D3DXToRadian(rot); }
	void SetParent(CModel* pModel) { m_pParent = pModel; }
	void SetOffsetPos(const D3DXVECTOR3& pos) { m_OffsetPos = pos; }
	void SetOffsetRot(const D3DXVECTOR3& rot) { m_OffsetRot = rot; }

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; }
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXVECTOR3 GetRot(void) { return m_rot; }
	D3DXVECTOR3 GetOffsetPos(void) const { return m_OffsetPos; }
	D3DXVECTOR3 GetOffsetRot(void) const { return m_OffsetRot; }
	const char* GetPath(void) { return m_Path; }
	CModel* GetParent(void) { return m_pParent; }

private:
	static constexpr float OUTLINE_THICKNESS = 0.2f;// アウトラインの太さ

	int*					m_nIdxTexture;
	D3DXVECTOR3				m_pos;					// 位置
	D3DXVECTOR3				m_rot;					// 向き
	D3DXVECTOR3				m_move;					// 移動量
	D3DXVECTOR3				m_OffsetPos;			// オフセット
	D3DXVECTOR3				m_OffsetRot;			// オフセット
	LPD3DXMESH				m_pMesh;				// メッシュへのポインタ
	LPD3DXBUFFER			m_pBuffMat;				// マテリアルへのポインタ
	DWORD					m_dwNumMat;				// マテリアル数
	D3DXMATRIX				m_mtxWorld;				// ワールドマトリックス
	CModel*					m_pParent;				// 親モデルへのポインタ
	char					m_Path[MAX_PATH];		// ファイルパス
	LPDIRECT3DVERTEXSHADER9 m_pOutlineVS;			// アウトライン頂点シェーダ
	LPDIRECT3DPIXELSHADER9  m_pOutlinePS;			// アウトラインピクセルシェーダ
	LPD3DXCONSTANTTABLE     m_pVSConsts;			// 頂点シェーダのコンスタントテーブル
	LPD3DXCONSTANTTABLE     m_pPSConsts;			// ピクセルシェーダのコンスタントテーブル
};

#endif
