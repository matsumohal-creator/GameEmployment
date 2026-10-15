#include "Minimap.h"
#include "../../Player/Player.h"
#include "../../Enemy/EnemyBase.h"

Minimap::Minimap()
    : m_MapHandle(-1)
    , m_MapX(0)
    , m_MapY(0)
    , m_MapWidth(240)
    , m_MapHeight(240)
    // 仮のワールド座標範囲。ステージに合わせて調整する
    , m_MinX(-60.0f)
    , m_MaxX(60.0f)
    , m_MinZ(-60.0f)
    , m_MaxZ(60.0f)
{
}

Minimap::~Minimap()
{
    Fin();
}

void Minimap::Init()
{
    // 画面サイズに合わせて右上に配置
    int screenWidth;
    int screenHeight;
    GetDrawScreenSize(&screenWidth, &screenHeight);

    const int margin = 20;

    m_MapX = screenWidth - m_MapWidth - margin;
    m_MapY = margin;
}

void Minimap::Load()
{
    Fin();

    m_MapHandle = LoadGraph("Data/UI/Minimap/Minimap.png");
}

void Minimap::Draw(
    Player* player,
    const std::list<EnemyBase*>& enemies)
{
    if (m_MapHandle == -1)
    {
        return;
    }

    // マップ画像
    DrawExtendGraph(
        m_MapX,
        m_MapY,
        m_MapX + m_MapWidth,
        m_MapY + m_MapHeight,
        m_MapHandle,
        TRUE);

    // マップの外枠
    DrawBox(
        m_MapX,
        m_MapY,
        m_MapX + m_MapWidth,
        m_MapY + m_MapHeight,
        GetColor(255, 255, 255),
        FALSE);

    // 敵のマーク
    EnemyBase* markedEnemy =
        (player != nullptr) ? player->GetMarkedEnemy() : nullptr;

    for (EnemyBase* enemy : enemies)
    {
        if (enemy == nullptr || enemy->IsDead())
        {
            continue;
        }

        VECTOR pos = enemy->GetPos();

        const bool isMarked = (enemy == markedEnemy);

        if (isMarked)
        {
            // ロックオン中の敵は黄色
            DrawMarker(
                pos.x, pos.z,
                GetColor(255, 220, 0),
                6);
        }
        else
        {
            // 通常の敵は赤色
            DrawMarker(
                pos.x, pos.z,
                GetColor(255, 60, 60),
                4);
        }
    }

    // プレイヤーのマーク
    if (player != nullptr)
    {
        VECTOR pos = player->GetPos();

        DrawMarker(
            pos.x, pos.z,
            GetColor(60, 220, 255),
            5);
    }
}

void Minimap::Fin()
{
    if (m_MapHandle != -1)
    {
        DeleteGraph(m_MapHandle);
        m_MapHandle = -1;
    }
}

void Minimap::SetWorldBounds(
    float minX, float maxX,
    float minZ, float maxZ)
{
    if (minX >= maxX || minZ >= maxZ)
    {
        return;
    }

    m_MinX = minX;
    m_MaxX = maxX;
    m_MinZ = minZ;
    m_MaxZ = maxZ;
}

bool Minimap::WorldToMap(
    float worldX, float worldZ,
    int& screenX, int& screenY) const
{
    if (worldX < m_MinX || worldX > m_MaxX ||
        worldZ < m_MinZ || worldZ > m_MaxZ)
    {
        return false;
    }

    const float normalizedX =
        (worldX - m_MinX) / (m_MaxX - m_MinX);

    // ワールド座標の +Z 側を画像の上側に対応させる
    const float normalizedY =
        (m_MaxZ - worldZ) / (m_MaxZ - m_MinZ);

    screenX = m_MapX +
        static_cast<int>(normalizedX * m_MapWidth);

    screenY = m_MapY +
        static_cast<int>(normalizedY * m_MapHeight);

    return true;
}

void Minimap::DrawMarker(
    float worldX, float worldZ,
    unsigned int color, int radius)
{
    int screenX;
    int screenY;

    if (!WorldToMap(worldX, worldZ, screenX, screenY))
    {
        return;
    }

    DrawCircle(
        screenX,
        screenY,
        radius,
        color,
        TRUE);
}
