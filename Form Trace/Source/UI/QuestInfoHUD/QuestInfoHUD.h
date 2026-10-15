#pragma once
#include "DxLib.h"
#include "../../Scene/Quest/QuestData.h"

class QuestInfoHUD
{
public:
    QuestInfoHUD();
    ~QuestInfoHUD();

    void Init();
    void Load();
    void Draw(
        int remainingLife,
        const QuestData& quest,
        int currentProgress,
        bool targetDefeated
    );
    void Fin();

private:
    int m_X;
    int m_Y;
};
