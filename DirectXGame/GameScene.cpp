#include "GameScene.h"
#include "MyMath.h"

// using namespace KamataEngine;

void GameScene::Initialize() {
	model_ = Model::Create();
	camera_.Initialize();

	modelPlayer_ = Model::CreateFromOBJ("player", true);

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(modelPlayer_, &camera_);

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	skydome_ = new Skydome();

	skydome_->Initialize(modelSkydome_, &camera_);

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// ブロック
	modelBlock_ = Model::CreateFromOBJ("block", true);
	/*const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;*/

	/*const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;*/

	//worldTransformBlocks_.resize(kNumBlockVirtical);

	//for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
	//	worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	//	worldTransformBlocks_[i] = new WorldTransform();
	//	worldTransformBlocks_[i]->Initialize();
	//	worldTransformBlocks_[i]->translation_.x = kBlockWidth * i;
	//	worldTransformBlocks_[i]->translation_.y = 0.0f;
	//}
	//for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
	//	for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
	//		if (j % 2 == 0)
	//		{
	//		    continue;
	//		}
	//		if ((i + j) % 2 == 0) {
	//			continue;
	//		}
	//		worldTransformBlocks_[i][j] = new WorldTransform();
	//		worldTransformBlocks_[i][j]->Initialize();
	//		worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
	//		worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
	//	}
	//}

	GenerateBloacks();

	// デバッグ
	debugCamera_ = new DebugCamera(1280, 720);
}

GameScene::~GameScene() {
	delete model_;
	delete player_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	delete debugCamera_;
	delete modelSkydome_;
	//マップチップforルド
	delete mapChipField_;
}

void GameScene::Update() {
	player_->Update();
	skydome_->Update();
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			worldTransformBlock->TransferMatrix();
		}
	}
	debugCamera_->Update();

#ifdef _DEBUG

	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}

#endif // DEBUG

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		camera_.UpdateMatrix();
	}

	/*for (WorldTransform* worldTransformBlock : worldTransformBlocks_)
	{
	    worldTransformBlock->matWorld_ =
	        MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
	    worldTransformBlock->TransferMatrix();
	}*/
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());
	player_->Draw();
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}
	skydome_->Draw();
	/*for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {
	    modelBlock_->Draw(*worldTransformBlock, camera_);
	}*/
	Model::PostDraw();
}

void GameScene::GenerateBloacks() {
	//要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	//要素数を変更する
	//列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
	//1列の要素数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}
	//ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j,i)==MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}
