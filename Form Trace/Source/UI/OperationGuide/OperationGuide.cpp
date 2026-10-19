#include "OperationGuide.h"
#include "DxLib.h"

OperationGuide::OperationGuide()
{
    m_IsInitialized = false;

    m_LightAttackName = "í èÌçUåÇ";
    m_HeavyAttackName = "ã≠çUåÇ";

    m_LightAttackButton = "A";
    m_HeavyAttackButton = "W";

    m_ScreenWidth = 1280;
    m_ScreenHeight = 720;
}

void OperationGuide::Init()
{
    GetDrawScreenSize(&m_ScreenWidth, &m_ScreenHeight);

    m_IsInitialized = true;
}

void OperationGuide::SetAttackGuide(
    const char* lightAttackName,
    const char* heavyAttackName,
    const char* lightAttackButton,
    const char* heavyAttackButton)
{
    m_LightAttackName =
        lightAttackName ? lightAttackName : "é„çUåÇ";

    m_HeavyAttackName =
        heavyAttackName ? heavyAttackName : "ã≠çUåÇ";

    m_LightAttackButton =
        lightAttackButton ? lightAttackButton : "A";

    m_HeavyAttackButton =
        heavyAttackButton ? heavyAttackButton : "W";
}

void OperationGuide::Load()
{
}

void OperationGuide::Draw()
{
    if (!m_IsInitialized)
    {
        return;
    }

    // ÉpÉlÉãÇÃëÂÇ´Ç≥Ç∆à íu
    const int panelWidth = 240;
    const int panelHeight = 100;
    const int margin = 20;

    const int left =
        m_ScreenWidth - panelWidth - margin;

    const int top =
        m_ScreenHeight - panelHeight - margin;

    const int right =
        m_ScreenWidth - margin;

    const int bottom =
        m_ScreenHeight - margin;

    // îwåiÉpÉlÉã
    DrawBox(
        left, top, right, bottom,
        GetColor(25, 28, 36),
        TRUE
    );

    // ògê¸
    DrawBox(
        left, top, right, bottom,
        GetColor(110, 115, 130),
        FALSE
    );

    // é„çUåÇ
    DrawString(
        left + 14,
        top + 60,
        m_LightAttackButton.c_str(),
        GetColor(255, 230, 130)
    );

    DrawString(
        left + 70,
        top + 60,
        m_LightAttackName.c_str(),
        GetColor(255, 255, 255)
    );

    // ã≠çUåÇ
    DrawString(
        left + 14,
        top + 25,
        m_HeavyAttackButton.c_str(),
        GetColor(255, 170, 130)
    );

    DrawString(
        left + 70,
        top + 25,
        m_HeavyAttackName.c_str(),
        GetColor(255, 255, 255)
    );
}

void OperationGuide::Fin()
{
    m_IsInitialized = false;
}