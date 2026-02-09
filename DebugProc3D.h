//=============================================================================
//
// 3Dデバッグ表示処理 [DebugProc3D.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _DEBUGPROC3D_H_// このマクロ定義がされていなかったら
#define _DEBUGPROC3D_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "PhysicsWorld.h"

//*****************************************************************************
// 3Dデバッグクラス
//*****************************************************************************
class CDebugProc3D
{
public:
	CDebugProc3D();
	~CDebugProc3D();

	void Init(void);
	void Uninit(void);

	//*****************************************************************************
	// line描画関数
	//*****************************************************************************
	static void DrawLine3D(const D3DXVECTOR3& start, const D3DXVECTOR3& end, D3DXCOLOR color);
	static void DrawCollider(Collider* shape, D3DXCOLOR color);
	static void DrawBoxCollider(BoxCollider* box, D3DXCOLOR color);
	static void DrawCapsuleCollider(CapsuleCollider* capsule, D3DXCOLOR color);
	static void DrawCylinderCollider(CylinderCollider* cylinder, D3DXCOLOR color);
	static void DrawSphereCollider(SphereCollider* sphere, D3DXCOLOR color);

private:
	static constexpr int	VERTEX	= 8;		// 頂点数
	static constexpr float	HALF	= 0.5f;		// 半分
	static constexpr float	DOUBLE	= 2.0f;		// 二倍

	static LPD3DXLINE m_pLine;   // ライン描画用オブジェクト

};

#endif