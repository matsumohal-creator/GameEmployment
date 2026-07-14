#pragma once
#include "../Collision/CollisionSphere.h"
#include <DxLib.h>

class EnemyBase;
class Player;

// 弾の所有者を表す列挙型
enum BulletOwner
{
    OWNER_PLAYER,
    OWNER_ENEMY
};

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
    virtual BulletBase* Clone() = 0;

public:
	// 敵に当たったときや
    // 味方に当たったときの処理をオーバーライドするための仮想関数
    virtual void OnHitEnemy(EnemyBase* enemy) {}
    virtual void OnHitPlayer(Player* player) {}

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

    void SetOwner(BulletOwner owner)
    {
        m_Owner = owner;
    }

protected:
    VECTOR m_Pos;
    VECTOR m_Rot;
    VECTOR m_Scale;
	VECTOR m_Move; // 弾の移動量を表す変数
	CollisionSphere* m_SphereCollision; // 当たり判定用の球体
	BulletOwner m_Owner; // 弾の所有者を表す変数
	bool m_IsDead; // 弾が消滅しているかどうかを表す変数
	void CheckHitEnemy(); // 敵に当たったかをチェックする関数
	void CheckHitPlayer(); // プレイヤーに当たったかをチェックする関数
};