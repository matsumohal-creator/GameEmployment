#pragma once
#include <vector>

class Stage;
struct GameObject;

// プレイヤーオブジェクト管理クラス
class StageManager
{
public:
	StageManager();	// コンストラクタ
	~StageManager();	// デストラクタ

	static void CreateInstance() { if (!m_Instance) m_Instance = new StageManager; }
	static StageManager* GetInstance() { return m_Instance; }
	static void DeleteInstance() { if (m_Instance) delete m_Instance; m_Instance = nullptr; }

	void Load(const char* fileName);	// ロード
	void Start();	// 開始
	void Draw();	// 描画
	void Fin();		// 終了

private:
	static StageManager* m_Instance;
	std::vector<GameObject> m_Objects;
};
