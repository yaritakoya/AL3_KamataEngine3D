#pragma once
#include "KamataEngine.h"
#include "GameScene.h"

using namespace KamataEngine;

class Item {
public:
	void Initialize(Model* model, Camera* camera, const Vector3& pos) {
		model_ = model;
		camera_ = camera;
		worldTransform_.Initialize();
		worldTransform_.translation_ = pos;
	}

	void Update() { WorldTransformUpdate(worldTransform_); }

	void Draw() { model_->Draw(worldTransform_, *camera_); }

private:
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	WorldTransform worldTransform_;
};
