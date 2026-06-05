#pragma once

#include "DxLib.h"

class MyMath
{
public:
	// ベクトル関連の計算関数
	static VECTOR VecCreate(VECTOR startPos, VECTOR endPos);
	static VECTOR VecAdd(VECTOR vecA, VECTOR vecB);
	static VECTOR VecScale(VECTOR vecA, float scale);
	static float VecDot(VECTOR vecA, VECTOR vecB);
	static float VecCross2D(VECTOR vecA, VECTOR vecB);
	static VECTOR VecCross3D(VECTOR vecA, VECTOR vecB);
	static float VecLong(VECTOR vec);
	static VECTOR VecNormalize(VECTOR vec);
	static VECTOR VecForwardZX(float rotY);
	// 行列関連の計算関数
	static MATRIX MatCreate();
	static MATRIX MatAdd(MATRIX matA, MATRIX matB);
	static MATRIX MatSubt(MATRIX matA, MATRIX matB);
	static MATRIX MatScale(MATRIX mat, float scale);
	static MATRIX MatMult(MATRIX matA, MATRIX matB);
	static MATRIX MatTransposition(MATRIX mat);
	static MATRIX MatTranslation(VECTOR trans);
	static MATRIX MatScale(VECTOR scale);
	static MATRIX MatRotationPitch(float rot);
	static MATRIX MatRotationYaw(float rot);
	static MATRIX MatRotationRoll(float rot);
	static VECTOR MatTransform(MATRIX transMat, VECTOR pos);
};
