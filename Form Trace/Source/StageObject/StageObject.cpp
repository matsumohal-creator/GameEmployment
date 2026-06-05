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
}

void StageObject::Update()
{
	MV1SetPosition(m_Handle, m_Pos);
	MV1SetRotationXYZ(m_Handle, m_Rot);
	MV1SetScale(m_Handle, m_Scale);
}

void StageObject::Draw()
{
	MV1DrawModel(m_Handle);
}

void StageObject::Fin()
{
	MV1DeleteModel(m_Handle);
}
