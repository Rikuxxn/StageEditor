//=============================================================================
//
// ブロックマネージャー処理 [BlockManager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "BlockManager.h"
#include "json.hpp"
#include "FileDialogUtils.h"
#include "Manager.h"
#include "imgui_internal.h"
#include "RayCast.h"
#include "Edit.h"
#include "RigidBody.h"

// JSONの使用
using json = nlohmann::json;

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
std::vector<CBlock*> CBlockManager::m_blocks = {};	// ブロックの情報
int CBlockManager::m_selectedIdx = 0;				// 選択中のインデックス
CBlock* CBlockManager::m_draggingBlock = {};
CBlock* CBlockManager::m_selectedBlock = {};
std::unordered_map<CBlock::TYPE, std::string> CBlockManager::s_FilePathMap; 

//=============================================================================
// コンストラクタ
//=============================================================================
CBlockManager::CBlockManager()
{
	// 値のクリア
	m_selectedBlock		= nullptr;		// 選択中のブロック
	m_prevSelectedIdx	= -1;			// 直前に選択したか
	m_isDragging		= false;		// ドラッグ中か
	m_thumbWidth		= THUMB_WIDTH;	// サムネイルの幅
	m_thumbHeight		= THUMB_HEIGHT;	// サムネイルの高さ
}
//=============================================================================
// デストラクタ
//=============================================================================
CBlockManager::~CBlockManager()
{
	// なし
}
//=============================================================================
// ユニークポインタの生成
//=============================================================================
std::unique_ptr<CBlockManager> CBlockManager::Create(void)
{ 
	return std::make_unique<CBlockManager>();
}
//=============================================================================
// 生成処理
//=============================================================================
CBlock* CBlockManager::CreateBlock(CBlock::TYPE type, D3DXVECTOR3 pos, bool isDynamic)
{
	const char* path = CBlockManager::GetFilePathFromType(type);

	CBlock* newBlock = CBlock::Create(path, pos, D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1), type, isDynamic);

	if (newBlock)
	{
		m_blocks.push_back(newBlock);
	}

	return newBlock;
}
//=============================================================================
// 初期化処理
//=============================================================================
void CBlockManager::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	InitThumbnailRenderTarget(pDevice);

	// モデルリストの読み込み
	LoadConfig("data/ModelList.json");

	// サムネイルキャッシュ作成
	GenerateThumbnailsForResources();

	// 動的配列を空にする (サイズを0にする)
	m_blocks.clear();
}
//=============================================================================
// サムネイルのレンダーターゲットの初期化
//=============================================================================
HRESULT CBlockManager::InitThumbnailRenderTarget(LPDIRECT3DDEVICE9 device)
{
	if (!device)
	{
		return E_FAIL;
	}

	// サムネイル用レンダーターゲットテクスチャの作成
	if (FAILED(device->CreateTexture(
		(UINT)m_thumbWidth, (UINT)m_thumbHeight,
		1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_pThumbnailRT, nullptr)))
	{
		return E_FAIL;
	}

	// サムネイル用深度ステンシルサーフェスの作成
	if (FAILED(device->CreateDepthStencilSurface(
		(UINT)m_thumbWidth, (UINT)m_thumbHeight,
		D3DFMT_D24S8,
		D3DMULTISAMPLE_NONE, 0, TRUE,
		&m_pThumbnailZ, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}
//=============================================================================
// サムネイルのレンダーターゲットの設定
//=============================================================================
IDirect3DTexture9* CBlockManager::RenderThumbnail(CBlock* pBlock)
{
	if (!pBlock || !m_pThumbnailRT || !m_pThumbnailZ)
	{
		return nullptr;
	}

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	if (!pDevice)
	{
		return nullptr;
	}

	// サムネイル描画用の新規テクスチャ作成
	IDirect3DTexture9* pTex = nullptr;
	if (FAILED(pDevice->CreateTexture(
		(UINT)m_thumbWidth, (UINT)m_thumbHeight, 1,
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT, &pTex, nullptr)))
	{
		return nullptr;
	}

	// 現在のレンダーターゲットと深度バッファを保存
	LPDIRECT3DSURFACE9 pOldRT = nullptr;
	LPDIRECT3DSURFACE9 pOldZ = nullptr;
	LPDIRECT3DSURFACE9 pNewRT = nullptr;

	pDevice->GetRenderTarget(0, &pOldRT);
	pDevice->GetDepthStencilSurface(&pOldZ);
	pTex->GetSurfaceLevel(0, &pNewRT);

	// サムネイル用のレンダーターゲットに切り替え
	pDevice->SetRenderTarget(0, pNewRT);
	pDevice->SetDepthStencilSurface(m_pThumbnailZ);

	// クリア
	pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);
	pDevice->BeginScene();

	// 固定カメラ
	D3DXVECTOR3 eye(-50.0f, 80.0f, -80.0f), at(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX matView, matProj;
	D3DXMatrixLookAtLH(&matView, &eye, &at, &up);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&matProj,
		D3DXToRadian(80.0f),						// 視野角
		m_thumbWidth / m_thumbHeight, // アスペクト比
		1.0f,										// 近クリップ面
		1000.0f);									// 遠クリップ面

	pDevice->SetTransform(D3DTS_VIEW, &matView);
	pDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	// 既存のライトを無効化
	CLight::Uninit();

	// ライトの設定処理
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// サムネイル用モデル描画
	pBlock->Draw();

	// 描画終了
	pDevice->EndScene();

	// 元のレンダーターゲットと深度バッファに戻す
	pDevice->SetRenderTarget(0, pOldRT);
	pDevice->SetDepthStencilSurface(pOldZ);

	if (pOldRT) pOldRT->Release();
	if (pOldZ)  pOldZ->Release();
	if (pNewRT) pNewRT->Release();

	return pTex;
}
//=============================================================================
// サムネイル用のモデル生成処理
//=============================================================================
void CBlockManager::GenerateThumbnailsForResources(void)
{
	// 既存サムネイルを解放
	for (auto tex : m_thumbnailTextures)
	{
		if (tex)
		{
			tex->Release();
		}
	}

	m_thumbnailTextures.clear();
	m_thumbnailTextures.resize((size_t)CBlock::TYPE_MAX, nullptr);

	for (size_t i = 0; i < (int)CBlock::TYPE_MAX; ++i)
	{
		// 一時ブロック生成（位置は原点）
		CBlock::TYPE payloadType = static_cast<CBlock::TYPE>(i);
		CBlock* pTemp = CreateBlock(payloadType, D3DXVECTOR3(0, 0, 0), false);
		if (!pTemp)
		{
			continue;
		}

		if (!m_thumbnailTextures[i])
		{
			// サムネイル作成
			m_thumbnailTextures[i] = RenderThumbnail(pTemp);
		}

		pTemp->Kill();                 // 削除フラグを立てる
		CleanupDeadBlocks();           // 配列から取り除き、メモリ解放
	}
}
//=============================================================================
// サムネイルの破棄
//=============================================================================
void CBlockManager::ReleaseThumbnailRenderTarget(void)
{
	// レンダーターゲットの破棄
	if (m_pThumbnailRT)
	{
		m_pThumbnailRT->Release();
		m_pThumbnailRT = nullptr;
	}

	if (m_pThumbnailZ)
	{
		m_pThumbnailZ->Release();
		m_pThumbnailZ = nullptr;
	}

	// サムネイルキャッシュも解放しておく
	for (auto& tex : m_thumbnailTextures)
	{
		if (tex)
		{
			tex->Release();
			tex = nullptr;
		}
	}
	m_thumbnailTextures.clear();
	m_thumbnailsGenerated = false;
}
//=============================================================================
// サムネイルテクスチャの取得処理
//=============================================================================
IDirect3DTexture9* CBlockManager::GetThumbnailTexture(size_t index)
{
	assert(index < m_thumbnailTextures.size());
	return m_thumbnailTextures[index];
}
//=============================================================================
// 終了処理
//=============================================================================
void CBlockManager::Uninit(void)
{
	// サムネイルの破棄
	ReleaseThumbnailRenderTarget();

	// 動的配列を空にする (サイズを0にする)
	m_blocks.clear();
}
//=============================================================================
// 削除予約があるブロックの削除処理
//=============================================================================
void CBlockManager::CleanupDeadBlocks(void)
{
	for (int nCnt = (int)m_blocks.size() - 1; nCnt >= 0; nCnt--)
	{
		if (m_blocks[nCnt]->IsDead())
		{
			// ブロックの終了処理
			m_blocks[nCnt]->Uninit();
			m_blocks.erase(m_blocks.begin() + nCnt);
		}
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void CBlockManager::Update(void)
{
	// 情報の更新
	UpdateInfo();
}
//=============================================================================
// 描画処理
//=============================================================================
void CBlockManager::Draw(void)
{
#ifdef _DEBUG
	//// 選択中のブロックだけコライダー描画
	//CBlock* pSelectBlock = GetSelectedBlock();
	//if (pSelectBlock != nullptr)
	//{
	//	pSelectBlock->DrawCollider();
	//}
#endif
}
//=============================================================================
// 情報の更新処理
//=============================================================================
void CBlockManager::UpdateInfo(void)
{
	// GUIスタイルの取得
	ImGuiStyle& style = ImGui::GetStyle();

	style.GrabRounding		= 10.0f;		// スライダーのつまみを丸く
	style.ScrollbarRounding = 10.0f;		// スクロールバーの角
	style.ChildRounding		= 10.0f;		// 子ウィンドウの角
	style.WindowRounding	= 10.0f;		// ウィンドウ全体の角

	// 場所
	CImGuiManager::Instance().SetPosImgui(ImVec2(1900.0f, 20.0f));

	// サイズ
	CImGuiManager::Instance().SetSizeImgui(ImVec2(420.0f, 500.0f));

	// 最初のGUI
	CImGuiManager::Instance().StartImgui(u8"BlockInfo", CImGuiManager::IMGUITYPE_DEFOULT);

	// ブロックがない場合
	if (m_blocks.empty())
	{
		ImGui::Text("No blocks placed yet.");
		m_selectedIdx = -1;      // 空ならインデックスをリセット
		m_selectedBlock = nullptr;
	}
	else
	{
		// ブロックの総数
		ImGui::Text("Block Num %d", (int)m_blocks.size());

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// インデックス選択
		ImGui::SliderInt("Block Index", &m_selectedIdx, 0, (int)m_blocks.size() - 1);

		// 範囲外対策
		if (m_selectedIdx >= (int)m_blocks.size())
		{
			m_selectedIdx = (int)m_blocks.size() - 1;
		}

		// 前回選んでたブロックを解除（範囲チェック追加）
		if (m_prevSelectedIdx != -1 && m_prevSelectedIdx < (int)m_blocks.size() && m_prevSelectedIdx != m_selectedIdx)
		{
			m_blocks[m_prevSelectedIdx]->SetSelected(false);
		}

		// 対象ブロックの取得（範囲チェック追加）
		if (m_selectedIdx >= 0 && m_selectedIdx < (int)m_blocks.size())
		{
			m_selectedBlock = m_blocks[m_selectedIdx];
			UpdateTransform(m_selectedBlock);
		}
		else
		{
			m_selectedBlock = nullptr;
		}
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

	// ブロックタイプ一覧
	if (ImGui::TreeNode("Block Types"))
	{
		ImGui::BeginChild("BlockTypeList", ImVec2(0, 500), true); // スクロール領域

		int numTypes = (int)CBlock::TYPE_MAX;

		for (int nCnt = 0; nCnt < numTypes; nCnt++)
		{
			IDirect3DTexture9* pThumb = GetThumbnailTexture(nCnt); // サムネイル取得

			if (!pThumb)
			{
				continue; // nullptr はスキップ
			}

			ImGui::PushID(nCnt);
			ImGui::Image(reinterpret_cast<ImTextureID>(pThumb), ImVec2(m_thumbWidth, m_thumbHeight));

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				m_isDragging = true;// ドラッグ中
				CBlock::TYPE payloadType = static_cast<CBlock::TYPE>(nCnt);
				ImGui::SetDragDropPayload("BLOCK_TYPE", &payloadType, sizeof(payloadType));
				ImGui::Text("Block Type %d", nCnt);
				ImGui::Image(reinterpret_cast<ImTextureID>(pThumb), ImVec2(m_thumbWidth, m_thumbHeight));
				ImGui::EndDragDropSource();
			}

			// マウスの取得
			CInputMouse* pMouse = CManager::GetInputMouse();

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && m_isDragging)
			{
				// 現在のImGuiの内部状態（コンテキスト）へのポインターを取得
				ImGuiContext* ctx = ImGui::GetCurrentContext();

				if (ctx->DragDropPayload.IsDataType("BLOCK_TYPE"))
				{
					// ドラッグ中を解除
					m_isDragging = false;

					CBlock::TYPE draggedType = *(CBlock::TYPE*)ctx->DragDropPayload.Data;

					D3DXVECTOR3 pos = pMouse->GetGroundHitPosition();
					pos.y = 30.0f;

					// ブロックの生成
					m_draggingBlock = CreateBlock(draggedType, pos, false);
				}
			}

			ImGui::PopID();

			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 画像間の余白
		}

		ImGui::EndChild();
		ImGui::TreePop();
	}

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

	if (ImGui::Button("Save") || (pKeyboard->GetPress(DIK_LCONTROL) && pKeyboard->GetPress(DIK_S)))
	{
		// ダイアログを開いてファイルに保存
		std::string path = OpenWindowsSaveFileDialog();

		if (!path.empty())
		{
			// データの保存
			CBlockManager::SaveToJson(path.c_str());
		}
	}

	ImGui::SameLine(0);

	if (ImGui::Button("Load"))
	{
		// ダイアログを開いてファイルを開く
		std::string path = OpenWindowsOpenFileDialog();

		if (!path.empty())
		{
			// データの読み込み
			CBlockManager::LoadFromJson(path.c_str());
		}
	}

	ImGui::End();

	// マウス選択処理
	PickBlockFromMouseClick();
}
//=============================================================================
// ブロック情報の調整処理
//=============================================================================
void CBlockManager::UpdateTransform(CBlock* selectedBlock)
{
	if (selectedBlock)
	{
		// 選択中のブロックの色を変える
		selectedBlock->SetSelected(true);

		D3DXVECTOR3 pos = selectedBlock->GetPos();	// 選択中のブロックの位置の取得
		D3DXVECTOR3 rot = selectedBlock->GetRot();	// 選択中のブロックの向きの取得
		D3DXVECTOR3 size = selectedBlock->GetSize();// 選択中のブロックのサイズの取得

		// ラジアン→角度に一時変換（静的変数で保持し操作中のみ更新）
		static D3DXVECTOR3 degRot = D3DXToDegree(rot);
		static bool m_initializedDegRot = false;

		if (!m_initializedDegRot)
		{
			degRot = D3DXToDegree(rot);
			m_initializedDegRot = true;
		}

		// 編集中は動的か静的か設定
		bool isDynamic = selectedBlock->IsDynamicBlock(); // 現在の状態を取得

		// チェックボックス表示（チェックで static / 動的かどうかを切り替え）
		if (ImGui::Checkbox("is Dynamic", &isDynamic)) // チェックしたら true = dynamic
		{
			selectedBlock->SetIsDynamic(isDynamic);
			selectedBlock->RecreatePhysics();

			// 編集モード中は強制的に静的扱いにする
			if (selectedBlock->IsEditMode())
			{
				selectedBlock->SetEditMode(true); // Kinematic化
			}
		}

		//*********************************************************************
		// POS の調整
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// ラベル
		ImGui::Text("POS"); ImGui::SameLine(60); // ラベルの位置ちょっと調整

		// X
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_pos_x", &pos.x, 1.0f, -9000.0f, 9000.0f, "%.1f");

		// Y
		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_pos_y", &pos.y, 1.0f, -9000.0f, 9000.0f, "%.1f");

		// Z
		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_pos_z", &pos.z, 1.0f, -9000.0f, 9000.0f, "%.1f");

		//*********************************************************************
		// ROT の調整
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::Text("ROT"); ImGui::SameLine(60);

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedX = ImGui::DragFloat("##Block_rot_x", &degRot.x, 0.1f, -180.0f, 180.0f, "%.2f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedY = ImGui::DragFloat("##Block_rot_y", &degRot.y, 0.1f, -180.0f, 180.0f, "%.2f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedZ = ImGui::DragFloat("##Block_rot_z", &degRot.z, 0.1f, -180.0f, 180.0f, "%.2f");

		bool isRotChanged = changedX || changedY || changedZ;
		
		//*********************************************************************
		// SIZE の調整
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// ラベル
		ImGui::Text("SIZE"); ImGui::SameLine(60); // ラベルの位置ちょっと調整

		// X
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_size_x", &size.x, 0.1f, -100.0f, 100.0f, "%.1f");

		// Y
		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_size_y", &size.y, 0.1f, -100.0f, 100.0f, "%.1f");

		// Z
		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_size_z", &size.z, 0.1f, -100.0f, 100.0f, "%.1f");

		// 前回のサイズを保持
		static D3DXVECTOR3 prevSize = selectedBlock->GetSize();

		// サイズ変更チェック
		bool isSizeChanged = (size != prevSize);

		//*********************************************************************
		// MASS
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// 質量調整
		float mass = selectedBlock->GetMass();
		ImGui::Text("Mass:");
		ImGui::SameLine();
		ImGui::DragFloat("##Mass", &mass, 1.0f, 0.0f, 50.0f, "%.1f");

		// 角度→ラジアンに戻す
		D3DXVECTOR3 rotRad = D3DXToRadian(degRot);

		// 位置の設定
		selectedBlock->SetPos(pos);

		// サイズの設定
		selectedBlock->SetSize(size);

		// サイズ(拡大率)が変わったときだけ呼ぶ
		if (isSizeChanged)
		{
			prevSize = size; // 更新
		}

		if (isRotChanged)
		{
			// 回転が変わった時だけセット
			selectedBlock->SetRot(rotRad);

			// dynamic ブロックなら Rigidbody にも反映
			if (selectedBlock->IsDynamicBlock() && selectedBlock->GetRigidBody())
			{
				D3DXQUATERNION q;
				D3DXQuaternionRotationYawPitchRoll(&q, rotRad.y, rotRad.x, rotRad.z);
				selectedBlock->GetRigidBody()->SetOrientation(q);
			}
		}
		else
		{
			// 編集していない時はdegRotをselectedBlockの値に同期しておく
			degRot = D3DXToDegree(selectedBlock->GetRot());
		}

		//*********************************************************************
		// ブロックの削除
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));

		if (ImGui::Button("Delete"))
		{
			if (m_blocks[m_selectedIdx])
			{
				// 選択中のブロックを削除
				m_blocks[m_selectedIdx]->Uninit();
			}

			m_blocks.erase(m_blocks.begin() + m_selectedIdx);

			// 選択インデックスを調整
			if (m_selectedIdx >= (int)m_blocks.size())
			{
				m_selectedIdx = (int)m_blocks.size() - 1;
			}

			m_prevSelectedIdx = -1;
		}

		ImGui::PopStyleColor(3);
	}

	// 最後に保存
	m_prevSelectedIdx = m_selectedIdx;
}
//=============================================================================
// ブロック選択処理
//=============================================================================
void CBlockManager::PickBlockFromMouseClick(void)
{
	// ImGuiがマウスを使ってるなら選択処理をキャンセル
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

	// 左クリックのみ
	if (!CManager::GetInputMouse()->GetTrigger(0))
	{
		return;
	}

	// レイ取得（CRayCastを使用）
	D3DXVECTOR3 rayOrigin, rayDir;
	CRayCast::GetMouseRay(rayOrigin, rayDir);

	float minDist = FLT_MAX;
	int hitIndex = -1;

	for (size_t i = 0; i < m_blocks.size(); ++i)
	{
		CBlock* block = m_blocks[i];

		// ワールド行列の取得（位置・回転・拡大を含む）
		D3DXMATRIX world = block->GetWorldMatrix();

		D3DXVECTOR3 modelSize = block->GetModelSize();
		D3DXVECTOR3 scale = block->GetSize();

		// 拡大率を適用
		D3DXVECTOR3 halfSize;
		halfSize.x = modelSize.x * 0.5f;
		halfSize.y = modelSize.y * 0.5f;
		halfSize.z = modelSize.z * 0.5f;

		float dist = 0.0f;

		// OBBとマウスのレイの交差判定
		if (CRayCast::IntersectOBB(rayOrigin, rayDir, world, halfSize, dist))
		{
			if (dist < minDist)
			{
				minDist = dist;
				hitIndex = i;
			}
		}
	}

	// 選択状態を反映
	if (hitIndex >= 0)
	{
		// 以前選ばれていたブロックを非選択に
		if (m_prevSelectedIdx != -1 && m_prevSelectedIdx != hitIndex)
		{
			m_blocks[m_prevSelectedIdx]->SetSelected(false);
		}

		// 新しく選択
		m_selectedIdx = hitIndex;
		m_blocks[m_selectedIdx]->SetSelected(true);
		m_prevSelectedIdx = hitIndex;
	}
}
//=============================================================================
// タイプからXファイルパスを取得
//=============================================================================
const char* CBlockManager::GetFilePathFromType(CBlock::TYPE type)
{
	auto it = s_FilePathMap.find(type);
	return (it != s_FilePathMap.end()) ? it->second.c_str() : "";
}
//=============================================================================
// ブロック情報の保存処理
//=============================================================================
void CBlockManager::SaveToJson(const char* filename)
{
	// JSONオブジェクト
	json j;

	// 1つづつJSON化
	for (const auto& block : m_blocks)
	{
		json b;
		block->SaveToJson(b);

		// 追加
		j.push_back(b);
	}

	// 出力ファイルストリーム
	std::ofstream file(filename);

	if (file.is_open())
	{
		file << std::setw(4) << j;

		// ファイルを閉じる
		file.close();
	}
}
//=============================================================================
// ブロック情報の読み込み処理
//=============================================================================
void CBlockManager::LoadFromJson(const char* filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{// 開けなかった
		return;
	}

	json j;
	file >> j;

	// ファイルを閉じる
	file.close();

	// 既存のブロックを消す
	for (auto block : m_blocks)
	{
		if (block != nullptr)
		{
			// ブロックの終了処理
			block->Uninit();
		}
	}

	// 動的配列を空にする (サイズを0にする)
	m_blocks.clear();
	
	// 新たに生成
	for (const auto& b : j)
	{
		CBlock::TYPE type = b["type"];
		D3DXVECTOR3 pos(b["pos"][0], b["pos"][1], b["pos"][2]);
		bool isDynamic = b["is_dynamic"];

		// ブロックの生成
		CBlock* block = CreateBlock(type, pos, isDynamic);

		if (!block)
		{
			continue;
		}

		block->LoadFromJson(b);
	}
}
//=============================================================================
// モデルリストの読み込み
//=============================================================================
void CBlockManager::LoadConfig(const std::string& filename)
{
	std::ifstream ifs(filename);

	if (!ifs)
	{// 開けなかった
		MessageBox(nullptr, "モデルリストのオープンに失敗 ", "警告！", MB_ICONWARNING);

		return;
	}

	json j;
	ifs >> j;

	// j は配列になってるのでループする
	for (auto& block : j)
	{
		int typeInt = block["type"];
		std::string filepath = block["modelpath"];

		s_FilePathMap[(CBlock::TYPE)typeInt] = filepath;
	}
}
//=============================================================================
// ライト更新処理
//=============================================================================
void CBlockManager::UpdateLight(void)
{
	for (const auto& block : m_blocks)
	{
		block->UpdateLight();
	}
}
//=============================================================================
// 全ブロックの取得
//=============================================================================
std::vector<CBlock*>& CBlockManager::GetAllBlocks(void)
{
	return m_blocks;
}