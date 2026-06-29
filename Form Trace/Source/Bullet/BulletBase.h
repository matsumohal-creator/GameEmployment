#pragma once
#include "../Collision/CollisionSphere.h"
#include <DxLib.h>

class EnemyBase;

class BulletBase
{
public:
    BulletBase();
    virtual ~BulletBase();

public:
    virtual void Load() {}
    virtual void Start();
    virtual void Step() {}
    virtual void Update() {}
    virtual void Draw() {}

public:
    // 敵に当たったときの処理をオーバーライドするための仮想関数
    virtual void OnHitEnemy(EnemyBase* enemy) {}

public:
    virtual BulletBase* Clone() = 0;

public:
    void SetTransform(
        VECTOR pos,
        VECTOR rot,
        VECTOR scale);

    void SetMove(VECTOR move)
    {
        m_Move = move;
    }

    VECTOR GetPos() const
    {
        return m_Pos;
    }

    bool IsDead() const
    {
        return m_IsDead;
    }

    void Destroy()
    {
        m_IsDead = true;
    }

    const CollisionSphere* GetSphere() const
    {
        return m_SphereCollision;
    }

protected:
    VECTOR m_Pos;
    VECTOR m_Rot;
    VECTOR m_Scale;

    VECTOR m_Move;
    CollisionSphere* m_SphereCollision;
    bool m_IsDead;
    void CheckHitEnemy();
};