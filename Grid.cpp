//=============================================================================
//
// グリッド処理 [Grid.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Grid.h"
#include "Manager.h"

namespace Axis
{
    namespace Length
    {
        const D3DXVECTOR3 X{ 150.0f, 0.0f, 0.0f };
        const D3DXVECTOR3 Y{ 0.0f, 150.0f, 0.0f };
        const D3DXVECTOR3 Z{ 0.0f, 0.0f, 150.0f };
    }
}

//=============================================================================
// コンストラクタ
//=============================================================================
CGrid::CGrid()
{
	// 値のクリア
    m_mtxWorld  = {};   // ワールドマトリックス
    m_vertices  = {};   // 頂点
    m_size      = 0;    // 半径
    m_spacing   = 0.0f; // マス間隔
}
//=============================================================================
// デストラクタ
//=============================================================================
CGrid::~CGrid()
{
	// リストのクリア
	m_vertices.clear();
}
//=============================================================================
// ユニークポインタの生成
//=============================================================================
std::unique_ptr<CGrid> CGrid::Create(void)
{
    return std::make_unique<CGrid>();
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CGrid::Init(void)
{
	m_size      = SIZE;
	m_spacing   = SPACING;

	// リストのクリア
	m_vertices.clear();

    // X軸方向の線
    for (int nCnt = -m_size; nCnt <= m_size; nCnt++)
    {
        AddLine(D3DXVECTOR3(-m_size * m_spacing, 0, nCnt * m_spacing), 
            D3DXVECTOR3(m_size * m_spacing, 0, nCnt * m_spacing),
            D3DCOLOR_XRGB(128,128,128));
    }

    // Z軸方向の線
    for (int nCnt = -m_size; nCnt <= m_size; nCnt++)
    {
        AddLine(D3DXVECTOR3(nCnt * m_spacing, 0, -m_size * m_spacing), 
            D3DXVECTOR3(nCnt * m_spacing, 0, m_size * m_spacing),
            D3DCOLOR_XRGB(128,128,128));
    }

    // 名前空間Axisの使用
    using namespace Axis;

    // マニピュレータの描画
    AddLine(INIT_VEC3, Length::X, D3DCOLOR_XRGB(255, 0, 0));// X軸
    AddLine(INIT_VEC3, Length::Y, D3DCOLOR_XRGB(0, 255, 0));// Y軸
    AddLine(INIT_VEC3, Length::Z, D3DCOLOR_XRGB(0, 0, 255));// Z軸

	return S_OK;
}
//=============================================================================
// 描画処理
//=============================================================================
void CGrid::Draw(void)
{
    // デバイスの取得
    LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE); // ライティング無効

    // 頂点フォーマットの設定
    pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

    // テクスチャの設定
    pDevice->SetTexture(0, nullptr);

    // 描画
    pDevice->DrawPrimitiveUP(D3DPT_LINELIST, m_vertices.size() / 2, m_vertices.data(), sizeof(CUSTOMVERTEX));

    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE); // ライティングを戻す
}
//=============================================================================
// ライン追加処理
//=============================================================================
void CGrid::AddLine(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, D3DCOLOR col)
{
    m_vertices.push_back({ p1,col });
    m_vertices.push_back({ p2,col });
}