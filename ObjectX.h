//=============================================================================
//
// Xファイル処理 [ObjectX.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _OBJECTX_H_// このマクロ定義がされていなかったら
#define _OBJECTX_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Object.h"

//*****************************************************************************
// Xファイルクラス
//*****************************************************************************
class CObjectX : public CObject
{
public:
	CObjectX(int nPriority = 3);
	virtual ~CObjectX();

	static CObjectX* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawNormal(LPDIRECT3DDEVICE9 pDevice);
	void SetOutlineShaderConstants(LPDIRECT3DDEVICE9 pDevice);

	//*****************************************************************************
	// setter関数
	//*****************************************************************************
	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }
	void SetTexPath(const std::vector<std::string>& texPaths) { m_texPaths = texPaths; }
	void SetSize(D3DXVECTOR3 size) { m_size = size; }
	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRot(D3DXVECTOR3 rot) { m_rot = rot; }

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	const char* GetPath(void) { return m_szPath; }
	const std::vector<std::string>& GetTexPaths(void) const { return m_texPaths; }	
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXVECTOR3 GetRot(void) { return m_rot; }
	D3DXVECTOR3 GetSize(void) const { return m_size; }		// 拡大率
	D3DXVECTOR3 GetModelSize(void) { return m_modelSize; }	// モデルの元サイズ
	virtual D3DXCOLOR GetCol(void) const { return INIT_XCOL_WHITE; }
	D3DXCOLOR GetMaterialColor(void) const;
	LPD3DXMESH GetMesh(void)const { return m_pMesh; }
	DWORD GetNumMat(void) const { return m_dwNumMat; }// Xファイル読み込み時に取得済みのマテリアル数

private:
	static constexpr float OUTLINE_THICKNESS = 0.4f;// アウトラインの太さ

	int*						m_nIdxTexture;
	D3DXVECTOR3					m_pos;				// 位置
	D3DXVECTOR3					m_rot;				// 向き
	D3DXVECTOR3					m_move;				// 移動量
	D3DXVECTOR3					m_size;				// サイズ
	D3DXVECTOR3					m_modelSize;		// モデルの元サイズ（全体の幅・高さ・奥行き）
	LPD3DXMESH					m_pMesh;			// メッシュへのポインタ
	LPD3DXMESH					m_pOutlineMesh;		// アウトライン用メッシュへのポインタ
	LPD3DXBUFFER				m_pBuffMat;			// マテリアルへのポインタ
	DWORD						m_dwNumMat;			// マテリアル数
	D3DXMATRIX					m_mtxWorld;			// ワールドマトリックス
	LPDIRECT3DVERTEXSHADER9		m_pOutlineVS;		// アウトライン頂点シェーダ
	LPDIRECT3DPIXELSHADER9		m_pOutlinePS;		// アウトラインピクセルシェーダ
	LPD3DXCONSTANTTABLE			m_pVSConsts;		// 頂点シェーダのコンスタントテーブル
	LPD3DXCONSTANTTABLE			m_pPSConsts;		// ピクセルシェーダのコンスタントテーブル
	char						m_szPath[MAX_PATH];	// ファイルパス
	std::vector<std::string>	m_texPaths;			// 複数テクスチャを保持
};

#endif