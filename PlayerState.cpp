//=============================================================================
//
// プレイヤーの状態処理 [PlayerState.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "PlayerState.h"
#include "Player.h"
#include "RigidBody.h"

//=============================================================================
// 待機状態の開始処理
//=============================================================================
void CPlayerStandState::OnStart(CPlayer* /*pPlayer*/)
{

}
//=============================================================================
// 待機状態の更新処理
//=============================================================================
void CPlayerStandState::OnUpdate(CPlayer* pPlayer)
{
	// 入力を取得
	CPlayer::InputData input = pPlayer->GatherInput();

	// 前方移動入力があれば通常移動ステートに移行
	if (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f)
	{
		m_pMachine->ChangeState<CPlayerMoveState>();
	}

	D3DXVECTOR3 move = pPlayer->GetMove();
	move *= CPlayer::DECELERATION_RATE; // 減速率
	if (fabsf(move.x) < 0.01f) move.x = 0;
	if (fabsf(move.z) < 0.01f) move.z = 0;

	// 移動量を設定
	pPlayer->SetMove(move);

	if (auto rb = pPlayer->GetRigidBody())
	{
		D3DXVECTOR3 vel = rb->GetVelocity();
		vel.x = move.x; // X方向速度
		vel.z = move.z; // Z方向速度
		rb->SetVelocity(vel);  // RigidBody にセット
	}
}


//=============================================================================
// 移動状態の開始処理
//=============================================================================
void CPlayerMoveState::OnStart(CPlayer* /*pPlayer*/)
{

}
//=============================================================================
// 移動状態の更新処理
//=============================================================================
void CPlayerMoveState::OnUpdate(CPlayer* pPlayer)
{
	// 入力取得
	CPlayer::InputData input = pPlayer->GatherInput();

	// フラグ更新
	pPlayer->UpdateMovementFlags(input.moveDir);

	// 目標速度計算
	D3DXVECTOR3 targetMove = input.moveDir;

	if (targetMove.x != 0.0f || targetMove.z != 0.0f)
	{
		D3DXVec3Normalize(&targetMove, &targetMove);
		targetMove *= CPlayer::SPEED;
	}
	else
	{
		targetMove = D3DXVECTOR3(0, 0, 0);
	}

	// 現在速度との補間（イージング）
	D3DXVECTOR3 currentMove = pPlayer->GetMove();

	currentMove.x += (targetMove.x - currentMove.x) * CPlayer::ACCEL_RATE;
	currentMove.z += (targetMove.z - currentMove.z) * CPlayer::ACCEL_RATE;

	// 補間後の速度をプレイヤーにセット
	pPlayer->SetMove(currentMove);

	if (auto rb = pPlayer->GetRigidBody())
	{
		D3DXVECTOR3 vel = rb->GetVelocity();
		vel.x = currentMove.x; // X方向速度
		vel.z = currentMove.z; // Z方向速度
		rb->SetVelocity(vel);  // RigidBody にセット
	}

	// 移動していなければ待機ステートに戻す
	if (!pPlayer->GetIsMoving())
	{
		// 待機状態
		m_pMachine->ChangeState<CPlayerStandState>();
	}
}
