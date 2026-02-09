//=============================================================================
//
// 当たり判定処理 [collisionUtils.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "collisionUtils.h"

// 名前空間の使用
using namespace std;

//=============================================================================
// シリンダーとAABBの当たり判定(その距離を返す)
//=============================================================================
bool CCollision::CheckCylinderAABBCollisionWithHitDistance(
	const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight, const D3DXVECTOR3& cylinderDir,
	const D3DXVECTOR3& aabbMin, const D3DXVECTOR3& aabbMax,
	float* outHitDistance)
{
	// シリンダー軸方向のベクトル
	D3DXVECTOR3 axis = cylinderDir;

	// AABBの8頂点を取得
	D3DXVECTOR3 verts[8] =
	{
		{aabbMin.x, aabbMin.y, aabbMin.z},
		{aabbMax.x, aabbMin.y, aabbMin.z},
		{aabbMin.x, aabbMax.y, aabbMin.z},
		{aabbMax.x, aabbMax.y, aabbMin.z},
		{aabbMin.x, aabbMin.y, aabbMax.z},
		{aabbMax.x, aabbMin.y, aabbMax.z},
		{aabbMin.x, aabbMax.y, aabbMax.z},
		{aabbMax.x, aabbMax.y, aabbMax.z},
	};

	// 頂点群を軸方向に射影して最小最大を求める（軸方向の範囲）
	float minProj = FLT_MAX;
	float maxProj = -FLT_MAX;
	for (int i = 0; i < 8; i++)
	{
		D3DXVECTOR3 v = verts[i] - cylinderCenter;
		float proj = D3DXVec3Dot(&v, &axis);
		if (proj < minProj) minProj = proj;
		if (proj > maxProj) maxProj = proj;
	}

	// 軸方向の距離判定
	float halfHeight = cylinderHeight * 0.5f;

	if (maxProj < -halfHeight || minProj > halfHeight)
	{
		// 軸方向で重なりなし
		return false;
	}

	// 軸方向に被っている部分の距離をclampして判定軸上の点を取得
	float clampProj = std::max(-halfHeight, std::min(0.5f * (minProj + maxProj), halfHeight));
	D3DXVECTOR3 closestPointOnAxis = cylinderCenter + axis * clampProj;

	// 軸に垂直な2つのベクトルを計算（シリンダー軸に垂直）
	D3DXVECTOR3 up = axis;
	D3DXVECTOR3 right, forward;

	// axisに直交するベクトルを1つ決める
	if (fabs(up.x) < 0.9f)
	{
		right = D3DXVECTOR3(1, 0, 0);
	}
	else
	{
		right = D3DXVECTOR3(0, 1, 0);
	}

	D3DXVec3Cross(&right, &right, &up);
	D3DXVec3Normalize(&right, &right);

	D3DXVec3Cross(&forward, &up, &right);
	D3DXVec3Normalize(&forward, &forward);

	// AABBの頂点を断面座標系に変換して、最小最大を求める（2D長方形の境界）
	float minX = FLT_MAX, maxX = -FLT_MAX;
	float minY = FLT_MAX, maxY = -FLT_MAX;
	for (int i = 0; i < 8; i++)
	{
		D3DXVECTOR3 v = verts[i] - closestPointOnAxis;
		float x = D3DXVec3Dot(&v, &right);
		float y = D3DXVec3Dot(&v, &forward);
		if (x < minX) minX = x;
		if (x > maxX) maxX = x;
		if (y < minY) minY = y;
		if (y > maxY) maxY = y;
	}

	// 矩形の最近接点を円の中心に投影（クランプ）
	float closestX = std::max(minX, std::min(0.0f, maxX));
	float closestY = std::max(minY, std::min(0.0f, maxY));

	// 中心からの距離
	float distX = closestX;
	float distY = closestY;

	float distSq = distX * distX + distY * distY;

	if (distSq <= cylinderRadius * cylinderRadius)
	{
		if (outHitDistance)
		{
			// 発射位置から求める
			D3DXVECTOR3 startPos = cylinderCenter + axis * halfHeight;	// 後端
			D3DXVECTOR3 hitPos = cylinderCenter + axis * clampProj;		// 当たった位置
			D3DXVECTOR3 disPos = hitPos - startPos;
			*outHitDistance = D3DXVec3Length(&disPos);
		}

		// 当たり判定あり
		return true;
	}

	return false;
}
//=============================================================================
// シリンダーとカプセルの当たり判定
//=============================================================================
bool CCollision::CheckCapsuleCylinderCollision_Dir(
	const D3DXVECTOR3& capsuleCenter, float capsuleRadius, float capsuleHeight,
	const D3DXVECTOR3& cylinderCenter, float cylinderRadius, float cylinderHeight,
	const D3DXVECTOR3& cylinderDir, bool flag)
{

	float collisionCylinderRadius;
	float collisionCapsuleRadius;

	// flagは使わなければfalseでいい
	if (flag)
	{
		// ==== 半径を縮小 ====
		collisionCylinderRadius = cylinderRadius * 0.40f; // 円柱は50%
		collisionCapsuleRadius = capsuleRadius * 0.85f;   // カプセルは90%
	}
	else
	{
		collisionCylinderRadius = cylinderRadius;
		collisionCapsuleRadius = capsuleRadius;
	}

	// ベクトル差
	D3DXVECTOR3 delta = capsuleCenter - cylinderCenter;

	// 軸方向に射影
	float proj = D3DXVec3Dot(&delta, &cylinderDir);

	// 軸上距離判定
	float halfHeightSum = (capsuleHeight * 0.5f) + (cylinderHeight * 0.5f);
	float axisDist = 0.0f;

	if (proj > halfHeightSum)
	{
		axisDist = proj - halfHeightSum;
	}
	else if (proj < -halfHeightSum)
	{
		axisDist = -halfHeightSum - proj;
	}
	else
	{
		axisDist = 0.0f; // 軸上は重なっている
	}

	// 軸直交面の距離
	D3DXVECTOR3 closestPointOnAxis = cylinderCenter + cylinderDir * proj;
	D3DXVECTOR3 diff = capsuleCenter - closestPointOnAxis;
	float radialDist = D3DXVec3Length(&diff);

	// ==== 判定 ====
	if (axisDist <= collisionCapsuleRadius + collisionCylinderRadius &&
		radialDist <= collisionCapsuleRadius + collisionCylinderRadius)
	{
		return true;
	}

	return false;
}
//=============================================================================
// AABB同士の当たり判定
//=============================================================================
bool CCollision::CheckCollisionAABB(D3DXVECTOR3 pos1, D3DXVECTOR3 modelSize1, D3DXVECTOR3 scale1, 
	D3DXVECTOR3 pos2, D3DXVECTOR3 modelSize2, D3DXVECTOR3 scale2)
{
	//=========================================================================
	// ブロック1の情報
	//=========================================================================

	// AABBを取得
	D3DXVECTOR3 Pos_1 = pos1;				// ブロックの位置
	D3DXVECTOR3 ModelSize_1 = modelSize1;	// ブロックの元のサイズ（中心原点）
	D3DXVECTOR3 Scale_1 = scale1;			// ブロックの拡大率

	// 元サイズに拡大率を適用する
	D3DXVECTOR3 Size_1;
	Size_1.x = ModelSize_1.x * Scale_1.x;
	Size_1.y = ModelSize_1.y * Scale_1.y;
	Size_1.z = ModelSize_1.z * Scale_1.z;

	// 最小値と最大値を求める
	D3DXVECTOR3 Min_1 = Pos_1 - Size_1 * 0.5f;
	D3DXVECTOR3 Max_1 = Pos_1 + Size_1 * 0.5f;

	//=========================================================================
	// ブロック2の情報
	//=========================================================================

	// AABBを取得
	D3DXVECTOR3 Pos_2 = pos2;				// ブロックの位置
	D3DXVECTOR3 ModelSize_2 = modelSize2;	// ブロックの元のサイズ（中心原点）
	D3DXVECTOR3 Scale_2 = scale2;			// ブロックの拡大率

	// 元サイズに拡大率を適用する
	D3DXVECTOR3 Size_2;
	Size_2.x = ModelSize_2.x * Scale_2.x;
	Size_2.y = ModelSize_2.y * Scale_2.y;
	Size_2.z = ModelSize_2.z * Scale_2.z;

	// 最小値と最大値を求める
	D3DXVECTOR3 Min_2 = Pos_2 - Size_2 * 0.5f;
	D3DXVECTOR3 Max_2 = Pos_2 + Size_2 * 0.5f;

	//=========================================================================
	// AABB同士の交差チェック
	//=========================================================================
	bool isOverlap =
		Min_1.x <= Max_2.x && Max_1.x >= Min_2.x &&
		Min_1.y <= Max_2.y && Max_1.y >= Min_2.y &&
		Min_1.z <= Max_2.z && Max_1.z >= Min_2.z;

	if (isOverlap)
	{// 交差している
		return true;
	}

	return false;
}
//=============================================================================
// AABBとカプセルの当たり判定(カプセルをAABB変換)
//=============================================================================
bool CCollision::CheckCapsuleAABBCollision(D3DXVECTOR3 pos, D3DXVECTOR3 modelSize, D3DXVECTOR3 scale, 
	D3DXVECTOR3 capsulePos, float Radius, float Height)
{
	//=========================================================================
	// ブロック1の情報
	//=========================================================================

	// AABB を取得
	D3DXVECTOR3 Pos = pos;				// ブロックの位置
	D3DXVECTOR3 ModelSize = modelSize;	// ブロックの元のサイズ（中心原点）
	D3DXVECTOR3 Scale = scale;			// ブロックの拡大率

	// 元サイズに拡大率を適用する
	D3DXVECTOR3 Size;
	Size.x = modelSize.x * scale.x;
	Size.y = modelSize.y * scale.y;
	Size.z = modelSize.z * scale.z;

	D3DXVECTOR3 Min = Pos - Size * 0.5f;
	D3DXVECTOR3 Max = Pos + Size * 0.5f;

	//=========================================================================
	// カプセルコライダーの情報
	//=========================================================================

	// カプセルコライダーのサイズからAABBサイズを計算
	D3DXVECTOR3 pSize;
	pSize.x = Radius * 2.0f;
	pSize.z = Radius * 2.0f;
	pSize.y = Height + Radius * 2.0f;

	// 最小値と最大値を求める
	D3DXVECTOR3 pMin = capsulePos - pSize * 0.5f;
	D3DXVECTOR3 pMax = capsulePos + pSize * 0.5f;

	//=========================================================================
	// 交差チェック
	//=========================================================================
	bool isOverlap =
		Min.x <= pMax.x && Max.x >= pMin.x &&
		Min.y <= pMax.y && Max.y >= pMin.y &&
		Min.z <= pMax.z && Max.z >= pMin.z;

	if (isOverlap)
	{// 交差している
		return true;
	}

	return false;
}
//=============================================================================
// OBB同士の当たり判定
//=============================================================================
bool CCollision::CheckCollisionOBB(const CBoxCollider* obb1, const CBoxCollider* obb2)
{
	// 各OBBの回転行列
	const D3DXMATRIX& R1 = obb1->GetRotation();
	const D3DXMATRIX& R2 = obb2->GetRotation();

	D3DXVECTOR3 Axis1[3] =
	{
		{R1._11,R1._12,R1._13},// X軸
		{R1._21,R1._22,R1._23},// Y軸
		{R1._31,R1._32,R1._33},// Z軸
	};

	D3DXVECTOR3 Axis2[3] =
	{
		{R2._11,R2._12,R2._13},// X軸
		{R2._21,R2._22,R2._23},// Y軸
		{R2._31,R2._32,R2._33},// Z軸
	};

	// 15個の軸でチェック (3+3+9)
	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		if (!OverlapOnAxis(obb1, obb2, Axis1[nCnt])) return false;
		if (!OverlapOnAxis(obb1, obb2, Axis2[nCnt])) return false;
	}

	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		for (int nCnt2 = 0; nCnt2 < 3; nCnt2++)
		{
			D3DXVECTOR3 axis;
			D3DXVec3Cross(&axis, &Axis1[nCnt], &Axis2[nCnt2]);
			if (D3DXVec3LengthSq(&axis) > 1e-6f) // ほぼゼロベクトルは除外
			{
				if (!OverlapOnAxis(obb1, obb2, axis))
				{
					return false;
				}
			}
		}
	}

	return true; // 全て重なっている
}
//=============================================================================
// OBBとカプセルの当たり判定
//=============================================================================
bool CCollision::CheckCapsuleOBBCollision(const CCapsuleCollider* capsule, const CBoxCollider* obb)
{
	// カプセル中心線の端点（ワールド座標）
	D3DXVECTOR3 capCenter = capsule->GetPosition();
	float h = capsule->GetHeight() * 0.5f;

	D3DXVECTOR3 capA = capCenter + D3DXVECTOR3(0, h, 0);
	D3DXVECTOR3 capB = capCenter - D3DXVECTOR3(0, h, 0);

	// OBBのローカル空間へ変換
	D3DXMATRIX invRot;
	D3DXMatrixTranspose(&invRot, &obb->GetRotation()); // 回転行列の逆 = 転置
	D3DXVECTOR3 obbCenter = obb->GetPosition();
	D3DXVECTOR3 halfSize = obb->GetSize() * 0.5f;

	D3DXVECTOR3 localA = capA - obbCenter;
	D3DXVECTOR3 localB = capB - obbCenter;
	D3DXVec3TransformNormal(&localA, &localA, &invRot);
	D3DXVec3TransformNormal(&localB, &localB, &invRot);

	// カプセル線分とAABBの最近接点を求める
	D3DXVECTOR3 closest = ClosestPointSegmentAABB(localA, localB, -halfSize, halfSize);

	// 最近接点とカプセル線分の距離
	float distSq = DistanceSqPointSegment(closest, localA, localB);

	return distSq <= (capsule->GetRadius() * capsule->GetRadius());
}


//=============================================================================
// 点と線分の最近接距離を返す関数
//=============================================================================
float CCollision::DistanceSqPointSegment(const D3DXVECTOR3& p, const D3DXVECTOR3& a, const D3DXVECTOR3& b)
{
	D3DXVECTOR3 ab = b - a;
	D3DXVECTOR3 ap = p - a;
	float t = D3DXVec3Dot(&ap, &ab) / D3DXVec3Dot(&ab, &ab);
	t = max(0.0f, min(1.0f, t));
	D3DXVECTOR3 closest = a + ab * t;
	D3DXVECTOR3 dis = p - closest;
	return D3DXVec3LengthSq(&dis);
}
//=============================================================================
// 線分とAABBの最近接点
//=============================================================================
D3DXVECTOR3 CCollision::ClosestPointSegmentAABB(const D3DXVECTOR3& a, const D3DXVECTOR3& b,
	const D3DXVECTOR3& minVal, const D3DXVECTOR3& maxVal)
{
	// 線分の中点を取って、AABBにクランプする(簡易)
	D3DXVECTOR3 mid = (a + b) * 0.5f;
	D3DXVECTOR3 result = mid;
	result.x = max(minVal.x, min(result.x, maxVal.x));
	result.y = max(minVal.y, min(result.y, maxVal.y));
	result.z = max(minVal.z, min(result.z, maxVal.z));
	return result;
}
//=============================================================================
// OBB を axis に投影して min/max を返す関数
//=============================================================================
void CCollision::ProjectOBB(const CBoxCollider* obb, const D3DXVECTOR3& axis,
	float& outMin, float& outMax)
{
	// 正規化
	D3DXVECTOR3 nAxis;
	D3DXVec3Normalize(&nAxis, &axis);

	// OBB情報
	D3DXVECTOR3 center = obb->GetPosition();
	D3DXVECTOR3 half = obb->GetSize() * 0.5f;
	const D3DXMATRIX& R = obb->GetRotation();

	// ローカル軸を取り出す
	D3DXVECTOR3 obbAxis[3] =
	{
		{ R._11, R._12, R._13 },// X軸
		{ R._21, R._22, R._23 },// Y軸
		{ R._31, R._32, R._33 }	// Z軸
	};

	// 中心を投影
	float centerProj = D3DXVec3Dot(&center, &nAxis);

	// 各軸方向の寄与 = (半径 * cosθ)
	float r = 0.0f;
	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		float e = D3DXVec3Dot(&obbAxis[nCnt], &nAxis);
		r += fabsf(e) * half[nCnt]; // half[i] = (x, y, z)
	}

	outMin = centerProj - r;
	outMax = centerProj + r;
}

//=============================================================================
// 2つのOBBが指定軸上で重なるか判定する関数
//=============================================================================
bool CCollision::OverlapOnAxis(const CBoxCollider* obb1, const CBoxCollider* obb2,
	const D3DXVECTOR3& axis)
{
	float min1, max1;
	float min2, max2;

	ProjectOBB(obb1, axis, min1, max1);
	ProjectOBB(obb2, axis, min2, max2);

	return !(max1 < min2 || max2 < min1);
}
