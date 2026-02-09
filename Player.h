//=============================================================================
//
// プレイヤー処理 [Player.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PLAYER_H_// このマクロ定義がされていなかったら
#define _PLAYER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "DebugProc3D.h"
#include "Block.h"
#include "Game.h"
#include "Motion.h"
#include "Model.h"
#include "State.h"
#include "Object.h"


//*****************************************************************************
// プレイヤークラス
//*****************************************************************************
class CPlayer : public CObject
{
public:
	CPlayer(int nPriority = 2);
	~CPlayer();

	// 入力データ構造体
	struct InputData
	{
		D3DXVECTOR3 moveDir;      // 前後移動ベクトル
	};

	static constexpr float SPEED				= 100.0f;	// 移動スピード
	static constexpr float DECELERATION_RATE	= 0.85f;	// 減速率
	static constexpr float ACCEL_RATE			= 0.15f;	// イージング率

	// プレイヤーモーションの種類
	typedef enum
	{
		NEUTRAL = 0,		// 待機
		MOVE,				// 移動
		MAX
	}PLAYER_MOTION;

	static CPlayer* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//*****************************************************************************
	// flagment関数
	//*****************************************************************************
	//bool OnGround(btDiscreteDynamicsWorld* world, btRigidBody* playerBody, float rayLength);

	//*****************************************************************************
	// setter関数
	//*****************************************************************************
	void SetMove(D3DXVECTOR3 move) { m_move = move; }


	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXVECTOR3 GetRot(void) { return m_rot; };
	D3DXVECTOR3 GetSize(void) { return m_size; }
	D3DXVECTOR3 GetMove(void) const { return m_move; }
	D3DXVECTOR3 GetColliderPos(void) const { return m_colliderPos; }
	CMotion* GetMotion(void) { return m_pMotion; }
	bool GetOnGround(void) { return m_bOnGround; }
	bool GetIsMoving(void) const { return m_bIsMoving; }
	D3DXVECTOR3 GetForward(void);
	InputData GatherInput(void);
	RigidBody* GetRigidBody(void) const { return m_pRigidBody.get(); }						// RigidBodyの取得
	void ReleasePhysics(void);														// Physics破棄用

	// ステート用にフラグ更新
	void UpdateMovementFlags(const D3DXVECTOR3& moveDir)
	{
		m_bIsMoving = (moveDir.x != 0.0f || moveDir.z != 0.0f);
	}

private:
	static constexpr int	MAX_PARTS		= 32;		// 最大パーツ数
	static constexpr float	MASS			= 1.0f;		// 質量
	static constexpr float	MAX_GRAVITY		= -0.26f;	// 重力加速度
	static constexpr float	CAPSULE_RADIUS	= 16.5f;	// カプセルコライダーの半径
	static constexpr float	CAPSULE_HEIGHT	= 40.0f;	// カプセルコライダーの高さ
	static constexpr float	DOUBLE			= 2.0f;		// 二倍

	CModel*						m_apModel[MAX_PARTS];	// モデル(パーツ)へのポインタ
	CMotion*					m_pMotion;				// モーションへのポインタ
	CDebugProc3D*				m_pDebug3D;				// 3Dデバッグ表示へのポインタ
	std::shared_ptr<RigidBody>	m_pRigidBody;			// リジッドボディ
	std::shared_ptr<Collider>	m_pShape;				// コライダー
	D3DXVECTOR3					m_pos;					// 位置
	D3DXVECTOR3					m_rot;					// 向き
	D3DXVECTOR3					m_rotDest;				// 向き
	D3DXVECTOR3					m_move;					// 移動量
	D3DXVECTOR3					m_targetMove;			// 目標速度
	D3DXVECTOR3					m_currentMove;			// 実際の移動速度
	D3DXVECTOR3					m_size;					// サイズ
	D3DXVECTOR3					m_colliderPos;			// カプセルコライダーの位置
	D3DXMATRIX					m_mtxWorld;				// ワールドマトリックス
	int							m_nNumModel;			// モデル(パーツ)の総数
	float						m_radius;				// カプセルコライダーの半径
	float						m_height;				// カプセルコライダーの高さ
	bool						m_bIsMoving;			// 移動入力フラグ
	bool						m_bOnGround;			// 接地フラグ

	// ステートを管理するクラスのインスタンス
	StateMachine<CPlayer> m_stateMachine;
};

#endif
