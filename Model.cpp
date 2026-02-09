//=============================================================================
//
// モデル処理 [Model.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Model.h"
#include "Texture.h"
#include "Renderer.h"
#include "Manager.h"
#include "cstdio"

//=============================================================================
// コンストラクタ
//=============================================================================
CModel::CModel()
{
	// 値のクリア
	memset(m_Path, 0, sizeof(m_Path));	// ファイルパス
	m_nIdxTexture	= 0;				// テクスチャインデックス
	m_pos			= INIT_VEC3;		// 位置
	m_rot			= INIT_VEC3;		// 向き
	m_move			= INIT_VEC3;		// 移動量
	m_pMesh			= nullptr;			// メッシュへのポインタ
	m_pBuffMat		= nullptr;			// マテリアルへのポインタ
	m_dwNumMat		= NULL;				// マテリアル数
	m_mtxWorld		= {};				// ワールドマトリックス
	m_pParent		= nullptr;			// 親モデルへのポインタ
	m_pOutlineVS	= nullptr;			// アウトライン頂点シェーダ
	m_pOutlinePS	= nullptr;			// アウトラインピクセルシェーダ
	m_pVSConsts		= nullptr;			// 頂点シェーダのコンスタントテーブル
	m_pPSConsts		= nullptr;			// ピクセルシェーダのコンスタントテーブル
}
//=============================================================================
// デストラクタ
//=============================================================================
CModel::~CModel()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CModel* CModel::Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CModel* pModel = new CModel;

	// nullptrだったら
	if (pModel == nullptr)
	{
		return nullptr;
	}

	pModel->m_pos = pos;
	pModel->m_rot = D3DXToRadian(rot);
	strcpy_s(pModel->m_Path, pFilepath); // パス保存

	// 初期化失敗時
	if (FAILED(pModel->Init()))
	{
		return nullptr;
	}

	return pModel;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CModel::Init(void)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// Xファイルの読み込み
	D3DXLoadMeshFromX(m_Path,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&m_pBuffMat,
		NULL,
		&m_dwNumMat,
		&m_pMesh);

	int nNumVtx;		// 頂点数
	DWORD sizeFVF;		// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;		// 頂点バッファへのポインタ

	// メッシュが読み込まれているか確認
	if (m_pMesh == nullptr)
	{
		MessageBox(nullptr, "Xファイルの読み込みに失敗しました（メッシュがNULLです）", "エラー", MB_OK | MB_ICONERROR);
		return 0;
	}

	// スムーズ化
	{
		ID3DXMesh* pTempMesh = nullptr;

		// 隣接情報を作成
		DWORD* pAdjacency = new DWORD[m_pMesh->GetNumFaces() * 3];
		m_pMesh->GenerateAdjacency(1e-6f, pAdjacency);

		// 法線生成（スムーズ化）
		HRESULT hr = D3DXComputeNormals(m_pMesh, pAdjacency);

		if (FAILED(hr))
		{
			// モデルに法線が無い場合、いったんクローンして法線を付与
			D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
			m_pMesh->GetDeclaration(decl);
			m_pMesh->CloneMesh(D3DXMESH_SYSTEMMEM, decl, pDevice, &pTempMesh);

			if (pTempMesh)
			{
				pTempMesh->GenerateAdjacency(1e-6f, pAdjacency);
				D3DXComputeNormals(pTempMesh, pAdjacency);
				m_pMesh->Release();
				m_pMesh = pTempMesh;
			}
		}

		delete[] pAdjacency;
	}

	// 頂点数の取得
	nNumVtx = m_pMesh->GetNumVertices();

	// 頂点フォーマットの取得
	sizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

	// 頂点バッファのロック
	m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);


	// 最大値・最小値はなし


	// 頂点バッファのアンロック
	m_pMesh->UnlockVertexBuffer();

	D3DXMATERIAL* pMat;// マテリアルへのポインタ

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	m_nIdxTexture = new int[m_dwNumMat];

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		if (pMat[nCntMat].pTextureFilename != nullptr)
		{// テクスチャファイルが存在する
			// テクスチャの登録
			m_nIdxTexture[nCntMat] = pTexture->RegisterDynamic(pMat[nCntMat].pTextureFilename);
		}
		else
		{// テクスチャが存在しない
			m_nIdxTexture[nCntMat] = -1;
		}
	}

	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// 頂点シェーダをコンパイル
	pRenderer->CompileVertexShader("OutlineVS.hlsl", "VSMain", &m_pOutlineVS, &m_pVSConsts);

	// ピクセルシェーダをコンパイル
	pRenderer->CompilePixelShader("OutlinePS.hlsl", "PSMain", &m_pOutlinePS, &m_pPSConsts);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CModel::Uninit(void)
{
	// テクスチャインデックスの破棄
	if (m_nIdxTexture != nullptr)
	{
		delete[] m_nIdxTexture;
		m_nIdxTexture = nullptr;
	}

	// メッシュの破棄
	if (m_pMesh != nullptr)
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}

	// マテリアルの破棄
	if (m_pBuffMat != nullptr)
	{
		m_pBuffMat->Release();
		m_pBuffMat = nullptr;
	}

	// シェーダーの破棄
	if (m_pOutlineVS) { m_pOutlineVS->Release(); m_pOutlineVS = nullptr; }
	if (m_pOutlinePS) { m_pOutlinePS->Release(); m_pOutlinePS = nullptr; }
	if (m_pVSConsts) { m_pVSConsts->Release();  m_pVSConsts = nullptr; }
	if (m_pPSConsts) { m_pPSConsts->Release();  m_pPSConsts = nullptr; }
}
//=============================================================================
// 更新処理
//=============================================================================
void CModel::Update(void)
{
	// 位置を更新
	m_pos.x += m_move.x;
	m_pos.z += m_move.z;
	m_pos.y += m_move.y;
}
//=============================================================================
// 描画処理
//=============================================================================
void CModel::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向き反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y + m_OffsetRot.y, m_rot.x + m_OffsetRot.x, m_rot.z + m_OffsetRot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x + m_OffsetPos.x, m_pos.y + m_OffsetPos.y, m_pos.z + m_OffsetPos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	D3DXMATRIX mtxParent;

	if (m_pParent != nullptr)
	{// 親が存在する
		// 親モデルのマトリックスを取得
		mtxParent = m_pParent->GetMtxWorld();
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);
	}
	else
	{// 親が存在しない
		// 現在(最新)のマトリックスを取得する
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);
	}

	// ワールドマトリックス設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// アウトライン描画
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW); // カリング反転
	pDevice->SetVertexShader(m_pOutlineVS);
	pDevice->SetPixelShader(m_pOutlinePS);

	// 定数の設定処理
	SetOutlineShaderConstants(pDevice);

	for (int nCnt = 0; nCnt < (int)m_dwNumMat; nCnt++)
	{
		// モデルの描画(アウトライン用)
		m_pMesh->DrawSubset(nCnt);
	}

	// 状態リセット
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);

	// 通常描画
	DrawNormal(pDevice);
}
//=============================================================================
// 通常モデル描画処理
//=============================================================================
void CModel::DrawNormal(LPDIRECT3DDEVICE9 pDevice)
{
	// テクスチャの取得
	CTexture* pTexture = CManager::GetTexture();

	D3DMATERIAL9 matDef;

	// 現在のマテリアルの保存
	pDevice->GetMaterial(&matDef);

	// マテリアルデータへのポインタ
	D3DXMATERIAL* pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		// マテリアル設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// テクスチャ設定
		if (m_nIdxTexture[nCntMat] == -1)
			pDevice->SetTexture(0, nullptr);
		else
			pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture[nCntMat]));

		// 描画
		m_pMesh->DrawSubset(nCntMat);
	}

	// 元マテリアルに戻す
	pDevice->SetMaterial(&matDef);
}
//=============================================================================
// 定数設定処理
//=============================================================================
void CModel::SetOutlineShaderConstants(LPDIRECT3DDEVICE9 pDevice)
{
	// 行列を取得
	D3DXMATRIX view, proj, wvp;
	pDevice->GetTransform(D3DTS_WORLD, &m_mtxWorld);
	pDevice->GetTransform(D3DTS_VIEW, &view);
	pDevice->GetTransform(D3DTS_PROJECTION, &proj);

	wvp = m_mtxWorld * view * proj;

	// 定数をシェーダに渡す
	if (m_pVSConsts) // 頂点シェーダ定数テーブル
	{
		m_pVSConsts->SetMatrix(pDevice, "g_mWorld", &m_mtxWorld);
		m_pVSConsts->SetMatrix(pDevice, "g_mWorldViewProj", &wvp);
		m_pVSConsts->SetFloat(pDevice, "g_OutlineWidth", OUTLINE_THICKNESS); // アウトライン太さ
	}

	if (m_pPSConsts) // ピクセルシェーダ定数テーブル
	{
		D3DXVECTOR4 col(0.0f, 0.0f, 0.0f, 1.0f); // 黒アウトライン
		m_pPSConsts->SetVector(pDevice, "g_OutlineColor", &col);
	}
}