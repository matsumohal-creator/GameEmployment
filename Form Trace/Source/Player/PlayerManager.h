#pragma once
#include "DxLib.h"

class Player;

// プレイヤーオブジェクト管理クラス
class PlayerManager
{
public:
	PlayerManager();	// コンストラクタ
	~PlayerManager();	// デストラクタ

public:
	// プレイヤーマネージャーを生成する
	// PlayerManagerはゲーム上に１つのみとするので
	// ２回目以降はnewが通らないようになっている
	// （デザインパターン：シングルトン）
	static void CreateInstance() { if (!m_Instance) m_Instance = new PlayerManager; }
	// マネージャーの関数が呼びたいときに使用する、マネージャー取得関数
	static PlayerManager* GetInstance() { return m_Instance; }
	// 使わなくなったら削除する際の削除関数
	static void DeleteInstance() { if (m_Instance) delete m_Instance; m_Instance = nullptr; }

public:
	void Init();	// 初期化
	void Load();	// ロード
	void Start();	// 開始
	void Step();	// ステップ
	void Update();	// 更新
	void Draw();	// 描画
	void Fin();		// 終了

public:
	// プレイヤーを生成する
	Player* CreatePlayer();
	Player* CreatePlayer(VECTOR pos, VECTOR rot, VECTOR scale);

	// 管理中のプレイヤーを取得する
	Player* GetPlayer() { return m_Player; }

private:
	// 生成されたPlayerManager自身を格納する変数
	// PlayerManagerはゲーム上に１つのみなのでstaticにしている
	static PlayerManager* m_Instance;
	// 生成されたプレイヤーの参照
	Player* m_Player;
};