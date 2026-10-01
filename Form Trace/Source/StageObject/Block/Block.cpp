#include "Block.h"
#include "../../Collision/CollisionManager.h"
#include "../../Collision/CollisionAABB.h"

void Block::Start()
{

}

void Block::Update()
{
	VECTOR rot = VGet(
		m_Rot.x * DX_PI_F / 180.0f,
		m_Rot.y * DX_PI_F / 180.0f,
		m_Rot.z * DX_PI_F / 180.0f
	);

	// モデルにTransformを設定
	MV1SetPosition(m_Handle, m_Pos);
	MV1SetRotationXYZ(m_Handle, rot);
	MV1SetScale(m_Handle,VGet(1.0f, 1.0f, 1.0f));

	// まだAABBを作っていない場合だけ生成
	if (m_AABBs.empty())
	{
		int frameNum = MV1GetFrameNum(m_Handle);

		for (int i = 0; i < frameNum; i++)
		{
			const char* name = MV1GetFrameName(m_Handle, i);

			// ReferenceMeshをセットアップ
			int result = MV1SetupReferenceMesh(m_Handle, i, TRUE);

			if (result != 0)
			{
				continue;
			}

			// Transform済みReferenceMeshを更新
			result = MV1RefreshReferenceMesh(m_Handle, i, TRUE);

			if (result != 0)
			{
				MV1TerminateReferenceMesh(
					m_Handle,
					i,
					TRUE
				);

				continue;
			}

			// Transform済みReferenceMeshを取得
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
				// ReferenceMeshはTransform済みなので、
				// m_Pos分を引いてLocalPosとして保存する
				VECTOR localPos = VGet(
					center.x - m_Pos.x,
					center.y - m_Pos.y,
					center.z - m_Pos.z
				);

				aabb->SetTargetPos(&m_Pos);
				aabb->SetLocalPos(localPos);
				aabb->SetSize(size);

				m_AABBs.push_back(aabb);
			}

			MV1TerminateReferenceMesh(
				m_Handle,
				i,
				TRUE
			);
		}
	}
}

StageObject* Block::Clone()
{
	Block* clone = new Block;

	*clone = *this;
	clone->m_Handle = MV1DuplicateModel(m_Handle);
	clone->m_AABBs.clear();

	for (CollisionAABB* aabb : m_AABBs) 
	{
		CollisionAABB* cloneAABB = CollisionManager::GetInstance()->CreateAABB();
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

