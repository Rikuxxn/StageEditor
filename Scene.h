//=============================================================================
//
// シーン処理 [Scene.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _SCENE_H_// このマクロ定義がされていなかったら
#define _SCENE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Object.h"

//*****************************************************************************
// シーンクラス
//*****************************************************************************
class CScene : public CObject
{
public:
	typedef enum
	{
		MODE_EDIT = 0,
		MODE_MAX
	}MODE;

	CScene(MODE mode);
	virtual ~CScene();

	static CScene* Create(MODE mode);
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	virtual void OnDeviceReset(void) {}
	virtual void ReleaseThumbnail(void) {}
	virtual void ResetThumbnail(void) {}

	MODE GetMode(void) { return m_mode; }

private:
	MODE m_mode;// モード
};

#endif