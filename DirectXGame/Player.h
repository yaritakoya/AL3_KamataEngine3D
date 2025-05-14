#pragma once
#include "KamataEngine.h"
#include <vector>

using namespace KamataEngine;

// 自キャラ
class Player {
private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	uint32_t textureHandle_ = 0u;

	std::vector<std::vector<WorldTransform*>> worldTransformPlayer_;

public:
	// 初期化
	void Initialize(Model* model, Camera* camera);
	// 更新
	void Update();
	// 描画
	void Draw();
};