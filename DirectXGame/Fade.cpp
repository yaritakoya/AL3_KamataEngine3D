#include "Fade.h"
#include <algorithm>

using namespace KamataEngine;

void Fade::Initialize() {
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("white1x1.png");
	// 0213 10枚目 スプライトの生成
	sprite_ = Sprite::Create(textureHandle_, Vector2{});
	sprite_->SetSize(Vector2(WinApp::kWindowWidth, WinApp::kWindowHeight)); // 画面サイズ
	sprite_->SetColor(Vector4(0, 0, 0, 1));
}

void Fade::Update() {
	// フェード状態による分岐
	switch (status_) {
	case Status::None:
		// 何もしない
		break;
	case Status::FadeIn:
		// フェードイン処理
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェードインの持続時間を超えたら終了
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(1.0f - counter_ / duration_, 0.0f, 1.0f))); // スプライトの色を更新
		break;
	case Status::FadeOut:
		// フェードアウト処理
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェードインの持続時間を超えたら終了
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を小さくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f))); // スプライトの色を更新
		break;
	default:
		break;
	}
}

void Fade::Draw() {
	if (status_ == Status::None) {
		return; // フェードが行われていない場合は描画しない
	}
	// 0213 11枚目 スプライト描画
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	sprite_->Draw();
	Sprite::PostDraw();
}

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::stop() {
	status_ = Status::None;                 // フェードを停止
}

bool Fade::IsFinished() const { 
	//フェード状態による分岐
	switch (status_) {
	case Fade::Status::FadeIn:
	case Fade::Status::FadeOut:
		if (counter_ >= duration_) {
			return true; // フェードが終了している
		} else {
			return false; // フェードがまだ続いている
		}
	}
	return true;
}
