#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include "Player.h"

using namespace KamataEngine;

class Hand {
public:
	/// 初期化
	void Initialize(Model* model, Camera* camera, Player* player);

	/// 更新
	void Update();

	/// 描画
	void Draw();

private:
	enum class State { Idle, Forward, Hold, Return };

	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;
	// 参照するプレイヤー
	Player* player_ = nullptr;

	// オフセット位置（プレイヤー前方に生成）
	Vector3 offset_ = {0.0f, 0.0f, 1.0f};

	// 発射関連
	State state_ = State::Idle;
	Vector3 velocity_ = {};
	float speed_ = 0.5f;        // 移動速度
	float maxDistance_ = 10.0f; // 最大到達距離
	float traveled_ = 0.0f;     // 移動した距離
};
