#pragma once
#include "KamataEngine.h"
#include <algorithm>
#include <numbers>
#include <vector>

using namespace KamataEngine;

// 自キャラ
class Player {
public:
	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();

	// 左右
	enum class LRDrection {
		kRight,
		kLeft,
	};

	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	const Vector3& GetVelocity() const { return velocity_; }

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	uint32_t textureHandle_ = 0u;

	std::vector<std::vector<WorldTransform*>> worldTransformPlayer_;

	Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.01f;
	static inline const float kAttenuation = 1.0f;
	static inline const float kLimitRunSpeed = 1.0f;
	//旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	LRDrection lrDirection_ = LRDrection::kRight;
	//旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float turnTimer_ = 0.0f;
	//設置状態フラグ
	bool onGround_ = true;
	//重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.1f;
	static inline const float kLimitFallSpeed = 0.5f;
	static inline const float kJumpAcceleration = 1.0f;

};