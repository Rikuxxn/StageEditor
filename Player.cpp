//=============================================================================
//
// プレイヤー処理 [Player.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "Player.h"
#include "Texture.h"
#include "Manager.h"
#include "PlayerState.h"
#include "Collider.h"
#include "RigidBody.h"

namespace ColliderParam
{
	namespace Pos
	{
		const D3DXVECTOR3 OFFSET{ 0.0f, 36.0f, 0.0f };
	}

	const D3DXCOLOR COLOR{ 0.0f, 1.0f, 0.3f, 1.0f };
}

//=============================================================================
// コンストラクタ
//=============================================================================
CPlayer::CPlayer(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	memset(m_apModel, 0, sizeof(m_apModel));			// モデル(パーツ)へのポインタ
	m_pos				= INIT_VEC3;					// 位置
	m_rot				= INIT_VEC3;					// 向き
	m_rotDest			= INIT_VEC3;					// 目標の向き
	m_move				= INIT_VEC3;					// 移動量
	m_targetMove		= INIT_VEC3;					// 目標速度
	m_currentMove		= INIT_VEC3;					// 実際の移動速度
	m_size				= D3DXVECTOR3(1.0f, 1.0f, 1.0f);// サイズ
	m_mtxWorld			= {};							// ワールドマトリックス
	m_nNumModel			= 0;							// モデル(パーツ)の総数
	m_pMotion			= nullptr;						// モーションへのポインタ
	m_bIsMoving			= false;						// 移動入力フラグ
	m_bOnGround			= false;						// 接地フラグ
	m_pDebug3D			= nullptr;						// 3Dデバッグ表示へのポインタ
	m_radius			= 0.0f;							// カプセルコライダーの半径
	m_height			= 0.0f;							// カプセルコライダーの高さ
}
//=============================================================================
// デストラクタ
//=============================================================================
CPlayer::~CPlayer()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CPlayer* CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CPlayer* pPlayer = new CPlayer;

	// nullptrだったら
	if (pPlayer == nullptr)
	{
		return nullptr;
	}

	pPlayer->m_pos = pos;
	pPlayer->m_rot = D3DXToRadian(rot);

	// 初期化失敗時
	if (FAILED(pPlayer->Init()))
	{
		return nullptr;
	}

	return pPlayer;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CPlayer::Init(void)
{
	// 名前空間ColliderParamの使用
	using namespace ColliderParam;

	CModel* pModels[MAX_PARTS];
	int nNumModels = 0;

	// パーツの読み込み
	m_pMotion = CMotion::Load("data/motion.txt", pModels, nNumModels, MAX);

	for (int nCnt = 0; nCnt < nNumModels && nCnt < MAX_PARTS; nCnt++)
	{
		m_apModel[nCnt] = pModels[nCnt];

		// オフセット考慮
		m_apModel[nCnt]->SetOffsetPos(m_apModel[nCnt]->GetPos());
		m_apModel[nCnt]->SetOffsetRot(m_apModel[nCnt]->GetRot());
	}

	// パーツ数を代入
	m_nNumModel = nNumModels;

	//*********************************************************************
	// カプセルコライダーの設定
	//*********************************************************************

	m_radius = CAPSULE_RADIUS;
	m_height = CAPSULE_HEIGHT;

	m_pShape = std::make_shared<CapsuleCollider>(m_radius, m_height);

	// コライダー中心 = 足元 + オフセット
	m_colliderPos = m_pos + Pos::OFFSET;

	// 質量を設定
	D3DXVECTOR3 inertia(0, 0, 0);  // 慣性

	m_pShape->calculateLocalInertia(MASS, inertia);

	// リジッドボディの生成
	m_pRigidBody = std::make_shared<RigidBody>(m_pShape, MASS);

	D3DXVECTOR3 euler = GetRot(); // オイラー角（ラジアン）
	D3DXQUATERNION q;
	D3DXQuaternionRotationYawPitchRoll(&q, euler.y, euler.x, euler.z);

	// 位置の設定
	m_pRigidBody->SetTransform(m_colliderPos, q, GetSize());

	m_pRigidBody->SetIsDynamic(true);			// ダイナミックブロックかどうか

	m_pRigidBody->SetLinearFactor(D3DXVECTOR3(1, 1, 1));
	m_pRigidBody->SetAngularFactor(D3DXVECTOR3(0, 0, 0));
	m_pRigidBody->SetFriction(1.5f);// 摩擦
	m_pRigidBody->SetRollingFriction(0.0f);// 転がり摩擦

	// 物理ワールドに追加
	PhysicsWorld* pWorld = CManager::GetPhysicsWorld();

	if (pWorld != nullptr)
	{
		pWorld->AddRigidBody(m_pRigidBody);
	}

	//// ステンシルシャドウの生成
	//m_pShadowS = CShadowS::Create("data/MODELS/stencilshadow.x",GetPos());
	//m_pShadowS->SetStencilRef(1);// 個別のステンシルバッファの参照値を設定

	// インスタンスのポインタを渡す
	m_stateMachine.Start(this);

	// 初期状態のステートをセット
	m_stateMachine.ChangeState<CPlayerStandState>();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CPlayer::Uninit(void)
{
	ReleasePhysics();

	for (int nCnt = 0; nCnt < MAX_PARTS; nCnt++)
	{
		if (m_apModel[nCnt] != nullptr)
		{
			m_apModel[nCnt]->Uninit();
			delete m_apModel[nCnt];
			m_apModel[nCnt] = nullptr;
		}
	}

	if (m_pMotion != nullptr)
	{
		delete m_pMotion;
		m_pMotion = nullptr;
	}

	// オブジェクトの破棄(自分自身)
	this->Release();
}
//=============================================================================
// Physicsの破棄
//=============================================================================
void CPlayer::ReleasePhysics(void)
{
	auto world = CManager::GetPhysicsWorld();

	//// 剛体やシェイプを消す前に派生クラス用の特殊処理破棄を呼ぶ
	//OnPhysicsReleased();

	// リジッドボディの破棄
	if (m_pRigidBody)
	{
		if (world)
		{
			world->RemoveRigidBody(m_pRigidBody);
		}

		m_pRigidBody = nullptr;
	}

	// シェイプの破棄
	if (m_pShape)
	{
		m_pShape = nullptr;
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void CPlayer::Update(void)
{
	// 名前空間ColliderParamの使用
	using namespace ColliderParam;

	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	// カメラの角度の取得
	D3DXVECTOR3 CamRot = pCamera->GetRot();

	// ステートマシン更新
	m_stateMachine.Update();

	// 入力判定の取得
	InputData input = GatherInput();

	// 向きの更新処理
	if (m_rotDest.y - m_rot.y > D3DX_PI)
	{
		m_rot.y += D3DX_PI * DOUBLE;
	}
	else if (m_rotDest.y - m_rot.y < -D3DX_PI)
	{
		m_rot.y -= D3DX_PI * DOUBLE;
	}

	m_rot.y += (m_rotDest.y - m_rot.y) * 0.09f;

	// 移動入力があればプレイヤー向きを入力方向に
	if ((input.moveDir.x != 0.0f || input.moveDir.z != 0.0f))
	{
		m_rotDest.y = atan2f(-input.moveDir.x, -input.moveDir.z);
	}

	// クォータニオンにして Rigidbody に渡す
	D3DXQUATERNION q;
	D3DXQuaternionRotationYawPitchRoll(&q, m_rot.y, 0, 0);
	m_pRigidBody->SetOrientation(q);

	// Rigidbody から物理座標を取得（カプセル中心）
	D3DXVECTOR3 rigidPos = m_pRigidBody->GetPosition();

	// カプセルコライダーに反映
	m_colliderPos = rigidPos;

	// モデル描画やゲーム上の座標は足元基準に変換
	m_pos = rigidPos - Pos::OFFSET;
}
//=============================================================================
// 描画処理
//=============================================================================
void CPlayer::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxSize;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// サイズを反映
	D3DXMatrixScaling(&mtxSize, GetSize().x, GetSize().y, GetSize().z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxSize);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, GetRot().y, GetRot().x, GetRot().z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, GetPos().x, GetPos().y, GetPos().z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	for (int nCntMat = 0; nCntMat < m_nNumModel; nCntMat++)
	{
		// モデル(パーツ)の描画
		if (m_apModel[nCntMat])
		{
			m_apModel[nCntMat]->Draw();
		}
	}

	// カプセルコライダーの描画
	if (auto cap = m_pShape->As<CapsuleCollider>())
	{
		m_pDebug3D->DrawCapsuleCollider(cap, ColliderParam::COLOR);
	}
}
//=============================================================================
// プレイヤーの前方ベクトル取得
//=============================================================================
D3DXVECTOR3 CPlayer::GetForward(void)
{
	// プレイヤーの回転角度（Y軸）から前方ベクトルを計算
	float yaw = GetRot().y;

	D3DXVECTOR3 forward(-sinf(yaw), 0.0f, -cosf(yaw));

	// 正規化する
	D3DXVec3Normalize(&forward, &forward);

	return forward;
}
//=============================================================================
// 入力判定取得関数
//=============================================================================
CPlayer::InputData CPlayer::GatherInput(void)
{
	InputData input{};
	input.moveDir = INIT_VEC3;

	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();	// キーボードの取得
	CInputJoypad* pJoypad = CManager::GetInputJoypad();			// ジョイパッドの取得
	XINPUT_STATE* pStick = CInputJoypad::GetStickAngle();		// スティックの取得
	CCamera* pCamera = CManager::GetCamera();					// カメラの取得
	D3DXVECTOR3 CamRot = pCamera->GetRot();						// カメラ角度の取得

	// ---------------------------
	// ゲームパッド入力
	// ---------------------------
	if (pJoypad->GetStick() && pStick)
	{
		float stickX = pStick->Gamepad.sThumbLX;
		float stickY = pStick->Gamepad.sThumbLY;
		float magnitude = sqrtf(stickX * stickX + stickY * stickY);

		if (magnitude >= CInputJoypad::DEADZONE)
		{
			stickX /= magnitude;
			stickY /= magnitude;
			float normMag = std::min((magnitude - CInputJoypad::DEADZONE) / (32767.0f - CInputJoypad::DEADZONE), 1.0f);
			stickX *= normMag;
			stickY *= normMag;

			D3DXVECTOR3 dir;
			float yaw = CamRot.y;

			dir.x = -(stickX * cosf(yaw) + stickY * sinf(yaw));
			dir.z = stickX * sinf(-yaw) + stickY * cosf(yaw);
			dir.z = -dir.z;

			input.moveDir += D3DXVECTOR3(dir.x, 0, dir.z);
		}
	}

	// ---------------------------
	// キーボード入力
	// ---------------------------
	if (pKeyboard->GetPress(DIK_W))
	{
		input.moveDir += D3DXVECTOR3(-sinf(CamRot.y), 0, -cosf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_S))
	{
		input.moveDir += D3DXVECTOR3(sinf(CamRot.y), 0, cosf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_A))
	{
		input.moveDir += D3DXVECTOR3(cosf(CamRot.y), 0, -sinf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_D))
	{
		input.moveDir += D3DXVECTOR3(-cosf(CamRot.y), 0, sinf(CamRot.y));
	}

	// 正規化
	if (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f)
	{
		D3DXVec3Normalize(&input.moveDir, &input.moveDir);
	}

	return input;
}
