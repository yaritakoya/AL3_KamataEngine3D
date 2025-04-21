#pragma once
#include "KamataEngine.h"
#include "Player.h"

using namespace KamataEngine;

// ゲームシーン
class GameScene {
public:
	// デストラクタ
	~GameScene();
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 3Dモデル
	Model* model_ = nullptr;
	// カメラ
	Camera camera_;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// スプライト
	Sprite* sprite_ = nullptr;
	// 自キャラ
	Player* player_ = nullptr;
};
