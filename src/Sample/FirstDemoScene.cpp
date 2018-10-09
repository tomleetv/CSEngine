#include "FirstDemoScene.h"
#include "../Component/LightComponent.h"
#include "../Component/CameraComponent.h"
#include "../../Assets/teapot_smooth.h"
#include "../Component/MaterialComponent.h"
#include "../Component/RenderComponent.h"


FirstDemoScene::FirstDemoScene() {
	
}


FirstDemoScene::~FirstDemoScene() {
	
}


void FirstDemoScene::Init() {
	//===============
	//Managing Memory Test
	SGameObject* a = new SGameObject();
	SGameObject* b = new SGameObject();
	SGameObject* d = new SGameObject();

	cube = new ObjSurface(CH02::teapot_smoothNumVerts, CH02::teapot_smoothVerts, CH02::teapot_smoothNormals);
	cube->SetUndestroyable(false);
	b->Destroy();
	c = new SGameObject();
	c->CreateComponent<DrawableStaticMeshComponent>();
	c->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube, VertexFlagsNormals);
	c->CreateComponent<MaterialComponent>();
	c->GetComponent<MaterialComponent>()->SetShininess(40);

	c->CreateComponent<RenderComponent>();
	c->GetComponent<RenderComponent>()->SetShaderHandle(0);

	c2 = new SGameObject();
	c2->CreateComponent<DrawableStaticMeshComponent>();
	c2->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube, VertexFlagsNormals);
	c2->CreateComponent<MaterialComponent>();
	c2->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 0, 0 });
	c2->GetTransform()->m_position.y = 0.5f;

	 
	c2->CreateComponent<RenderComponent>();
	c2->GetComponent<RenderComponent>()->SetShaderHandle(0);

	c3 = new SGameObject();
	c3->CreateComponent<DrawableStaticMeshComponent>();
	c3->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube, VertexFlagsNormals);
	c3->CreateComponent<MaterialComponent>();
	c3->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 1, 0 });
	c3->GetComponent<MaterialComponent>()->SetShininess(2);
	c3->GetTransform()->m_position.y = -0.5f;


	c3->CreateComponent<RenderComponent>();
	c3->GetComponent<RenderComponent>()->SetShaderHandle(0);

	a->CreateComponent<CameraComponent>();
	a->GetTransform()->m_position = vec3{ 0, 0, 3.f };
	a->GetComponent<CameraComponent>()->SetTarget(c);

	d->CreateComponent<LightComponent>();
	d->GetTransform()->m_position = vec3{ 0, 0.3f, 0 };
	d->GetComponent<LightComponent>()->SetDirection(vec4{ 1.f, 0.5f, 1.f, 1.0f });
	d->GetComponent<LightComponent>()->SetColorAmbient(vec4{ 0.1f, 0.1f, 0.1f, 1 });
	d->GetComponent<LightComponent>()->DisableSpecular = false;
	d->GetComponent<LightComponent>()->DisableDiffuse = true;
	//===============
}


void FirstDemoScene::Tick(float elapsedTime) {

	if(startTIme == 0) {
		startTIme = elapsedTime;
	}

	//===============
	c->GetTransform()->m_rotation.y += 0.1f;
	c->GetTransform()->m_position.y = sinf(elapsedTime*0.001) * 0.1f;
	//===============

	if(elapsedTime - startTIme > 3000) {
		startTIme = elapsedTime;
		isUnvisible = !isUnvisible;
		switchingObject();
	}
}


void FirstDemoScene::Destroy() {
}


void FirstDemoScene::switchingObject() {

	c3->SetIsEnable(!isUnvisible);

	if(isUnvisible) {
		c2->Destroy();
	}else {
		c2 = new SGameObject();
		c2->CreateComponent<DrawableStaticMeshComponent>();
		c2->GetComponent<DrawableStaticMeshComponent>()->SetMesh(*cube, VertexFlagsNormals);
		c2->CreateComponent<MaterialComponent>();
		c2->GetComponent<MaterialComponent>()->SetMaterialAmbient(vec3{ 1, 0, 0 });
		c2->GetComponent<MaterialComponent>()->SetMaterialSpecular(vec3{ 0, 0, 0 });
		c2->GetTransform()->m_position.y = 0.5f;


		c2->CreateComponent<RenderComponent>();
		c2->GetComponent<RenderComponent>()->SetShaderHandle(0);
	}

}
