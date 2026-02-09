//=============================================================================
//
// ブロックリスト処理 [BlockList.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "BlockList.h"
#include "Collider.h"

namespace ColliderParam
{
	namespace Capsule
	{
		constexpr float RADIUS = 16.5f;
		constexpr float HEIGHT = 40.0f;
	}
}

//=============================================================================
// ボックスブロックのコリジョン生成処理
//=============================================================================
std::shared_ptr<Collider> CBoxBlock::CreateCollisionShape(const D3DXVECTOR3& size)
{
	// ボックスコライダー
	return std::make_shared <BoxCollider>(size);
}
//=============================================================================
// シリンダーブロックのコリジョン生成処理
//=============================================================================
std::shared_ptr<Collider> CCylinderBlock::CreateCollisionShape(const D3DXVECTOR3& size)
{
	D3DXVECTOR3 dirY(0, 1, 0);

	// シリンダーコライダー
	return std::make_shared <CylinderCollider>(size, dirY);
}
//=============================================================================
// スフィアブロックのコリジョン生成処理
//=============================================================================
std::shared_ptr<Collider> CSphereBlock::CreateCollisionShape(const D3DXVECTOR3& size)
{
	// スフィアコライダー
	return std::make_shared <SphereCollider>(size);
}
//=============================================================================
// カプセルブロックのコリジョン生成処理
//=============================================================================
std::shared_ptr<Collider> CCapsuleBlock::CreateCollisionShape(const D3DXVECTOR3& /*size*/)
{
	// 名前空間ColliderParamの使用
	using namespace ColliderParam;

	// カプセルコライダー
	return std::make_shared <CapsuleCollider>(Capsule::RADIUS, Capsule::HEIGHT);
}
