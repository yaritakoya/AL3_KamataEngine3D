#include "Player.h"
#include "MyMath.h"
#include "cassert"

void Player::Initialize(Model* model, Camera* camera) {
	// nullポインタチェック
	assert(model);
	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	camera_ = camera;
	// ワールド変換の初期化
	worldTransform_.Initialize();
}

void Player::Update() {
	for (std::vector<WorldTransform*>& worldTransformPlayerLine : worldTransformPlayer_) {
		for (WorldTransform* worldTransformPlayer : worldTransformPlayerLine) {
			if (!worldTransformPlayer) {
				continue;
			}
			worldTransformPlayer->matWorld_ = MakeAffineMatrix(worldTransformPlayer->scale_, worldTransformPlayer->rotation_, worldTransformPlayer->translation_);
			worldTransformPlayer->TransferMatrix();
		}
	}
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }