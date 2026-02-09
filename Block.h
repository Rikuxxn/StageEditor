//=============================================================================
//
// ブロック処理 [Block.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCK_H_
#define _BLOCK_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "ObjectX.h"
#include "PhysicsWorld.h"
#include "DebugProc3D.h"
#include "json.hpp"

//*****************************************************************************
// 前方宣言
//*****************************************************************************
class CBlock;
class Collider;
class RigidBody;

//*****************************************************************************
// 型定義
//*****************************************************************************
using BlockCreateFunc = std::function<CBlock* ()>;

// JSONの使用
using json = nlohmann::json;

//*****************************************************************************
// ブロッククラス
//*****************************************************************************
class CBlock : public CObjectX
{
public:
	CBlock(int nPriority = 3);
	virtual ~CBlock() = default;

	//*****************************************************************************
	// ブロックの種類
	//*****************************************************************************
	enum TYPE
	{
		TYPE_BOX = 0,
		TYPE_CYLINDER,
		TYPE_SPHERE,
		TYPE_CAPSULE,
		TYPE_MAX
	};

	static CBlock* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size, TYPE type, bool isDynamic);	// ブロックの生成
	static void InitFactory(void);
	virtual HRESULT Init(void);
	void Kill(void) { m_isDead = true; }												// ブロック削除
	void Uninit(void);
	void ReleasePhysics(void);
	void Update(void);
	void Draw(void);
	void DrawCollider(void);
	void CreatePhysicsFromScale(const D3DXVECTOR3& scale);								// ブロックスケールによるコライダーの生成
	void CreatePhysics(const D3DXVECTOR3& pos, const D3DXVECTOR3& size);				// コライダーの生成
	void RecreatePhysics(void);
	virtual std::shared_ptr<Collider> CreateCollisionShape(const D3DXVECTOR3& size);
	virtual void SaveToJson(json& b);
	virtual void LoadFromJson(const json& b);
	virtual void UpdateLight(void) {}

	//*****************************************************************************
	// flagment関数
	//*****************************************************************************
	bool IsSelected(void) const { return m_bSelected; }									// ブロックが選択中のフラグを返す
	bool IsEditMode(void) const { return m_isEditMode; }								// エディット中かどうか
	virtual bool IsDynamicBlock(void) const { return m_isDynamic; }						// 動的ブロックの判別
	bool IsDead(void) const { return m_isDead; }										// 削除予約の取得

	//*****************************************************************************
	// setter関数
	//*****************************************************************************
	void SetType(TYPE type) { m_Type = type; }											// タイプの設定
	void SetSelected(bool flag) { m_bSelected = flag; }									// 選択中のフラグを返す
	void SetEditMode(bool enable);
	//void SetColliderSize(const D3DXVECTOR3& size) { m_colliderSize = size; }			// コライダーサイズの設定
	//void SetColliderManual(const D3DXVECTOR3& newSize);									// コライダーサイズの手動設定用
	//void SetColliderOffset(const D3DXVECTOR3& offset) { m_colliderOffset = offset; }	// コライダーのオフセットの設定
	void SetIsDynamic(bool isDynamic) { m_isDynamic = isDynamic; }

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	virtual D3DXCOLOR GetCol(void) const override;										// カラーの取得
	TYPE GetType(void) const { return m_Type; }											// タイプの取得
	D3DXMATRIX GetWorldMatrix(void);
	RigidBody* GetRigidBody(void) { return m_pRigidBody.get(); }

	virtual float GetMass(void) const { return DEFAULT_MASS; }								// 質量の取得
	virtual int GetCollisionFlags(void) const { return 0; }// デフォルトはフラグなし
	virtual D3DXVECTOR3 GetLinearFactor(void) const { return DEFAULT_LINEAR_FACTOR; }
	virtual D3DXVECTOR3 GetAngularFactor(void) const { return DEFAULT_ANGLAR_FACTOR; }
	virtual float GetRollingFriction(void) const { return DEFAULT_ROLLING_FRICTION; }
	virtual float GetFriction(void) const { return DEFAULT_FRICTION; }

private:
	static constexpr float	DEFAULT_MASS				= 1.0f;							// デフォルトの質量
	static constexpr float	DEFAULT_ROLLING_FRICTION	= 1.7f;							// デフォルトの回転摩擦
	static constexpr float	DEFAULT_FRICTION			= 2.5f;							// デフォルトの摩擦
	const D3DXVECTOR3		DEFAULT_LINEAR_FACTOR		= { 1.0f, 1.0f, 1.0f };			// デフォルト線形係数
	const D3DXVECTOR3		DEFAULT_ANGLAR_FACTOR		= { 1.0f, 1.0f, 1.0f };			// デフォルト角度係数
	const D3DXCOLOR			SELECTED_COLOR				= { 1.0f, 0.0f, 0.0f, 0.6f };	// ブロック選択時の色
	const D3DXCOLOR			COLLIDER_COLOR				= { 0.0f, 1.0f, 0.3f, 1.0f };	// コライダーの色

	std::shared_ptr<RigidBody>							m_pRigidBody;					// リジッドボディ
	std::shared_ptr<Collider>							m_pShape;						// コライダー
	CDebugProc3D*										m_pDebug3D;						// 3Dデバッグ表示へのポインタ
	D3DXCOLOR											m_col;							// アルファ値
	D3DXCOLOR											m_baseCol;						// ベースのアルファ値
	bool												m_bSelected;					// 選択フラグ
	bool												m_isDead;						// 削除予約フラグ
	bool												m_isEditMode;					// 編集中かどうか
	bool												m_isDynamic;					// 動的ブロックかどうか
	static std::unordered_map<TYPE, BlockCreateFunc>	m_BlockFactoryMap;				// ファクトリー
	TYPE												m_Type;							// 種類

};

#endif