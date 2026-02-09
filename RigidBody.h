//=============================================================================
//
// 剛体処理 [RigidBody.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RIGIDBODY_H_// このマクロ定義がされていなかったら
#define _RIGIDBODY_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************

//*****************************************************************************
// 前方宣言
//*****************************************************************************
class Collider;
class BoxCollider;
class CapsuleCollider;
class CylinderCollider;
class SphereCollider;

//*****************************************************************************
// リジッドボディクラス
//*****************************************************************************
class RigidBody
{
public:
    RigidBody(std::shared_ptr<Collider> col, float mass);

    // 重力適用処理
    void ApplyGravity(float dt, D3DXVECTOR3 gravity);

    // 外力の適用
    void ApplyForce(const D3DXVECTOR3& force);

    // 衝突点の適用処理
    void ApplyForceAtPoint(const D3DXVECTOR3& force, const D3DXVECTOR3& point);

    // インパルスの適用
    void ApplyImpulse(const D3DXVECTOR3& impulse, const D3DXVECTOR3& relPos);

    // 更新
    void Integrate(float dt, const D3DXVECTOR3& gravity);

    // ダイナミックブロックかどうか
    bool IsDynamic(void) const { return m_isDynamic; }

    bool IsOnGround(void) const { return m_onGround; }

    void SetIsDynamic(bool flag) { m_isDynamic = flag; }
    void SetLinearFactor(const D3DXVECTOR3& factor) { m_LinearFactor = factor; }
    void SetAngularFactor(const D3DXVECTOR3& factor) { m_AngularFactor = factor; }
    void SetAngularVelocity(const D3DXVECTOR3& vel) { m_AngularVelocity = vel; }
    void SetFriction(float f) { m_Friction = f; }
    void SetRollingFriction(float f) { m_RollingFriction = f; }
    void SetVelocity(D3DXVECTOR3 vel) { m_Velocity = vel; }
    void SetRestitution(float r) { m_Restitution = r; }
    void SetOnGround(bool flag) { m_onGround = flag; }
    void SetOrientation(const D3DXQUATERNION& q);
    void SetTransform(const D3DXVECTOR3& pos, const D3DXQUATERNION& rot, const D3DXVECTOR3& scale);

    std::shared_ptr<Collider> GetCollider(void) const { return m_Collider; }
    const D3DXVECTOR3& GetPosition(void) const { return m_Position; }
    const D3DXVECTOR3& GetRotation(void) const { return m_Rotation; }
    const D3DXVECTOR3& GetVelocity(void) const { return m_Velocity; }
    const D3DXVECTOR3& GetScale(void) const { return m_Scale; }
    const float GetFriction(void) const { return m_Friction; }
    const D3DXVECTOR3& GetAngularFactor(void) const { return m_AngularFactor; }
    const D3DXVECTOR3& GetAngularVelocity(void) const { return m_AngularVelocity; }
    const float GetRollingFriction(void) const { return m_RollingFriction; }
    const D3DXVECTOR3& GetInertia(void) const { return m_Inertia; }
    const float GetMass(void) { return m_Mass; }
    float GetRestitution(void) const { return m_Restitution; }
    const D3DXQUATERNION& GetOrientation(void) const { return m_Orientation; }

private:
    std::shared_ptr<Collider>   m_Collider;            // コライダーのポインタ
    D3DXVECTOR3                 m_Position;            // 位置
    D3DXVECTOR3                 m_Velocity;            // 速度
    D3DXVECTOR3                 m_Scale;               // 拡大率
    D3DXVECTOR3                 m_AccumulatedForce;    // 外力の蓄積
    D3DXVECTOR3                 m_AccumulatedTorque;   // トルクの蓄積
    D3DXVECTOR3                 m_AngularFactor;       // 回転方向
    D3DXVECTOR3                 m_Inertia;             // 慣性モーメント（回転しにくさ）
    D3DXVECTOR3                 m_InertiaInv;          // 慣性
    D3DXVECTOR3                 m_Rotation;            // 向き
    D3DXVECTOR3                 m_LinearFactor;        // 移動方向
    D3DXVECTOR3                 m_AngularVelocity;     // 角速度
    D3DXQUATERNION              m_Orientation;         // 原点
    float                       m_Friction;            // 摩擦
    float                       m_RollingFriction;     // 回転摩擦
    float                       m_Restitution;         // 反発係数
    float                       m_Mass;                // 質量
    bool                        m_isDynamic;           // 動的ブロックかどうか
    bool                        m_onGround;            // 乗っているかどうか
};

#endif