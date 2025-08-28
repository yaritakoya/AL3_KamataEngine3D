#include "TitleScene.h"
#include "MyMath.h"
#include <numbers>

TitleScene::~TitleScene() {
	delete modelPlayer_;
	delete modelTitle_;
	delete modelTitle1_;
	delete fade_;
}

void TitleScene::Initialize() {
	modelTitle1_ = Model::CreateFromOBJ("title1", true);
	modelTitle_ = Model::CreateFromOBJ("titleFont", true);
	modelPlayer_ = Model::CreateFromOBJ("player");

	// カメラ初期化
	camera_.Initialize();

	const float kPlayerTitle = 2.0f;
	const float kPlayerTitle1 = 2.5f;

	worldTransformTitle_.Initialize();
	worldTransformTitle1_.Initialize();

	worldTransformTitle_.scale_ = {kPlayerTitle, kPlayerTitle, kPlayerTitle};
	worldTransformTitle1_.scale_ = {kPlayerTitle1, kPlayerTitle1, kPlayerTitle1};

	const float kPlayerScale = 10.0f;

	worldTransformPlayer_.Initialize();

	worldTransformPlayer_.scale_ = {kPlayerScale, kPlayerScale, kPlayerScale};

	worldTransformPlayer_.rotation_.y = 0.95f * std::numbers::pi_v<float>;

	worldTransformPlayer_.translation_.x = -2.0f;

	worldTransformPlayer_.translation_.y = -10.0f;

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void TitleScene::Update() {

	switch (phase_) {
	case TitleScene::Phase::kMain:
		// タイトルシーンの終了条件
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			// フェードアウト開始
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;
	case TitleScene::Phase::kFadeIn:
		// フェードイン中
		fade_->Update();
		if (fade_->IsFinished()) {
			// フェードイン終了
			phase_ = Phase::kMain;
		}
		break;
	case TitleScene::Phase::kFadeOut:
		// フェードアウト中
		fade_->Update();
		if (fade_->IsFinished()) {
			// フェードアウト終了
			finished_ = true;
		}
		break;
	}

	counter_ += 1.0f / 60.0f;
	counter_ = std::fmod(counter_, kTimeTitleMove);

	float angle = counter_ / kTimeTitleMove * 2.0f * std::numbers::pi_v<float>;

	worldTransformTitle_.translation_.y = std::sin(angle) + 10.0f;

	camera_.TransferMatrix();

	// アフィン変換～DirectXに転送(タイトル座標)
	WorldTransformUpdate(worldTransformTitle_);
	WorldTransformUpdate(worldTransformTitle1_);

	// アフィン変換～DirectXに転送（プレイヤー座標）
	WorldTransformUpdate(worldTransformPlayer_);
}

void TitleScene::Draw() {

	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	
	Model::PreDraw(commandList);

	modelTitle1_->Draw(worldTransformTitle1_, camera_);
	modelTitle_->Draw(worldTransformTitle_, camera_);
	modelPlayer_->Draw(worldTransformPlayer_, camera_);

	Model::PostDraw();
	
	// フェード
	fade_->Draw();
}