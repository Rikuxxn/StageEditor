//=============================================================================
//
// 当たり判定処理 [collisionUtils.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _COLLISIONUTILS_H_// このマクロ定義がされていなかったら
#define _COLLISIONUTILS_H_// 2重インクルード防止のマクロ定義

// 前方宣言
class CBoxCollider;
class CCapsuleCollider;
class CCylinderCollider;

//*****************************************************************************
// 当たり判定クラス
//*****************************************************************************
class CCollision
{
public:

	// シリンダーとAABBの当たり判定(その距離を返す)
	static bool CheckCylinderAABBCollisionWithHitDistance(
		const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight, const D3DXVECTOR3& cylinderDir,
		const D3DXVECTOR3& aabbMin, const D3DXVECTOR3& aabbMax,
		float* outHitDistance);

	// シリンダーとカプセルの当たり判定
	static bool CheckCapsuleCylinderCollision_Dir(
		const D3DXVECTOR3& capsuleCenter, float capsuleRadius, float capsuleHeight,
		const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight,
		const D3DXVECTOR3& cylinderDir, bool flag);

	// AABB同士の当たり判定
	static bool CheckCollisionAABB(D3DXVECTOR3 pos1, D3DXVECTOR3 modelSize1, D3DXVECTOR3 scale1,
		D3DXVECTOR3 pos2, D3DXVECTOR3 modelSize2, D3DXVECTOR3 scale2);

	// AABBとカプセルの当たり判定(カプセルをAABB変換)
	static bool CheckCapsuleAABBCollision(D3DXVECTOR3 pos, D3DXVECTOR3 modelSize, D3DXVECTOR3 scale,
		D3DXVECTOR3 capsulePos, float Radius, float Height);

	// OBB同士の当たり判定
	static bool CheckCollisionOBB(const CBoxCollider* obb1, const CBoxCollider* obb2);

	// カプセルとOBBの当たり判定
	static bool CheckCapsuleOBBCollision(const CCapsuleCollider* capsule, const CBoxCollider* obb);

private:
	//*************************************************************************
	// 補助関数
	//*************************************************************************

	// OBB を axis に投影して min/max を返す関数
	static void ProjectOBB(const CBoxCollider* obb, const D3DXVECTOR3& axis,
		float& outMin, float& outMax);

	// 2つのOBBが指定軸上で重なるか判定する関数
	static bool OverlapOnAxis(const CBoxCollider* obb1, const CBoxCollider* obb2,
		const D3DXVECTOR3& axis);

	static float DistanceSqPointSegment(const D3DXVECTOR3& p, const D3DXVECTOR3& a, const D3DXVECTOR3& b);

	static D3DXVECTOR3 ClosestPointSegmentAABB(const D3DXVECTOR3& a, const D3DXVECTOR3& b,
		const D3DXVECTOR3& minVal, const D3DXVECTOR3& maxVal);
};


//*****************************************************************************
// コライダークラス
//*****************************************************************************
class CCollider
{
public:
	enum TYPE
	{
		TYPE_BOX,		// ボックスコライダー
		TYPE_CAPSULE,	// カプセルコライダー
		TYPE_CYLINDER,	// シリンダーコライダー
	};

	CCollider(TYPE type) : m_Type(type) {}
	virtual ~CCollider() {}

	TYPE GetType(void) const { return m_Type; }

	// ワールド位置を更新する
	virtual void SetPosition(const D3DXVECTOR3& pos) { m_Position = pos; }
	virtual const D3DXVECTOR3& GetPosition(void) const { return m_Position; }

protected:
	TYPE m_Type;
	D3DXVECTOR3 m_Position;
};

//*****************************************************************************
// ボックスコライダー(OBB)クラス
//*****************************************************************************
class CBoxCollider : public CCollider
{
public:
	CBoxCollider(const D3DXVECTOR3& size)// size : 元サイズ
	: CCollider(TYPE_BOX), m_Size(size) {}

	// 位置と回転の反映
	void UpdateTransform(const D3DXVECTOR3& pos, const D3DXVECTOR3& rotRad, const D3DXVECTOR3& scale)
	{
		// 実サイズ = モデル元サイズ × スケール
		m_ScaledSize.x = m_Size.x * scale.x;
		m_ScaledSize.y = m_Size.y * scale.y;
		m_ScaledSize.z = m_Size.z * scale.z;

		// 位置
		m_Position = pos;

		// 回転行列
		D3DXMatrixRotationYawPitchRoll(&m_Rotation, rotRad.y, rotRad.x, rotRad.z);
	}

	const D3DXVECTOR3& GetSize(void) const { return m_Size; }
	const D3DXMATRIX& GetRotation(void) const { return m_Rotation; }

private:
	D3DXVECTOR3 m_Size;			// 幅・高さ・奥行き
	D3DXVECTOR3 m_ScaledSize;	// 拡大率反映後サイズ
	D3DXMATRIX  m_Rotation;		// OBB用の回転行列
};

//*****************************************************************************
// カプセルコライダークラス
//*****************************************************************************
class CCapsuleCollider : public CCollider
{
public:
	CCapsuleCollider(float radius, float height)
	: CCollider(TYPE_CAPSULE), m_fRadius(radius), m_fHeight(height) {}

	float GetRadius(void) const { return m_fRadius; }
	float GetHeight(void) const { return m_fHeight; }

private:
	float m_fRadius;
	float m_fHeight;
};

//*****************************************************************************
// シリンダーコライダークラス
//*****************************************************************************
class CCylinderCollider : public CCollider
{
public:
	CCylinderCollider(float radius, float height, const D3DXVECTOR3& dir)
	: CCollider(TYPE_CYLINDER), m_fRadius(radius), m_fHeight(height), m_Dir(dir) {}

	float GetRadius(void) const { return m_fRadius; }
	float GetHeight(void) const { return m_fHeight; }
	const D3DXVECTOR3& GetDirection(void) const { return m_Dir; }

private:
	float m_fRadius;
	float m_fHeight;
	D3DXVECTOR3 m_Dir;  // 軸方向
};

#endif