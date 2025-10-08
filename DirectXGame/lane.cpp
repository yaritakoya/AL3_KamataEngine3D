#include "lane.h"

void Lane::Initialize(Model* model, const Vector3& position) {
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	worldTransform_.translation_ = {4.0f, 2.0f, 0.0f};

	WorldTransformUpdate(worldTransform_);
}

void Lane::Update() {
	// Laneは固定オブジェクトなので特に処理は不要
	WorldTransformUpdate(worldTransform_);
}

void Lane::Draw() {
	// Playerと同じ描画方法
	model_->Draw(worldTransform_,*camera_);
}