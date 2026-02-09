//=============================================================================
//
// コンパイル処理 [pch.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PCH_H_// このマクロ定義がされていなかったら
#define _PCH_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// ビルド時の警告対処用マクロ
//*****************************************************************************
#define DIRECTINPUT_VERSION (0x0800)							

#define NOMINMAX       // windows.h の前に入れる

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "windows.h"
#include "d3dx9.h"												// 描画処理に必要
#include "dinput.h"												// 入力処理に必要
#include "Xinput.h"												// ジョイパッド処理に必要
#include "xaudio2.h"											// 2Dサウンド処理に必要
#include "X3DAudio.h"											// 3Dサウンド処理に必要
#include "array"
#include "string"
#include "vector"
#include "stdio.h"
#include "fstream"
#include "commdlg.h"
#include "functional"

//*****************************************************************************
// ライブラリのリンク
//*****************************************************************************
#pragma comment(lib,"d3d9.lib")									// 描画処理に必要
#pragma comment(lib,"d3dx9.lib")								// [d3d9.lib]の拡張ライブラリ
#pragma comment(lib,"dxguid.lib")								// DirectXコンポーネント(部品)使用に必要
#pragma comment(lib,"winmm.lib")								// システム時刻取得に必要
#pragma comment(lib,"dinput8.lib")								// 入力処理に必要
#pragma comment(lib,"xinput.lib")								// ジョイパッド処理に必要
#pragma comment(lib,"X3DAudio.lib")								// 3Dサウンド処理に必要

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME	  "WindowClass"								// ウィンドウクラスの名前
#define WINDOW_NAME	  "STAGE EDITOR with SEED Physics"			// ウィンドウの名前(キャプションに表示)
#define SCREEN_WIDTH  (1280)									// ウィンドウの幅
#define SCREEN_HEIGHT (720)										// ウィンドウの高さ
#define FVF_VERTEX_2D (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define FVF_VERTEX_3D (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)	// 座標・法線・カラー・テクスチャ

//*****************************************************************************
// 初期化用マクロ定義
//*****************************************************************************
#define INIT_VEC3	(D3DXVECTOR3(0.0f,0.0f,0.0f))
#define INIT_COL	(D3DCOLOR_RGBA (0,0,0,255))
#define INIT_XCOL	(D3DXCOLOR (0.0f,0.0f,0.0f,1.0f))
#define INIT_XCOL_WHITE	(D3DXCOLOR (1.0f,1.0f,1.0f,1.0f))


//*****************************************************************************
// 画面の種類
//*****************************************************************************
typedef enum
{
	MODE_TITLE = 0,												// タイトル画面
	MODE_GAME,													// ゲーム画面
	MODE_RESULT,												// リザルト画面
	MODE_RANKING,												// ランキング画面
	MODE_MAX
}MODE;

//*****************************************************************************
// 頂点情報[2D]の構造体
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;											// 頂点座標
	float rhw;													// 座標変換用係数
	D3DCOLOR col;												// 座標カラー
	D3DXVECTOR2 tex;											// テクスチャ座標
}VERTEX_2D;

//*****************************************************************************
// 頂点情報[3D]の構造体
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;											//頂点座標
	D3DXVECTOR3 nor;											//法線ベクトル
	D3DCOLOR col;												//座標カラー
	D3DXVECTOR2 tex;											//テクスチャ座標
}VERTEX_3D;

#endif