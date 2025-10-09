#include "lane.h"
#include <cmath>

void Lane::Initialize(Model* model, const Vector3& position) {
	model_ = model;
	worldTransform_.Initialize();

	// Player の目の前に配置されるように、少し前方に出す
	// Player の向きがZ+方向を向いていると仮定
	worldTransform_.translation_ = position + Vector3(0.0f, 0.0f, 5.0f);

	// サイズを大きくして見やすく
	worldTransform_.scale_ = {5.0f, 0.5f, 3.0f};

	// Player のカメラから見えるように、Player と同じ向き（Z方向）に回転
	worldTransform_.rotation_.y = 0.0f;

	WorldTransformUpdate(worldTransform_);
}

void Lane::Update() { WorldTransformUpdate(worldTransform_); }

void Lane::Draw() {
	// カメラが設定されていれば Player の視点で描画
	if (camera_) {
		model_->Draw(worldTransform_, *camera_);
	}

	// デバッグ確認：Lane の位置を赤い球で可視化
	//DebugDrawSphere(worldTransform_.translation_, 0.5f, {1, 0, 0, 1});
}
