#include "GameScene.h"
#include "MyMath.h"
#include <cstdlib>

GameScene::~GameScene() {
	// 3Dモデルデータの開放
	delete modelBlock_;
	// デバッグカメラの開放
	delete debugCamera_;

	//
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

void GameScene::Initialize() {
	// 3Dモデルデータの生成
	modelBlock_ = Model::Create();

	// カメラの初期化
	camera_.Initialize();

	// ブロック数
	const uint32_t kNumBlockVirtcal = 1;
	const uint32_t kNumBlockHorizontal = 10;

	// ブロック1個分のサイズ
	const float kBlockWidth = 2.0f;

	// 行数を設定
	worldTransformBlocks_.resize(kNumBlockVirtcal);

	// 列数を設定
	for (uint32_t i = 0; i < kNumBlockVirtcal; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// ブロック生成
	for (uint32_t i = 0; i < kNumBlockVirtcal; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {

			worldTransformBlocks_[i][j] = new WorldTransform();
			worldTransformBlocks_[i][j]->Initialize();
			// 横一列に配置
			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
			worldTransformBlocks_[i][j]->translation_.y = 0.0f;
			worldTransformBlocks_[i][j]->translation_.z = 0.0f;
		}
	}
	// ランダムアニメーション用のインデックスを初期化
	randomIndex_ = rand() % worldTransformBlocks_[0].size();
	
	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
}

void GameScene::Update() {

	// アニメーションタイマー
	animationTimer_ += 0.03f;
	// ランダムアニメーション用のタイマー
	randomTimer_ += 0.03f;
	// ランダムアニメーション用のタイマーが1秒を超えたら、次のランダムなブロックを選ぶ
	if (randomTimer_ >= 1.0f) {
		randomTimer_ = 0.0f;
		// 次のランダムなブロック
		randomIndex_ = rand() % worldTransformBlocks_[0].size();
	}

	// ブロックの高さ
	const float kBlockHeight = 2.0f;

	// 2秒で1周
	float timer = fmodf(animationTimer_, 2.0f);

	// ブロック更新
	for (uint32_t i = 0; i < worldTransformBlocks_.size(); i++) {
		for (uint32_t j = 0; j < worldTransformBlocks_[i].size(); j++) {

			WorldTransform* worldTransformBlock = worldTransformBlocks_[i][j];

			if (!worldTransformBlock) {
				continue;
			}

			float scaleY = 1.0f;

			switch (blockAnimation_) {
			//==============================
			// 奇数・偶数で交互
			//==============================
			case BlockAnimation::OddEven: {
				// 偶数・奇数どちらを動かすか
				bool isActive;
				// 0～1秒で偶数、1～2秒で奇数を動かす
				if (timer < 1.0f) {
					isActive = (j % 2 == 0);
				} else {
					isActive = (j % 2 == 1);
				}
				// 選ばれたブロックだけ動く
				if (isActive) {
					// 0～1のアニメーション時間
					float localTime = fmodf(animationTimer_, 1.0f);
					// 滑らかに伸び縮み
					scaleY = 1.0f + sinf(localTime * 3.141592f);
				}
				break;
			}

			//==============================
			// 波アニメーション
			//==============================
			case BlockAnimation::Wave: {
				// 波の速さ
				const float waveSpeed = 3.0f;
				// ブロック間の間隔
				const float waveLength = 0.5f;
				// 波を計算
				float wave = sinf(animationTimer_ * waveSpeed - j * waveLength);
				// -1～1 を 0～1 に変換
				wave = (wave + 1.0f) * 0.5f;
				// 1～2倍に変換
				scaleY = 1.0f + wave;
				break;
			}

			//==============================
			// ランダム
			//==============================
			case BlockAnimation::Random: {
				// 選ばれたブロックだけ動く
				if (j == static_cast<uint32_t>(randomIndex_)) {
					// 0～1秒で伸びて戻る
					scaleY = 1.0f + sinf(randomTimer_ * 3.141592f);
				}
				break;
			}

			//==============================
			// 中央から（後で追加）
			//==============================
			case BlockAnimation::Center:
				break;
			}

			// スケール設定
			worldTransformBlock->scale_.x = 1.0f;
			worldTransformBlock->scale_.y = scaleY;
			worldTransformBlock->scale_.z = 1.0f;

			// 下を固定して上だけ伸ばす
			worldTransformBlock->translation_.y = (scaleY - 1.0f) * kBlockHeight * 0.5f;

			// 行列更新
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			worldTransformBlock->TransferMatrix();
		}
	}
//#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		blockAnimation_ = BlockAnimation::Wave;
	}

	if (Input::GetInstance()->TriggerKey(DIK_2)) {
		blockAnimation_ = BlockAnimation::OddEven;
	}

	if (Input::GetInstance()->TriggerKey(DIK_3)) {
		blockAnimation_ = BlockAnimation::Random;
	}

	if (Input::GetInstance()->TriggerKey(DIK_4)) {
		blockAnimation_ = BlockAnimation::Center;
	}
//#endif
}

void GameScene::Draw() {
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}
	// 3Dモデル描画後処理
	Model::PostDraw();
}
