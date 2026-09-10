#pragma once
#include "DxLib.h"
#include "../Library/json/json.hpp"
#include <string> 

// Unityが出力したjsonデータ
// 必ずUnityに合わせること
struct GameObject
{
	int mapID = 0;
	int id = 0;
	VECTOR pos = {};
	VECTOR rot = {};
	VECTOR scale = {};
	std::string name = "";
};

// Unityが出力した配置オブジェクトID
// 必ずUnityに合わせること
enum LcateObject
{
	// Floor
	STAGE_FLOOR_00,

	// Player
	STAGE_PLAYER,

	// Block
	STAGE_BLOCK_00,
	STAGE_BLOCK_01,

	// Enemy
	STAGE_RED_ENEMY,
	STAGE_BLUE_ENEMY,
	STAGE_TUTORIAL_ENEMY,

	// Boss
	STAGE_HANNIBAL,
};

// jsonにあるTransformをDxLibのVECTORに変換する関数
inline VECTOR JsonConvXYZ(const nlohmann::json& j, VECTOR& v)
{
	v.x = j.value("x", 0.0f);
	v.y = j.value("y", 0.0f);
	v.z = j.value("z", 0.0f);
	return v;
}

// jsonにあるオブジェクトデータをGameObject構造体に格納するために必要
inline void from_json(const nlohmann::json& j, GameObject& obj)
{
	obj.mapID = j.value("mapID", 0);
	obj.id = j.value("id", 0);
	JsonConvXYZ(j["position"], obj.pos);
	JsonConvXYZ(j["rotation"], obj.rot);
	JsonConvXYZ(j["scale"], obj.scale);
	obj.name = j.value("name", "");
}