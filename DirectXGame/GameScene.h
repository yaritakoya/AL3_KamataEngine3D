#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "Skydome.h"
#include "MapChipField.h"
#include <vector>
#include "CameraController.h"

using namespace KamataEngine;

class GameScene {

	Model* model_ = nullptr;

	Camera camera_;
	Player* player_ = nullptr;

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	Model* modelSkydome_ = nullptr;

	Model* modelPlayer_ = nullptr;

	Skydome* skydome_ = nullptr;

	CameraController* cameraController_ = nullptr;

private:
	uint32_t textureHandle_ = 0;
	Model* modelBlock_;
	DebugCamera* debugCamera_ = nullptr;
	bool isDebugCameraActive_ = false;
	//マップチップフィールド
	MapChipField* mapChipField_;

public:
	// 初期化
	void Initialize();
	~GameScene();
	// 更新
	void Update();
	// 描画
	void Draw();

	void GenerateBloacks();
};