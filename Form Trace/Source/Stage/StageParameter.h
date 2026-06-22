#pragma once
#include "DxLib.h"
#include "../Library/json/json.hpp"
#include <string> 

// Unityが出力したjsonデータ
// 必ずUnityに合わせること
struct GameObject
{
	int id = 1;
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
	FLOOR_00,
	// Player
	PLAYER,
	// Block
	BLOCK_00,
	// Enemy
	ENEMY_00,
	ENEMY_01,
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
	obj.id = j.value("id", 0);
	JsonConvXYZ(j["position"], obj.pos);
	JsonConvXYZ(j["rotation"], obj.rot);
	JsonConvXYZ(j["scale"], obj.scale);
	obj.name = j.value("name", "");
}