#pragma once

class Player;
class EnemyBase;

// プレイヤーHUD
class PlayerHUD
{
public:
    PlayerHUD();
    ~PlayerHUD();

    void Init();
    void Load();
    void Draw(Player* player);
    void Fin();

private:
    // 画像ハンドル
    int m_HPHandle;
    int m_StaminaHandle;
    int m_FrameHandle;

    // 敵画像のハンドル
    int m_EnemyHandles[4];

    // HUDの表示位置
    int m_PosX;
    int m_PosY;

private:
    // ゲージ描画
    void DrawGauge(
        int handle,
        int x,
        int y,
        int visibleLeft,
        int visibleRight,
        float rate
    );

    // マーク対象の敵画像を描画
    void DrawMarkedEnemy(EnemyBase* enemy);
};