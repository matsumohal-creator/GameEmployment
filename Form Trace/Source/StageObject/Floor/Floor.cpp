#include "Floor.h"
#include "../../Collision/CollisionManager.h"
#include "../../Collision/CollisionAABB.h"
#include "../../Collision/CollisionOBB.h"
#include <cstring>

void Floor::Start()
{
	// チュートリアル用のAABB / OBB判定を作成する
    if (m_QuestID == QUEST_TUTORIAL)
    {
        // ========================================
        // Tutorial用
        // ========================================

        int frameNum = MV1GetFrameNum(m_Handle);

        for (int i = 0; i < frameNum; i++)
        {
            const char* name = MV1GetFrameName(m_Handle, i);

            int result =
                MV1SetupReferenceMesh(m_Handle, i, TRUE);

            if (result != 0)
            {
                continue;
            }

            MV1_REF_POLYGONLIST refMesh =
                MV1GetReferenceMesh(m_Handle, i, TRUE);

            VECTOR minPos = refMesh.MinPosition;
            VECTOR maxPos = refMesh.MaxPosition;

            VECTOR center = VGet(
                (minPos.x + maxPos.x) * 0.5f,
                (minPos.y + maxPos.y) * 0.5f,
                (minPos.z + maxPos.z) * 0.5f
            );

            VECTOR size = VGet(
                maxPos.x - minPos.x,
                maxPos.y - minPos.y,
                maxPos.z - minPos.z
            );

            bool isDiagonalWall =
                name &&
                (
                    strcmp(name, "Wall__4_") == 0 ||
                    strcmp(name, "Wall__5_") == 0 ||
                    strcmp(name, "Wall__6_") == 0 ||
                    strcmp(name, "Wall__7_") == 0
                    );

            if (isDiagonalWall)
            {
                CollisionOBB* obb =
                    CollisionManager::GetInstance()->CreateOBB();

                if (obb)
                {
                    obb->SetTargetPos(&m_Pos);
                    obb->SetLocalPos(center);

                    obb->SetSize(
                        VGet(
                            25.0f,
                            50.0f,
                            1.0f
                        )
                    );

                    float rotationY = 0.0f;

                    if (strcmp(name, "Wall__4_") == 0 ||
                        strcmp(name, "Wall__5_") == 0)
                    {
                        rotationY =
                            45.0f * DX_PI_F / 180.0f;
                    }
                    else if (strcmp(name, "Wall__6_") == 0 ||
                        strcmp(name, "Wall__7_") == 0)
                    {
                        rotationY =
                            -45.0f * DX_PI_F / 180.0f;
                    }

                    obb->SetRotationY(rotationY);

                    m_OBBs.push_back(obb);
                }
            }
            else
            {
                CollisionAABB* aabb =
                    CollisionManager::GetInstance()->CreateAABB();

                if (aabb)
                {
                    aabb->SetTargetPos(&m_Pos);
                    aabb->SetLocalPos(center);
                    aabb->SetSize(size);

                    m_AABBs.push_back(aabb);
                }
            }

            MV1TerminateReferenceMesh(m_Handle, i, TRUE);
        }
    }
    // QuestIDがQUEST_CITYの場合の処理
    else if (m_QuestID == QUEST_CITY)
    {
        // ========================================
        // 市街地用
        // ========================================

        int frameNum = MV1GetFrameNum(m_Handle);

        for (int i = 0; i < frameNum; i++)
        {
            const char* name = MV1GetFrameName(m_Handle, i);

            // 名前が取得できないフレームは無視
            if (name == nullptr)
            {
                continue;
            }

            // ----------------------------------------
            // AABB対象か確認
            // ----------------------------------------
            bool isAABB =
                strcmp(name, "Bridge") == 0 ||
                strcmp(name, "Bridge.001") == 0 ||
                strcmp(name, "Object1") == 0 ||
                strcmp(name, "OuterWall") == 0 ||
                strcmp(name, "OuterWall.001") == 0 ||
                strcmp(name, "OuterWall.002") == 0 ||
                strcmp(name, "OuterWall.003") == 0 ||
                strcmp(name, "PerimeterFloor") == 0 ||
                strcmp(name, "PerimeterFloor.001") == 0 ||
                strcmp(name, "PerimeterFloor.002") == 0 ||
                strcmp(name, "PerimeterFloor.003") == 0 ||
                strcmp(name, "Transparent") == 0 ||
                strcmp(name, "Transparent.002") == 0 ||
                strcmp(name, "Transparent.003") == 0 ||
                strcmp(name, "Transparent.005") == 0 ||
                strcmp(name, "Transparent.008") == 0 ||
                strcmp(name, "Transparent.010") == 0 ||
                strcmp(name, "Water") == 0;

            // ----------------------------------------
            // OBB対象か確認
            // ----------------------------------------
            bool isOBB =
                strcmp(name, "Bridge.002") == 0 ||
                strcmp(name, "Object") == 0 ||
                strcmp(name, "Object2") == 0 ||
                strcmp(name, "Object3") == 0 ||
                strcmp(name, "Object4") == 0 ||
                strcmp(name, "Object5") == 0 ||
                strcmp(name, "PerimeterFloor.004") == 0 ||
                strcmp(name, "PerimeterFloor.005") == 0 ||
                strcmp(name, "PerimeterFloor.006") == 0 ||
                strcmp(name, "PerimeterFloor.007") == 0 ||
                strcmp(name, "Transparent.001") == 0 ||
                strcmp(name, "Transparent.004") == 0 ||
                strcmp(name, "Transparent.006") == 0 ||
                strcmp(name, "Transparent.007") == 0 ||
                strcmp(name, "Transparent.009") == 0;

            // ----------------------------------------
            // AABB / OBBどちらでもなければ無視
            // ----------------------------------------
            if (!isAABB && !isOBB)
            {
                continue;
            }

            // ----------------------------------------
            // 参照メッシュ取得
            // ----------------------------------------
            int result =
                MV1SetupReferenceMesh(m_Handle, i, TRUE);

            if (result != 0)
            {
                continue;
            }

            MV1_REF_POLYGONLIST refMesh =
                MV1GetReferenceMesh(m_Handle, i, TRUE);

            VECTOR minPos = refMesh.MinPosition;
            VECTOR maxPos = refMesh.MaxPosition;

            VECTOR center = VGet(
                (minPos.x + maxPos.x) * 0.5f,
                (minPos.y + maxPos.y) * 0.5f,
                (minPos.z + maxPos.z) * 0.5f
            );

            VECTOR size = VGet(
                maxPos.x - minPos.x,
                maxPos.y - minPos.y,
                maxPos.z - minPos.z
            );

            // ========================================
            // AABB
            // ========================================
            if (isAABB)
            {
                CollisionAABB* aabb =
                    CollisionManager::GetInstance()->CreateAABB();

                if (aabb)
                {
                    aabb->SetTargetPos(&m_Pos);
                    aabb->SetLocalPos(center);
                    aabb->SetSize(size);

                    m_AABBs.push_back(aabb);
                }
            }

            // ========================================
            // OBB
            // ========================================
            else if (isOBB)
            {
                CollisionOBB* obb =
                    CollisionManager::GetInstance()->CreateOBB();

                if (obb)
                {
                    obb->SetTargetPos(&m_Pos);

                    // --------------------------------------------------
                    // 基本はモデルのBoundingBoxをそのまま使用
                    // --------------------------------------------------
                    VECTOR obbCenter = center;
                    VECTOR obbSize = size;

                    // ==================================================
                    // Object4
                    // ==================================================
                    if (strcmp(name, "Object4") == 0)
                    {
                        // 三角形を横長・薄めのOBBとして近似
                        obbSize = VGet(
                            size.x * 1.5f,
                            size.y,
                            size.z * 0.5f
                        );

                        // 見た目の壁より手前側へ少し調整
                        obbCenter.z -= 5.0f;
						obbCenter.x -= 3.0f;
                    }

                    // ==================================================
                    // Object5
                    // ==================================================
                    else if (strcmp(name, "Object5") == 0)
                    {
                        // 厚みを減らし、横方向を広げる
                        obbSize = VGet(
                            size.x * 0.5f,
                            size.y,
                            size.z * 2.0f
                        );

                        // OBBを少し手前側へ
                        obbCenter.z += 8.0f;
                    }

                    // ==================================================
                    // PerimeterFloor.004
                    // ==================================================
                    else if (strcmp(name, "PerimeterFloor.004") == 0)
                    {
                        obbSize = VGet(
                            size.x * 1.0f,
                            size.y,
                            size.z * 1.0f
                        );
                    }

                    // ==================================================
                    // PerimeterFloor.005
                    // ==================================================
                    else if (strcmp(name, "PerimeterFloor.005") == 0)
                    {
                        obbSize = VGet(
                            size.x * 1.0f,
                            size.y,
                            size.z * 1.0f
                        );
                    }

                    // ==================================================
                    // PerimeterFloor.006
                    // ==================================================
                    else if (strcmp(name, "PerimeterFloor.006") == 0)
                    {
                        obbSize = VGet(
                            size.x * 1.20f,
                            size.y,
                            size.z * 1.20f
                        );
                    }

                    // ==================================================
                    // PerimeterFloor.007
                    // ==================================================
                    else if (strcmp(name, "PerimeterFloor.007") == 0)
                    {
                        obbSize = VGet(
                            size.x * 1.20f,
                            size.y,
                            size.z * 1.20f
                        );
                    }

                    // Transparent.001
                    else if (strcmp(name, "Transparent.001") == 0)
                    {
                        // 個別に調整
                        obbSize = VGet(
                            size.x * 1.4f,
                            size.y,
                            size.z * 0.1f
                        );


                        obbCenter.x += 0.7f;
                    }

                    // ==================================================
                    // Transparent.004
                    // ==================================================
                    else if (strcmp(name, "Transparent.004") == 0)
                    {
                        // Object4 / Object5と同様に、
                        // 横長・薄めに調整
                        obbSize = VGet(
                            size.x * 0.1f,
                            size.y,
                            size.z * 1.4
                        );
                        obbCenter.x += 0.5f;
                        obbCenter.z += 0.5f;
                    }

                    else if (strcmp(name, "Transparent.006") == 0)
                    {
                        obbSize = VGet(
                            size.x * 0.5f,
                            size.y,
                            size.z * 1.0f
                        );
                        obbCenter.x -= 1.5f;
                        obbCenter.z -= 1.5f;
                    }
                    else if (strcmp(name, "Transparent.007") == 0)
                    {
                        obbSize = VGet(
                            size.x * 0.5f,
                            size.y,
                            size.z * 1.0f
                        );
                        obbCenter.x += 2.0f;
                        obbCenter.z += 2.0f;
                    }
                    // ==================================================
                    // Transparent.009
                    // ==================================================
                    else if (strcmp(name, "Transparent.009") == 0)
                    {
                        // 横長・薄めに調整
                        obbSize = VGet(
                            size.x * 0.1f,
                            size.y,
                            size.z * 1.4f
                        );
						obbCenter.x -= 0.5f;
                        obbCenter.z -= 0.5f;
                    }

                    obb->SetLocalPos(obbCenter);
                    obb->SetSize(obbSize);

                    float rotationY = 0.0f;

                    // --------------------------------------------------
                    // Bridge.002
                    // --------------------------------------------------
                    if (strcmp(name, "Bridge.002") == 0)
                    {
                        rotationY =
                            -135.0f * DX_PI_F / 180.0f;
                    }

                    // --------------------------------------------------
                    // Object / Object2 / Object3
                    // --------------------------------------------------
                    else if (strcmp(name, "Object") == 0 ||
                        strcmp(name, "Object2") == 0 ||
                        strcmp(name, "Object3") == 0)
                    {
                        rotationY =
                            45.0f * DX_PI_F / 180.0f;
                    }

                    // --------------------------------------------------
                    // Object4
                    // --------------------------------------------------
                    else if (strcmp(name, "Object4") == 0)
                    {
                        rotationY =
                            45.0f * DX_PI_F / 180.0f;
                    }

                    // --------------------------------------------------
                    // Object5
                    // --------------------------------------------------
                    else if (strcmp(name, "Object5") == 0)
                    {
                        rotationY =
                            -45.0f * DX_PI_F / 180.0f;
                    }

                    // --------------------------------------------------
                    // Transparent系
                    // --------------------------------------------------
                    else if (strcmp(name, "Transparent.001") == 0 ||
                        strcmp(name, "Transparent.004") == 0 ||
                        strcmp(name, "Transparent.006") == 0 ||
                        strcmp(name, "Transparent.007") == 0 ||
                        strcmp(name, "Transparent.009") == 0)
                    {
                        rotationY =
                            -45.0f * DX_PI_F / 180.0f;
                    }

                    // --------------------------------------------------
                    // PerimeterFloor.004 / .007
                    // --------------------------------------------------
                    else if (strcmp(name, "PerimeterFloor.004") == 0 ||
                        strcmp(name, "PerimeterFloor.007") == 0)
                    {
                        //rotationY = -45.0f * DX_PI_F / 180.0f;
                    }

                    // --------------------------------------------------
                    // PerimeterFloor.005 / .006
                    // --------------------------------------------------
                    else if (strcmp(name, "PerimeterFloor.005") == 0 ||
                        strcmp(name, "PerimeterFloor.006") == 0)
                    {
                        //rotationY = 45.0f * DX_PI_F / 180.0f;
                    }

                    obb->SetRotationY(rotationY);

                    m_OBBs.push_back(obb);
                }
            }

            MV1TerminateReferenceMesh(m_Handle, i, TRUE);
        }
    }
}

// Floorは1個しか存在しないため、Cloneしない。
// StageObjectの純粋仮想関数を満たすためだけに実装する。
StageObject* Floor::Clone()
{
	return nullptr;
}