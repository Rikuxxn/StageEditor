//=============================================================================
//
// テクスチャ処理 [Texture.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Texture.h"
#include "Renderer.h"
#include "Manager.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
int CTexture::m_nNumAll = 0;

//=============================================================================
// コンストラクタ
//=============================================================================
CTexture::CTexture()
{
	// 値のクリア
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		m_apTexture[nCnt]		= {};
		m_apCubeTexture[nCnt]	= {};
	}
}
//=============================================================================
// デストラクタ
//=============================================================================
CTexture::~CTexture()
{
	// なし
}
//=============================================================================
// テクスチャの読み込み
//=============================================================================
HRESULT CTexture::Load(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 全てのテクスチャの読み込み
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		if (!TEXTURE[nCnt].empty()) // 空文字列じゃなければロード
		{
			HRESULT hr = D3DXCreateTextureFromFile(
				pDevice,
				TEXTURE[nCnt].c_str(),
				&m_apTexture[nCnt]);

			if (FAILED(hr))
			{
				m_apTexture[nCnt] = nullptr;
			}
		}
		else
		{
			m_apTexture[nCnt] = nullptr;
		}
	}

	return S_OK;
}
//=============================================================================
// テクスチャの破棄
//=============================================================================
void CTexture::Unload(void)
{
	// 全てのテクスチャの破棄
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		// クリア
		TEXTURE[nCnt].clear();

		if (m_apTexture[nCnt] != nullptr)
		{
			m_apTexture[nCnt]->Release();
			m_apTexture[nCnt] = nullptr;
		}

		if (m_apCubeTexture[nCnt] != nullptr)
		{
			m_apCubeTexture[nCnt]->Release();
			m_apCubeTexture[nCnt] = nullptr;
		}
	}
}
//=============================================================================
// テクスチャの指定処理
//=============================================================================
int CTexture::RegisterDynamic(const char* pFilename)
{
	// すでにロード済みならインデックス返す
	for (int nCnt = 0; nCnt < m_nNumAll; nCnt++)
	{
		if (!TEXTURE[nCnt].empty() && TEXTURE[nCnt] == pFilename)
		{
			return nCnt;
		}
	}

	// 新しいスロットにロード
	if (m_nNumAll < MAX_TEXTURE)
	{
		// デバイスの取得
		LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

		if (SUCCEEDED(D3DXCreateTextureFromFile(pDevice, pFilename, &m_apTexture[m_nNumAll])))
		{
			TEXTURE[m_nNumAll] = pFilename;  // std::string に代入
			return m_nNumAll++;
		}
	}
	return -1;
}
//=============================================================================
// キューブマップ用テクスチャの指定処理
//=============================================================================
int CTexture::RegisterCube(
	const char* px,
	const char* nx,
	const char* py,
	const char* ny,
	const char* pz,
	const char* nz)
{
	// 範囲外だったら
	if (m_nNumAll >= MAX_TEXTURE)
	{
		return -1;
	}

	// デバイスの取得
	LPDIRECT3DDEVICE9 device = CManager::GetRenderer()->GetDevice();

	// キューブマップテクスチャ
	LPDIRECT3DCUBETEXTURE9 cube = nullptr;

	// まず +X 画像からサイズ取得
	D3DXIMAGE_INFO info;
	if (FAILED(D3DXGetImageInfoFromFile(px, &info)))
	{
		return -1;

	}

	// CubeTexture生成
	if (FAILED(D3DXCreateCubeTexture(
		device,
		info.Width,
		1,
		0,
		info.Format,
		D3DPOOL_MANAGED,
		&cube)))
	{
		return -1;
	}

	const char* files[CUBEMAP_TEX_NUM] = { px, nx, py, ny, pz, nz };

	for (int nCnt = 0; nCnt < CUBEMAP_TEX_NUM; nCnt++)
	{
		LPDIRECT3DSURFACE9 face;
		cube->GetCubeMapSurface((D3DCUBEMAP_FACES)nCnt, 0, &face);

		if (FAILED(D3DXLoadSurfaceFromFile(
			face,
			nullptr,
			nullptr,
			files[nCnt],
			nullptr,
			D3DX_DEFAULT,
			0,
			nullptr)))
		{
			// 破棄
			face->Release();
			cube->Release();

			return -1;
		}

		// 破棄
		face->Release();
	}

	m_apCubeTexture[m_nNumAll] = cube;
	TEXTURE[m_nNumAll] = "cube6";

	return m_nNumAll++;
}
//=============================================================================
// テクスチャのアドレス取得
//=============================================================================
LPDIRECT3DTEXTURE9 CTexture::GetAddress(int nIdx)
{
	// 範囲外だったら
	if (nIdx < 0 || nIdx >= MAX_TEXTURE)
	{
		return nullptr;
	}

	return m_apTexture[nIdx];
}
//=============================================================================
// キューブマップ用テクスチャのアドレス取得
//=============================================================================
LPDIRECT3DCUBETEXTURE9 CTexture::GetCubeAddress(int nIdx)
{
	// 範囲外だったら
	if (nIdx < 0 || nIdx >= MAX_TEXTURE)
	{
		return nullptr;
	}

	return m_apCubeTexture[nIdx];
}
