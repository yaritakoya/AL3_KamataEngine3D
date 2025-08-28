#pragma once
#include "Fade.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class Gameover {
public:
	// シーンのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

	~Gameover();

	void Initialize();

	void Update();

	void Draw();

	// 02_12 26枚目
	bool IsFinished() const { return finished_; }

private:
	static inline const float kTimeTitleMove = 2.0f;

	// ビュープロジェクション
	Camera camera_;
	WorldTransform worldTransformTitle_;
	WorldTransform worldTransformTitle1_;
	WorldTransform worldTransformPlayer_;

	Model* modelPlayer_ = nullptr;
	Model* modelTitle_ = nullptr;
	Model* modelTitle1_ = nullptr;

	float counter_ = 0.0f;
	// 02_12 26枚目
	bool finished_ = false;

	// フェード
	Fade* fade_ = nullptr;

	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;
};
