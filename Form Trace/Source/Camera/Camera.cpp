#include "Camera.h"
#include "../Player/PlayerManager.h"
#include "../Player/Player.h"
#include "../Enemy/EnemyBase.h"
#include "../Input/Input.h"
#include "../MyMath/MyMath.h"
#include "../StageObject/StageObject.h"
#include "../StageObject/StageObjectManager.h"
#include "../StageObject/Floor/Floor.h"
#include "../StageObject/Block/Block.h"
#include "../Collision/CollisionAABB.h"
#include "../Collision/CollisionOBB.h"
#include <cmath>

#define CAMERA_NEAR_CLIP 0.01f
#define CAMERA_FAR_CLIP 1000.0f

#define PLAYER_DISTANCE -7.0f
#define ROTATION_SPEED 0.025f

#define X_ROTATION_MAX (DX_PI_F * 0.49f)

// 通常時のカメラ距離
#define CAMERA_DISTANCE_DEFAULT 7.0f

// ロックオン時の最大カメラ距離
#define CAMERA_DISTANCE_MAX 16.0f

// 敵との距離に応じたズーム量
#define CAMERA_ZOOM_FACTOR 0.25f

// カメラ距離の追従速度
#define CAMERA_ZOOM_SPEED 0.08f

// ロックオン時のカメラ回転追従速度
#define CAMERA_LOCKON_ROTATION_SPEED 0.10f

// ロックオン時の上下回転追従速度
#define CAMERA_LOCKON_PITCH_SPEED 0.08f

// ロックオン時の基本見下ろし角度
#define CAMERA_LOCKON_PITCH_DEFAULT 0.25f

// ロックオン時の見下ろし角度の追従速度
#define CAMERA_LOCKON_PITCH_SPEED 0.08f

// 壁からカメラを離すための余白
#define CAMERA_COLLISION_MARGIN 0.35f

// カメラが壁にめり込むのを防ぐための最小距離
#define CAMERA_COLLISION_MIN_DISTANCE 0.6f

// 壁にめり込んだ際のカメラの移動速度
#define CAMERA_COLLISION_IN_SPEED 0.35f

// 壁から離れる際のカメラの移動速度
#define CAMERA_COLLISION_OUT_SPEED 0.12f

namespace
{
    // 線分と箱の交差位置を求める。
    // rotationY が 0 なら AABB、回転がある場合は Y 軸回転の OBB。
    bool CheckCameraBoxHit(
        VECTOR start,
        VECTOR end,
        VECTOR center,
        VECTOR size,
        float rotationY,
        float& hitT)
    {
        VECTOR direction = VSub(end, start);
        VECTOR relative = VSub(start, center);

        // OBB の場合はローカル座標に変換する。
        float cosY = cosf(rotationY);
        float sinY = sinf(rotationY);

        VECTOR localStart = VGet(
            relative.x * cosY - relative.z * sinY,
            relative.y,
            relative.x * sinY + relative.z * cosY
        );

        VECTOR localDir = VGet(
            direction.x * cosY - direction.z * sinY,
            direction.y,
            direction.x * sinY + direction.z * cosY
        );

        VECTOR half = VScale(size, 0.5f);

        // 線分の始点が箱の内部にある場合は無視する。
        // 床などの大きな当たり判定による誤補正を抑えるため。
        if (fabsf(localStart.x) < half.x &&
            fabsf(localStart.y) < half.y &&
            fabsf(localStart.z) < half.z)
        {
            return false;
        }

        float tMin = 0.0f;
        float tMax = 1.0f;

        const float startValues[3] = {
            localStart.x, localStart.y, localStart.z
        };

        const float dirValues[3] = {
            localDir.x, localDir.y, localDir.z
        };

        const float halfValues[3] = {
            half.x, half.y, half.z
        };

        for (int axis = 0; axis < 3; axis++)
        {
            float s = startValues[axis];
            float d = dirValues[axis];
            float h = halfValues[axis];

            if (fabsf(d) < 0.000001f)
            {
                // この軸方向に進まない場合
                if (s < -h || s > h)
                {
                    return false;
                }

                continue;
            }

            float t1 = (-h - s) / d;
            float t2 = (h - s) / d;

            if (t1 > t2)
            {
                float temp = t1;
                t1 = t2;
                t2 = temp;
            }

            if (t1 > tMin) tMin = t1;
            if (t2 < tMax) tMax = t2;

            if (tMin > tMax)
            {
                return false;
            }
        }

        // 始点付近の接触は無視する。
        if (tMin <= 0.001f || tMin > 1.0f)
        {
            return false;
        }

        hitT = tMin;
        return true;
    }

    // AABB の交差判定
    bool CheckCameraAABBHit(
        VECTOR start,
        VECTOR end,
        const CollisionAABB* aabb,
        float& hitT)
    {
        if (!aabb)
        {
            return false;
        }

        return CheckCameraBoxHit(
            start,
            end,
            aabb->GetPos(),
            aabb->GetSize(),
            0.0f,
            hitT
        );
    }

    // OBB の交差判定
    bool CheckCameraOBBHit(
        VECTOR start,
        VECTOR end,
        const CollisionOBB* obb,
        float& hitT)
    {
        if (!obb)
        {
            return false;
        }

        return CheckCameraBoxHit(
            start,
            end,
            obb->GetPos(),
            obb->GetSize(),
            obb->GetRotationY(),
            hitT
        );
    }
}

// 基底クラスのコンストラクタ呼ぶ際は追加で書く
Camera::Camera() : CameraBase()
{
    m_TargetPlayer = nullptr;
    m_CameraDistance = CAMERA_DISTANCE_DEFAULT;
    m_CameraActualDistance = CAMERA_DISTANCE_DEFAULT;
}

Camera::~Camera()
{
	Fin();
}

void Camera::Init()
{
}

void Camera::Load()
{
}

void Camera::Start()
{
	// 初期カメラ角度
	// 少し下向きにしてプレイヤーを見やすくする
	m_Rot.x = 0.20f;
	m_Rot.y = 0.0f;
	m_Rot.z = 0.0f;

    m_CameraDistance = CAMERA_DISTANCE_DEFAULT;
    m_CameraActualDistance = CAMERA_DISTANCE_DEFAULT;

	// ニア、ファークリップの設定
	SetCameraNearFar(CAMERA_NEAR_CLIP, CAMERA_FAR_CLIP);

	// アップベクトル設定
	m_UpVec = VGet(0.0f, 1.0f, 0.0f);

	// 追従するプレイヤー
	m_TargetPlayer = PlayerManager::GetInstance()->GetPlayer();
}

void Camera::Step()
{
	// キー入力処理
	if (Input::IsInputKey(ACTION_CAMERA_LEFT))
	{
		// 左回転
		m_Rot.y += ROTATION_SPEED;
	}
	if (Input::IsInputKey(ACTION_CAMERA_RIGHT))
	{
		// 右回転
		m_Rot.y -= ROTATION_SPEED;
	}
	if (Input::IsInputKey(ACTION_CAMERA_UP))
	{
		// 手前回転
		m_Rot.x += ROTATION_SPEED;
	}
	if (Input::IsInputKey(ACTION_CAMERA_DOWN))
	{
		// 奥回転
		m_Rot.x -= ROTATION_SPEED;
	}

	// X軸回転は頭上や足元を超えようとするとカメラ向きがひっくり返るため
	// 超えないように止める
	if (m_Rot.x >= X_ROTATION_MAX)
	{
		m_Rot.x = X_ROTATION_MAX;
	}
	else if (m_Rot.x <= -X_ROTATION_MAX)
	{
		m_Rot.x = -X_ROTATION_MAX;
	}
}

void Camera::Update()
{
	if (!m_TargetPlayer)
		return;

	// プレイヤーの座標
	VECTOR playerPos = m_TargetPlayer->GetPos();
	
	// 通常時はプレイヤーを注視する
	m_Target = playerPos;
    m_Target.y += 1.0f;

	// 通常時は標準のカメラ距離を使用
	float targetCameraDistance = CAMERA_DISTANCE_DEFAULT;

	// ロックオン中は敵との距離に応じてカメラを引く
	EnemyBase* lockOnEnemy = m_TargetPlayer->GetLockOnEnemy();

	if (lockOnEnemy && !lockOnEnemy->IsDead())
	{
		VECTOR enemyPos = lockOnEnemy->GetPos();
		VECTOR diff = VSub(enemyPos, playerPos);

		float enemyDistance = sqrtf(
			diff.x * diff.x +
			diff.y * diff.y +
			diff.z * diff.z
		);

		// プレイヤーと敵の中間を注視点にする
        m_Target = VScale(VAdd(playerPos, enemyPos), 0.5f);
        m_Target.y += 0.5f;

		// 敵の方向をカメラの目標角度にする
		float targetYaw = atan2f(diff.x, diff.z);

		// 現在角度と目標角度の差を求める
		float angleDiff = targetYaw - m_Rot.y;

		// 角度差を -PI ～ PI の範囲に収める
		while (angleDiff > DX_PI_F)
			angleDiff -= DX_PI_F * 2.0f;

		while (angleDiff < -DX_PI_F)
			angleDiff += DX_PI_F * 2.0f;

		// 敵の方向へ滑らかに回転する
        m_Rot.y += angleDiff * CAMERA_LOCKON_ROTATION_SPEED;

        // 敵の高さに応じて上下角度を調整
        float horizontalDistance = sqrtf(
            diff.x * diff.x +
            diff.z * diff.z
        );

        float targetPitch = atan2f(
            diff.y,
            horizontalDistance
        );

        // 敵が同じ高さでも、少し見下ろす角度を確保する
        if (targetPitch < CAMERA_LOCKON_PITCH_DEFAULT)
        {
            targetPitch = CAMERA_LOCKON_PITCH_DEFAULT;
        }

        if (targetPitch > X_ROTATION_MAX)
            targetPitch = X_ROTATION_MAX;

        if (targetPitch < -X_ROTATION_MAX)
            targetPitch = -X_ROTATION_MAX;

        float pitchDiff = targetPitch - m_Rot.x;
        m_Rot.x += pitchDiff * CAMERA_LOCKON_PITCH_SPEED;


        // 敵との距離に応じて理想カメラ距離を調整
		targetCameraDistance =
			CAMERA_DISTANCE_DEFAULT +
			enemyDistance * CAMERA_ZOOM_FACTOR;

		// カメラが引きすぎないように制限
		if (targetCameraDistance > CAMERA_DISTANCE_MAX)
		{
			targetCameraDistance = CAMERA_DISTANCE_MAX;
		}
	}

	// カメラ距離を滑らかに変化させる
	m_CameraDistance +=
		(targetCameraDistance - m_CameraDistance)
		* CAMERA_ZOOM_SPEED;

	// ① プレイヤーを原点に移動させる平行移動行列
	VECTOR vecOrigin = MyMath::VecScale(m_Target, -1.0f);
	MATRIX targetOrigin = MyMath::MatTranslation(vecOrigin);

	// ② プレイヤーの手前に移動する平行移動行列を用意
	MATRIX distans = MyMath::MatTranslation(VGet(0.0f, 0.0f, -m_CameraDistance));

	// ③ カメラのピッチ行列
	MATRIX pitch = MyMath::MatRotationPitch(m_Rot.x);
	// ④ カメラのヨー行列
	MATRIX yaw = MyMath::MatRotationYaw(m_Rot.y);

	// ⑤ プレイヤーを元の位置に戻す平行移動行列
	MATRIX targetPos = MyMath::MatTranslation(m_Target);

	// ⑥ ①～⑤の行列を逆順に合成
	MATRIX cameraMat = MyMath::MatMult(targetPos, yaw);
	cameraMat = MyMath::MatMult(cameraMat, pitch);
	cameraMat = MyMath::MatMult(cameraMat, distans);
	cameraMat = MyMath::MatMult(cameraMat, targetOrigin);

    VECTOR idealCameraPos = MyMath::MatTransform(cameraMat, m_Target);

    float nearestHitT = 1.0f;
    bool hitObstacle = false;

    StageObjectManager* stageManager = StageObjectManager::GetInstance();

    if (stageManager)
    {
        auto objects = stageManager->GetStageObjects();

        for (StageObject* obj : objects)
        {
            if (!obj)
            {
                continue;
            }

            // Floor の AABB と OBB
            Floor* floor = dynamic_cast<Floor*>(obj);

            if (floor)
            {
                for (CollisionAABB* aabb : floor->GetAABBs())
                {
                    float hitT = 1.0f;

                    if (CheckCameraAABBHit(
                        m_Target,
                        idealCameraPos,
                        aabb,
                        hitT))
                    {
                        if (hitT < nearestHitT)
                        {
                            nearestHitT = hitT;
                            hitObstacle = true;
                        }
                    }
                }

                for (CollisionOBB* obb : floor->GetOBBs())
                {
                    float hitT = 1.0f;

                    if (CheckCameraOBBHit(
                        m_Target,
                        idealCameraPos,
                        obb,
                        hitT))
                    {
                        if (hitT < nearestHitT)
                        {
                            nearestHitT = hitT;
                            hitObstacle = true;
                        }
                    }
                }
            }

            // Block の AABB
            Block* block = dynamic_cast<Block*>(obj);

            if (block)
            {
                for (CollisionAABB* aabb : block->GetAABBs())
                {
                    float hitT = 1.0f;

                    if (CheckCameraAABBHit(
                        m_Target,
                        idealCameraPos,
                        aabb,
                        hitT))
                    {
                        if (hitT < nearestHitT)
                        {
                            nearestHitT = hitT;
                            hitObstacle = true;
                        }
                    }
                }
            }
        }
    }

    // 衝突地点の手前へカメラを補正
    float allowedDistance = m_CameraDistance;

    if (hitObstacle)
    {
        float idealDistance = sqrtf(
            (idealCameraPos.x - m_Target.x) *
            (idealCameraPos.x - m_Target.x) +
            (idealCameraPos.y - m_Target.y) *
            (idealCameraPos.y - m_Target.y) +
            (idealCameraPos.z - m_Target.z) *
            (idealCameraPos.z - m_Target.z)
        );

        allowedDistance =
            idealDistance * nearestHitT -
            CAMERA_COLLISION_MARGIN;

        if (allowedDistance < CAMERA_COLLISION_MIN_DISTANCE)
        {
            allowedDistance = CAMERA_COLLISION_MIN_DISTANCE;
        }

        if (allowedDistance > idealDistance)
        {
            allowedDistance = idealDistance;
        }
    }

    // 内側へは速く、外側へはゆっくり戻す
    float correctionSpeed =
        (allowedDistance < m_CameraActualDistance)
        ? CAMERA_COLLISION_IN_SPEED
        : CAMERA_COLLISION_OUT_SPEED;

    m_CameraActualDistance +=
        (allowedDistance - m_CameraActualDistance) *
        correctionSpeed;

    // 理想位置への方向を保ったまま距離だけ補正
    VECTOR cameraDirection = VSub(idealCameraPos, m_Target);

    float directionLength = sqrtf(
        cameraDirection.x * cameraDirection.x +
        cameraDirection.y * cameraDirection.y +
        cameraDirection.z * cameraDirection.z
    );

    if (directionLength > 0.0001f)
    {
        cameraDirection = VScale(
            cameraDirection,
            1.0f / directionLength
        );

        m_Pos = VAdd(
            m_Target,
            VScale(cameraDirection, m_CameraActualDistance)
        );
    }
    else
    {
        m_Pos = idealCameraPos;
    }
    /*
	// ⑦ プレイヤーの位置から行列変換を計算したものがカメラの座標となる
	m_Pos = MyMath::MatTransform(cameraMat, m_Target);

	// カメラ位置を計算
	m_Pos = MyMath::MatTransform(cameraMat, m_Target);
    */
	SetCameraPositionAndTargetAndUpVec(m_Pos, m_Target, m_UpVec);
}

void Camera::Draw()
{
	DrawFormatString(0, 60, GetColor(255, 255, 255), "カメラの座標[%f, %f, %f]", m_Pos.x, m_Pos.y, m_Pos.z);
}

void Camera::Fin()
{

}
