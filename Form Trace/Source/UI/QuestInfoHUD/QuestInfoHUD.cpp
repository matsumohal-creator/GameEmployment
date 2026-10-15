
#include "QuestInfoHUD.h"

QuestInfoHUD::QuestInfoHUD()
    : m_X(1340)
    , m_Y(275)
{
}

QuestInfoHUD::~QuestInfoHUD()
{
}

void QuestInfoHUD::Init()
{
}

void QuestInfoHUD::Load()
{
}

void QuestInfoHUD::Draw(
    int remainingLife,
    const QuestData& quest,
    int currentProgress,
    bool targetDefeated)
{
    const unsigned int white = GetColor(255, 255, 255);
    const unsigned int gray = GetColor(200, 200, 200);
    const unsigned int panel = GetColor(20, 25, 35);
    const unsigned int accent = GetColor(80, 180, 255);
    const unsigned int green = GetColor(100, 255, 120);
    const unsigned int red = GetColor(255, 120, 120);

    const int panelWidth = 240;
    const int panelHeight = 64;

    // 背景パネル
    DrawBox(
        m_X,
        m_Y,
        m_X + panelWidth,
        m_Y + panelHeight,
        panel,
        TRUE
    );

    // パネルの枠
    DrawBox(
        m_X,
        m_Y,
        m_X + panelWidth,
        m_Y + panelHeight,
        accent,
        FALSE
    );

    // クエスト条件に応じた進捗表示
    switch (quest.condition.type)
    {
    case QUEST_CONDITION_DEFEAT_ALL:
        DrawFormatString(
            m_X + 12,
            m_Y + 12,
            white,
            "残り敵数：%d",
            currentProgress
        );
        break;

    case QUEST_CONDITION_DEFEAT_COUNT:
        DrawFormatString(
            m_X + 12,
            m_Y + 12,
            white,
            "討伐進捗：%d / %d",
            currentProgress,
            quest.condition.targetCount
        );
        break;

    case QUEST_CONDITION_DEFEAT_TARGET_TYPE:
        DrawString(
            m_X + 12,
            m_Y + 12,
            targetDefeated ? "対象討伐：達成" : "対象討伐：未達成",
            targetDefeated ? green : red
        );
        break;

    case QUEST_CONDITION_NONE:
    default:
        DrawString(
            m_X + 12,
            m_Y + 12,
            "特別条件",
            white
        );
        break;
    }

    // 復活可能な残り回数
    DrawFormatString(
        m_X + 12,
        m_Y + 36,
        white,
        "復活残り：%d",
        remainingLife);
}

void QuestInfoHUD::Fin()
{
}
