//=============================================================================
//
// ファイルダイアログ処理 [FileDialogUtils.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _FILEDIALOGUTILS_H_// このマクロ定義がされていなかったら
#define _FILEDIALOGUTILS_H_// 2重インクルード防止のマクロ定義

std::string OpenWindowsSaveFileDialog(void);
std::string OpenWindowsOpenFileDialog(void);

#endif