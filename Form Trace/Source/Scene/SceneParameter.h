#pragma once

enum SceneState
{
	INIT,
	LOAD,
	START,
	LOOP,
	FIN,
	SCENE_STATE_MAX,
	SCENE_STATE_NONE = -1
};

enum SceneType
{
	TITLE,      // タイトル
	QUEST,      // クエスト選択
	LOADING,    // ロード
	PLAY,       // フィールド
	CLEAR,      // クエストクリア
	SCENE_TYPE_NONE = -1
};

