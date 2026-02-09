//=============================================================================
//
// 物理世界処理 [PhysicsWorld.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "PhysicsWorld.h"
#include "Block.h"
#include "Collider.h"
#include "RigidBody.h"

//=============================================================================
// ヘルパー関数
//=============================================================================
// 線分p1-q1とp2-q2間の最短距離を求める（最近接点も返す）
float PhysicsWorld::DistanceSqSegmentSegment(
    const D3DXVECTOR3& p1, const D3DXVECTOR3& q1,
    const D3DXVECTOR3& p2, const D3DXVECTOR3& q2,
    D3DXVECTOR3* outClosest1, D3DXVECTOR3* outClosest2)
{
    D3DXVECTOR3 d1 = q1 - p1;
    D3DXVECTOR3 d2 = q2 - p2;
    D3DXVECTOR3 r = p1 - p2;
    float a = D3DXVec3LengthSq(&d1);
    float e = D3DXVec3LengthSq(&d2);
    float f = D3DXVec3Dot(&d2, &r);

    float s, t;
    const float EPS = 1e-6f;

    if (a <= EPS && e <= EPS)
    {
        s = t = 0.0f;
        if (outClosest1)
        {
            *outClosest1 = p1;
        }
        if (outClosest2)
        {
            *outClosest2 = p2;
        }

        D3DXVECTOR3 dis = p1 - p2;
        return D3DXVec3LengthSq(&dis);
    }

    if (a <= EPS)
    {
        s = 0.0f;
        t = f / e;
        t = std::clamp(t, 0.0f, 1.0f);
    }
    else
    {
        float c = D3DXVec3Dot(&d1, &r);
        if (e <= EPS)
        {
            t = 0.0f;
            s = -c / a;
            s = std::clamp(s, 0.0f, 1.0f);
        }
        else
        {
            float b = D3DXVec3Dot(&d1, &d2);
            float denom = a * e - b * b;

            if (denom != 0.0f)
            {
                s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            }
            else
            {
                s = 0.0f;
            }

            t = (b * s + f) / e;
            t = std::clamp(t, 0.0f, 1.0f);
            s = (b * t - c) / a;
            s = std::clamp(s, 0.0f, 1.0f);
        }
    }

    if (outClosest1)
    {
        *outClosest1 = p1 + d1 * s;
    }
    if (outClosest2)
    {
        *outClosest2 = p2 + d2 * t;
    }

    D3DXVECTOR3 diff = (p1 + d1 * s) - (p2 + d2 * t);
    return D3DXVec3LengthSq(&diff);
}

//=============================================================================
// 線分上の最近接点
//=============================================================================
D3DXVECTOR3 PhysicsWorld::ClosestPointOnLineSegment(const D3DXVECTOR3& point, const D3DXVECTOR3& a, const D3DXVECTOR3& b)
{
    D3DXVECTOR3 ab = b - a;
    float abLenSq = D3DXVec3LengthSq(&ab);
    if (abLenSq < 1e-6f)
    {
        return a; // 長さほぼ0なら始点帰す
    }

    D3DXVECTOR3 vec = point - a;
    float t = D3DXVec3Dot(&vec, &ab) / abLenSq;
    t = std::max(0.0f, std::min(1.0f, t)); // 0～1 にクランプ

    return a + ab * t;
}
//=============================================================================
// OBBの投影補助関数
//=============================================================================
void PhysicsWorld::ProjectOBB(const D3DXVECTOR3& axis, BoxCollider* obb, float& outMin, float& outMax)
{
    const D3DXVECTOR3& c = obb->GetPosition();
    const D3DXVECTOR3& h = obb->GetScaledSize() * HALF;
    D3DXMATRIX R = obb->GetRotation();

    std::array<D3DXVECTOR3, AXIS> axes =
    {
        D3DXVECTOR3(R._11, R._12, R._13),
        D3DXVECTOR3(R._21, R._22, R._23),
        D3DXVECTOR3(R._31, R._32, R._33)
    };

    float r = h.x * fabs(D3DXVec3Dot(&axis, &axes[0])) +
        h.y * fabs(D3DXVec3Dot(&axis, &axes[1])) +
        h.z * fabs(D3DXVec3Dot(&axis, &axes[2]));

    float cProj = D3DXVec3Dot(&axis, &c);
    outMin = cProj - r;
    outMax = cProj + r;
}
//=============================================================================
// 点をOBBに投影して最近接点を返す
//=============================================================================
D3DXVECTOR3 PhysicsWorld::ClosestPointOnOBB(const D3DXVECTOR3& point, BoxCollider* obb)
{
    D3DXVECTOR3 d = point - obb->GetPosition();
    D3DXVECTOR3 closest = obb->GetPosition();
    D3DXMATRIX R = obb->GetRotation();
    D3DXVECTOR3 half = obb->GetScaledSize() * HALF;

    // 行列
    std::array<D3DXVECTOR3, AXIS> axes =
    {
        D3DXVECTOR3(R._11,R._12,R._13),
        D3DXVECTOR3(R._21,R._22,R._23),
        D3DXVECTOR3(R._31,R._32,R._33)
    };

    for (int nCnt = 0; nCnt < AXIS; nCnt++)
    {
        float dist = D3DXVec3Dot(&d, &axes[nCnt]);
        dist = std::clamp(dist, -half[nCnt], half[nCnt]);
        closest += axes[nCnt] * dist;
    }

    return closest;
}


//=============================================================================
// カプセル vs カプセル判定
//=============================================================================
bool PhysicsWorld::CapsuleCapsuleCollision(CapsuleCollider* a, CapsuleCollider* b, D3DXVECTOR3& outPush)
{
    D3DXVECTOR3 aTop = a->GetPosition() + D3DXVECTOR3(0, a->GetHeight() * HALF, 0);
    D3DXVECTOR3 aBottom = a->GetPosition() - D3DXVECTOR3(0, a->GetHeight() * HALF, 0);
    D3DXVECTOR3 bTop = b->GetPosition() + D3DXVECTOR3(0, b->GetHeight() * HALF, 0);
    D3DXVECTOR3 bBottom = b->GetPosition() - D3DXVECTOR3(0, b->GetHeight() * HALF, 0);

    D3DXVECTOR3 closestA, closestB;
    float distSq = DistanceSqSegmentSegment(aBottom, aTop, bBottom, bTop, &closestA, &closestB);
    float radiusSum = a->GetRadius() + b->GetRadius();

    if (distSq < radiusSum * radiusSum)
    {
        D3DXVECTOR3 dir = closestB - closestA;
        float len = sqrtf(D3DXVec3LengthSq(&dir));
        if (len > 1e-6f)
        {
            outPush = dir * ((radiusSum - len) / len);
        }
        else
        {
            outPush = D3DXVECTOR3(0, radiusSum, 0);
        }

        return true;
    }
    return false;
}

//=============================================================================
// カプセル vs OBB
//=============================================================================
bool PhysicsWorld::CapsuleBoxCollision(CapsuleCollider* cap, BoxCollider* box, D3DXVECTOR3& outPush)
{
    // カプセルの上下点
    D3DXVECTOR3 capTop = cap->GetPosition() + D3DXVECTOR3(0, cap->GetHeight() * HALF, 0);
    D3DXVECTOR3 capBottom = cap->GetPosition() - D3DXVECTOR3(0, cap->GetHeight() * HALF, 0);

    // OBB の最近接点（カプセル中心を基準に仮取得）
    D3DXVECTOR3 closestBox = ClosestPointOnOBB(cap->GetPosition(), box);

    // カプセル線分上で最も OBB 側に近い点を取得
    D3DXVECTOR3 closestCapsule = ClosestPointOnLineSegment(closestBox, capTop, capBottom);

    // 衝突方向
    D3DXVECTOR3 delta = closestCapsule - closestBox;
    float distSq = D3DXVec3LengthSq(&delta);

    // 衝突してたら押し返す
    float radius = cap->GetRadius();
    float radiusSq = radius * radius;

    if (distSq < radiusSq)
    {
        float len = sqrtf(distSq);

        if (len > 1e-6f)
        {
            // 正規化した方向 × (めり込み量)
            D3DXVECTOR3 normal = delta / len;
            outPush = -normal * (radius - len);
        }
        else
        {
            // 衝突点が完全に一致してる場合（沈み込み）
            outPush = D3DXVECTOR3(0, radius, 0);
        }

        return true;
    }

    return false;
}
//=============================================================================
// シリンダー vs Box
//=============================================================================
bool PhysicsWorld::CylinderBoxCollision(CylinderCollider* cyl, BoxCollider* box, D3DXVECTOR3& outPush)
{
    // Cylinder 情報
    D3DXVECTOR3 cylPos = cyl->GetPosition();         // Cylinderの中心
    float radius = cyl->GetRadius();
    float halfHeight = cyl->GetHeight() * HALF;

    // Box 情報（OBB対応）
    D3DXVECTOR3 boxCenter = box->GetPosition();
    const D3DXMATRIX& boxRot = box->GetRotation();
    D3DXVECTOR3 boxHalfExtents = box->GetScaledSize() * HALF;

    // Cylinderの中心をOBBローカルへ変換
    D3DXMATRIX invRot;
    D3DXMatrixTranspose(&invRot, &boxRot);
    D3DXVECTOR3 localPos = cylPos - boxCenter;
    D3DXVec3TransformNormal(&localPos, &localPos, &invRot);

    // まずXY平面（地面平面）での最近接点を計算（高さ判定とは別）
    D3DXVECTOR3 closestLocal = localPos;
    closestLocal.x = std::clamp(closestLocal.x, -boxHalfExtents.x, boxHalfExtents.x);
    closestLocal.z = std::clamp(closestLocal.z, -boxHalfExtents.z, boxHalfExtents.z);

    // 高さ方向の判定（Cylinder軸Yだけ特別扱い）
    if (localPos.y > boxHalfExtents.y + halfHeight || localPos.y < -boxHalfExtents.y - halfHeight)
    {
        return false;  // 高さが完全にはみ出してるなら当たらない
    }

    // ローカル空間からワールドへ戻す
    D3DXVECTOR3 closestWorld = closestLocal;
    D3DXVec3TransformNormal(&closestWorld, &closestWorld, &boxRot);
    closestWorld += boxCenter;

    // CylinderのXZ円とBoxとの衝突判定
    D3DXVECTOR3 delta = cylPos - closestWorld;
    delta.y = 0; // 高さ方向は円判定に含めない
    float distSq = D3DXVec3LengthSq(&delta);

    if (distSq < radius * radius)
    {
        float dist = sqrtf(distSq);

        if (dist > 1e-6f)
        {
            outPush = delta * ((radius - dist) / dist);
        }
        else
        {
            // 真下 or 真上から来た場合
            outPush = D3DXVECTOR3(0, radius, 0);
        }

        return true;
    }

    return false;
}

//=============================================================================
// シリンダー vs カプセル
//=============================================================================
bool PhysicsWorld::CylinderCapsuleCollision(CylinderCollider* cyl, CapsuleCollider* cap, D3DXVECTOR3& outPush)
{
    const D3DXVECTOR3 cylPos = cyl->GetPosition();
    const float cylR = cyl->GetRadius();
    const float cylH = cyl->GetHeight();

    const D3DXVECTOR3 capPos = cap->GetPosition();
    const float capR = cap->GetRadius();
    const float capH = cap->GetHeight();

    // シリンダー線分（上下中心）
    D3DXVECTOR3 cylTop = cylPos + D3DXVECTOR3(0, cylH * HALF, 0);
    D3DXVECTOR3 cylBottom = cylPos - D3DXVECTOR3(0, cylH * HALF, 0);

    // カプセル線分（上下中心）
    D3DXVECTOR3 capTop = capPos + D3DXVECTOR3(0, capH * HALF, 0);
    D3DXVECTOR3 capBottom = capPos - D3DXVECTOR3(0, capH * HALF, 0);

    // 最近接点を取得
    D3DXVECTOR3 closestCyl, closestCap;
    float distSq = DistanceSqSegmentSegment(
        cylBottom, cylTop,
        capBottom, capTop,
        &closestCyl, &closestCap
    );

    float radiusSum = cylR + capR;

    if (distSq < radiusSum * radiusSum)
    {
        // 衝突ベクトル
        D3DXVECTOR3 dir = closestCap - closestCyl;
        float dist = sqrtf(distSq);

        if (dist > 1e-6f)
        {
            D3DXVec3Normalize(&dir, &dir);
        }
        else
        {
            dir = D3DXVECTOR3(1, 0, 0); // 完全重なり時はX方向に押す
        }

        float penetration = radiusSum - dist;
        outPush = dir * penetration;

        return true;
    }

    // 衝突なし
    outPush = INIT_VEC3;
    return false;
}

//=============================================================================
// シリンダー vs シリンダー
//=============================================================================
bool PhysicsWorld::CylinderCylinderCollision(CylinderCollider* a, CylinderCollider* b, D3DXVECTOR3& outPush)
{
    D3DXVECTOR3 delta = b->GetPosition() - a->GetPosition();
    float dist = sqrtf(delta.x * delta.x + delta.z * delta.z);
    float radiusSum = a->GetRadius() + b->GetRadius();
    if (dist < radiusSum)
    {
        D3DXVECTOR3 V = delta * (radiusSum - dist);
        D3DXVec3Normalize(&outPush, &V);
        return true;
    }
    return false;
}

//=============================================================================
// スフィア vs ボックス
//=============================================================================
bool PhysicsWorld::SphereBoxCollision(SphereCollider* sphere, BoxCollider* box, D3DXVECTOR3& outPush)
{
    // 球の中心位置
    const D3DXVECTOR3 spherePos = sphere->GetPosition();
    const float sphereRadius = sphere->GetRadius();

    // OBB（Box）の情報
    const D3DXVECTOR3 boxCenter = box->GetPosition();
    const D3DXMATRIX& boxRot = box->GetRotation();
    const D3DXVECTOR3 boxHalfExtents = box->GetScaledSize() * HALF; // サイズの半分

    // 球の中心をOBBローカル空間に変換
    D3DXMATRIX invRot;
    D3DXMatrixTranspose(&invRot, &boxRot); // 回転行列の転置＝逆行列（正規直交）
    D3DXVECTOR3 localPos = spherePos - boxCenter;
    D3DXVec3TransformNormal(&localPos, &localPos, &invRot);

    // 最近接点（ローカル）
    D3DXVECTOR3 closestLocal = localPos;
    closestLocal.x = std::clamp(closestLocal.x, -boxHalfExtents.x, boxHalfExtents.x);
    closestLocal.y = std::clamp(closestLocal.y, -boxHalfExtents.y, boxHalfExtents.y);
    closestLocal.z = std::clamp(closestLocal.z, -boxHalfExtents.z, boxHalfExtents.z);

    // ワールド空間に戻す
    D3DXVECTOR3 closestWorld = closestLocal;
    D3DXVec3TransformNormal(&closestWorld, &closestWorld, &boxRot);
    closestWorld += boxCenter;

    // 衝突判定
    D3DXVECTOR3 delta = spherePos - closestWorld;
    float distSq = D3DXVec3LengthSq(&delta);

    if (distSq < sphereRadius * sphereRadius)
    {
        float dist = sqrtf(distSq);
        if (dist > 1e-6f)
        {
            outPush = -delta * ((sphereRadius - dist) / dist);
        }
        else
        {
            // 球中心がボックスの中心近くに完全埋まってる場合
            outPush = D3DXVECTOR3(0, sphereRadius, 0);
        }
        return true;
    }

    return false;
}

//=============================================================================
// スフィア vs カプセル
//=============================================================================
bool PhysicsWorld::SphereCapsuleCollision(SphereCollider* sphere, CapsuleCollider* capsule, D3DXVECTOR3& outPush)
{
    // カプセル軸の両端点を取得（ワールド座標）
    D3DXVECTOR3 p1 = capsule->GetPosition() + D3DXVECTOR3(0, capsule->GetHeight() * HALF, 0); // 上側
    D3DXVECTOR3 p2 = capsule->GetPosition() - D3DXVECTOR3(0, capsule->GetHeight() * HALF, 0); // 下側

    // 線分上の最近接点を取得
    D3DXVECTOR3 closest = ClosestPointOnLineSegment(sphere->GetPosition(), p1, p2);

    // 距離を計算
    D3DXVECTOR3 delta = sphere->GetPosition() - closest;
    float distSq = D3DXVec3LengthSq(&delta);

    // 半径合算（スフィア + カプセル半球）
    float radius = sphere->GetRadius() + capsule->GetRadius();

    if (distSq < radius * radius)
    {
        float dist = sqrtf(distSq);
        if (dist > 1e-6f)
        {
            outPush = -delta * ((radius - dist) / dist);
        }
        else
        {
            // 完全に重なった時
            outPush = D3DXVECTOR3(0, radius, 0);
        }
        return true;
    }

    return false;
}

//=============================================================================
// スフィア vs シリンダー（軸方向Y固定）
//=============================================================================
bool PhysicsWorld::SphereCylinderCollision(SphereCollider* sphere, CylinderCollider* cylinder, D3DXVECTOR3& outPush)
{
    D3DXVECTOR3 cylPos = cylinder->GetPosition();
    float halfHeight = cylinder->GetHeight() * 0.5f;

    float clampedY = std::max(cylPos.y - halfHeight, std::min(sphere->GetPosition().y, cylPos.y + halfHeight));
    D3DXVECTOR3 delta = sphere->GetPosition() - D3DXVECTOR3(cylPos.x, clampedY, cylPos.z);

    float distSq = delta.x * delta.x + delta.z * delta.z;
    float radiusSum = sphere->GetRadius() + cylinder->GetRadius();

    if (distSq < radiusSum * radiusSum)
    {
        float dist = sqrtf(distSq);
        if (dist > 1e-6f)
        {
            outPush = -delta * ((radiusSum - dist) / dist);
        }
        else
        {
            outPush = D3DXVECTOR3(radiusSum, 0, 0);
        }

        return true;
    }
    return false;
}

//=============================================================================
// スフィア vs スフィア
//=============================================================================
bool PhysicsWorld::SphereSphereCollision(SphereCollider* s1, SphereCollider* s2, D3DXVECTOR3& outPush)
{
    D3DXVECTOR3 delta = s1->GetPosition() - s2->GetPosition();
    float distSq = D3DXVec3LengthSq(&delta);
    float rSum = s1->GetRadius() + s2->GetRadius();

    if (distSq < rSum * rSum)
    {
        float dist = sqrtf(distSq);
        if (dist > 1e-6f)
        {
            outPush = -delta * ((rSum - dist) / dist);
        }
        else
        {
            outPush = D3DXVECTOR3(rSum, 0, 0); // 完全重なり
        }
        return true;
    }

    return false;
}

//=============================================================================
// OBB vs OBB 衝突判定
//=============================================================================
bool PhysicsWorld::BoxBoxCollision(BoxCollider* a, BoxCollider* b, D3DXVECTOR3& outPush)
{
    std::array<D3DXVECTOR3, 15> axes;

    // aのローカル軸
    D3DXMATRIX Ra = a->GetRotation();
    axes[0] = D3DXVECTOR3(Ra._11, Ra._12, Ra._13);// X軸
    axes[1] = D3DXVECTOR3(Ra._21, Ra._22, Ra._23);// Y軸
    axes[2] = D3DXVECTOR3(Ra._31, Ra._32, Ra._33);// Z軸

    // bのローカル軸
    D3DXMATRIX Rb = b->GetRotation();
    axes[3] = D3DXVECTOR3(Rb._11, Rb._12, Rb._13);// X軸
    axes[4] = D3DXVECTOR3(Rb._21, Rb._22, Rb._23);// Y軸
    axes[5] = D3DXVECTOR3(Rb._31, Rb._32, Rb._33);// Z軸

    // 9つのクロス軸
    int idx = 6;
    for (int nCnt = 0; nCnt < AXIS; nCnt++)
    {
        for (int nCnt2 = 0; nCnt2 < AXIS; nCnt2++)
        {
            D3DXVECTOR3 axis;
            D3DXVec3Cross(&axis, &axes[nCnt], &axes[3 + nCnt2]);

            if (D3DXVec3LengthSq(&axis) > 1e-6f)
            {
                D3DXVec3Normalize(&axis, &axis);
                axes[idx++] = axis;
            }
        }
    }

    float minOverlap = FLT_MAX;
    D3DXVECTOR3 smallestAxis(0, 0, 0);

    for (int nCnt = 0; nCnt < idx; nCnt++)
    {
        float minA, maxA, minB, maxB;
        ProjectOBB(axes[nCnt], a, minA, maxA);
        ProjectOBB(axes[nCnt], b, minB, maxB);

        float overlap = std::min(maxA, maxB) - std::max(minA, minB);

        if (overlap <= 0.0f)
        {
            return false; // 隙間あり → 衝突なし
        }

        if (overlap < minOverlap)
        {
            minOverlap = overlap;
            smallestAxis = axes[nCnt];
        }
    }

    // 押し戻し方向は a→b
    D3DXVECTOR3 dir = b->GetPosition() - a->GetPosition();

    if (D3DXVec3Dot(&dir, &smallestAxis) < 0.0f)
    {
        smallestAxis = -smallestAxis;
    }

    outPush = smallestAxis * minOverlap;
    return true;
}

//=============================================================================
// 2体の簡易AABB押し戻し
//=============================================================================
bool PhysicsWorld::CheckCollision(RigidBody* a, RigidBody* b, D3DXVECTOR3& outPush)
{
    auto colA = a->GetCollider();
    auto colB = b->GetCollider();

    if (colA->GetType() == Collider::BOX && colB->GetType() == Collider::BOX)
    {
        return BoxBoxCollision(static_cast<BoxCollider*>(colA.get()), static_cast<BoxCollider*>(colB.get()), outPush);
    }
    else if (colA->GetType() == Collider::CAPSULE && colB->GetType() == Collider::BOX)
    {
        return CapsuleBoxCollision(static_cast<CapsuleCollider*>(colA.get()), static_cast<BoxCollider*>(colB.get()), outPush);
    }
    else if (colA->GetType() == Collider::BOX && colB->GetType() == Collider::CAPSULE)
    {
        bool collided = CapsuleBoxCollision(static_cast<CapsuleCollider*>(colB.get()), static_cast<BoxCollider*>(colA.get()), outPush);
        outPush = -outPush;
        return collided;
    }
    else if (colA->GetType() == Collider::CAPSULE && colB->GetType() == Collider::CAPSULE)
    {
        return CapsuleCapsuleCollision(static_cast<CapsuleCollider*>(colA.get()), static_cast<CapsuleCollider*>(colB.get()), outPush);
    }
    else if (colA->GetType() == Collider::CYLINDER && colB->GetType() == Collider::BOX)
    {
        return CylinderBoxCollision(static_cast<CylinderCollider*>(colA.get()), static_cast<BoxCollider*>(colB.get()), outPush);
    }
    else if (colA->GetType() == Collider::BOX && colB->GetType() == Collider::CYLINDER)
    {
        bool collided = CylinderBoxCollision(static_cast<CylinderCollider*>(colB.get()), static_cast<BoxCollider*>(colA.get()), outPush);
        outPush = -outPush;
        return collided;
    }
    else if (colA->GetType() == Collider::CYLINDER && colB->GetType() == Collider::CAPSULE)
    {
        return CylinderCapsuleCollision(static_cast<CylinderCollider*>(colA.get()), static_cast<CapsuleCollider*>(colB.get()), outPush);
    }
    else if (colA->GetType() == Collider::CAPSULE && colB->GetType() == Collider::CYLINDER)
    {
        bool collided = CylinderCapsuleCollision(static_cast<CylinderCollider*>(colB.get()), static_cast<CapsuleCollider*>(colA.get()), outPush);
        outPush = -outPush;
        return collided;
    }
    else if (colA->GetType() == Collider::CYLINDER && colB->GetType() == Collider::CYLINDER)
    {
        return CylinderCylinderCollision(static_cast<CylinderCollider*>(colA.get()), static_cast<CylinderCollider*>(colB.get()), outPush);
    }
    else if (colA->GetType() == Collider::SPHERE && colB->GetType() == Collider::BOX)
    {
        return SphereBoxCollision(static_cast<SphereCollider*>(colA.get()), static_cast<BoxCollider*>(colB.get()), outPush);
    }
    else if (colA->GetType() == Collider::BOX && colB->GetType() == Collider::SPHERE)
    {
        bool collided = SphereBoxCollision(static_cast<SphereCollider*>(colB.get()), static_cast<BoxCollider*>(colA.get()), outPush);
        outPush = -outPush;
        return collided;
    }
    else if (colA->GetType() == Collider::SPHERE && colB->GetType() == Collider::CAPSULE)
    {
        return SphereCapsuleCollision(static_cast<SphereCollider*>(colA.get()), static_cast<CapsuleCollider*>(colB.get()), outPush);
    }
    else if (colA->GetType() == Collider::CAPSULE && colB->GetType() == Collider::SPHERE)
    {
        bool collided = SphereCapsuleCollision(static_cast<SphereCollider*>(colB.get()), static_cast<CapsuleCollider*>(colA.get()), outPush);
        outPush = -outPush;
        return collided;
    }
    else if (colA->GetType() == Collider::SPHERE && colB->GetType() == Collider::CYLINDER)
    {
        return SphereCylinderCollision(static_cast<SphereCollider*>(colA.get()), static_cast<CylinderCollider*>(colB.get()), outPush);
    }
    else if (colA->GetType() == Collider::CYLINDER && colB->GetType() == Collider::SPHERE)
    {
        bool collided = SphereCylinderCollision(static_cast<SphereCollider*>(colB.get()), static_cast<CylinderCollider*>(colA.get()), outPush);
        outPush = -outPush;
        return collided;
    }
    else if (colA->GetType() == Collider::SPHERE && colB->GetType() == Collider::SPHERE)
    {
        return SphereSphereCollision(static_cast<SphereCollider*>(colA.get()), static_cast<SphereCollider*>(colB.get()), outPush);
    }

    return false;
}
//=============================================================================
// 接触点の取得
//=============================================================================
D3DXVECTOR3 PhysicsWorld::GetActualCollisionPoint(RigidBody* a, RigidBody* b, const D3DXVECTOR3& push)
{
    // ブロックの中心(位置)とハーフサイズを取得
    D3DXVECTOR3 posA = a->GetPosition();
    D3DXVECTOR3 posB = b->GetPosition();
    D3DXVECTOR3 halfA = a->GetScale() * HALF;
    D3DXVECTOR3 halfB = b->GetScale() * HALF;

    // 衝突面に近い中心点
    D3DXVECTOR3 contactA = posA;
    contactA.x += push.x > 0 ? halfA.x : -halfA.x;
    contactA.y += push.y > 0 ? halfA.y : -halfA.y;
    contactA.z += push.z > 0 ? halfA.z : -halfA.z;

    D3DXVECTOR3 contactB = posB;
    contactB.x += push.x > 0 ? -halfB.x : halfB.x;
    contactB.y += push.y > 0 ? -halfB.y : halfB.y;
    contactB.z += push.z > 0 ? -halfB.z : halfB.z;

    // 中点を返す
    return (contactA + contactB) * HALF;
}
//=============================================================================
// 端乗り判定関数
//=============================================================================
bool PhysicsWorld::IsUnstableStack(RigidBody* A, RigidBody* B)
{
    D3DXVECTOR3 posA = A->GetPosition();
    D3DXVECTOR3 posB = B->GetPosition();
    D3DXVECTOR3 diff = posA - posB;

    D3DXVECTOR3 halfA = A->GetScale() * HALF;
    D3DXVECTOR3 halfB = B->GetScale() * HALF;

    float limitX = (halfA.x + halfB.x) * 0.6f; // ズレ閾値
    float limitZ = (halfA.z + halfB.z) * 0.6f;

    return (fabsf(diff.x) > limitX || fabsf(diff.z) > limitZ);
}
//=============================================================================
// 当たり判定シミュレーション
//=============================================================================
void PhysicsWorld::StepSimulation(float dt)
{
    // 移動反映
    for (auto& body : m_Bodies)
    {
        body->SetOnGround(false);

        if (body->IsDynamic())
        {
            body->Integrate(dt, m_Gravity);
        }
    }

    // 衝突解決の反復
    for (int iter = 0; iter < ITERATIONS; iter++)
    {
        for (size_t nCnt = 0; nCnt < m_Bodies.size(); nCnt++)
        {
            for (size_t nCnt2 = nCnt + 1; nCnt2 < m_Bodies.size(); nCnt2++)
            {
                RigidBody* A = m_Bodies[nCnt].get();
                RigidBody* B = m_Bodies[nCnt2].get();
                D3DXVECTOR3 push;

                if (CheckCollision(A, B, push))
                {
                    // 衝突点・法線を求める簡易版
                    D3DXVECTOR3 normal = INIT_VEC3;
                    D3DXVec3Normalize(&normal, &push);

                    // ここで接地判定
                    if (normal.y > 0.7f)
                    {
                        if (A->IsDynamic()) A->SetOnGround(true);
                        if (B->IsDynamic()) B->SetOnGround(true);
                    }

                    // 相対速度
                    D3DXVECTOR3 relVel = B->GetVelocity() - A->GetVelocity();
                    float velAlongNormal = D3DXVec3Dot(&relVel, &normal);

                    // 衝突反発（インパルス）
                    float e = std::min(A->GetRestitution(), B->GetRestitution());
                    float t = -(1 + e) * velAlongNormal / (1 / A->GetMass() + 1 / B->GetMass());

                    D3DXVECTOR3 impulse = normal * t;

                    D3DXVECTOR3 contactPoint = GetActualCollisionPoint(A, B, push);
                    D3DXVECTOR3 relPosA = contactPoint - A->GetPosition();
                    D3DXVECTOR3 relPosB = contactPoint - B->GetPosition();

                    A->ApplyImpulse(-impulse, relPosA/*D3DXVECTOR3(0, 0, 0)*/);
                    B->ApplyImpulse(impulse, relPosB/*D3DXVECTOR3(0, 0, 0)*/);

                    // 位置補正
                    if (A->IsDynamic() && B->IsDynamic())
                    {
                        // 両方ダイナミックなら半分ずつ移動
                        A->SetTransform(A->GetPosition() - push * 0.5f, A->GetOrientation(), A->GetScale());
                        B->SetTransform(B->GetPosition() + push * 0.5f, B->GetOrientation(), B->GetScale());
                    }
                    else if (A->IsDynamic() && !B->IsDynamic())
                    {
                        // B が静的なら A だけ動かす
                        A->SetTransform(A->GetPosition() - push, A->GetOrientation(), A->GetScale());
                    }
                    else if (!A->IsDynamic() && B->IsDynamic())
                    {
                        // A が静的なら B だけ動かす
                        B->SetTransform(B->GetPosition() + push, B->GetOrientation(), B->GetScale());
                    }
                }
            }
        }
    }

    // 最終補正
    for (auto& body : m_Bodies)
    {
        if (!body->IsDynamic())
        {
            continue;
        }

        if (body->IsOnGround())
        {
            D3DXVECTOR3 vel = body->GetVelocity();
            if (vel.y < 0)
            {
                vel.y = 0; // 下方向を消す
            }

            body->SetVelocity(vel);
        }
    }
}
//=============================================================================
// 剛体の削除
//=============================================================================
void PhysicsWorld::RemoveRigidBody(std::shared_ptr<RigidBody> body)
{
    if (!body)
    {
        return;
    }

    auto it = std::find(m_Bodies.begin(), m_Bodies.end(), body);
    if (it != m_Bodies.end())
    {
        m_Bodies.erase(it);
    }
}
