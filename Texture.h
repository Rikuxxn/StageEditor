//=============================================================================
//
// テクスチャ処理 [Texture.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TEXTURE_H_// このマクロ定義がされていなかったら
#define _TEXTURE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************

//*****************************************************************************
// テクスチャクラス
//*****************************************************************************
class CTexture
{
public:
	CTexture();
	~CTexture();

	HRESULT Load(void);
	void Unload(void);
	int RegisterDynamic(const char* pFilename);
	int RegisterCube(
		const char* px,
		const char* nx,
		const char* py,
		const char* ny,
		const char* pz,
		const char* nz);
	LPDIRECT3DTEXTURE9 GetAddress(int nIdx);
	LPDIRECT3DCUBETEXTURE9 GetCubeAddress(int nIdx);

private:
	static constexpr int MAX_TEXTURE		= 128;			// テクスチャの最大数
	static constexpr int CUBEMAP_TEX_NUM	= 6;			// キューブマップテクスチャの枚数

	std::string				TEXTURE[MAX_TEXTURE];			// テクスチャパス格納
	LPDIRECT3DTEXTURE9		m_apTexture[MAX_TEXTURE];		// テクスチャ配列
	LPDIRECT3DCUBETEXTURE9  m_apCubeTexture[MAX_TEXTURE];	// キューブ用テクスチャ配列
	static int				m_nNumAll;						// 総数
};

#endif