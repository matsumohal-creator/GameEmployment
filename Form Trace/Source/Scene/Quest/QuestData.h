#pragma once

// クエストIDの列挙型
enum QuestID
{
	QUEST_TUTORIAL,
	QUEST_CITY,
	QUEST_TEST,
	QUEST_ID_MAX
};

// クエストデータ構造体
struct QuestData
{
	// クエストID
	QuestID id;
	// クエスト名
	const char* name;
	// クエストの説明
	const char* description;

	// 使用するステージデータ
	const char* stagePath;

	// 最大残機
	int maxLife;

	/*後で実装する場合のために残しておく
	// 勝利条件
	QuestClearCondition clearCondition;

	// 登場する敵
	// EnemyType enemyTypes[];

	// 報酬
	int reward;
	*/
};