#pragma once
#include "DxLib.h"

class CameraBase 
{
public:
	CameraBase();			// コンストラクタ
	virtual ~CameraBase();	// デストラクタはvirtual（仮想関数）にしないと継承した後に呼ばれない

public:
	// 派生クラスには初期～終了までは実装必須にするため純粋仮想関数にする
	virtual void Init() = 0;
	virtual void Load() = 0;
	virtual void Start() = 0;
	virtual void Step() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Fin() = 0;

public:
	// 基底クラスにある変数のアクセサは基底クラスに書く
	VECTOR GetRot() { return m_Rot; }
	VECTOR GetTarget() { return m_Target; }

	void SetRot(VECTOR rot) { m_Rot = rot; }
	void SetTarget(VECTOR target) { m_Target = target; }

// protectedは継承したクラスであればアクセスできるアクセス制限
protected:
	// どの派生先でも共通で使用する変数は基底クラスに書く
	VECTOR m_Pos;
	VECTOR m_Rot;
	VECTOR m_Target;
	VECTOR m_UpVec;
};
