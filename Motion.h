//=============================================================================
//
// モーション処理 [Motion.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MOTION_H_// このマクロ定義がされていなかったら
#define _MOTION_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Model.h"

//*****************************************************************************
// モーションクラス
//*****************************************************************************
class CMotion
{
public:
	CMotion();
	~CMotion();

	static constexpr float RESET_MOTION_RATE = 1.0f;// モーションレートのリセット

	static CMotion* Load(const char* pFilepath, CModel* pModel[], int& nNumModel, int nMaxMotion);
	void LoadModelInfo(FILE* pFile, char* aString, CModel* pModel[], int& nNumModel, int& nIdx);
	void LoadCharacterSet(FILE* pFile, char* aString, CModel* pModel[], int nNumModel, int parentIdx[]);
	void LoadMotionSet(FILE* pFile, char* aString, CMotion* pMotion, int& nCntMotion, int nMaxMotion);
	void Update(CModel** pModel, int& nNumModel);
	void StartBlendMotion(int  motionTypeBlend, int nFrameBlend);
	void SetMotion(int  motionType);
	void AdvanceKeyCounter(int motionType, int& nKey, int& nCounter, bool bLoop);
	void SetMotionSpeedRate(float rate);
	bool IsCurrentMotion(int motionType) const;
	bool IsCurrentMotionEnd(int motionType) const;
	int GetMotionType(void) { return m_motionType; }

	bool EventMotionRange(int motionType, int startKey, int endKey, int startFrame, int endFrame) const;
	bool EventMotionRange(int motionType, int Key, int Frame) const;
	float GetMotionRate(void) const;
	int GetMotionFrame(void);

private:
	bool FindToken(FILE* f, char* buf, const char* token);

private:
	static constexpr int	MAX_WORD = 1024;	// 最大文字数
	static constexpr int	MAX_PARTS = 32;	// 最大パーツ数
	static constexpr int	MAX_KEY = 256;	// 最大キー数
	static constexpr int	MAX_MOTION = 20;	// モーションの最大数
	static constexpr float	DOUBLE = 2.0f;	// 二倍

	//*************************************************************************
	// キー構造体
	//*************************************************************************
	typedef struct
	{
		float fPosX;							// 位置(X)
		float fPosY;							// 位置(Y)
		float fPosZ;							// 位置(Z)
		float fRotX;							// 向き(X)
		float fRotY;							// 向き(Y)
		float fRotZ;							// 向き(Z)
	}KEY;

	//*************************************************************************
	// 補間関数(位置)
	//*************************************************************************
	inline D3DXVECTOR3 LerpPos(const KEY& a, const KEY& b, float t);

	//*************************************************************************
	// 補間関数(向き)
	//*************************************************************************
	inline D3DXVECTOR3 LerpRot(const KEY& a, const KEY& b, float t);

	//*************************************************************************
	// キー情報の構造体
	//*************************************************************************
	typedef struct
	{
		int nFrame;								// 再生フレーム
		KEY aKey[MAX_PARTS];					// 各パーツのキー要素
	}KEY_INFO;
	void ParseKeySet(FILE* f, char* buf, KEY_INFO& keyInfo);
	void ParseKey(FILE* f, char* buf, KEY_INFO& keyInfo,
		int& posIdx, int& rotIdx);

	//*************************************************************************
	// モーション情報の構造体
	//*************************************************************************
	typedef struct
	{
		bool	 bLoop;							// ループするかどうか
		int		 nNumKey;						// キーの総数
		KEY_INFO aKeyInfo[MAX_KEY];				// キー情報
		int		 startKey, startFrame;			// キーの開始・終了フレーム
	}MOTION_INFO;

	MOTION_INFO m_aMotionInfo[MAX_MOTION];		// モーション情報
	int			m_motionType;					// モーションの種類
	int			m_nNumMotion;					// モーション総数
	bool		m_bLoopMotion;					// ループするかどうか
	int			m_nNumKey;						// キーの総数
	int			m_nKey;							// 現在のキーNo.
	int			m_nCounterMotion;				// モーションのカウンター
	bool		m_bFinishMotion;				// 現在のモーションが終了しているかどうか
	bool		m_bBlendMotion;					// ブレンドモーションがあるかどうか
	int			m_motionTypeBlend;				// ブレンドモーションがあるかどうか
	bool		m_bLoopMotionBlend;				// ブレンドモーションがループするかどうか
	int			m_nNumKeyBlend;					// ブレンドモーションのキー数
	int			m_nKeyBlend;					// ブレンドモーションの現在のキーNo.
	int			m_nCounterMotionBlend;			// ブレンドモーションのカウンター
	int			m_nFrameBlend;					// ブレンドのフレーム数(何フレームかけてブレンドするか)
	int			m_nCounterBlend;				// ブレンドカウンター
	float		m_motionSpeedRate;				// モーションスピード
	float		m_motionCounterAcc;				// 小数進行用
};

#endif