//=============================================================================
//
// ファイルダイアログ処理 [FileDialogUtils.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "FileDialogUtils.h"

//=============================================================================
// ファイルを保存
//=============================================================================
std::string OpenWindowsSaveFileDialog(void)
{
    CHAR szFile[MAX_PATH] = { 0 };

    OPENFILENAMEA ofn = { 0 };
    ofn.lStructSize   = sizeof(OPENFILENAMEA);
    ofn.hwndOwner     = NULL; // ウィンドウハンドル
    ofn.lpstrFilter   = "JSON Files\0*.json\0All Files\0*.*\0"; // ファイルの種類の名前
    ofn.lpstrFile     = szFile;
    ofn.nMaxFile      = MAX_PATH;
    ofn.Flags         = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER;
    ofn.lpstrDefExt   = "json";

    if (GetSaveFileNameA(&ofn))
    {
        return std::string(szFile);
    }
    return std::string();
}
//=============================================================================
// ファイルを開く
//=============================================================================
std::string OpenWindowsOpenFileDialog(void)
{
    CHAR szFile[MAX_PATH] = { 0 };

    OPENFILENAMEA ofn = { 0 };
    ofn.lStructSize   = sizeof(OPENFILENAMEA);
    ofn.hwndOwner     = NULL;
    ofn.lpstrFilter   = "JSON Files\0*.json\0All Files\0*.*\0"; // ファイルの種類の名前
    ofn.lpstrFile     = szFile;
    ofn.nMaxFile      = MAX_PATH;
    ofn.Flags         = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER;
    ofn.lpstrDefExt   = "json";

    if (GetOpenFileNameA(&ofn))
    {
        return std::string(szFile);
    }
    return std::string();
}
