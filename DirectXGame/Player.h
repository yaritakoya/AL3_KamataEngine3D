#include "KamataEngine.h"
#include <vector>

using namespace KamataEngine;

class MapChipField;
// 自キャラ
class Player {

	enum LRDirection {
		kRiget,
		kLeft,
	};

	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};

private:
	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		Vector3 move = {};
	};
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	uint32_t textureHandle_ = 0u;

	std::vector<std::vector<WorldTransform*>> worldTransformPlayer_;

	Vector3 velocity_ = {};

	// 移動
	static inline const float kAcceleration = 0.2f;

	static inline const float kAttenuationLanding = 0.1f;
	static inline const float kLimitRunSpeed = 0.1f;

	// 自機の回転
	LRDirection lrDirection_ = LRDirection::kRiget;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;

	static inline const float kTimeTurn = 0.3f;

	// 着地状態フラグ
	bool onGround_ = true;

	// 重力加速度
	static inline const float kGravityAcceleration = 0.1f;
	// 最大落下速度
	static inline const float kLimitFallSpeed = 0.5f;
	// ジャンプ初速
	static inline const float kJumpAcceleration = 1.0f;
	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 0.1f;
	//微小な数値
	static inline const float kGroundSearchHeight = 0.06f;
	//着地時の速度減速率
	static inline const float kAttenuationWall = 0.1f;

public:
	// 初期化
	void Initialize(Model* model, Camera* camera, Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();

	void InputMove();

	void AnimateTurn();

	void CheckMapCollision(CollisionMapInfo& info);

	void CheckMapCollisionUp(CollisionMapInfo& info);

	void CheckMapCollisionDown(CollisionMapInfo& info);

	void CheckMapCollisionRight(CollisionMapInfo& info);

	void CheckMapCollisionLeft(CollisionMapInfo& info);

	void CheckMapLanding(const CollisionMapInfo& info);

	void Contact(const CollisionMapInfo& info);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const Vector3& GetValocity() const { return velocity_; }
	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	void MapHitMove(const CollisionMapInfo& info);
	void CheckMapCeiling(const CollisionMapInfo& info);
};