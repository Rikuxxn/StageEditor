//=============================================================================
//
// レイキャスト処理 [RayCast.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RAYCAST_H_// このマクロ定義がされていなかったら
#define _RAYCAST_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************

//*****************************************************************************
// レイキャストクラス
//*****************************************************************************
class CRayCast
{
public:

    // マウス位置からワールドレイを取得
    static bool GetMouseRay(D3DXVECTOR3& outOrigin, D3DXVECTOR3& outDir);

    // レイとAABBの当たり判定
    static bool IntersectAABB(const D3DXVECTOR3& rayOrigin,
        const D3DXVECTOR3& rayDir,
        const D3DXVECTOR3& boxMin,
        const D3DXVECTOR3& boxMax,
        float& outDist);

    // レイとOBBの当たり判定
    static bool IntersectOBB(
        const D3DXVECTOR3& rayOrigin,
        const D3DXVECTOR3& rayDir,
        const D3DXMATRIX& worldMatrix,
        const D3DXVECTOR3& halfSize,
        float& outDistance
    );

private:
    static constexpr int AXIS = 3;// 各軸
};

#endif