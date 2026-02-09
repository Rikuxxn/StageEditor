//=============================================================================
//
// ブロックマネージャー処理 [bBockManager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCKMANAGER_H_
#define _BLOCKMANAGER_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Block.h"
#include "cassert"

//*****************************************************************************
// ブロックマネージャークラス
//*****************************************************************************
class CBlockManager
{
public:
	CBlockManager();
	~CBlockManager();

    static std::unique_ptr<CBlockManager>Create(void);// ユニークポインタの生成
    static CBlock* CreateBlock(CBlock::TYPE type, D3DXVECTOR3 pos, bool isDynamic);
    void Init(void);
    void Uninit(void);// 終了処理
    void CleanupDeadBlocks(void);// 削除予約があるブロックの削除
    void Update(void);
    void Draw(void);
    void UpdateInfo(void); // ImGuiでの操作関数をここで呼ぶ用
    void SaveToJson(const char* filename);
    void LoadFromJson(const char* filename);
    void LoadConfig(const std::string& filename);
    void UpdateLight(void);

    //*****************************************************************************
    // ImGuiサムネイル描画用関数
    //*****************************************************************************
    void ReleaseThumbnailRenderTarget(void);
    HRESULT InitThumbnailRenderTarget(LPDIRECT3DDEVICE9 device);
    IDirect3DTexture9* RenderThumbnail(CBlock* pBlock);
    void GenerateThumbnailsForResources(void);
    IDirect3DTexture9* GetThumbnailTexture(size_t index);


    //*****************************************************************************
    // ImGuiでの操作関数
    //*****************************************************************************
    void UpdateTransform(CBlock* selectedBlock);
    void PickBlockFromMouseClick(void);

    //*****************************************************************************
    // getter関数
    //*****************************************************************************
    static std::vector<CBlock*>& GetAllBlocks(void);
    static CBlock* GetSelectedBlock(void) { return m_selectedBlock; }

private:
    static const char* GetFilePathFromType(CBlock::TYPE type);

private:
    static constexpr float THUMB_WIDTH = 100.0f;// サムネイルの高さ
    static constexpr float THUMB_HEIGHT = 100.0f;// サムネイルの高さ

    //*****************************************************************************
    // ブロック管理
    //*****************************************************************************
    static std::vector<CBlock*> m_blocks;               // ブロック情報
    static CBlock*              m_draggingBlock;        // ドラッグ中のブロック情報
    static CBlock*              m_selectedBlock;        // 選択中のブロック
    static int                  m_selectedIdx;          // 選択中のインデックス
    int                         m_prevSelectedIdx;      // 前回の選択中のインデックス
    bool                        m_isDragging;           // ドラッグ中か

    //*****************************************************************************
    // ファイルパス管理
    //*****************************************************************************
    static std::unordered_map<CBlock::TYPE, std::string> s_FilePathMap;


    //*****************************************************************************
    // サムネイル用リソース
    //*****************************************************************************
    LPDIRECT3DTEXTURE9              m_pThumbnailRT;
    LPDIRECT3DSURFACE9              m_pThumbnailZ;
    std::vector<IDirect3DTexture9*> m_thumbnailTextures;            // サムネイルのテクスチャ
    float                           m_thumbWidth;                   // サムネイルの幅
    float                           m_thumbHeight;                  // サムネイルの高さ
    bool                            m_thumbnailsGenerated = false;  // 一度だけ作るフラグ
};

#endif
