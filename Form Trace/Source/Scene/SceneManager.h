#pragma once
#include "SceneBase.h"
#include "SceneParameter.h"


class SceneManager
{
public:
	SceneManager();
	virtual ~SceneManager();

public:
	static void CreateInstance() { if (!m_Instance) m_Instance = new SceneManager; }
	static SceneManager* GetInstance() { return m_Instance; }
	static void DeleteInstance() { if (m_Instance)delete m_Instance; m_Instance = nullptr; }

public:
	void Init();
	void Update();
	void Fin();

	void ChangeScene(SceneType type);


	// 以下の関数は外部から呼ぶことはないのでprivate
private:
	void InitScene();
	void LoadScene();
	void StartScene();
	void LoopScene();
	void FinScene();

	void CreateScene(SceneType type);

private:
	static SceneManager* m_Instance;

	// 現在動いているシーン
	SceneBase* m_NowScene;

	// 現在の状態
	SceneState m_State;

	// 次に遷移するシーン
	SceneType m_NextScene;

	// 状態ごとの関数を呼ぶ関数ポインタ配列
	void (SceneManager::* m_StateFunc[SCENE_STATE_MAX])(void);
};
