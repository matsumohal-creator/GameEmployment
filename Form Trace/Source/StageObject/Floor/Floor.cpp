#include "Floor.h"
#include "../../Collision/CollisionManager.h"
#include "../../Collision/CollisionAABB.h"

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

		MV1TerminateReferenceMesh(m_Handle, i, TRUE);
	}
}

StageObject* Floor::Clone()
{
	Floor* clone = new Floor;

	*clone = *this;

	clone->m_Handle = MV1DuplicateModel(m_Handle);

	clone->m_AABBs.clear();

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

	return clone;
}