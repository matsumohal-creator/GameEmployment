
#pragma once

#include "DxLib.h"
#include <list>

class Player;
class EnemyBase;

class Minimap
{
public:
    Minimap();
    ~Minimap();

    void Init();
    void Load();
    void Draw(Player* player, const std::list<EnemyBase*>& enemies);
    void Fin();

    // ミニマップに対応するワールド座標の範囲
    void SetWorldBounds(
        float minX, float maxX,
        float minZ, float maxZ);

private:
    // ワールド座標からミニマップ上の画面座標へ変換
    bool WorldToMap(
        float worldX, float worldZ,
        int& screenX, int& screenY) const;

    // マークを描画
    void DrawMarker(
        float worldX, float worldZ,
        unsigned int color, int radius);

private:
    int m_MapHandle;

    // ミニマップの画面上の位置とサイズ
    int m_MapX;
    int m_MapY;
    int m_MapWidth;
    int m_MapHeight;

    // 対応するワールド座標の範囲
    float m_MinX;
    float m_MaxX;
    float m_MinZ;
    float m_MaxZ;
};
