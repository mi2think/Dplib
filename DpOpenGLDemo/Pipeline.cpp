#include "Pipeline.h"

Pipeline::Pipeline()
	: scale_(1, 1, 1)
	, position_(0, 0, 0)
	, rotate_(0, 0, 0)
{

}

void Pipeline::Scale(float s)
{
	Scale(s, s, s);
}

void Pipeline::Scale(float x, float y, float z)
{
	scale_.x = x;
	scale_.y = y;
	scale_.z = z;
}

void Pipeline::Scale(const Vector3f& scale)
{
	Scale(scale.x, scale.y, scale.z);
}

void Pipeline::WorldPos(float x, float y, float z)
{
	position_.x = x;
	position_.y = y;
	position_.z = z;
}

void Pipeline::WorldPos(const Vector3f& pos)
{
	WorldPos(pos.x, pos.y, pos.z);
}

void Pipeline::Rotate(float x, float y, float z)
{
	rotate_.x = x;
	rotate_.y = y;
	rotate_.z = z;
}

void Pipeline::Rotate(const Vector3f& rorate)
{
	Rotate(rorate.x, rorate.y, rorate.z);
}

void Pipeline::SetPerspectiveProj(const PersProjInfo& proj)
{
	persProjInfo_ = proj;
}

const Matrix44f& Pipeline::GetOGLWorldTrans()
{
	// Note: OpenGL using matrix by col major
	Matrix44f translationTrans;
	MaxtrixTranslation(translationTrans, position_);
	translationTrans.Transpose();

	Matrix44f scaleTrans;
	MatrixScaling(scaleTrans, scale_);
	scaleTrans.Transpose();

	// Note:
	// (rx * ry * rz)' = rz' * ry' * rx'
	Matrix44f rotateTrans;
	MatrixRotationZYX(rotateTrans, rotate_.x, rotate_.y, rotate_.z);
	rotateTrans.Transpose();

	// (t * r * s) = t' * r' * s'
	MatrixMultiply(worldTrans_, rotateTrans, scaleTrans);
	MatrixMultiply(worldTrans_, translationTrans, worldTrans_);

	return worldTrans_;
}

const Matrix44f& Pipeline::GetOGLProjTrans()
{
	const PersProjInfo& p = persProjInfo_;

	const float ar = p.Width / p.Height;
	const float zRange = p.zNear - p.zFar;
	const float tanHalfFOV = tanf(angle2radian(p.FOV / 2.0f));

	auto& m = projTrans_.m;

	m[0][0] = 1.0f / (tanHalfFOV * ar); m[0][1] = 0.0f;					m[0][2] = 0.0f;							m[0][3] = 0.0;
	m[1][0] = 0.0f;                     m[1][1] = 1.0f / tanHalfFOV;	m[1][2] = 0.0f;							m[1][3] = 0.0;
	m[2][0] = 0.0f;                     m[2][1] = 0.0f;					m[2][2] = (-p.zNear - p.zFar) / zRange; m[2][3] = 2.0f * p.zFar * p.zNear / zRange;
	m[3][0] = 0.0f;                     m[3][1] = 0.0f;					m[3][2] = 1.0f;							m[3][3] = 0.0;

	return projTrans_;
}

const Matrix44f& Pipeline::GetOGLWorldProjTrans()
{
	GetOGLProjTrans();

	GetOGLWorldTrans();

	MatrixMultiply(worldProjTrans_, projTrans_, worldTrans_);

	return worldProjTrans_;
}