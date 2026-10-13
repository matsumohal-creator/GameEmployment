#include "DxLib.h"
#include "PlayerHUD.h"
#include "../../Player/Player.h"
#include "../../Enemy/EnemyBase.h"
#include "../../Enemy/EnemyManager.h"

namespace
{
    constexpr float HUD_SCALE = 2.0f;
}

PlayerHUD::PlayerHUD()
{
    m_HPHandle = -1;
    m_StaminaHandle = -1;
    m_FrameHandle = -1;

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

    int x = m_PosX + static_cast<int>(15 * HUD_SCALE);
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