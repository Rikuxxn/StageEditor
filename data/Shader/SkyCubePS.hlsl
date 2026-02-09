//=============================================================================
//
// スカイキューブ処理 [SkyCubePS.hlsl]
// Author : RIKU TANEKAWA
//
//=============================================================================

//=============================================================================
// 定数バッファ
//=============================================================================
samplerCUBE gSkyCube : register(s0);// キューブマップ用サンプラ
float3      gLightColor;            // 環境ライト色
float       gExposure;              // 明るさ
float       gTime;                  // 時間
float       gScrollSpeed;           // スクロールスピード

//=============================================================================
// ピクセルシェーダ入力
//=============================================================================
struct PS_INPUT
{
    float3 Dir : TEXCOORD0;
};

//=============================================================================
// ピクセルシェーダ本体
//=============================================================================
float4 PSMain(PS_INPUT In) : COLOR0
{
    // 方向ベクトルの正規化
    float3 dir = normalize(In.Dir);
    
    // Y軸回転（横スクロール）
    float angle = gTime * gScrollSpeed;

    float s = sin(angle);
    float c = cos(angle);

    float3 rotated;
    rotated.x = dir.x * c - dir.z * s;
    rotated.y = dir.y;
    rotated.z = dir.x * s + dir.z * c;
    
    //// キューブマップ取得
    //float4 col = texCUBE(gSkyCube, rotated);

    ////  環境ライト適用
    //col.rgb *= gLightColor;

    //// 露出
    //col.rgb *= gExposure;

    return texCUBE(gSkyCube, rotated);
}
