//=============================================================================
//
// フェード処理 [Fade.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Fade.h"
#include "Manager.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CFade::CFade()
{
	// 値のクリア
	m_pVtxBuff	= nullptr;			// 頂点バッファ
	m_pos		= INIT_VEC3;		// 位置
	m_fade		= FADE_NONE;		//フェードの状態
	m_fadeCol	= INIT_XCOL;		// フェードカラー
	m_SceneNext = CScene::MODE_MAX;	//次の画面
	m_fWidth	= 0.0f;				// 幅
	m_fHeight	= 0.0f;				// 高さ
}
//=============================================================================
// デストラクタ
//=============================================================================
CFade::~CFade()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CFade* CFade::Create(CScene::MODE mode)
{
	CFade* pFade = new CFade;

	// nullptrだったら
	if (pFade == nullptr)
	{
		return nullptr;
	}

	// 初期化失敗時
	if (FAILED(pFade->Init(mode)))
	{
		return nullptr;
	}

	return pFade;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CFade::Init(CScene::MODE mode)
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

	// 初期化
	m_pos = INIT_VEC3;
	m_fWidth = FHD_WIDTH;
	m_fHeight = FHD_HEIGHT;

	m_fade = FADE_IN;
	m_SceneNext = mode;	//次の画面
	m_fadeCol = INIT_XCOL;

	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x, m_pos.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x, m_pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y + m_fHeight, 0.0f);

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(m_fadeCol);
	pVtx[1].col = D3DXCOLOR(m_fadeCol);
	pVtx[2].col = D3DXCOLOR(m_fadeCol);
	pVtx[3].col = D3DXCOLOR(m_fadeCol);

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
void CFade::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void CFade::Update(void)
{
	VERTEX_2D* pVtx;

	if (m_fade != FADE_NONE)
	{
		if (m_fade == FADE_IN)
		{//フェードイン状態
			m_fadeCol.a -= FADE_SPEED;//ポリゴンを透明にしておく

			if (m_fadeCol.a <= 0.0f)
			{
				m_fadeCol.a = 0.0f;
				m_fade = FADE_NONE;
			}
		}
		else if (m_fade == FADE_OUT)
		{
			m_fadeCol.a += FADE_SPEED;

			if (m_fadeCol.a >= 1.0f)
			{
				m_fadeCol.a = 1.0f;
				m_fade = FADE_IN;

				//モード設定
				CManager::SetMode(m_SceneNext);

				return;
			}
		}

		//頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		//頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(m_fadeCol);
		pVtx[1].col = D3DXCOLOR(m_fadeCol);
		pVtx[2].col = D3DXCOLOR(m_fadeCol);
		pVtx[3].col = D3DXCOLOR(m_fadeCol);

		//頂点バッファをアンロックする
		m_pVtxBuff->Unlock();
	}
}
//=============================================================================
// 描画処理
//=============================================================================
void CFade::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0, nullptr);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
//=============================================================================
// フェードの設定処理
//=============================================================================
void CFade::SetFade(CScene::MODE modeNext)
{
	m_fade = FADE_OUT;//フェードアウト状態に
	m_SceneNext = modeNext;//次の画面を設定
}
