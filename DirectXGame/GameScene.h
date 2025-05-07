#pragma once
#include "KamataEngine.h"
#include <vector>

using namespace KamataEngine;

// ゲームシーン
class GameScene {
public:
	~GameScene();
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// ブロック用のワールドトランスフォーム
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

private:
	// 3Dモデルデータ
	Model* modelBlock_ = Model::CreateFromOBJ("cube");
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// カメラ
	Camera camera_;
	// デバックカメラ有効
	bool isDebugCameraActive_ = false;
	//デバックカメラの生成
	DebugCamera* debugCamera_ = nullptr;

};
