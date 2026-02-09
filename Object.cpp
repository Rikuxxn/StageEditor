//=============================================================================
//
// オブジェクト処理 [Object.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Object.h"
#include "Camera.h"
#include "Manager.h"
#include "Edit.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
CObject* CObject::m_apTop[MAX_OBJ_PRIORITY] = {};
CObject* CObject::m_apCur[MAX_OBJ_PRIORITY] = {};
int CObject::m_nNumAll = 0;

//=============================================================================
// コンストラクタ
//=============================================================================
CObject::CObject(int nPriority)
{
	m_nPriority = 0;
	m_type = TYPE_NONE;

	// オブジェクト(自分)をリストに追加
	m_pPrev = m_apCur[nPriority];

	if (m_pPrev != nullptr)
	{
		m_pPrev->m_pNext = this;
	}

	m_pNext = nullptr;

	if (m_apTop[nPriority] == nullptr)
	{
		m_apTop[nPriority] = this;
	}

	m_apCur[nPriority] = this;

	m_nNumAll++;
	m_nPriority = nPriority;
	m_bDeath = false;
}
//=============================================================================
// デストラクタ
//=============================================================================
CObject::~CObject()
{
	// 今はなし
}
//=============================================================================
// 全てのオブジェクトの破棄
//=============================================================================
void CObject::ReleaseAll(void)
{
	for (int nPriority = 0; nPriority < MAX_OBJ_PRIORITY; nPriority++)
	{
		CObject* pObject = m_apTop[nPriority];

		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext;// 次のオブジェクトを保存

			// 終了処理
			pObject->Uninit();

			// オブジェクトのリスト削除と破棄
			pObject->Destroy();

			pObject = pObjectNext;// 次のオブジェクトを代入
		}
	}
}
//=============================================================================
// 全てのオブジェクトの更新処理
//=============================================================================
void CObject::UpdateAll(void)
{
	for (int nPriority = 0; nPriority < MAX_OBJ_PRIORITY; nPriority++)
	{
		CObject* pObject = m_apTop[nPriority];

		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext;// 次のオブジェクトを保存

			if (!pObject->m_bDeath)
			{
				// 更新処理
				pObject->Update();
			}

			pObject = pObjectNext;// 次のオブジェクトを代入
		}
	}

	for (int nPriority = 0; nPriority < MAX_OBJ_PRIORITY; nPriority++)
	{
		CObject* pObject = m_apTop[nPriority];

		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext;// 次のオブジェクトを保存

			if (pObject->m_bDeath)
			{
				// オブジェクトのリスト削除と破棄
				pObject->Destroy();
			}

			pObject = pObjectNext;// 次のオブジェクトを代入
		}
	}

}
//=============================================================================
// 全てのオブジェクトの描画処理
//=============================================================================
void CObject::DrawAll(void)
{
	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	// カメラの設定
	pCamera->SetCamera();

	// ブロックの当たり判定の描画
	CEdit::GetBlockManager()->Draw();

	for (int nPriority = 0; nPriority < MAX_OBJ_PRIORITY; nPriority++)
	{
		CObject* pObject = m_apTop[nPriority];

		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext;// 次のオブジェクトを保存

			// 描画処理
			pObject->Draw();

			pObject = pObjectNext;// 次のオブジェクトを代入
		}
	}
}
//=============================================================================
// オブジェクトの破棄フラグ設定
//=============================================================================
void CObject::Release(void)
{
	m_bDeath = true;
}
//=============================================================================
// オブジェクトの総数の取得
//=============================================================================
int CObject::GetNumObject(void)
{
	return m_nNumAll;
}
//=============================================================================
// タイプの設定処理
//=============================================================================
void CObject::SetType(TYPE type)
{
	m_type = type;
}
//=============================================================================
// タイプの取得
//=============================================================================
CObject::TYPE CObject::GetType(void)
{
	return m_type;
}
//=============================================================================
// オブジェクトのリスト削除と破棄
//=============================================================================
void CObject::Destroy(void)
{
	int nPriority = this->m_nPriority;

	if (m_pPrev != nullptr && m_pNext != nullptr)
	{// 間を消したとき
		m_pNext->m_pPrev = m_pPrev;
		m_pPrev->m_pNext = m_pNext;
	}
	else if (m_pPrev == nullptr && m_pNext != nullptr)
	{// 先頭を消したとき
		m_pNext->m_pPrev = nullptr;
		m_apTop[nPriority] = m_pNext;
	}
	else if (m_pPrev != nullptr && m_pNext == nullptr)
	{// 最後尾を消したとき
		m_pPrev->m_pNext = nullptr;
		m_apCur[nPriority] = m_pPrev;
	}
	else if (m_pPrev == nullptr && m_pNext == nullptr)
	{// 唯一の要素だった場合
		m_apTop[nPriority] = nullptr;
		m_apCur[nPriority] = nullptr;
	}

	m_pPrev = nullptr;
	m_pNext = nullptr;

	m_nNumAll--;

	delete this;
}