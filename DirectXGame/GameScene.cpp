#include "GameScene.h"
#include "MyMath.h"

using namespace KamataEngine;

GameScene::~GameScene() {

	delete sprite_;
	delete model_;

	delete block_model_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete debugCamera_;
	delete modelSkydome_;
	delete mapChipField_;

	// 02_09 10枚目 敵クラス削除→02_10 6枚目で削除
	//	delete enemies_;

	// 02_10 6枚目 敵クラス削除
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	// 02_11_17枚目
	delete deathParticles_;
	delete deathParticle_model_;

	// フェード
	delete fade_;
}

void GameScene::Initialize() {

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("sample.png");
	// スプライト生成
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	// 3Dモデル生成
	model_ = Model::Create();
	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();

	// カメラ初期化
	camera_.Initialize();

	// ブロックモデル
	block_model_ = Model::CreateFromOBJ("block");

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 02_03 skydome生成
	skydome_ = new Skydome();
	// 初期化
	modelSkydome_ = Model::CreateFromOBJ("SkyDome", true);
	skydome_->Initialize(modelSkydome_, &camera_);

	// 02_04マップチップ
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	// 02_07 マップチップクラスを作ってからプレイヤークラスを作る
	// という順番に入れ替える
	// 02_01から追加 プレイヤー生成
	player_ = new Player();

	// プレイヤーモデル
	player_model_ = Model::CreateFromOBJ("player");
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 18);

	// Player 初期化
	player_ = new Player();
	player_->Initialize(player_model_, &camera_, {4.0f, 2.0f, 0.0f});

	// Player の位置を取得
	Vector3 playerPos = player_->GetWorldPosition();

	// Lane 初期化（Player の位置を渡す）
	laneModel_ = Model::CreateFromOBJ("lane");
	lane_ = new Lane();
	lane_->Initialize(laneModel_, playerPos);

	// Player のカメラを共有
	lane_->SetCamera(&camera_);

	// 02_07 スライド5枚目
	player_->SetMapChipField(mapChipField_);

	// 02_06カメラコントローラ スライド13枚目
	CController_ = new CameraController(); // 生成
	CController_->Initialize(&camera_);    // 初期化
	CController_->SetTarget(player_);      // 追従対象セット
	CController_->Reset();                 // リセット

	// 02_06カメラコントローラ スライド18枚目
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	CController_->SetMovableArea(cameraArea);

	// 02_09 10枚目 敵クラス → 02_10の5枚目で削除
	//	enemy_ = new Enemy();
	// 02_09 10枚目 敵モデル
	enemy_model_ = Model::CreateFromOBJ("enemy");
	// 02_09 10枚目 敵位置決めて敵クラス初期化 → 02_10の5枚目で削除
	//	Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(14, 18);
	// enemy_->Initialize(enemy_model_, &camera_, enemyPosition);
	float playerY = player_->GetWorldPosition().y; // プレイヤーの高さに合わせる

	// 画面右端から出現（例: x = 50.0f）
	float startX = 30.0f;
	float startZ = 0.0f;

	// 3体並べる（間隔2.0f）
	for (int32_t i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();

		Vector3 enemyPosition = {startX + i * 4.0f, playerY, startZ};

		newEnemy->Initialize(enemy_model_, &camera_, enemyPosition);

		// 移動速度を調整するための関数がある場合
		newEnemy->SetMoveSpeed(0.2f); // ← 速度アップ（デフォルト0.05〜0.1程度なら倍速）

		enemies_.push_back(newEnemy);
	}

	// 02_11_16枚目 モデル読み込み
	deathParticle_model_ = Model::CreateFromOBJ("deathParticle");

	// 02_12_4枚目 ゲームプレイフェーズから開始
	phase_ = Phase::kFadeIn;

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

// 02_12 10枚目 GameScene::Update関数で呼び出しておく
// player->draw();をif(!player_->IsDead()){}で囲む
void GameScene::ChangePhase() {

	switch (phase_) {
	case Phase::kPlay:
		// 02_12 13枚目 if文から中身まで全部実装
		// Initialize関数のいきなりパーティクル発生処理は消す
		if (player_->IsDead()) {
			// 死亡演出
			phase_ = Phase::kDeath;

			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(deathParticle_model_, &camera_, deathParticlesPosition);
		}
		break;
	case Phase::kDeath:
		// デス演出フェーズの処理
		if (deathParticles_->IsFinished()) {
			// デス演出終了
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			// ゲームプレイへ切り替え
			phase_ = Phase::kPlay;
		}
		break;
	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			finished_ = true; // ゲームシーン終了フラグを立てる
		}
		break;
	}
}

void GameScene::GenerateBlocks() {

	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {

		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

// ゲームシーン更新
void GameScene::Update() {

	ChangePhase();

	// フェード
	fade_->Update();

	// 02_12 5枚目 まず追加
	switch (phase_) {
	case Phase::kPlay:
		// ゲームプレイフェーズの処理
		break;
	case Phase::kDeath:
		// 02_12 34枚目 デス演出フェーズの処理
		// deathParticles_->IsFinished関数をDeathParticles.hに実装
		deathParticles_->Update();
		break;
	case Phase::kFadeIn:
		// フェード
		fade_->Update();
		break;
	case Phase::kFadeOut:
		// フェードアウト中
		fade_->Update();
		break;
	}

	player_->Update();
	skydome_->Update();
	CController_->Update();

	// 敵の更新
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	// ★左端チェック & リスポーン
	for (Enemy* enemy : enemies_) {
		Vector3 pos = enemy->GetWorldPosition();
		if (pos.x <= 0.0f) {
			pos.x = 30.0f;
			pos.y = (rand() % 2 == 0) ? 3.0f : 9.0f;
			enemy->SetWorldPosition(pos);
		}
	}

	for (Enemy* enemy : enemies_) {
		Vector3 pos = enemy->GetWorldPosition();
		if (pos.x <= 0.0f) {
			pos.x = 30.0f; // 右端リスポーン
			enemy->SetWorldPosition(pos);
		}
	}

#ifdef _DEBUG
	// if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
	//	// フラグをトグル
	//	isDebugCameraActive_ = !isDebugCameraActive_;
	// }
#endif

	// カメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		camera_.UpdateMatrix();
	}

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock)
				continue;

			// アフィン変換～DirectXに転送
			WorldTransformUpdate(*worldTransformBlock);
		}
	}

	// デバッグカメラの更新
	debugCamera_->Update();

	// 02_10 22枚目 衝突判定
	CheckAllCollisions();

	// 02_11 18枚目 デスパーティクルあれば更新
	if (deathParticles_) {
		deathParticles_->Update();
	}
	// 敵の死エフェクトを更新（Player の deathParticles_ と同じように扱う）
	for (auto it = enemyDeathParticles_.begin(); it != enemyDeathParticles_.end();) {
		(*it)->Update();
		if ((*it)->IsFinished()) {
			delete *it;
			it = enemyDeathParticles_.erase(it);
		} else {
			++it;
		}
	}
}

void GameScene::Draw() {

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dオブジェクト描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 自キャラの描画
	if (phase_ == Phase::kPlay || phase_ == Phase::kFadeIn) {
		player_->Draw();
	}

	// 天球描画
	skydome_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			block_model_->Draw(*worldTransformBlock, camera_);
		}
	}

	// 02_09 12枚目 敵更新 → 02_10 7枚目で更新
	//	enemy_->Draw();
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	// 既にあるプレイヤー用デスパーティクル描画の直後に追加
	if (deathParticles_) {
		deathParticles_->Draw();
	}

	// 既にあるプレイヤー用デスパーティクル描画の直後に追加
	if (deathParticles_) {
		deathParticles_->Draw();
	}

	// --- ここに敵用デスパーティクルを描画（Player と同じやり方） ---
	for (DeathParticles* effect : enemyDeathParticles_) {
		effect->Draw();
	}

	Model::PostDraw();

	// スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// スプライト描画後処理
	Sprite::PostDraw();

	// フェード
	fade_->Draw();
}

void GameScene::CheckAllCollisions() {

	AABB aabb1, aabb2;

#pragma region 自キャラと敵キャラの当たり判定
	{
		// 自キャラのAABBを取得
		aabb1 = player_->GetAABB();

		for (Enemy* enemy : enemies_) {
			aabb2 = enemy->GetAABB();

			if (IsCollision(aabb1, aabb2)) {
				// Playerは死なない → Enemyを初期位置に戻す
				enemy->ResetPosition();
			}
		}
	}
#pragma endregion

#pragma region プレイヤー攻撃と敵キャラの当たり判定
	{
		if (player_->IsAttacking()) {
			AABB attackAABB = player_->GetAttackAABB();

			// 攻撃AABBの中心X
			float attackCenterX = (attackAABB.min.x + attackAABB.max.x) * 0.5f;

			// AABBの横幅
			float attackWidth = attackAABB.max.x - attackAABB.min.x;
			float halfWidth = attackWidth * 0.5f;
			float quarterWidth = halfWidth * 0.5f;

			// 元々の削除対象リスト（コメント化せずそのまま保持）
			std::vector<Enemy*> toRemove;

			for (Enemy* enemy : enemies_) {
				AABB enemyAABB = enemy->GetAABB();
				if (IsCollision(attackAABB, enemyAABB)) {

					// ★ ヒット位置（敵の中心）
					Vector3 enemyPos = enemy->GetWorldPosition();
					float distX = enemyPos.x - attackCenterX;
					float absDist = fabsf(distX);

					// ★ 判定（スコア加算）
					if (absDist < quarterWidth * 0.5f) {
						score_ += 2;
						// OutputDebugStringA("PERFECT\n");
					} else if (absDist < halfWidth) {
						score_ += 1;
						// OutputDebugStringA("GREAT\n");
					} else {
						// OutputDebugStringA("BAD\n");
					}

					// ★ エフェクト発生（従来通り）
					DeathParticles* enemyEffect = new DeathParticles;
					enemyEffect->Initialize(deathParticle_model_, &camera_, enemyPos);
					enemyDeathParticles_.push_back(enemyEffect);

					// ★ 敵を右端へワープ（Yランダム）
					enemyPos.x = 30.0f;
					enemyPos.y = (rand() % 2 == 0) ? 3.0f : 9.0f;
					enemy->SetWorldPosition(enemyPos);
					
					player_->AddBounce();

					// ★ 削除処理は残すがコメントアウト
					// toRemove.push_back(enemy);
				}
			}

			// ▼削除処理そのものはコメントアウトして保持▼
			/*
			for (Enemy* e : toRemove) {
			    auto it = std::find(enemies_.begin(), enemies_.end(), e);
			    if (it != enemies_.end()) {
			        delete *it;
			        enemies_.erase(it);
			    }
			}
			*/
		}
	}
#pragma endregion
}
