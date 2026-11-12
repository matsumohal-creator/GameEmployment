#include "DxLib.h"
#include "PlayerHUD.h"
#include "../../Player/Player.h"
#include "../../Enemy/EnemyBase.h"
#include "../../Enemy/EnemyManager.h"

namespace
{
    constexpr float HUD_SCALE = 2.0f;

    // 回復ゲージの配置・サイズ
    constexpr int HEAL_GAUGE_FRAME_SIZE = 48;
    constexpr int HEAL_GAUGE_SIZE = 26;

    // フレーム内の青い円の配置
    constexpr int HEAL_GAUGE_OFFSET_X = 5;
    constexpr int HEAL_GAUGE_OFFSET_Y = 16;

    // HPゲージ外枠と回復ゲージの間隔
    constexpr int HEAL_GAUGE_MARGIN = 8;
}

PlayerHUD::PlayerHUD()
{
    m_HPHandle = -1;
    m_StaminaHandle = -1;
    m_FrameHandle = -1;

    m_HealGaugeHandle = -1;
    m_HealGaugeFrameHandle = -1;

    m_PosX = 20;
    m_PosY = 20;

    for (int i = 0; i < 4; i++)
    {
        m_EnemyHandles[i] = -1;
    }
}

PlayerHUD::~PlayerHUD()
{
    Fin();
}

void PlayerHUD::Init()
{
    m_PosX = 20;
    m_PosY = 20;
}

void PlayerHUD::Load()
{
    // 画像を読み込む
    m_HPHandle = LoadGraph("Data/UI/HP.png");
    m_StaminaHandle = LoadGraph("Data/UI/Stamina.png");
    m_FrameHandle = LoadGraph("Data/UI/Outer_frame.png");

    m_EnemyHandles[RED_ENEMY] = LoadGraph("Data/UI/Enemy/RedEnemy.png");
    m_EnemyHandles[BLUE_ENEMY] = LoadGraph("Data/UI/Enemy/BlueEnemy.png");
    m_EnemyHandles[TUTORIAL_ENEMY] = LoadGraph("Data/UI/Enemy/TutorialEnemy.png");
    m_EnemyHandles[HANNIBAL] = LoadGraph("Data/UI/Enemy/Hannibal.png");

    m_HealGaugeHandle = LoadGraph("Data/UI/Gauge.png");
    m_HealGaugeFrameHandle = LoadGraph("Data/UI/HealGauge.png");
}

void PlayerHUD::DrawGauge(
    int handle,
    int x,
    int y,
    int visibleLeft,
    int visibleRight,
    float rate)
{
    if (handle == -1)
    {
        return;
    }

    if (rate < 0.0f) rate = 0.0f;
    if (rate > 1.0f) rate = 1.0f;

    int cropRight =
        visibleLeft +
        static_cast<int>(
            (visibleRight - visibleLeft) * rate
            );

    if (cropRight <= 0)
    {
        return;
    }

    int drawWidth =
        static_cast<int>(cropRight * HUD_SCALE);

    int drawHeight =
        static_cast<int>(48 * HUD_SCALE);

    DrawRectExtendGraph(
        x,
        y,
        x + drawWidth,
        y + drawHeight,
        0,
        0,
        cropRight,
        48,
        handle,
        TRUE
    );
}

void PlayerHUD::DrawHealGauge(Player* player)
{
    if (!player)
    {
        return;
    }

    if (m_HealGaugeHandle == -1 ||
        m_HealGaugeFrameHandle == -1)
    {
        return;
    }

    // 回復ゲージの残量
    float rate = 0.0f;

    if (player->GetMaxHealGauge() > 0)
    {
        rate = static_cast<float>(player->GetHealGauge()) / player->GetMaxHealGauge();
    }

    // 0.0～1.0に制限
    if (rate < 0.0f) rate = 0.0f;
    if (rate > 1.0f) rate = 1.0f;

    // フレームの描画位置
    int frameX = m_PosX - 20 + static_cast<int>(208 * HUD_SCALE);
    int frameY = m_PosY - 10;

    // 2.1倍スケールでのゲージサイズ
    int maxGaugeSize =
        static_cast<int>(HEAL_GAUGE_SIZE * (HUD_SCALE * 1.05f));

    // 下から表示する高さ
    int gaugeHeight =
        static_cast<int>(maxGaugeSize * rate);

    // フレームを描画
    int frameSize =
        static_cast<int>(HEAL_GAUGE_FRAME_SIZE * HUD_SCALE);

    DrawExtendGraph(frameX,frameY,frameX + frameSize,frameY + frameSize,m_HealGaugeFrameHandle,TRUE);

    // ゲージが残っている場合だけ描画
    if (gaugeHeight > 0)
    {
        // ゲージ画像の描画位置
        int gaugeX = frameX +
            static_cast<int>(HEAL_GAUGE_OFFSET_X * HUD_SCALE);

        int gaugeY = frameY + 5 +
            static_cast<int>(HEAL_GAUGE_OFFSET_Y * HUD_SCALE);

        // 円の中心と半径
        float centerX = gaugeX + maxGaugeSize / 2.0f;
        float centerY = gaugeY + maxGaugeSize / 2.0f;
        float radius = maxGaugeSize / 2.0f - 1.0f;

        // 青色（Gauge.pngに近い色）
        int gaugeColor = GetColor(0, 160, 233);

        // 下端を固定して水位を上げる
        // ゲージ量に応じて水位を最大4px下げる
        int levelCorrection =
            (rate >= 1.0f)
            ? 0
            : static_cast<int>(4.0f * rate);

        int fillTop = gaugeY + maxGaugeSize - gaugeHeight + levelCorrection;

        int fillBottom = gaugeY + maxGaugeSize;

        // 1ラインずつ円の内側を描画
        for (int y = fillTop; y < fillBottom; y++)
        {
            float dy = (y + 0.5f) - centerY;

            // 円の外側は描画しない
            float widthSquared = radius * radius - dy * dy;

            if (widthSquared <= 0.0f)
                continue;

            // この高さでの円の横幅
            float halfWidth = std::sqrt(widthSquared);

            int left = static_cast<int>(std::ceil(centerX - halfWidth));

            int right = static_cast<int>(std::floor(centerX + halfWidth));

            // 水位部分を描画
            if (left <= right)
            {
                DrawBox(left,y,right,y + 1,gaugeColor,TRUE);
            }
        }
    }
}

void PlayerHUD::Draw(Player* player)
{
    if (!player)
    {
        return;
    }

    // HPの残量
    float hpRate = 0.0f;

    if (player->GetMaxHP() > 0)
    {
        hpRate =
            static_cast<float>(player->GetHP()) /
            player->GetMaxHP();
    }

    // スタミナの残量
    float staminaRate = 0.0f;

    if (player->GetMaxStamina() > 0)
    {
        staminaRate =
            static_cast<float>(player->GetStamina()) /
            player->GetMaxStamina();
    }

    // 緑ゲージ
    DrawGauge(
        m_StaminaHandle,
        m_PosX,
        m_PosY,
        53,
        188,
        staminaRate
    );

    // 赤ゲージ
    DrawGauge(
        m_HPHandle,
        m_PosX,
        m_PosY,
        45,
        198,
        hpRate
    );

    // 回復ゲージ
    DrawHealGauge(player);

    DrawMarkedEnemy(player->GetMarkedEnemy());

    // 外枠は最後に描画
    if (m_FrameHandle != -1)
    {
        DrawExtendGraph(
            m_PosX,
            m_PosY,
            m_PosX + static_cast<int>(208 * HUD_SCALE),
            m_PosY + static_cast<int>(48 * HUD_SCALE),
            m_FrameHandle,
            TRUE
        );
    }
}

void PlayerHUD::Fin()
{
    if (m_HPHandle != -1)
    {
        DeleteGraph(m_HPHandle);
        m_HPHandle = -1;
    }

    if (m_StaminaHandle != -1)
    {
        DeleteGraph(m_StaminaHandle);
        m_StaminaHandle = -1;
    }

    if (m_FrameHandle != -1)
    {
        DeleteGraph(m_FrameHandle);
        m_FrameHandle = -1;
    }

    if (m_HealGaugeHandle != -1)
    {
        DeleteGraph(m_HealGaugeHandle);
        m_HealGaugeHandle = -1;
    }

    if (m_HealGaugeFrameHandle != -1)
    {
        DeleteGraph(m_HealGaugeFrameHandle);
        m_HealGaugeFrameHandle = -1;
    }

    for (int i = 0; i < ENEMY_TYPE_MAX; i++)
    {
        if (m_EnemyHandles[i] != -1)
        {
            DeleteGraph(m_EnemyHandles[i]);
            m_EnemyHandles[i] = -1;
        }
    }
}

void PlayerHUD::DrawMarkedEnemy(EnemyBase* enemy)
{
    if (!enemy || enemy->IsDead())
    {
        return;
    }

    int enemyType = enemy->GetEnemyType();

    if (enemyType < RED_ENEMY ||
        enemyType >= ENEMY_TYPE_MAX)
    {
        return;
    }

    int handle = m_EnemyHandles[enemyType];

    if (handle == -1)
    {
        return;
    }

    // 円形部分の位置・大きさに合わせて調整する
    constexpr int ENEMY_IMAGE_SIZE = 50;

    int x = m_PosX + static_cast<int>(18 * HUD_SCALE);
    int y = m_PosY + static_cast<int>(10 * HUD_SCALE);

    DrawExtendGraph(
        x,
        y,
        x + ENEMY_IMAGE_SIZE,
        y + ENEMY_IMAGE_SIZE,
        handle,
        TRUE
    );
}