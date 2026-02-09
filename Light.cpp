//=============================================================================
//
// ライト処理 [Light.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Light.h"
#include "Renderer.h"
#include "Manager.h"
#include "ObjectX.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
int CLight::m_lightCount = 0;
CLight::LightInfo CLight::m_lights[MAX_LIGHTS] = {};

//=============================================================================
// コンストラクタ
//=============================================================================
CLight::CLight()
{
	// 値のクリア
}
//=============================================================================
// デストラクタ
//=============================================================================
CLight::~CLight()
{
	// ライトの終了処理
	Uninit();
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CLight::Init(void)
{
	m_lightCount = 0;

	for (int nCnt = 0; nCnt < MAX_LIGHTS; nCnt++)
	{
		m_lights[nCnt].enabled = false;
	}

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CLight::Uninit(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	for (int nCnt = 0; nCnt < MAX_LIGHTS; nCnt++)
	{
		if (m_lights[nCnt].enabled)
		{
			pDevice->LightEnable(nCnt, FALSE);
			m_lights[nCnt].enabled = false;
		}
	}

	m_lightCount = 0;
}
//=============================================================================
// 更新処理
//=============================================================================
void CLight::Update(void)
{
    // なし
}
//=============================================================================
// ライトの追加処理
//=============================================================================
int CLight::AddLight(D3DLIGHTTYPE type, const D3DXCOLOR& diffuse, const D3DXVECTOR3& direction, const D3DXVECTOR3& position)
{
    if (m_lightCount >= MAX_LIGHTS)
    {
        return -1;
    }

    int index = m_lightCount;
    LightInfo& lightInfo = m_lights[index];

    ZeroMemory(&lightInfo.light, sizeof(D3DLIGHT9));
    lightInfo.light.Type = type;
    lightInfo.light.Diffuse = diffuse;
    lightInfo.position = position;
    lightInfo.light.Position = position;

//#ifdef DEBUG

    //// ライト用モデルの生成
    //CObjectX::Create("data/MODELS/test_block.x", position, D3DXVECTOR3(0.0f, 0.0f, 0.0f));

//#endif // DEBUG

    D3DXVECTOR3 dir = direction;
    if (D3DXVec3Length(&dir) == 0.0f)
    {
        dir = D3DXVECTOR3(0, -1, 0); // デフォルト下向き
    }

    D3DXVec3Normalize(&lightInfo.direction, &dir);
    lightInfo.light.Direction = lightInfo.direction;

    // ライトの種類に応じた設定
    if (type == D3DLIGHT_POINT)
    {
        lightInfo.light.Attenuation0 = 0.0f;
        lightInfo.light.Attenuation1 = 0.02f;
        lightInfo.light.Attenuation2 = 0.0f;
        lightInfo.light.Range = 105.0f;
    }
    else if (type == D3DLIGHT_SPOT)
    {
        lightInfo.light.Range = 1000.0f;
        lightInfo.light.Theta = D3DXToRadian(70.0f);  // 明るく照らす範囲（中心）
        lightInfo.light.Phi = D3DXToRadian(95.0f);  // 減衰していく外側        
        lightInfo.light.Falloff = 1.0f;
        lightInfo.light.Attenuation0 = 1.0f;
        lightInfo.light.Attenuation1 = 0.05f;    // 緩やかな減衰
        lightInfo.light.Attenuation2 = 0.0f;
    }

    // デバイスの取得
    LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

    pDevice->SetLight(index, &lightInfo.light);
    pDevice->LightEnable(index, TRUE);

    lightInfo.enabled = true;

    // ライトのカウントを増やす
    m_lightCount++;

    return index;
}
//=============================================================================
// ライトの削除処理
//=============================================================================
void CLight::DeleteLight(int index)
{
    if (index >= 0 && index < MAX_LIGHTS && m_lights[index].enabled)
    {
        // デバイスの取得
        LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

        // ライトをオフにする
        pDevice->LightEnable(index, FALSE);
        m_lights[index].enabled = false;

        // ライトのカウントを減らす
        m_lightCount--;
    }
}