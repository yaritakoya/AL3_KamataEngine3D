#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

using namespace KamataEngine;

class Lane {
public:
	// 初期化
	void Initialize(Model* model, const Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	// カメラをセット
	void SetCamera(Camera* camera) { camera_ = camera; }

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
};
