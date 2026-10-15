#pragma once

// クエストIDの列挙型
enum QuestID
{
	QUEST_TUTORIAL,
	QUEST_CITY,
	QUEST_TEST,
	QUEST_ID_MAX
};

// クエスト達成条件の種類
enum QuestConditionType
{
	QUEST_CONDITION_NONE, // 達成条件なし
	QUEST_CONDITION_DEFEAT_ALL,         // 敵を全滅させる
	QUEST_CONDITION_DEFEAT_COUNT,       // 指定数の敵を倒す
	QUEST_CONDITION_DEFEAT_TARGET_TYPE  // 指定種類の敵を倒す
};

// クエスト達成条件
struct QuestCondition
{
	QuestConditionType type;
	// 討伐数クエストの目標数
	int targetCount;
	// 討伐対象のEnemyType
	int targetEnemyType;
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

	// 達成条件
	QuestCondition condition;

	/*後で実装する場合のために残しておく

	// 報酬
	int reward;
	*/
};