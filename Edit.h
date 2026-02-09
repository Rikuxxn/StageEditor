//=============================================================================
//
// エディター処理 [Edit.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _EDIT_H_// このマクロ定義がされていなかったら
#define _EDIT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Scene.h"
#include "BlockManager.h"
#include "imguimaneger.h"
#include "Grid.h"
#include "Player.h"

//*****************************************************************************
// エディタークラス
//*****************************************************************************
class CEdit : public CScene
{
public:
	CEdit();
	~CEdit();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void OnDeviceReset(void) override;
	void ReleaseThumbnail(void) override;
	void ResetThumbnail(void) override;

	static CBlock* GetBlock(void) { return m_pBlock; }
	static CBlockManager* GetBlockManager(void) { return m_pBlockManager.get(); }
	static CImGuiManager* GetImGuiManager(void) { return m_pImGuiManager; }
private:
	static CBlock*							m_pBlock;			// ブロックへのポインタ
	static std::unique_ptr<CBlockManager>	m_pBlockManager;	// ブロックマネージャーへのポインタ
	static CImGuiManager*					m_pImGuiManager;	// ImGuiマネージャーへのポインタ
	std::unique_ptr<CGrid>					m_pGrid;			// グリッドへのポインタ
	CPlayer*								m_pPlayer;			// プレイヤーへのポインタ
};

#endif

