//=============================================================================
//
// レンダリング処理 [Renderer.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RENDERER_H_// このマクロ定義がされていなかったら
#define _RENDERER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "imguimaneger.h"


//*****************************************************************************
// 前方宣言
//*****************************************************************************
class CDebugProc3D;


//*****************************************************************************
// レンダラークラス
//*****************************************************************************
class CRenderer
{
public:
	CRenderer();
	~CRenderer();

	HRESULT Init(HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(int fps);
	void ResetDevice(void);
	void OnResize(UINT width, UINT height);
	HRESULT CompileVertexShader(
		const char* filename,         // HLSLファイル名
		const char* entryPoint,       // エントリポイント名
		LPDIRECT3DVERTEXSHADER9* ppVS,// 作成したシェーダのポインタ
		LPD3DXCONSTANTTABLE* ppConsts // 定数テーブルを受け取る場合
	);
	HRESULT CompilePixelShader(
		const char* filename,         // HLSLファイル名
		const char* entryPoint,       // エントリポイント名
		LPDIRECT3DPIXELSHADER9* ppPS, // 作成したシェーダのポインタ
		LPD3DXCONSTANTTABLE* ppConsts // 定数テーブルを受け取る場合
	);

	//*****************************************************************************
	// flagment関数
	//*****************************************************************************
	bool NeedsReset(void) const;

	//*****************************************************************************
	// setter関数
	//*****************************************************************************
	void SetFPS(int fps) { m_nFPS = fps; }
	void SetBgCol(D3DXCOLOR col) { m_bgCol = col; }

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	static int GetFPS(void) { return m_nFPS; }
	static CDebugProc3D* GetDebug3D(void) { return m_pDebug3D; }
	LPDIRECT3DDEVICE9 GetDevice(void) { return m_pD3DDevice; };
	D3DXCOLOR GetBgCol(void) { return m_bgCol; }
	D3DPRESENT_PARAMETERS GetPresentParams(void) { return m_d3dpp; }
	LPDIRECT3DVERTEXSHADER9 GetSkyCubeVS(void)const { return m_pSkyCubeVS; }
	LPDIRECT3DPIXELSHADER9 GetSkyCubePS(void) const { return m_pSkyCubePS; }
	LPD3DXCONSTANTTABLE GetSkyCubeVSConsts(void) const { return m_pSkyVSConsts; }
	LPD3DXCONSTANTTABLE GetSkyCubePSConsts(void) const { return m_pSkyPSConsts; }

private:
	LPDIRECT3D9				m_pD3D;				// DirectX3Dオブジェクトへのポインタ
	LPDIRECT3DDEVICE9		m_pD3DDevice;		// デバイスへのポインタ
	static CDebugProc3D*	m_pDebug3D;			// 3Dデバッグ表示へのポインタ
	D3DXCOLOR				m_bgCol;			// 画面背景の色
	UINT					m_ResizeWidth;		// 再設定用の画面の幅
	UINT					m_ResizeHeight;		// 再設定用の画面の高さ
	D3DPRESENT_PARAMETERS	m_d3dpp;			// 再設定用のパラメーター
	LPDIRECT3DVERTEXSHADER9 m_pSkyCubeVS;		// キューブマップ頂点シェーダ
	LPDIRECT3DPIXELSHADER9  m_pSkyCubePS;		// キューブマップピクセルシェーダ
	ID3DXConstantTable*		m_pSkyVSConsts;		// キューブマップ頂点シェーダのコンスタントテーブル
	ID3DXConstantTable*		m_pSkyPSConsts;		// キューブマップピクセルシェーダのコンスタントテーブル
	static int				m_nFPS;				// FPS値の代入用

};
#endif