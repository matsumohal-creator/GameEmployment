#include "StageObject.h"

StageObject::StageObject()
{
	m_Handle = -1;
	m_Pos = {};
	m_Rot = {};
	m_Scale = {};
	m_AABB = nullptr;
}

StageObject::~StageObject()
{
	Fin();
}

void StageObject::Start()
{
}

void StageObject::Load(const char* fileName)
{
	m_Handle = MV1LoadModel(fileName);

	// Zバッファを有効にする
	MV1SetUseZBuffer(m_Handle, TRUE);
}

void StageObject::Update()
{
	MV1SetPosition(m_Handle, m_Pos);

	VECTOR rot = VGet(
		m_Rot.x * DX_PI_F / 180.0f,
		m_Rot.y * DX_PI_F / 180.0f,
		m_Rot.z * DX_PI_F / 180.0f
	);

	MV1SetRotationXYZ(m_Handle, rot);
	MV1SetScale(m_Handle, m_Scale);
}

void StageObject::Draw()
{
	MV1DrawModel(m_Handle);
}

void StageObject::Fin()
{
	if (m_Handle != -1)
	{
		MV1DeleteModel(m_Handle);
		m_Handle = -1;
		// Zバッファを無効にする
		MV1SetUseZBuffer(m_Handle, FALSE);
	}
}