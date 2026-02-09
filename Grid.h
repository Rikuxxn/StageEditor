//=============================================================================
//
// グリッド処理 [Grid.h]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// グリッドクラス
//*****************************************************************************
class CGrid
{
public:
    CGrid();
    ~CGrid();

    static std::unique_ptr<CGrid>Create(void);// ユニークポインタの生成
    HRESULT Init(void);
    void Draw(void);
    void AddLine(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, D3DCOLOR col);

private:
    static constexpr int    SIZE    = 15;       // 半径
    static constexpr float  SPACING = 30.0f;    // マス間隔

    // グリッド構造体
    struct CUSTOMVERTEX
    {
        D3DXVECTOR3 position;
        D3DCOLOR    color;
    };

    D3DXMATRIX                  m_mtxWorld;   // ワールドマトリックス
    std::vector<CUSTOMVERTEX>   m_vertices;   // 頂点
    int                         m_size;       // 半径
    float                       m_spacing;    // マス間隔
};

