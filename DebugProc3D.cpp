//=============================================================================
//
// 3Dデバッグ表示処理 [DebugProc3D.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================
#include "DebugProc3D.h"
#include "Renderer.h"
#include "Manager.h"
#include "Collider.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
LPD3DXLINE CDebugProc3D::m_pLine = nullptr;

//=============================================================================
// コンストラクタ
//=============================================================================
CDebugProc3D::CDebugProc3D()
{
	// 値のクリア

}
//=============================================================================
// デストラクタ
//=============================================================================
CDebugProc3D::~CDebugProc3D()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
void CDebugProc3D::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	if (pDevice != nullptr)
	{
		D3DXCreateLine(pDevice, &m_pLine);
	}
}
//=============================================================================
// 終了処理
//=============================================================================
void CDebugProc3D::Uninit(void)
{
	// ライン表示の破棄
	if (m_pLine != nullptr)
	{
		m_pLine->Release();
		m_pLine = nullptr;
	}
}
//=============================================================================
// ライン描画処理
//=============================================================================
void CDebugProc3D::DrawLine3D(const D3DXVECTOR3& start, const D3DXVECTOR3& end, D3DXCOLOR color)
{
    // デバイスの取得
    LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	struct VERTEX
	{
		D3DXVECTOR3 pos;
		D3DCOLOR color;
	};

	VERTEX v[2] =
    {
		{ start, color },
		{ end,   color }
	};

	// 頂点フォーマット設定
	pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);         // ライトを無効にする
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); // αテストを無効にする

	// ライン描画
	pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, v, sizeof(VERTEX));

    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);  // αテストを有効にする
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);          // ライトを有効にする
}
//=============================================================================
// コライダー描画処理
//=============================================================================
void CDebugProc3D::DrawCollider(Collider* shape, D3DXCOLOR color)
{
    if (auto capsule = shape->As<CapsuleCollider>())
    {
        // カプセルコライダーの描画
        DrawCapsuleCollider(capsule, color);
    }
    else if (auto box = shape->As<BoxCollider>())
    {
        // ボックスコライダーの描画
        DrawBoxCollider(box, color);
    }
    else if (auto cylinder = shape->As<CylinderCollider>())
    {
        // シリンダーコライダーの描画
        DrawCylinderCollider(cylinder, color);
    }
    else if (auto sphere = shape->As<SphereCollider>())
    {
        // スフィアコライダーの描画
        DrawSphereCollider(sphere, color);
    }
}
//=============================================================================
// ボックスコライダー描画処理
//=============================================================================
void CDebugProc3D::DrawBoxCollider(BoxCollider* box, D3DXCOLOR color)
{
    if (!box || !m_pLine)
    {
        return;
    }

    // デバイス取得
    LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

    // ワールド行列作成
    D3DXMATRIX matRot, matTrans, matWorld;
    D3DXMatrixRotationQuaternion(&matRot, &box->GetRotationQuat());
    D3DXMatrixTranslation(&matTrans, box->GetPosition().x, box->GetPosition().y, box->GetPosition().z);

    // ワールド行列 = 回転 × 平行移動
    matWorld = matRot * matTrans;

    // デバイスにワールド行列を設定
    pDevice->SetTransform(D3DTS_WORLD, &matWorld);

    // 半分サイズ取得
    D3DXVECTOR3 half = box->GetScaledSize() * HALF;

    // 8頂点
    D3DXVECTOR3 v[VERTEX] = 
    {
        {-half.x, -half.y, -half.z}, {half.x, -half.y, -half.z},
        {half.x, half.y, -half.z}, {-half.x, half.y, -half.z},
        {-half.x, -half.y, half.z}, {half.x, -half.y, half.z},
        {half.x, half.y, half.z}, {-half.x, half.y, half.z},
    };

    int edges[12][2] =
    {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    for (int nCnt = 0; nCnt < 12; nCnt++)
    {
        DrawLine3D(v[edges[nCnt][0]], v[edges[nCnt][1]], color);
    }
}
//=============================================================================
// カプセルコライダー描画処理
//=============================================================================
void CDebugProc3D::DrawCapsuleCollider(CapsuleCollider* capsule, D3DXCOLOR color)
{
    if (!capsule || !m_pLine)
    {
        return;
    }

    // デバイスの取得
    LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

    const int kNumSegments = 12;  // 分割数
    const int kNumRings = 4;      // 縦分割

    float radius = capsule->GetRadius();
    float halfHeight = capsule->GetHalfHeight();

    // capsuleCollider から座標取得
    D3DXVECTOR3 base = capsule->GetPosition();

    // 回転なし（固定）
    D3DXMATRIX matRot;
    D3DXMatrixIdentity(&matRot);

    // 平行移動行列
    D3DXMATRIX matTrans;
    D3DXMatrixTranslation(&matTrans, base.x, base.y, base.z);

    // ワールド行列 = 回転 × 平行移動
    D3DXMATRIX matWorld = matRot * matTrans;

    // デバイスにワールド行列を設定
    pDevice->SetTransform(D3DTS_WORLD, &matWorld);

    // カプセル方向はY軸固定
    D3DXVECTOR3 up(0, 1, 0);
    D3DXVECTOR3 side1(1, 0, 0);
    D3DXVECTOR3 side2(0, 0, 1);

    D3DXVECTOR3 top = up * halfHeight;
    D3DXVECTOR3 bottom = -up * halfHeight;

    for (int nCnt = 0; nCnt < kNumSegments; nCnt++)
    {
        float theta1 = (2.0f * D3DX_PI * nCnt) / kNumSegments;
        float theta2 = (2.0f * D3DX_PI * (nCnt + 1)) / kNumSegments;

        D3DXVECTOR3 dir1 = cosf(theta1) * side1 + sinf(theta1) * side2;
        D3DXVECTOR3 dir2 = cosf(theta2) * side1 + sinf(theta2) * side2;

        dir1 *= radius;
        dir2 *= radius;

        // 側面の線
        DrawLine3D(D3DXVECTOR3((top + dir1).x, (top + dir1).y, (top + dir1).z),
            D3DXVECTOR3((bottom + dir1).x, (bottom + dir1).y, (bottom + dir1).z), color);
    }

    // 上半球を描く
    for (int ring = 0; ring < kNumRings; ring++)
    {
        float theta1 = (D3DX_PI * HALF) * (ring / (float)kNumRings);       // 0 ～ π/2
        float theta2 = (D3DX_PI * HALF) * ((ring + 1) / (float)kNumRings);

        for (int seg = 0; seg < kNumSegments; seg++)
        {
            float phi1 = (DOUBLE * D3DX_PI * seg) / kNumSegments;
            float phi2 = (DOUBLE * D3DX_PI * (seg + 1)) / kNumSegments;

            // 球面座標 → 3D位置 (半径1の単位球ベース)
            D3DXVECTOR3 p1 = D3DXVECTOR3(
                cosf(phi1) * sinf(theta1),
                cosf(theta1),
                sinf(phi1) * sinf(theta1)
            );
            D3DXVECTOR3 p2 = D3DXVECTOR3(
                cosf(phi2) * sinf(theta1),
                cosf(theta1),
                sinf(phi2) * sinf(theta1)
            );
            D3DXVECTOR3 p3 = D3DXVECTOR3(
                cosf(phi2) * sinf(theta2),
                cosf(theta2),
                sinf(phi2) * sinf(theta2)
            );
            D3DXVECTOR3 p4 = D3DXVECTOR3(
                cosf(phi1) * sinf(theta2),
                cosf(theta2),
                sinf(phi1) * sinf(theta2)
            );

            // 半球なので、Y成分がup方向 → topにオフセット、半径スケール
            p1 = top + p1 * radius;
            p2 = top + p2 * radius;
            p3 = top + p3 * radius;
            p4 = top + p4 * radius;

            // 4点で四角形 → 2本の線でワイヤーフレーム
            DrawLine3D(D3DXVECTOR3(p1.x, p1.y, p1.z), D3DXVECTOR3(p2.x, p2.y, p2.z), color);
            DrawLine3D(D3DXVECTOR3(p2.x, p2.y, p2.z), D3DXVECTOR3(p3.x, p3.y, p3.z), color);
            DrawLine3D(D3DXVECTOR3(p3.x, p3.y, p3.z), D3DXVECTOR3(p4.x, p4.y, p4.z), color);
            DrawLine3D(D3DXVECTOR3(p4.x, p4.y, p4.z), D3DXVECTOR3(p1.x, p1.y, p1.z), color);
        }
    }

    // 下半球を描く
    for (int ring = 0; ring < kNumRings; ring++)
    {
        float theta1 = (D3DX_PI * HALF) * (ring / (float)kNumRings);
        float theta2 = (D3DX_PI * HALF) * ((ring + 1) / (float)kNumRings);

        for (int seg = 0; seg < kNumSegments; seg++)
        {
            float phi1 = (DOUBLE * D3DX_PI * seg) / kNumSegments;
            float phi2 = (DOUBLE * D3DX_PI * (seg + 1)) / kNumSegments;

            D3DXVECTOR3 p1 = D3DXVECTOR3(
                cosf(phi1) * sinf(theta1),
                -cosf(theta1),  // Y反転
                sinf(phi1) * sinf(theta1)
            );
            D3DXVECTOR3 p2 = D3DXVECTOR3(
                cosf(phi2) * sinf(theta1),
                -cosf(theta1),
                sinf(phi2) * sinf(theta1)
            );
            D3DXVECTOR3 p3 = D3DXVECTOR3(
                cosf(phi2) * sinf(theta2),
                -cosf(theta2),
                sinf(phi2) * sinf(theta2)
            );
            D3DXVECTOR3 p4 = D3DXVECTOR3(
                cosf(phi1) * sinf(theta2),
                -cosf(theta2),
                sinf(phi1) * sinf(theta2)
            );

            p1 = bottom + p1 * radius;
            p2 = bottom + p2 * radius;
            p3 = bottom + p3 * radius;
            p4 = bottom + p4 * radius;

            DrawLine3D(D3DXVECTOR3(p1.x, p1.y, p1.z), D3DXVECTOR3(p2.x, p2.y, p2.z), color);
            DrawLine3D(D3DXVECTOR3(p2.x, p2.y, p2.z), D3DXVECTOR3(p3.x, p3.y, p3.z), color);
            DrawLine3D(D3DXVECTOR3(p3.x, p3.y, p3.z), D3DXVECTOR3(p4.x, p4.y, p4.z), color);
            DrawLine3D(D3DXVECTOR3(p4.x, p4.y, p4.z), D3DXVECTOR3(p1.x, p1.y, p1.z), color);
        }
    }
}
//=============================================================================
// シリンダーコライダー描画処理
//=============================================================================
void CDebugProc3D::DrawCylinderCollider(CylinderCollider* cylinder, D3DXCOLOR color)
{
    if (!cylinder || !m_pLine)
    {
        return;
    }

    // デバイスの取得
    LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

    // ワールド変換
    D3DXMATRIX matRot, matTrans, matWorld;
    D3DXMatrixRotationQuaternion(&matRot, &cylinder->GetRotationQuat());
    D3DXMatrixTranslation(&matTrans, cylinder->GetPosition().x, cylinder->GetPosition().y, cylinder->GetPosition().z);

    // ワールド行列 = 回転 × 平行移動
    matWorld = matRot * matTrans;

    // デバイスにワールド行列を設定
    pDevice->SetTransform(D3DTS_WORLD, &matWorld);

    const int kSegments = 16;
    float r = cylinder->GetRadius();
    float halfH = cylinder->GetHeight() * HALF;

    // 上下円の頂点を計算
    for (int nCnt = 0; nCnt < kSegments; nCnt++)
    {
        float theta1 = (DOUBLE * D3DX_PI * nCnt) / kSegments;
        float theta2 = (DOUBLE * D3DX_PI * (nCnt + 1)) / kSegments;

        D3DXVECTOR3 top1(cosf(theta1) * r, halfH, sinf(theta1) * r);
        D3DXVECTOR3 top2(cosf(theta2) * r, halfH, sinf(theta2) * r);
        D3DXVECTOR3 bottom1(cosf(theta1) * r, -halfH, sinf(theta1) * r);
        D3DXVECTOR3 bottom2(cosf(theta2) * r, -halfH, sinf(theta2) * r);

        // 側面
        DrawLine3D(top1, bottom1, color);

        // 上円
        DrawLine3D(top1, top2, color);

        // 下円
        DrawLine3D(bottom1, bottom2, color);
    }
}
//=============================================================================
// スフィアコライダー描画処理
//=============================================================================
void CDebugProc3D::DrawSphereCollider(SphereCollider* sphere, D3DXCOLOR color)
{
    if (!sphere || !m_pLine)
    {
        return;
    }

    // デバイスの取得
    LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

    // ワールド変換
    D3DXMATRIX matTrans;
    D3DXMatrixTranslation(&matTrans, sphere->GetPosition().x, sphere->GetPosition().y, sphere->GetPosition().z);

    // デバイスにワールド行列を設定
    pDevice->SetTransform(D3DTS_WORLD, &matTrans);

    const int kSegments = 16;
    float r = sphere->GetRadius();
    D3DXVECTOR3 center(0, 0, 0);

    for (int nCnt = 0; nCnt < kSegments; nCnt++)
    {
        float t1 = (DOUBLE * D3DX_PI * nCnt) / kSegments;
        float t2 = (DOUBLE * D3DX_PI * (nCnt + 1)) / kSegments;

        // XY
        DrawLine3D(center + D3DXVECTOR3(cosf(t1), sinf(t1), 0) * r,
            center + D3DXVECTOR3(cosf(t2), sinf(t2), 0) * r, color);

        // YZ
        DrawLine3D(center + D3DXVECTOR3(0, cosf(t1), sinf(t1)) * r,
            center + D3DXVECTOR3(0, cosf(t2), sinf(t2)) * r, color);

        // XZ
        DrawLine3D(center + D3DXVECTOR3(cosf(t1), 0, sinf(t1)) * r,
            center + D3DXVECTOR3(cosf(t2), 0, sinf(t2)) * r, color);
    }
}
