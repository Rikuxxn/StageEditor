//=============================================================================
//
// プレイヤーの状態処理 [PlayerState.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PLAYERSTATE_H_// このマクロ定義がされていなかったら
#define _PLAYERSTATE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "State.h"

//*****************************************************************************
// 前方宣言
//*****************************************************************************
class CPlayer;
class CPlayerStandState;
class CPlayerMoveState;


//*****************************************************************************
// プレイヤーの待機状態
//*****************************************************************************
class CPlayerStandState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* /*pPlayer*/) override;
	void OnUpdate(CPlayer* pPlayer) override;
	void OnExit(CPlayer* /*pPlayer*/) override {}

private:

};

//*****************************************************************************
// プレイヤーの移動状態
//*****************************************************************************
class CPlayerMoveState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* /*pPlayer*/) override;
	void OnUpdate(CPlayer* pPlayer) override;
	void OnExit(CPlayer* /*pPlayer*/) override {}

private:

};

#endif