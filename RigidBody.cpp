//=============================================================================
//
// 剛体処理 [RigidBody.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "RigidBody.h"
#include "Collider.h"

//=============================================================================
// コンストラクタ
//=============================================================================
RigidBody::RigidBody(std::shared_ptr<Collider> col, float mass)
    : m_Collider(col), m_Mass(mass), m_Velocity(0, 0, 0),
    m_Friction(0.1f), m_RollingFriction(0.1f),
    m_LinearFactor(1, 1, 1), m_AngularFactor(1, 1, 1),
    m_Restitution(0.1f)
{
    m_onGround = false;
    m_AccumulatedForce = INIT_VEC3;
    m_AccumulatedTorque = INIT_VEC3;
    D3DXQuaternionIdentity(&m_Orientation);
    m_Scale = D3DXVECTOR3(1, 1, 1);
    m_AngularVelocity = D3DXVECTOR3(0, 0, 0);
    m_Inertia = D3DXVECTOR3(1, 1, 1);

    if (m_Collider)
    {
        m_Collider->calculateLocalInertia(m_Mass, m_Inertia);

        m_InertiaInv.x = (m_Inertia.x != 0) ? 1.0f / m_Inertia.x : 0.0f;
        m_InertiaInv.y = (m_Inertia.y != 0) ? 1.0f / m_Inertia.y : 0.0f;
        m_InertiaInv.z = (m_Inertia.z != 0) ? 1.0f / m_Inertia.z : 0.0f;
    }
}
//=============================================================================
// 重力適用処理
//=============================================================================
void RigidBody::ApplyGravity(float dt, D3DXVECTOR3 gravity)
{
    if (m_isDynamic && !m_onGround)
    {
        m_Velocity += gravity * m_Mass * dt;
    }
}
//=============================================================================
// 外力の適用
//=============================================================================
void RigidBody::ApplyForce(const D3DXVECTOR3& force)
{
    if (!m_isDynamic)
    {
        return;
    }

    m_AccumulatedForce += force;
}
//=============================================================================
// 衝突点の適用処理
//=============================================================================
void RigidBody::ApplyForceAtPoint(const D3DXVECTOR3& force, const D3DXVECTOR3& point)
{
    if (!m_isDynamic)
    {
        return;
    }

    m_AccumulatedForce += force;
    D3DXVECTOR3 r = point - m_Position;
    D3DXVECTOR3 torque = INIT_VEC3;
    D3DXVec3Cross(&torque, &r, &force);

    m_AccumulatedTorque += torque;
}
//=============================================================================
// インパルスの適用
//=============================================================================
void RigidBody::ApplyImpulse(const D3DXVECTOR3& impulse, const D3DXVECTOR3& relPos)
{
    if (!m_isDynamic)
    {
        return;
    }

    m_Velocity += impulse / m_Mass;

    D3DXVECTOR3 angImpulse = INIT_VEC3;
    D3DXVec3Cross(&angImpulse, &relPos, &impulse);

    m_AngularVelocity += D3DXVECTOR3(
        angImpulse.x * m_InertiaInv.x,
        angImpulse.y * m_InertiaInv.y,
        angImpulse.z * m_InertiaInv.z
    );
}
//=============================================================================
// 更新処理
//=============================================================================
void RigidBody::Integrate(float dt, const D3DXVECTOR3& gravity)
{
    if (!m_isDynamic)
    {
        return;
    }

    // 線形速度更新
    ApplyGravity(dt, gravity);

    m_Position += m_Velocity * dt;

    // 角速度更新
    D3DXVECTOR3 angAcc = D3DXVECTOR3(
        m_AccumulatedTorque.x * m_InertiaInv.x,
        m_AccumulatedTorque.y * m_InertiaInv.y,
        m_AccumulatedTorque.z * m_InertiaInv.z
    );

    m_AngularVelocity += angAcc * dt;

    // Quaternion更新
    if (D3DXVec3LengthSq(&m_AngularVelocity) > 1e-6f)
    {
        D3DXQUATERNION omega(m_AngularVelocity.x, m_AngularVelocity.y, m_AngularVelocity.z, 0);
        D3DXQUATERNION dq;
        D3DXQuaternionMultiply(&dq, &omega, &m_Orientation);

        dq.x *= 0.5f * dt;
        dq.y *= 0.5f * dt;
        dq.z *= 0.5f * dt;
        dq.w *= 0.5f * dt;

        m_Orientation.x += dq.x;
        m_Orientation.y += dq.y;
        m_Orientation.z += dq.z;
        m_Orientation.w += dq.w;

        D3DXQuaternionNormalize(&m_Orientation, &m_Orientation);
    }

    // 摩擦・転がり抵抗
    m_Velocity *= (1.0f - m_Friction * dt);

    // 減衰をフレーム固定率で
    m_AngularVelocity *= 0.98f;

    // コライダー更新
    if (m_Collider)
    {
        m_Collider->UpdateTransform(m_Position, m_Orientation, m_Scale);
    }

    // 力のリセット
    m_AccumulatedForce = INIT_VEC3;
    m_AccumulatedTorque = INIT_VEC3;
}
//=============================================================================
// 中心設定処理
//=============================================================================
void RigidBody::SetOrientation(const D3DXQUATERNION& q)
{
    m_Orientation = q;
    D3DXMATRIX mat;
    D3DXMatrixRotationQuaternion(&mat, &m_Orientation);

    // コライダーにも反映
    if (m_Collider)
    {
        m_Collider->UpdateTransform(m_Position, m_Orientation, m_Scale);
    }
}
//=============================================================================
// トランスフォーム設定処理
//=============================================================================
void RigidBody::SetTransform(const D3DXVECTOR3& pos, const D3DXQUATERNION& rot, const D3DXVECTOR3& scale)
{
    m_Position = pos;
    m_Orientation = rot;
    m_Scale = scale;

    if (m_Collider)
    {
        // コライダーの位置更新
        m_Collider->UpdateTransform(pos, rot, scale);
    }
}