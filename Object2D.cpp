//=============================================================================
//
// 2Dオブジェクト処理 [Object2D.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Object2D.h"
#include "Manager.h"
#include "Renderer.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CObject2D::CObject2D(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	m_pVtxBuff		= nullptr;						// 頂点バッファ
	m_pos			= INIT_VEC3;					// 位置
	m_rot			= INIT_VEC3;					// 向き
	m_move			= INIT_VEC3;					// 移動量
	m_col			= D3DCOLOR_ARGB(255, 0, 0, 0);	// 色
	m_fLength		= 0.0f;							// 長さ
	m_fAngle		= 0.0f;							// 角度
	m_fWidth		= 0.0f;							// 幅
	m_fHeight		= 0.0f;							// 高さ
	m_nCounterAnim	= 0;							// アニメーションカウンター
	m_nPatternAnim	= 0;							// アニメーションパターンNo.
	m_fUSpeed		= 0.0f;							// 横のスクロールスピード
	m_fVSpeed		= 0.0f;							// 縦のスクロールスピード
	m_aPosTexU		= 0.0f;							// 横のスクロール用変数
	m_fTexU			= 0.0f;							// テクスチャUV
	m_fTexV			= 0.0f;							// テクスチャUV
}
//=============================================================================
// デストラクタ
//=============================================================================
CObject2D::~CObject2D()
{
	// 今はなし
}
//=============================================================================
// 生成処理
//=============================================================================
CObject2D* CObject2D::Create(D3DXVECTOR3 pos, D3DXCOLOR col, float fWidth, float fHeight)
{
	// オブジェクトの生成
	CObject2D* pObject2D = new CObject2D;

	// nullptrだったら
	if (pObject2D == nullptr)
	{
		return nullptr;
	}

	pObject2D->SetPos(pos);
	pObject2D->SetSize(fWidth, fHeight);
	pObject2D->SetCol(col);

	// 初期化失敗時
	if (FAILED(pObject2D->Init()))
	{
		return nullptr;
	}

	return pObject2D;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CObject2D::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * VERTEX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_fWidth, m_pos.y - m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y - m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_fWidth, m_pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y + m_fHeight, 0.0f);

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CObject2D::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// オブジェクトの破棄(自分自身)
	this->Release();
}
//=============================================================================
// 更新処理
//=============================================================================
void CObject2D::Update()
{
	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 対角線の長さを算出する
	m_fLength = sqrtf(m_fWidth * m_fWidth + m_fHeight * m_fHeight);

	// 対角線の角度を算出する
	m_fAngle = atan2f(m_fWidth, m_fHeight);

	// 頂点座標の設定
	pVtx[0].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI + m_fAngle)) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI + m_fAngle)) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + sinf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(m_rot.z + (D3DX_PI - m_fAngle)) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + sinf(m_rot.z + (0.0f - m_fAngle)) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_rot.z + (0.0f - m_fAngle)) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + sinf(m_rot.z + (0.0f + m_fAngle)) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_rot.z + (0.0f + m_fAngle)) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}
//=============================================================================
// 描画処理
//=============================================================================
void CObject2D::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// テクスチャの設定
	pDevice->SetTexture(0, nullptr);
}
//=============================================================================
// テクスチャUVの設定
//=============================================================================
void CObject2D::SetUV(int nTexU, int nTexV)
{
	m_fTexU = 1.0f / nTexU;
	m_fTexV = 1.0f / nTexV;

	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(m_fTexU, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, m_fTexV);
	pVtx[3].tex = D3DXVECTOR2(m_fTexU, m_fTexV);

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}
//=============================================================================
// テクスチャUV移動処理
//=============================================================================
void CObject2D::MoveTexUV(float fLeft, float fTop, float fWidth, float fHeight)
{
	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].tex = D3DXVECTOR2(fLeft, fTop);
	pVtx[1].tex = D3DXVECTOR2(fLeft + fWidth, fTop);
	pVtx[2].tex = D3DXVECTOR2(fLeft, fTop + fHeight);
	pVtx[3].tex = D3DXVECTOR2(fLeft + fWidth, fTop + fHeight);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}
//=============================================================================
// 通常テクスチャアニメーション処理
//=============================================================================
void CObject2D::TextureAnim(int nTexPosX, int nTexPosY, int nAnimSpeed)
{
	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	float UV1 = 1.0f / nTexPosX;
	float UV2 = 1.0f / nTexPosY;

	m_nCounterAnim++;// カウンターを加算

	if ((m_nCounterAnim % nAnimSpeed) == 0)
	{
		m_nCounterAnim = 0;

		// nTexPosX枚目の次は1枚目に戻る
		m_nPatternAnim = (m_nPatternAnim + 1) % nTexPosX;

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(m_nPatternAnim * UV1, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(m_nPatternAnim * UV1 + UV1, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(m_nPatternAnim * UV1, UV2);
		pVtx[3].tex = D3DXVECTOR2(m_nPatternAnim * UV1 + UV1, UV2);

		// 頂点バッファをアンロックする
		m_pVtxBuff->Unlock();
	}
}
//=============================================================================
// テクスチャスクロール処理
//=============================================================================
void CObject2D::ScrollTexture(void)
{
	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	 m_aPosTexU += m_fUSpeed;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//テクスチャ座標の更新
	pVtx[0].tex = D3DXVECTOR2(m_aPosTexU, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(m_aPosTexU + 1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(m_aPosTexU, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(m_aPosTexU + 1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}
//=============================================================================
// サイズ設定
//=============================================================================
void CObject2D::SetSize(float fWidth, float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;
}
