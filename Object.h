//=============================================================================
//
// オブジェクト処理 [Object.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _OBJECT_H_// このマクロ定義がされていなかったら
#define _OBJECT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_OBJ_PRIORITY	(8)		// 優先順位


//*****************************************************************************
// オブジェクトクラス
//*****************************************************************************
class CObject
{
public:
	CObject(int nPriority = 3);
	virtual ~CObject();

	// オブジェクトの種類
	typedef enum
	{
		TYPE_NONE = 0,
		TYPE_PLAYER,
		TYPE_MAX
	}TYPE;

	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	static void ReleaseAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	static int GetNumObject(void);
	void SetType(TYPE type);
	TYPE GetType(void);
	void Destroy(void);
	bool GetDeath(void) { return m_bDeath; }

protected:
	void Release(void);

private:
	static CObject* m_apTop[MAX_OBJ_PRIORITY];	// 先頭のオブジェクトへのポインタ
	static CObject* m_apCur[MAX_OBJ_PRIORITY];	// 最後尾のオブジェクトへのポインタ
	CObject*		m_pPrev;					// 前のオブジェクトへのポインタ
	CObject*		m_pNext;					// 次のオブジェクトへのポインタ
	static int		m_nNumAll;					// オブジェクトの総数
	TYPE			m_type;						// 種類
	int				m_nPriority;				// 優先順位の位置
	bool			m_bDeath;					// 死亡フラグ
};

#endif
