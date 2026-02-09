//=============================================================================
//
// パラメータ定義 [Parameter.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PARAMETER_H_// このマクロ定義がされていなかったら
#define _PARAMETER_H_// 2重インクルード防止のマクロ定義


// カメラのパラメータ構造体
struct CameraParam
{
	D3DXVECTOR3 eye;
	D3DXVECTOR3 at;
	D3DXVECTOR3 rot;
	float		distance;
};

// ライトのパラメータ構造体
struct LightParam
{
	D3DLIGHTTYPE type;
	D3DXCOLOR    color;
	D3DXVECTOR3  dir;
	D3DXVECTOR3  pos;
};

#endif