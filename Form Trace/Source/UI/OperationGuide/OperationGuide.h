#pragma once

#include <string>

class OperationGuide
{
public:
    OperationGuide();

    void Init();
    void Draw();
    void Fin();

    // 弱攻撃・強攻撃の名前とボタン表記を設定
    void SetAttackGuide(
        const char* lightAttackName,
        const char* heavyAttackName,
        const char* lightAttackButton,
        const char* heavyAttackButton
    );

private:
    bool m_IsInitialized;

    std::string m_LightAttackName;
    std::string m_HeavyAttackName;

    std::string m_LightAttackButton;
    std::string m_HeavyAttackButton;

    int m_ScreenWidth;
    int m_ScreenHeight;
};