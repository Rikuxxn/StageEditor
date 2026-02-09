//=============================================================================
//
// モーション処理 [Motion.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Motion.h"
#include "Manager.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CMotion::CMotion()
{
	// 値のクリア
	m_motionType			= 0;				// モーションの種類
	m_nNumMotion			= 0;				// モーション総数
	m_bLoopMotion			= false;			// ループするかどうか
	m_nNumKey				= 0;				// キーの総数
	m_nKey					= 0;				// 現在のキーNo.
	m_nCounterMotion		= 0;				// モーションのカウンター
	m_bFinishMotion			= false;			// 現在のモーションが終了しているかどうか
	m_bBlendMotion			= false;			// ブレンドモーションがあるかどうか
	m_motionTypeBlend		= 0;				// ブレンドモーションの種類
	m_bLoopMotionBlend		= false;			// ブレンドモーションがループするかどうか
	m_nNumKeyBlend			= 0;				// ブレンドモーションのキー数
	m_nKeyBlend				= 0;				// ブレンドモーションの現在のキーNo.
	m_nCounterMotionBlend	= 0;				// ブレンドモーションのカウンター
	m_nFrameBlend			= 0;				// ブレンドのフレーム数(何フレームかけてブレンドするか)
	m_nCounterBlend			= 0;				// ブレンドカウンター
	m_motionSpeedRate		= RESET_MOTION_RATE;// 1.0 = 通常
	m_motionCounterAcc		= 0.0f;				// 小数進行用
}
//=============================================================================
// デストラクタ
//=============================================================================
CMotion::~CMotion()
{
	// なし
}
//=============================================================================
// モーションテキストファイルの読み込み
//=============================================================================
CMotion* CMotion::Load(const char* pFilepath, CModel* pModel[], int& nNumModel, int nMaxMotion)
{
	CMotion* pMotion = new CMotion;

	// ファイルを開く
	FILE* pFile = fopen(pFilepath, "r");

	if (!pFile)
	{
		return nullptr;  // ファイルが開けなかった
	}

	char aString[MAX_WORD];
	int nIdx = 0;
	int nCntMotion = 0;

	// 一時的な親のインデックス配列
	int parentIdx[MAX_PARTS];

	for (int nCnt = 0; nCnt < MAX_PARTS; nCnt++)
	{
		parentIdx[nCnt] = -1;
	}

	while (fscanf(pFile, "%s", aString) != EOF)
	{
		// SCRIPT 以外は無視
		if (strcmp(aString, "SCRIPT") != 0)
		{
			continue;
		}

		while (fscanf(pFile, "%s", aString) != EOF)
		{
			if (strcmp(aString, "END_SCRIPT") == 0)
			{
				break;
			}

			if (strcmp(aString, "NUM_MODEL") == 0 ||
				strcmp(aString, "MODEL_FILENAME") == 0)
			{
				pMotion->LoadModelInfo(pFile, aString, pModel, nNumModel, nIdx);
				continue;
			}

			if (strcmp(aString, "CHARACTERSET") == 0)
			{
				pMotion->LoadCharacterSet(pFile, aString, pModel, nNumModel, parentIdx);
				continue;
			}

			if (strcmp(aString, "MOTIONSET") == 0)
			{
				pMotion->LoadMotionSet(pFile, aString, pMotion, nCntMotion, nMaxMotion);
				continue;
			}

			// これら以外は無視
		}
	}

	// ファイルを閉じる
	fclose(pFile);

	// 親子関係設定
	for (int nCnt = 0; nCnt < nNumModel; nCnt++)
	{
		if (parentIdx[nCnt] >= 0 && parentIdx[nCnt] < nNumModel)
		{
			pModel[nCnt]->SetParent(pModel[parentIdx[nCnt]]);
		}
	}

	return pMotion;
}
//=============================================================================
// モデル情報の読み込み処理
//=============================================================================
void CMotion::LoadModelInfo(FILE* pFile, char* aString, CModel* pModel[], int& nNumModel, int& nIdx)
{
	if (strcmp(aString, "NUM_MODEL") == 0)
	{
		fscanf(pFile, "%s", aString); // "="

		if (strcmp(aString, "=") == 0)
		{
			fscanf(pFile, "%d", &nNumModel);
		}
	}
	else if (strcmp(aString, "MODEL_FILENAME") == 0)
	{
		fscanf(pFile, "%s", aString); // "="

		if (strcmp(aString, "=") == 0)
		{
			fscanf(pFile, "%s", aString);

			// モデルの生成
			pModel[nIdx] = CModel::Create(aString, D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 0, 0));
			nIdx++;
		}
	}
}
//=============================================================================
// キャラの設定処理
//=============================================================================
void CMotion::LoadCharacterSet(FILE* pFile, char* aString, CModel* pModel[], int nNumModel, int parentIdx[])
{
	while (fscanf(pFile, "%s", aString) != EOF)
	{
		if (strcmp(aString, "END_CHARACTERSET") == 0)
		{
			break;
		}

		if (strcmp(aString, "PARTSSET") != 0)
		{
			continue;
		}

		// PARTSSET開始
		int idx = -1;
		int pIdx = -1;
		D3DXVECTOR3 pos(0, 0, 0);
		D3DXVECTOR3 rot(0, 0, 0);

		// PARTSSET内ループ
		while (fscanf(pFile, "%s", aString) != EOF)
		{
			if (strcmp(aString, "END_PARTSSET") == 0)
			{
				break;
			}

			if (strcmp(aString, "INDEX") == 0)
			{
				fscanf(pFile, "%s", aString); // "="
				fscanf(pFile, "%d", &idx);
				continue;
			}

			if (strcmp(aString, "PARENT") == 0)
			{
				fscanf(pFile, "%s", aString); // "="
				fscanf(pFile, "%d", &pIdx);
				continue;
			}

			if (strcmp(aString, "POS") == 0)
			{
				fscanf(pFile, "%s", aString); // "="
				fscanf(pFile, "%f %f %f", &pos.x, &pos.y, &pos.z);
				continue;
			}

			if (strcmp(aString, "ROT") == 0)
			{
				fscanf(pFile, "%s", aString); // "="
				fscanf(pFile, "%f %f %f", &rot.x, &rot.y, &rot.z);
				continue;
			}

			// これら以外は無視
		}

		// 設定確定
		if (idx < 0 || idx >= nNumModel || pModel[idx] == nullptr)
		{
			continue;
		}

		pModel[idx]->SetPos(pos);
		pModel[idx]->SetRot(rot);
		parentIdx[idx] = pIdx;
	}
}
//=============================================================================
// トークン探索関数
//=============================================================================
bool CMotion::FindToken(FILE* f, char* buf, const char* token)
{
	while (fscanf(f, "%s", buf) != EOF)
	{
		if (strcmp(buf, token) == 0)
		{
			return true;
		}
	}

	return false;
}
//=============================================================================
// キーセットリード関数
//=============================================================================
void CMotion::ParseKeySet(FILE* f, char* buf, KEY_INFO& keyInfo)
{
	int posIdx = 0;
	int rotIdx = 0;

	while (fscanf(f, "%s", buf) != EOF)
	{
		if (strcmp(buf, "END_KEYSET") == 0)
		{
			return;
		}

		if (strcmp(buf, "KEY") != 0)
		{
			continue;
		}

		ParseKey(f, buf, keyInfo, posIdx, rotIdx);
	}
}
//=============================================================================
// キーリード関数
//=============================================================================
void CMotion::ParseKey(FILE* f, char* buf, KEY_INFO& keyInfo,
	int& posIdx, int& rotIdx)
{
	while (fscanf(f, "%s", buf) != EOF)
	{
		if (strcmp(buf, "END_KEY") == 0)
		{
			return;
		}

		if (strcmp(buf, "POS") == 0)
		{
			fscanf(f, "%s", buf); // "="
			fscanf(f, "%f %f %f",
				&keyInfo.aKey[posIdx].fPosX,
				&keyInfo.aKey[posIdx].fPosY,
				&keyInfo.aKey[posIdx].fPosZ);
			++posIdx;
			continue;
		}

		if (strcmp(buf, "ROT") == 0)
		{
			fscanf(f, "%s", buf); // "="
			fscanf(f, "%f %f %f",
				&keyInfo.aKey[rotIdx].fRotX,
				&keyInfo.aKey[rotIdx].fRotY,
				&keyInfo.aKey[rotIdx].fRotZ);
			++rotIdx;
			continue;
		}
	}
}
//=============================================================================
// モーションの読み込み処理
//=============================================================================
void CMotion::LoadMotionSet(FILE* pFile, char* aString, CMotion* pMotion, int& nCntMotion, int nMaxMotion)
{
	if (nCntMotion >= nMaxMotion)
	{
		return;
	}

	auto& motion = pMotion->m_aMotionInfo[nCntMotion];

	while (fscanf(pFile, "%s", aString) != EOF)
	{
		if (strcmp(aString, "END_MOTIONSET") == 0)
		{
			++nCntMotion;
			return;
		}

		if (strcmp(aString, "LOOP") == 0)
		{
			fscanf(pFile, "%s", aString);
			int loop;
			fscanf(pFile, "%d", &loop);
			motion.bLoop = (loop != 0);
			continue;
		}

		if (strcmp(aString, "NUM_KEY") != 0)
		{
			continue;
		}

		fscanf(pFile, "%s", aString);
		fscanf(pFile, "%d", &motion.nNumKey);

		for (int nCnt = 0; nCnt < motion.nNumKey; ++nCnt)
		{
			FindToken(pFile, aString, "KEYSET");
			FindToken(pFile, aString, "FRAME");

			fscanf(pFile, "%s", aString);
			fscanf(pFile, "%d", &motion.aKeyInfo[nCnt].nFrame);

			// キーセット
			ParseKeySet(pFile, aString, motion.aKeyInfo[nCnt]);
		}
	}
}
//=============================================================================
// キーカウンター更新
//=============================================================================
void CMotion::AdvanceKeyCounter(int motionType, int& nKey, int& nCounter, bool bLoop)
{
	m_motionCounterAcc += m_motionSpeedRate;

	// 進まないなら何もしない
	if (m_motionCounterAcc < 1.0f)
	{
		return;
	}

	int step = (int)m_motionCounterAcc;
	m_motionCounterAcc -= step;

	nCounter += step;

	const auto& motion = m_aMotionInfo[motionType];

	while (true)
	{
		const int frame = motion.aKeyInfo[nKey].nFrame;

		if (nCounter < frame)
		{
			break;
		}

		nCounter -= frame;
		++nKey;

		// キー範囲内なら続行
		if (nKey < motion.nNumKey)
		{
			continue;
		}

		// キー最後まで行ってループする場合
		if (bLoop)
		{
			nKey = 0;
			continue;
		}

		// ループしない場合は最後で停止
		nKey = motion.nNumKey - 1;
		break;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CMotion::Update(CModel** pModel, int& nNumModel)
{
	if (m_bBlendMotion && m_nFrameBlend > 0)
	{
		// ブレンド進行度
		float blendRate = (float)m_nCounterBlend / (float)m_nFrameBlend;

		for (int nCnt = 0; nCnt < nNumModel; nCnt++)
		{
			// 現在モーション
			int nextKey = (m_nKey + 1) % m_aMotionInfo[m_motionType].nNumKey;

			float tCur = (float)m_nCounterMotion / m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].nFrame;
			D3DXVECTOR3 posCurrent = LerpPos(m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCnt],
				m_aMotionInfo[m_motionType].aKeyInfo[nextKey].aKey[nCnt], tCur);
			D3DXVECTOR3 rotCurrent = LerpRot(m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCnt],
				m_aMotionInfo[m_motionType].aKeyInfo[nextKey].aKey[nCnt], tCur);

			// ブレンド先モーション
			int nextKeyBlend = (m_nKeyBlend + 1) % m_aMotionInfo[m_motionTypeBlend].nNumKey;

			float tBlend = (float)m_nCounterMotionBlend / m_aMotionInfo[m_motionTypeBlend].aKeyInfo[m_nKeyBlend].nFrame;
			D3DXVECTOR3 posBlend = LerpPos(m_aMotionInfo[m_motionTypeBlend].aKeyInfo[m_nKeyBlend].aKey[nCnt],
				m_aMotionInfo[m_motionTypeBlend].aKeyInfo[nextKeyBlend].aKey[nCnt], tBlend);
			D3DXVECTOR3 rotBlend = LerpRot(m_aMotionInfo[m_motionTypeBlend].aKeyInfo[m_nKeyBlend].aKey[nCnt],
				m_aMotionInfo[m_motionTypeBlend].aKeyInfo[nextKeyBlend].aKey[nCnt], tBlend);

			// ブレンド合成
			D3DXVECTOR3 posFinal = posCurrent * (1.0f - blendRate) + posBlend * blendRate;
			D3DXVECTOR3 rotFinal = rotCurrent * (1.0f - blendRate) + rotBlend * blendRate;

			pModel[nCnt]->SetOffsetPos(posFinal);
			pModel[nCnt]->SetOffsetRot(rotFinal);
		}

		// カウンター進行
		AdvanceKeyCounter(m_motionType, m_nKey, m_nCounterMotion, m_aMotionInfo[m_motionType].bLoop);
		AdvanceKeyCounter(m_motionTypeBlend, m_nKeyBlend, m_nCounterMotionBlend, m_aMotionInfo[m_motionTypeBlend].bLoop);

		// ブレンド進行
		m_nCounterBlend++;
		if (m_nCounterBlend >= m_nFrameBlend)
		{
			// ブレンド終了
			m_motionType = m_motionTypeBlend;
			m_nKey = m_nKeyBlend;
			m_nCounterMotion = m_nCounterMotionBlend;
			m_bBlendMotion = false;
		}
	}
	else
	{
		// 通常モーション
		for (int nCnt = 0; nCnt < nNumModel; nCnt++)
		{
			int nextKey = (m_nKey + 1 >= m_aMotionInfo[m_motionType].nNumKey) ?
				(m_aMotionInfo[m_motionType].bLoop ? 0 : m_nKey) : m_nKey + 1;

			float t = (float)m_nCounterMotion / m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].nFrame;

			D3DXVECTOR3 pos = LerpPos(m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCnt],
				m_aMotionInfo[m_motionType].aKeyInfo[nextKey].aKey[nCnt], t);
			D3DXVECTOR3 rot = LerpRot(m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCnt],
				m_aMotionInfo[m_motionType].aKeyInfo[nextKey].aKey[nCnt], t);

			pModel[nCnt]->SetOffsetPos(pos);
			pModel[nCnt]->SetOffsetRot(rot);
		}

		// カウンター進行
		AdvanceKeyCounter(m_motionType, m_nKey, m_nCounterMotion, m_aMotionInfo[m_motionType].bLoop);

		// 終了判定（ループしない場合のみ）
		if (!m_aMotionInfo[m_motionType].bLoop &&
			m_nKey == m_aMotionInfo[m_motionType].nNumKey - 1)
		{
			m_bFinishMotion = true;  // 最後のキーに到達したら終了フラグを立て続ける
		}
		else if (m_aMotionInfo[m_motionType].bLoop)
		{
			m_bFinishMotion = false; // ループモーションなら常に false
		}
	}
}
//=============================================================================
// 補間関数(位置)
//=============================================================================
inline D3DXVECTOR3 CMotion::LerpPos(const KEY& a, const KEY& b, float t)
{
	return D3DXVECTOR3(
		a.fPosX + (b.fPosX - a.fPosX) * t,
		a.fPosY + (b.fPosY - a.fPosY) * t,
		a.fPosZ + (b.fPosZ - a.fPosZ) * t
	);
}
//=============================================================================
// 補間関数(向き)
//=============================================================================
inline D3DXVECTOR3 CMotion::LerpRot(const KEY& a, const KEY& b, float t)
{
	auto delta = [](float from, float to)
	{
		float d = to - from;
		if (d > D3DX_PI)
		{
			d -= D3DX_PI * DOUBLE;
		}
		else if (d < -D3DX_PI)
		{
			d += D3DX_PI * DOUBLE;
		}

		return d;
	};

	return D3DXVECTOR3(
		a.fRotX + delta(a.fRotX, b.fRotX) * t,
		a.fRotY + delta(a.fRotY, b.fRotY) * t,
		a.fRotZ + delta(a.fRotZ, b.fRotZ) * t
	);
}
//=============================================================================
// モーションスピード設定処理
//=============================================================================
void CMotion::SetMotionSpeedRate(float rate)
{
	m_motionSpeedRate = std::max(rate, 0.1f); // 下限
}
//=============================================================================
// モーションブレンド開始処理
//=============================================================================
void CMotion::StartBlendMotion(int  motionTypeBlend, int nFrameBlend)
{
	m_motionTypeBlend = motionTypeBlend;
	m_nFrameBlend = nFrameBlend;
	m_nCounterBlend = 0;

	// 先頭からスタート
	m_nKeyBlend = 0;
	m_nCounterMotionBlend = 0;

	m_bBlendMotion = true;

	// ブレンド先のモーションキー数取得
	m_nNumKeyBlend = m_aMotionInfo[m_motionTypeBlend].nNumKey;

	m_bFinishMotion = false;
}
//=============================================================================
// モーションの設定処理
//=============================================================================
void CMotion::SetMotion(int  motionType)
{
	m_motionType = motionType;
	m_nKey = 0;
	m_nCounterMotion = 0;
	m_bFinishMotion = false;
}
//=============================================================================
// 現在のモーション
//=============================================================================
bool CMotion::IsCurrentMotion(int motionType) const
{
	bool currentMotion = (m_motionType == motionType);

	return currentMotion;
}
//=============================================================================
// モーションの終了判定
//=============================================================================
bool CMotion::IsCurrentMotionEnd(int motionType) const
{
	// ブレンド中もチェックする場合
	bool endCurrent = (m_motionType == motionType) && m_bFinishMotion;

	// ブレンド中で、ターゲットが motionType の場合も終了判定
	if (m_bBlendMotion && m_motionTypeBlend == motionType)
	{
		int lastKey = m_aMotionInfo[m_motionTypeBlend].nNumKey - 1;
		if (!m_aMotionInfo[m_motionTypeBlend].bLoop &&
			m_nKeyBlend == lastKey)
		{
			endCurrent = true;
		}
	}

	return endCurrent;
}
//=============================================================================
// モーションの範囲
//=============================================================================
bool CMotion::EventMotionRange(int motionType, int startKey, int endKey, int startFrame, int endFrame) const
{
	// モーションが一致しない or 終了していたら false
	if (m_motionType != motionType || m_bFinishMotion)
	{
		return false;
	}

	// 現在のキーが範囲内か
	if (m_nKey < startKey || m_nKey > endKey)
	{
		return false;
	}

	// 現在のフレームが範囲内か
	// （開始キーと終了キーが同じなら、startFrame/endFrameを適用）
	if (m_nKey == startKey && m_nCounterMotion < startFrame)
	{
		return false;
	}
	if (m_nKey == endKey && m_nCounterMotion > endFrame)
	{
		return false;
	}

	return true;
}
//=============================================================================
// モーションの範囲(1フレーム)
//=============================================================================
bool CMotion::EventMotionRange(int motionType, int Key, int Frame) const
{
	// モーションが一致しない or 終了していたら false
	if (m_motionType != motionType || m_bFinishMotion)
	{
		return false;
	}

	// 現在のキーが範囲内か
	if (m_nKey < Key || m_nKey > Key)
	{
		return false;
	}

	// 現在のフレームが範囲内か
	if (m_nKey == Key && m_nCounterMotion < Frame)
	{
		return false;
	}
	if (m_nKey == Key && m_nCounterMotion > Frame)
	{
		return false;
	}

	return true;
}
//=============================================================================
// モーション進行率
//=============================================================================
float CMotion::GetMotionRate(void) const
{
	const MOTION_INFO& info = m_aMotionInfo[m_motionType];

	// 全体フレーム数を算出
	int totalFrame = 0;
	for (int nCnt = 0; nCnt < info.nNumKey; nCnt++)
	{
		totalFrame += info.aKeyInfo[nCnt].nFrame;
	}

	if (totalFrame <= 0)
	{
		return 0.0f;
	}

	// 現在のキーまでの累積フレーム数
	int currentFrame = 0;
	for (int nCnt = 0; nCnt < m_nKey; nCnt++)
	{
		currentFrame += info.aKeyInfo[nCnt].nFrame;
	}

	currentFrame += m_nCounterMotion; // 現在キー内の進行も加算

	// 進行率を返す（0.0 ～ 1.0）
	return (float)currentFrame / (float)totalFrame;
}
//=============================================================================
// モーションフレームの取得
//=============================================================================
int CMotion::GetMotionFrame(void)
{
	return m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].nFrame;
}