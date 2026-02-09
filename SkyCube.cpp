//=============================================================================
//
// スカイキューブ処理 [SkyCube.cpp]
// Author: RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "SkyCube.h"
#include "Manager.h"


//=============================================================================
// コンストラクタ
//=============================================================================
CSkyCube::CSkyCube(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	m_pVtxBuff		= nullptr;		// 頂点バッファへのポインタ
	m_nIdxTexture	= -1;			// テクスチャのインデックス
	m_fTimer		= 0.0f;			// タイマー
}
//=============================================================================
// デストラクタ
//=============================================================================
CSkyCube::~CSkyCube()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CSkyCube* CSkyCube::Create(void)
{
	CSkyCube* pSkyCube = new CSkyCube;

	// nullptrだったら
	if (pSkyCube == nullptr)
	{
		return nullptr;
	}

	// 初期化失敗時
	if (FAILED(pSkyCube->Init()))
	{
		return nullptr;
	}

	return pSkyCube;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CSkyCube::Init(void)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// キューブマップ用テクスチャのIDの設定
	m_nIdxTexture = pTexture->RegisterCube(
		"data/SkyCube/posx.jpg",
		"data/SkyCube/negx.jpg",
		"data/SkyCube/posy.jpg",
		"data/SkyCube/negy.jpg",
		"data/SkyCube/posz.jpg",
		"data/SkyCube/negz.jpg");

	// フルスクリーンクアッド生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * (UINT)VERTEX,
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		nullptr);

	VERTEX_3D* pVtx = nullptr;

	// 頂点バッファをロック
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].pos = D3DXVECTOR3(-1, 1, 0);
	pVtx[1].pos = D3DXVECTOR3(1, 1, 0);
	pVtx[2].pos = D3DXVECTOR3(-1, -1, 0);
	pVtx[3].pos = D3DXVECTOR3(1, -1, 0);

	// 頂点バッファをアンロック
	m_pVtxBuff->Unlock();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CSkyCube::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// 自身の破棄
	this->Release();
}
//=============================================================================
// 更新処理
//=============================================================================
void CSkyCube::Update(void)
{
	//// スクロール処理
	//Scroll();
}
//=============================================================================
// スクロール処理
//=============================================================================
void CSkyCube::Scroll(void)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	m_fTimer += ADD_TIME;// タイマーを加算

	// キューブマップピクセルシェーダのコンスタントテーブルの取得
	LPD3DXCONSTANTTABLE pPSConsts = pRenderer->GetSkyCubePSConsts();

	if (!pPSConsts)
	{
		return;
	}

	pPSConsts->SetFloat(pDevice, "gTime", m_fTimer);
	pPSConsts->SetFloat(pDevice, "gScrollSpeed", 0.0003f);
}
//=============================================================================
// 描画処理
//=============================================================================
void CSkyCube::Draw(void)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	// Zテスト設定
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// サンプラーステートの設定
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	// シェーダの設定
	pDevice->SetVertexShader(pRenderer->GetSkyCubeVS());
	pDevice->SetPixelShader(pRenderer->GetSkyCubePS());

	// 行列（Viewの平行移動除去）
	D3DXMATRIX view = pCamera->GetViewMatrix();
	D3DXMATRIX proj = pCamera->GetProjMatrix();
	view._41 = view._42 = view._43 = 0.0f;

	D3DXMATRIX invVP;
	D3DXMATRIX vp = view * proj;
	D3DXMatrixInverse(&invVP, nullptr, &vp);

	// 頂点シェーダの定数
	pRenderer->GetSkyCubeVSConsts()->SetMatrix(pDevice, "gInvViewProj", &invVP);

	// 頂点バッファをデバイスのデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, CManager::GetTexture()->GetCubeAddress(m_nIdxTexture));

	// ポリゴン描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// 元に戻す
	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);

	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);

	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}
