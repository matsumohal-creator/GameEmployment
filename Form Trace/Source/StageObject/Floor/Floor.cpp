#include "Floor.h"
#include "../../Collision/CollisionManager.h"
#include "../../Collision/CollisionAABB.h"
#include "../../Collision/CollisionOBB.h"

void Floor::Start()
{
	int frameNum = MV1GetFrameNum(m_Handle);

	printfDx("===== Floor Model Frame Info =====\n");
	printfDx("Frame Num : %d\n", frameNum);

	for (int i = 0; i < frameNum; i++)
	{
		const char* name = MV1GetFrameName(m_Handle, i);

		int result = MV1SetupReferenceMesh(m_Handle, i, TRUE);

		if (result != 0)
		{
			printfDx(
				"Frame[%d] : %s\n"
				"  Reference Mesh Setup Failed\n",
				i,
				name ? name : "NULL"
			);

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
				obb->SetSize(VGet(
					25.0f,
					50.0f,
					1.0f
				));
				float rotationY = 0.0f;

				if (strcmp(name, "Wall__4_") == 0 ||
					strcmp(name, "Wall__5_") == 0)
				{
					rotationY = 45.0f * DX_PI_F / 180.0f;
				}
				else if (strcmp(name, "Wall__6_") == 0 ||
					strcmp(name, "Wall__7_") == 0)
				{
					rotationY = -45.0f * DX_PI_F / 180.0f;
				}

				obb->SetRotationY(rotationY);

				m_OBBs.push_back(obb);
			}

			printfDx(
				"Frame[%d] : %s\n"
				"  Type   : OBB\n"
				"  Center : (%f, %f, %f)\n"
				"  Size   : (%f, %f, %f)\n",
				i,
				name ? name : "NULL",
				center.x,
				center.y,
				center.z,
				size.x,
				size.y,
				size.z
			);
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

			printfDx(
				"Frame[%d] : %s\n"
				"  Type   : AABB\n"
				"  Center : (%f, %f, %f)\n"
				"  Size   : (%f, %f, %f)\n",
				i,
				name ? name : "NULL",
				center.x,
				center.y,
				center.z,
				size.x,
				size.y,
				size.z
			);
		}

		MV1TerminateReferenceMesh(m_Handle, i, TRUE);
	}
}

StageObject* Floor::Clone()
{
	Floor* clone = new Floor;

	*clone = *this;

	clone->m_Handle = MV1DuplicateModel(m_Handle);

	clone->m_AABBs.clear();
	clone->m_OBBs.clear();

	for (CollisionAABB* aabb : m_AABBs)
	{
		CollisionAABB* cloneAABB =
			CollisionManager::GetInstance()->CreateAABB();

		if (!cloneAABB)
		{
			continue;
		}

		cloneAABB->SetTargetPos(&clone->m_Pos);
		cloneAABB->SetLocalPos(aabb->GetLocalPos());
		cloneAABB->SetSize(aabb->GetSize());

		clone->m_AABBs.push_back(cloneAABB);
	}

	for (CollisionOBB* obb : m_OBBs)
	{
		CollisionOBB* cloneOBB =
			CollisionManager::GetInstance()->CreateOBB();

		if (!cloneOBB)
		{
			continue;
		}

		cloneOBB->SetTargetPos(&clone->m_Pos);
		cloneOBB->SetLocalPos(obb->GetLocalPos());
		cloneOBB->SetSize(obb->GetSize());
		cloneOBB->SetRotationY(obb->GetRotationY());

		clone->m_OBBs.push_back(cloneOBB);
	}


	return clone;
}