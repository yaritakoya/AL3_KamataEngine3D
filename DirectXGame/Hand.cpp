#include "Hand.h"
// #include "Input.h"
#include <numbers>

void Hand::Initialize(Model* model, Camera* camera, Player* player) {
	model_ = model;
	camera_ = camera;
	player_ = player;

	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.25f, 0.25f, 0.25f};
}

void Hand::Update() {
	if (!player_)
		return;

	const WorldTransform& playerWT = player_->GetWorldTransform();

	// プレイヤー前方ベクトル
	Vector3 forward = {std::sinf(playerWT.rotation_.y), 0.0f, std::cosf(playerWT.rotation_.y)};

	switch (state_) {
	case State::Idle:
		// Space 押したら発射
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			worldTransform_.translation_ = playerWT.translation_ + forward * offset_.z;
			worldTransform_.rotation_ = playerWT.rotation_;
			velocity_ = forward * speed_;
			traveled_ = 0.0f;
			state_ = State::Forward;
		}
		break;

	case State::Forward:
		worldTransform_.translation_ += velocity_;
		traveled_ += speed_;

		if (traveled_ >= maxDistance_) {
			// 最大距離に到達
			if (Input::GetInstance()->PushKey(DIK_SPACE)) {
				state_ = State::Hold; // 押しっぱなし → その場で停止
			} else {
				state_ = State::Return; // 押してなければ戻る
				// forward の各成分を反転して戻る向きに
				velocity_ = {-forward.x * speed_, -forward.y * speed_, -forward.z * speed_};
			}
		}
		break;

	case State::Hold:
		// Space を離したら戻り始める
		if (!Input::GetInstance()->PushKey(DIK_SPACE)) {
			state_ = State::Return;
			velocity_ = {-forward.x * speed_, -forward.y * speed_, -forward.z * speed_};
		}
		break;

	case State::Return: {
		// 毎フレーム Player の位置を目指す
		Vector3 toPlayer = {
		    playerWT.translation_.x - worldTransform_.translation_.x, playerWT.translation_.y - worldTransform_.translation_.y, playerWT.translation_.z - worldTransform_.translation_.z};

		// 距離を計算
		float distance = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y + toPlayer.z * toPlayer.z);

		// 正規化して速度ベクトルに変換
		if (distance > 0.001f) {
			Vector3 dir = {toPlayer.x / distance, toPlayer.y / distance, toPlayer.z / distance};
			velocity_ = {dir.x * speed_, dir.y * speed_, dir.z * speed_};
			worldTransform_.translation_ += velocity_;
		}

		// 一定距離以内ならキャッチ（Idleに戻る）
		if (distance < 1.0f) {
			state_ = State::Idle;
		}
		break;
	}
	}

	// 行列更新
	WorldTransformUpdate(worldTransform_);
}

void Hand::Draw() {
	if (model_ && state_ != State::Idle) {
		model_->Draw(worldTransform_, *camera_);
	}
}
