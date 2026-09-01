#pragma once
#include "../SceneBase.h"
#include "QuestData.h"

class Quest : public SceneBase
{
public:
	Quest();
	~Quest();

public:
	void Init() override;
	void Load() override;
	void Start() override;
	void Step() override;
	void Update() override;
	void Draw() override;
	void Fin() override;

private:
	void UpdateSelect();
	void DrawQuestList();
	void DrawQuestDetail();

private:
	// クエスト一覧
	QuestData m_QuestList[QUEST_ID_MAX];

	// 現在選択しているクエスト
	int m_SelectIndex;
};