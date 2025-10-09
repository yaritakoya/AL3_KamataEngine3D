#pragma once

#include "KamataEngine.h"
#include "MyMath.h"

using namespace KamataEngine;

// 02_10 20枚目
class Player;

// 02_09 スライド4枚目
class Enemy {

public:
	// 02_09 スライド5枚目
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	// 02_09 スライド5枚目
	void Update();
	// 02_09 スライド5枚目
	void Draw();
	// 02_10 スライド14枚目
	AABB GetAABB();
	// 02_10 スライド14枚目 ワールド座標を取得
	Vector3 GetWorldPosition();
	// 02_10 スライド20枚目 衝突応答
	void OnCollision(const Player* player);

	void SetMoveSpeed(float speed) { moveSpeed_ = speed; }
	float GetMoveSpeed() const { return moveSpeed_; }
	void ResetPosition(); // 既存
	void SetActive(bool active) { isActive_ = active; }
	bool IsActive() const { return isActive_; }

private:
	// 02_09 6枚目 ザ・ワールド
	WorldTransform worldTransform_;
	// 02_09 6枚目 モデル
	Model* model_ = nullptr;
	// 02_09 6枚目 カメラ
	Camera* camera_ = nullptr;

	// 02_09 15枚目
	static inline const float kWalkSpeed = 0.1f;
	// 02_09 15枚目
	Vector3 velocity_ = {};

	// 02_09 19枚目
	static inline const float kWalkMotionAngleStart = 0.0f;
	// 02_09 19枚目
	static inline const float kWalkMotionAngleEnd = 30.0f;
	// 02_09 19枚目
	static inline const float kWalkMotionTime = 1.0f;
	// 02_09 20枚目
	float walkTimer = 0.0f;

	// 02_10 14枚目 当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 敵の移動速度（フレーム単位の移動量など）
	float moveSpeed_ = 2.0f; // 初期値はお好みで（例: 0.1f）
	bool isActive_ = true; // 表示・当たり判定ON
	Vector3 basePosition_;
};