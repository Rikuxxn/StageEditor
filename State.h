//=============================================================================
//
// 状態処理 [State.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _STATE_H_
#define _STATE_H_

template<typename OwnerType>

//*****************************************************************************
// 前方宣言
//*****************************************************************************
class StateMachine;

//*****************************************************************************
// 全ての状態の基底となるクラス
//*****************************************************************************
template<typename OwnerType> // テンプレート引数で状態を管理する対象の型を指定
class StateBase
{
protected:
	friend class StateMachine<OwnerType>; // ステートマシン以外から呼び出し関数などにアクセスできないように

	// 状態が開始したときに一度だけ呼ばれる処理
	virtual void OnStart(OwnerType* /*a_pOwner*/) {}// ステートの持ち主を引数から取得できるように

	// 状態が更新するときに呼ばれる処理
	virtual void OnUpdate(OwnerType* /*a_pOwner*/) {}

	// 状態が終了したときに一度だけ呼ばれる処理
	virtual void OnExit(OwnerType* /*a_pOwner*/) {}

private:

	// この状態を管理しているステートマシーンをセット
	void SetMachine(StateMachine<OwnerType>* a_pMachine)
	{
		m_pMachine = a_pMachine;
	}

	// 開始関数をマシンから呼ぶための関数
	void CallStart(OwnerType* a_pOwner) // 仮想関数をマシンが安全に呼ぶための関数を追加
	{
		if (m_pMachine == nullptr || a_pOwner == nullptr)
		{
			return;
		}
		OnStart(a_pOwner);
	}

	// 更新関数をマシンから呼ぶための関数
	void CallUpdate(OwnerType* a_pOwner)
	{
		if (m_pMachine == nullptr || a_pOwner == nullptr)
		{
			return;
		}
		OnUpdate(a_pOwner);
	}

	// 終了関数をマシンから呼ぶための関数
	void CallExit(OwnerType* a_pOwner)
	{
		if (m_pMachine == nullptr || a_pOwner == nullptr)
		{
			return;
		}
		OnExit(a_pOwner);
	}

protected:

	StateMachine<OwnerType>* m_pMachine = nullptr; // このステートを管理しているステートマシンのポインタを保存

	// 
	OwnerType* m_pOwner = nullptr;
};


//*****************************************************************************
// 状態を管理するクラス
//*****************************************************************************
// テンプレート引数は状態を管理する対象の型
template<typename OwnerType> // テンプレート引数で状態を管理する対象の型を指定
class StateMachine
{
public:
	StateMachine()
		:m_fnChangeState([]() {})
	{

	}

	// ステートマシンの開始関数
	void Start(OwnerType* a_pOwner) // 初期化時にオーナーをセット
	{
		m_pOwner = a_pOwner;
		m_fnChangeState = []() {};
	}

	// 状態を変更する関数
	// 引き数はステートのコンストラクタに渡す値
	// ステートの変更処理を関数ポインタの中に閉じ込め更新が終わった後に呼ぶ
	template<typename StateType, typename...ArgType>// ステートの型を指定したら関数内でインスタンスを作成し、コンストラクタに値を渡せる
	void ChangeState(ArgType...a_args)
	{
		// ステートの変更命令を格納する
		m_fnChangeState = [&]() // ステートの変更命令を関数ポインタに格納する
		{
			// オーナーがセットされてなければ何もしない
			if (m_pOwner == nullptr)
			{
				return;
			}

			// もしすでにステートがセットされてたら終了する
			if (m_spNowState != nullptr)
			{
				m_spNowState->CallExit(m_pOwner);
				m_spNowState = nullptr;
			}

			// 新しいステートを作成
			m_spNowState = std::make_shared<StateType>(a_args...);
			if (m_spNowState == nullptr)
			{
				return;
			}
			// 新しいステートにこのマシーンをセット
			m_spNowState->SetMachine(this);

			// ステートの開始
			m_spNowState->CallStart(m_pOwner);
		};
	}


	// 状態の更新
	void Update()
	{
		// ステートの変更命令があれば処理する
		m_fnChangeState(); // ステートが更新する前にステートの変更命令があれば実行		
		m_fnChangeState = []() {};


		// ステートがセットされてたら更新する
		if (m_spNowState != nullptr)
		{
			m_spNowState->CallUpdate(m_pOwner);
		}
	}

private:

	// 状態の持ち主のポインタ
	OwnerType* m_pOwner = nullptr;

	// 今のステート
	std::shared_ptr<StateBase<OwnerType>> m_spNowState = nullptr;

	// ステートの変更命令を保存しておく関数オブジェクト
	std::function<void()> m_fnChangeState;

};
#endif
