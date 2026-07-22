#include "Hannibal.h"
#include "../../../MyMath/MyMath.h"
#include "../../../Player/PlayerManager.h"
#include "../../../Player/Player.h"

Hannibal::Hannibal()
{
}

Hannibal::~Hannibal()
{
}

void Hannibal::Init()
{
}

void Hannibal::Load()
{
    // 後で差し替え
    m_Handle = MV1LoadModel("Data/Enemy/Boss/Hannibal/Hannibal.x");
}

void Hannibal::Start()
{
    m_MaxHP = 1000;
    m_HP = m_MaxHP;

    m_Attack = 50;

    // ボスは変身不可なので0
    m_TransformHP = 0;
    m_TransformAttack = 0;

    m_Move = VGet(0, 0, 0);

    m_IsDead = false;

    m_CollisionRadius = 2.0f;
    m_CollisionHeight = 5.0f;
}

void Hannibal::Step()
{
    m_Move = VGet(0, 0, 0);

    Player* player =
        PlayerManager::GetInstance()->GetPlayer();

    if (!player)
    {
        return;
    }

    VECTOR diff =
        VSub(player->GetPos(), m_Pos);

    float distSq =
        diff.x * diff.x +
        diff.z * diff.z;

    // プレイヤーを見つけたら接近
    if (distSq < 900.0f) // 30m
    {
        VECTOR dir = VNorm(diff);

        float faceRot =
            atan2f(dir.x, dir.z);

        m_Rot.y = faceRot + DX_PI_F;

        // 近すぎたら止まる
        if (distSq > 25.0f)
        {
            m_Move.x = dir.x * 0.04f;
            m_Move.z = dir.z * 0.04f;
        }
    }
}

EnemyBase* Hannibal::Clone()
{
    Hannibal* clone = new Hannibal;

    *clone = *this;

    clone->m_Handle =
        MV1DuplicateModel(m_Handle);

    return clone;
}