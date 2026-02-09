//=============================================================================
//
// コライダー処理 [Collider.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _COLLIDER_H_// このマクロ定義がされていなかったら
#define _COLLIDER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************


//*****************************************************************************
// 前方宣言
//*****************************************************************************
class BoxCollider;
class CapsuleCollider;
class CylinderCollider;
class SphereCollider;

//=============================================================================
// コライダークラス
//=============================================================================
class Collider
{
public:
    enum TYPE
    {
        BOX,
        CAPSULE,
        CYLINDER,
        SPHERE
    };

    Collider(TYPE type) : m_Type(type) {}
    virtual ~Collider() {}

    virtual void UpdateTransform(const D3DXVECTOR3& /*pos*/, const D3DXQUATERNION& /*rot*/, const D3DXVECTOR3& /*scale*/) {}
    TYPE GetType(void) const { return m_Type; }

    // テンプレート関数
    template <typename T>
    T* As(void)
    {
        if constexpr (std::is_same_v<T, BoxCollider>)
        {
            return m_Type == BOX ? reinterpret_cast<T*>(this) : nullptr;
        }
        else if constexpr (std::is_same_v<T, CapsuleCollider>)
        {
            return m_Type == CAPSULE ? reinterpret_cast<T*>(this) : nullptr;
        }
        else if constexpr (std::is_same_v<T, CylinderCollider>)
        {
            return m_Type == CYLINDER ? reinterpret_cast<T*>(this) : nullptr;
        }
        else if constexpr (std::is_same_v<T, SphereCollider>)
        {
            return m_Type == SPHERE ? reinterpret_cast<T*>(this) : nullptr;
        }
    }

    // ワールド変換の取得
    virtual void SetPosition(const D3DXVECTOR3& pos) { m_Position = pos; }
    virtual const D3DXVECTOR3& GetPosition(void) const { return m_Position; }

    // ローカル慣性モーメントを計算
    virtual void calculateLocalInertia(float /*mass*/, D3DXVECTOR3& inertia) const
    {
        inertia = INIT_VEC3;
    }

protected:
    TYPE m_Type;
    D3DXVECTOR3 m_Position;
};

//=============================================================================
// ボックスコライダー(OBB)
//=============================================================================
class BoxCollider : public Collider
{
public:
    BoxCollider(const D3DXVECTOR3& size)
        : Collider(BOX), m_Size(size) {}

    // 位置・回転・スケールを反映
    void UpdateTransform(const D3DXVECTOR3& pos, const D3DXQUATERNION& rot, const D3DXVECTOR3& scale) override;
    void calculateLocalInertia(float mass, D3DXVECTOR3& inertia) const override;

    const D3DXVECTOR3& GetScaledSize(void) const { return m_ScaledSize; }
    const D3DXMATRIX& GetRotation(void) const { return m_Rotation; }
    const D3DXQUATERNION& GetRotationQuat(void) const { return m_RotationQuat; }

private:
    D3DXVECTOR3     m_Size;         // 元サイズ
    D3DXVECTOR3     m_ScaledSize;   // スケール反映後サイズ
    D3DXMATRIX      m_Rotation;     // 回転行列
    D3DXQUATERNION  m_RotationQuat; // quaternion
};

//=============================================================================
// カプセルコライダー
//=============================================================================
class CapsuleCollider : public Collider
{
public:
    CapsuleCollider(float radius, float height)
        : Collider(CAPSULE), m_Radius(radius), m_Height(height) {}

    void UpdateTransform(const D3DXVECTOR3& pos, const D3DXQUATERNION& rot, const D3DXVECTOR3& scale);

    // 慣性モーメント
    void calculateLocalInertia(float mass, D3DXVECTOR3& inertia) const;

    float GetRadius(void) const { return m_Radius; }
    float GetHeight(void) const { return m_Height; }
    float GetHalfHeight(void) const { return m_Height * HALF; }
    D3DXVECTOR3 GetTop(void) const;
    D3DXVECTOR3 GetBottom(void) const;

private:
    static constexpr float HALF = 0.5f; // 半分

    D3DXVECTOR3     m_Scale;            // スケール保持
    D3DXQUATERNION  m_Rotation;         // 回転
    float           m_Radius;           // 半径
    float           m_Height;           // 高さ
};

//=============================================================================
// シリンダーコライダー
//=============================================================================
class CylinderCollider : public Collider
{
public:
    CylinderCollider(const D3DXVECTOR3& size, const D3DXVECTOR3& dir)
        : Collider(CYLINDER), m_Size(size), m_Dir(dir)
    {
        m_Radius = size.x * HALF;
        m_Height = size.y;
    }

    void UpdateTransform(const D3DXVECTOR3& pos, const D3DXQUATERNION& rot, const D3DXVECTOR3& scale);

    // 慣性モーメント
    void calculateLocalInertia(float mass, D3DXVECTOR3& inertia) const;

    float GetRadius(void) const { return m_RadiusScaled; }
    float GetHeight(void) const { return m_HeightScaled; }
    const D3DXVECTOR3& GetDirection(void) const { return m_Dir; }
    const D3DXMATRIX& GetRotation(void) const { return m_Rotation; }
    const D3DXQUATERNION& GetRotationQuat(void) const { return m_RotationQuat; }

private:
    static constexpr float HALF = 0.5f; // 半分

    D3DXMATRIX      m_Rotation;         // 回転行列
    D3DXVECTOR3     m_Size;             // サイズ
    D3DXVECTOR3     m_Dir;              // 方向
    D3DXQUATERNION  m_RotationQuat;     // クォータニオン
    float           m_Radius;           // 半径
    float           m_Height;           // 高さ
    float           m_RadiusScaled;     // 拡大後の半径
    float           m_HeightScaled;     // 拡大後の高さ
};

//=============================================================================
// スフィアコライダー
//=============================================================================
class SphereCollider : public Collider
{
public:
    SphereCollider(const D3DXVECTOR3& size)
        : Collider(SPHERE), m_Size(size),
        m_Radius(std::max({ size.x, size.y, size.z }) * HALF),
        m_ScaledRadius(m_Radius) {}

    // 位置・スケール・回転を反映
    void UpdateTransform(const D3DXVECTOR3& pos, const D3DXQUATERNION& rot, const D3DXVECTOR3& scale) override;

    // 慣性モーメント
    void calculateLocalInertia(float mass, D3DXVECTOR3& inertia) const override;

    float GetRadius(void) const { return m_ScaledRadius; }
    const D3DXMATRIX& GetRotation(void) const { return m_Rotation; }
    const D3DXQUATERNION& GetRotationQuat(void) const { return m_RotationQuat; }

private:
    static constexpr float HALF = 0.5f; // 半分

    D3DXMATRIX      m_Rotation;         // 回転行列
    D3DXVECTOR3     m_Size;             // サイズ
    D3DXQUATERNION  m_RotationQuat;     // クォータニオン
    float           m_Radius;           // 元半径
    float           m_ScaledRadius;     // スケール反映後
};

#endif