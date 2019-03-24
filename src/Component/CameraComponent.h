#pragma once
#include "SComponent.h"
#include "../Util/Vector.h"
#include "DrawableStaticMeshComponent.h"

class CameraComponent : public SComponent {
public:
	enum CAMERATYPE { PERSPECTIVE, ORTHO };
public:
	CameraComponent();
	~CameraComponent();


	void Exterminate() override;
	void Init() override;
	void Tick(float elapsedTime) override;

	SComponent* Clone(SGameObject* object) override;

	void CopyReference(SComponent* src, std::map<SGameObject*, SGameObject*> lists_obj,
						   std::map<SComponent*, SComponent*> lists_comp) override;

	mat4 GetCameraMatrix() const {
		return m_cameraMatrix;
	}

	mat4 GetProjectionMatrix() {
		if(!m_isProjectionInited){
			SetProjectionMatrix();
		}
		return m_projectionMatrix;
	}

	void SetTarget(vec3 target);
	void SetTarget(SGameObject* gameobject);
	void SetUp(vec3 up);

	void SetCameraType(CAMERATYPE type);
	void SetPerspectiveFov(float fov);
	void SetZDepthRange(float near, float far);

	void SetPerspective(float fov, float near, float far);
	void SetOrtho(float left, float right, float top, float bottom);

	void SetProjectionMatrix();


private:
	void SetCameraMatrix();

private:
	vec3* m_eye;
	vec3 m_target;
	vec3 m_up;
	SGameObject* m_targetObject;

	mat4 m_cameraMatrix;
	mat4 m_projectionMatrix;
	vec3 m_resultTarget;


	CAMERATYPE m_type = PERSPECTIVE;
	bool m_isProjectionInited = false;

	//perspective
	float m_pFov = 45.f;
	const float* m_pRatio;

	//Ortho
	float m_oLeft = -1.f;
	float m_oRight = 1.f;
	float m_oBottom = -1.f;
	float m_oTop = 1.f;

	float m_Near = 0.1f;
	float m_Far = 100.f;

};

