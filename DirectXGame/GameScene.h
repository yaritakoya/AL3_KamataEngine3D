#pragma once
#include "KamataEngine.h"
#include <vector>

using namespace KamataEngine;

// ゲームシーン
class GameScene {
public:
	enum class BlockAnimation { Wave, OddEven, Random, Center };

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
	// アニメーションの種類
	BlockAnimation blockAnimation_ = BlockAnimation::Wave;
	// アニメーションタイマー
	float animationTimer_ = 0.0f;
	// ランダムアニメーション用のタイマー
	float randomTimer_ = 0.0f;
	// ランダムアニメーション用のインデックス
	int randomIndex_ = 0;
};
