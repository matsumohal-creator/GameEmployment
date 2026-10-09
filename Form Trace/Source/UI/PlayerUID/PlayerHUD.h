#pragma once

class Player;

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
};