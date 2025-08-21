#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Fade {
public:
	//フェードの状態
	enum class Status {
		None, // 何もしない
		FadeIn,   // フェードイン
		FadeOut,  // フェードアウト
	};

	void Initialize();
	void Update();
	void Draw();
	
	//フェード開始
	void Start(Status status, float duration);
	//フェード終了
	void stop();
	//フェード終了判定
	bool IsFinished() const;

private:
	//テクスチャハンドル
	uint32_t textureHandle_ = 0;
	
	// 02_13 10枚目 スプライト
	Sprite* sprite_ = nullptr;

	// 現在のフェードの状態
	Status status_ = Status::None;

	//フェードの持続時間
	float duration_ = 0.0f;
	//時間経過カウンター
	float counter_ = 0.0f;
};