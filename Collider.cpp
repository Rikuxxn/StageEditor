//=============================================================================
//
// コライダー処理 [Collider.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Collider.h"


//=============================================================================
// ボックスコライダー(OBB)のトランスフォーム処理
//=============================================================================
void BoxCollider::UpdateTransform(const D3DXVECTOR3& pos, const D3DXQUATERNION& rot, const D3DXVECTOR3& scale)
{
    // 実サイズ = 元サイズ × スケール
    m_ScaledSize.x = m_Size.x * scale.x;
    m_ScaledSize.y = m_Size.y * scale.y;
    m_ScaledSize.z = m_Size.z * scale.z;

    // ワールド位置
    m_Position = pos;
    m_RotationQuat = rot;

    // 回転行列
    D3DXMatrixRotationQuaternion(&m_Rotation, &rot);
}
//=============================================================================
// ボックスコライダー(OBB)の慣性計算処理
//=============================================================================
void BoxCollider::calculateLocalInertia(float mass, D3DXVECTOR3& inertia) const
{
    if (mass <= 0.0f)
    {
        inertia = INIT_VEC3;
        return;
    }

    D3DXVECTOR3 s = m_ScaledSize;
    inertia.x = (1.0f / 12.0f) * mass * (s.y * s.y + s.z * s.z);
    inertia.y = (1.0f / 12.0f) * mass * (s.x * s.x + s.z * s.z);
    inertia.z = (1.0f / 12.0f) * mass * (s.x * s.x + s.y * s.y);
}


//=============================================================================
// カプセルコライダーの上点取得処理
//=============================================================================
D3DXVECTOR3 CapsuleCollider::GetTop(void) const
{
    // 位置の取得
    D3DXVECTOR3 pos = GetPosition();

    return pos + D3DXVECTOR3(0, m_Height * HALF * m_Scale.y, 0);
}
//=============================================================================
// カプセルコライダーの下点取得処理
//=============================================================================
D3DXVECTOR3 CapsuleCollider::GetBottom(void) const
{
    // 位置の取得
    D3DXVECTOR3 pos = GetPosition();

    return pos - D3DXVECTOR3(0, m_Height * HALF * m_Scale.y, 0);
}
//=============================================================================
// カプセルコライダーのトランスフォーム処理
//=============================================================================
void CapsuleCollider::UpdateTransform(const D3DXVECTOR3& pos, const D3DXQUATERNION& rot, const D3DXVECTOR3& scale)
{
    m_Position = pos;
    m_Scale = scale;  // 半径・高さにスケールをかけるときに利用
    m_Rotation = rot;
}
//=============================================================================
// カプセルコライダーの慣性計算処理
//=============================================================================
void CapsuleCollider::calculateLocalInertia(float mass, D3DXVECTOR3& inertia) const
{
    float r = m_Radius;
    float h = m_Height;
    inertia.y = HALF * mass * r * r;
    inertia.x = inertia.z = (1.0f / 12.0f) * mass * (3.0f * r * r + h * h);
}


//=============================================================================
// シリンダーコライダーのトランスフォーム処理
//=============================================================================
void CylinderCollider::UpdateTransform(const D3DXVECTOR3& pos, const D3DXQUATERNION& rot, const D3DXVECTOR3& scale)
{
    m_Position = pos;

    m_RadiusScaled = m_Radius * (scale.x + scale.z) * HALF;
    m_HeightScaled = m_Height * scale.y;

    m_RotationQuat = rot;
    D3DXMatrixRotationQuaternion(&m_Rotation, &rot);
}
//=============================================================================
// シリンダーコライダーの慣性計算処理
//=============================================================================
void CylinderCollider::calculateLocalInertia(float mass, D3DXVECTOR3& inertia) const
{
    float r = m_RadiusScaled;
    float h = m_HeightScaled;

    inertia.y = 0.5f * mass * r * r;          // 軸方向
    inertia.x = inertia.z = (1.0f / 12.0f) * mass * (3.0f * r * r + h * h); // 横
}


//=============================================================================
// スフィアコライダーのトランスフォーム処理
//=============================================================================
void SphereCollider::UpdateTransform(const D3DXVECTOR3& pos, const D3DXQUATERNION& rot, const D3DXVECTOR3& scale)
{
    m_Position = pos;

    // スケール反映
    float s = std::max({ scale.x, scale.y, scale.z });
    m_ScaledRadius = m_Radius * s;

    m_RotationQuat = rot;
    D3DXMatrixRotationQuaternion(&m_Rotation, &rot);
}
//=============================================================================
// スフィアコライダーの慣性計算処理
//=============================================================================
void SphereCollider::calculateLocalInertia(float mass, D3DXVECTOR3& inertia) const
{
    if (mass <= 0.0f)
    {
        inertia = INIT_VEC3;
        return;
    }

    // 球の慣性モーメント I = 2/5 * m * r^2
    float I = (2.0f / 5.0f) * mass * m_ScaledRadius * m_ScaledRadius;
    inertia = D3DXVECTOR3(I, I, I);
}
