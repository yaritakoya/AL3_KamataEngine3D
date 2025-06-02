#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;
using namespace MathUtility;

class Player;

//矩形
struct Rect {
	float left = 0.0f;	//左端
	float right = 1.0f;	//右端
	float bottom = 0.0f;//下端
	float top = 1.0f;	//上端
};

class CameraController {

public:
	void Initialize();
	void Update();
	void setTarget(Player* target) { target_ = target; }
	void Reset();
	void SetMovableArea(Rect area) { movableArea_ = area; }

	const Camera& GetViewProjection() const { return camera_; }
	
	//座標補間割合
	static inline const float kInterpolationRate = 0.2f;

	Vector3 targetCoordinate_;

	//速度掛け率
	static inline const float kVelocityBias = 30.0f;
	//追従対象の各方向へのカメラ移動範囲
	static inline const Rect targetMargin = {-9.0f, 9.0f, -5.0f, 5.0f};

private:
	Camera camera_;
	Player* target_ = nullptr;
	CameraController* cameraContoller_ = nullptr;
	// 追従対象とカメラの座標の差(オフセット)
	Vector3 targetOffset_ = {0, 0, -15.0f};
	//カメラ移動範囲
	Rect movableArea_ = {0, 100, 0, 100};
};
