#include "CameraController.h"
#include "Player.h"

void CameraController::Initialize() {
	// カメラの初期化
	camera_.Initialize();
	// カメラコントローラの初期化
	// cameraContoller_->setTarget(target_);
	// cameraContoller_->Reset();
}

void CameraController::Update() {
	const Vector3& targetVelocity = target_->GetValocity();
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットと追従対象の速度からカメラの目標座標を計算
	targetCoordinate_ = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;
	// 座標補間によりゆったり追従
	camera_.translation_.x = Lerp(camera_.translation_.x, targetCoordinate_.x, kInterpolationRate);
	//追従対象が画面外に出ないように補正
	camera_.translation_.x = max(camera_.translation_.x, targetWorldTransform.translation_.x + targetMargin.left);
	camera_.translation_.x = min(camera_.translation_.x, targetWorldTransform.translation_.x + targetMargin.right);
	camera_.translation_.y = max(camera_.translation_.y, targetWorldTransform.translation_.y + targetMargin.bottom);
	camera_.translation_.y = min(camera_.translation_.y, targetWorldTransform.translation_.y + targetMargin.top);
	// 移動範囲制限
	camera_.translation_.x = max(camera_.translation_.x, movableArea_.left);
	camera_.translation_.x = min(camera_.translation_.x, movableArea_.right);
	camera_.translation_.y = max(camera_.translation_.y, movableArea_.bottom);
	camera_.translation_.y = min(camera_.translation_.y, movableArea_.top);
	// 行列を更新する
	camera_.UpdateMatrix();
}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
	// 行列を更新する
	camera_.UpdateMatrix();
}
