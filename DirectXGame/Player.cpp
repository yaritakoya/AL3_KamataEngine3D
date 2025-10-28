#define NOMINMAX

#include "Player.h"
#include "MapChipField.h"
#include "MyMath.h"

#include <algorithm>
#include <cassert>
#include <numbers>

// EaseOutQuart関数（0〜1 のtを渡す）
float EaseOutQuart(float t) { return 1 - powf(1 - t, 4); }

void Player::Initialize(Model* model, Model* laneModel, Camera* camera, const Vector3& position) {

	assert(model);
	// モデル
	model_ = model;
	laneModel_ = laneModel;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	camera_ = camera;

	// 右側に初期配置（X方向に+5.0f ずらす例）
	worldTransform_.translation_ = {4.0f, 3.0f, 0.0f};
	worldTransform_.scale_ = {1.5f, 1.5f, 1.5f};

	// 行列更新
	WorldTransformUpdate(worldTransform_);
}

void Player::InputMove() {
	Input* input = Input::GetInstance();

	const float kLowY = 3.0f;
	const float kHighY = 9.0f;

	// 上下入力：押された瞬間に移動開始
	if (!isMoving_) {
		if (input->TriggerKey(DIK_UP)) {
			startY_ = worldTransform_.translation_.y;
			targetY_ = kHighY;
			moveTimer_ = 0.0f;
			isMoving_ = true;
		} else if (input->TriggerKey(DIK_DOWN)) {
			startY_ = worldTransform_.translation_.y;
			targetY_ = kLowY;
			moveTimer_ = 0.0f;
			isMoving_ = true;
		}
	}

	// ←→入力による移動は削除（左右移動禁止）
}

// --- 攻撃関連実装 ---

void Player::TryStartAttack() {
	Input* input = Input::GetInstance();

	// Space押下時に攻撃開始（押しっぱなし防止）
	if (attackTimer_ <= 0.0f && input->TriggerKey(DIK_SPACE)) {
		attackTimer_ = kAttackDuration;
		// 攻撃開始時のSEやエフェクトをここで鳴らせます
	}
}

AABB Player::GetAttackAABB() const {
	AABB aabb;
	Vector3 p = worldTransform_.translation_;

	float dir = (lrDirection_ == LRDirection::kRight) ? +1.0f : -1.0f;
	float halfRange = kAttackRange * 0.5f;
	float halfHeight = kAttackHeight * 0.5f;
	float halfDepth = kAttackDepth * 0.5f;
	float offsetX = 1.5f;

	// ★ バウンド中は攻撃判定を固定高さに
	float attackY = isBouncing_ ? attackBaseY_ : p.y;

	Vector3 center = {p.x + dir * (0.5f + halfRange) + offsetX, attackY, p.z};

	aabb.min = {center.x - halfRange, center.y - halfHeight, center.z - halfDepth};
	aabb.max = {center.x + halfRange, center.y + halfHeight, center.z + halfDepth};

	return aabb;
}

AABB Player::GetAttackAABB_Upper() const {
	AABB aabb;
	Vector3 p = worldTransform_.translation_;

	float dir = (lrDirection_ == LRDirection::kRight) ? +1.0f : -1.0f;
	float halfRange = kAttackRange * 0.5f;
	float halfHeight = kAttackHeight * 0.5f;
	float halfDepth = kAttackDepth * 0.5f;
	float offsetX = 1.5f;

	// ★攻撃判定を上の固定位置に配置
	float attackY = 9.0f;

	Vector3 center = {p.x + dir * (0.5f + halfRange) + offsetX, attackY, p.z};

	aabb.min = {center.x - halfRange, center.y - halfHeight, center.z - halfDepth};
	aabb.max = {center.x + halfRange, center.y + halfHeight, center.z + halfDepth};

	return aabb;
}


// 02_07 スライド13枚目
void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

// 02_08スライド14枚目 設置状態の切り替え処理
void Player::UpdateOnGround(const CollisionMapInfo& info) {

	info;

	if (onGround_) {
		// 02_08スライド18枚目 ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			// 落下判定
			// 落下なら空中状態に切り替え

			// 02_08スライド19枚目(このelseブロック全部)
			std::array<Vector3, kNumCorner> positionsNew;

			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}

			bool hit = false;

			MapChipType mapChipType;

			// 左下点の判定
			IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 落下開始
			if (!hit) {
				//				DebugText::GetInstance()->ConsolePrintf("jump");
				onGround_ = false;
			}
		}
	} else {
		// 02_08スライド16枚目 地面に接触している場合の処理
		if (info.landing) {
			// 着地状態に切り替える（落下を止める）
			onGround_ = true;
			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロに
			velocity_.y = 0.0f;
		}
	}
}

// 02_08スライド27枚目 壁接地中の処理
void Player::UpdateOnWall(const CollisionMapInfo& info) {

	if (info.hitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

#pragma region CheckMapCollision

// 02_07 スライド14枚目(上下左右全て)
void Player::CheckMapCollisionUp(CollisionMapInfo& info) {

	// 02_07スライド20枚目 上昇あり?
	if (info.move.y <= 0) {
		return;
	}

	// 02_07 スライド19枚目（下のfor文も）
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	// 02_07 スライド28枚目（下のfor文も）
	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？ 02_07 スライド34枚目
	if (hit) {
		// 現在座標が壁の外か判定
		IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, +kHeight / 2.0f, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
			info.ceiling = true;
		}
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {

	info;

	// 02_08 スライド7枚目 下降あり？
	if (info.move.y >= 0) {
		return;
	}

	// 02_08 スライド7枚目（下のfor文も）
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	// 02_08 スライド8枚目(右下、左下の判定まで)
	MapChipType mapChipType;

	// フラグ初期化
	bool hit = false;

	// 左下の判定
	IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 02_08スライド11枚目 ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, -kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
		// 地面に当たったことを記録する
		info.landing = true;
	}
}

// 中身入れるのは02_08スライド25枚目
void Player::CheckMapCollisionRight(CollisionMapInfo& info) {

	if (info.move.x <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 右側の当たり判定
	bool hit = false;

	// 右上点の判定
	IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// 現在座標が壁の外か判定
		IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
			info.hitWall = true;
		}
	}
}

// 中身入れるのは02_08スライド25枚目
void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {

	if (info.move.x >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 右側の当たり判定
	bool hit = false;

	// 左上点の判定
	IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// 現在座標が壁の外か判定
		IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, 0));

		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = std::max(0.0f, rect.right - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
			info.hitWall = true;
		}
	}
}

#pragma endregion

// 02_07 スライド17枚目
Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offsetTable[] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, //  kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  //  kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Player::Update() {
	// 入力（上下のみ）
	InputMove();

	// イージング移動中の処理
	if (isMoving_) {
		const float duration = 0.2f;           // 0.2秒で完了（調整可）
		moveTimer_ += 1.0f / 60.0f / duration; // 60fps基準
		float t = std::clamp(moveTimer_, 0.0f, 1.0f);
		float eased = EaseOutQuart(t);
		worldTransform_.translation_.y = startY_ + (targetY_ - startY_) * eased;

		if (t >= 1.0f) {
			isMoving_ = false;
			worldTransform_.translation_.y = targetY_; // 最終位置にスナップ
		}
	}

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = velocity_;
	collisionMapInfo.landing = false;
	collisionMapInfo.hitWall = false;

	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	// 移動
	worldTransform_.translation_ += collisionMapInfo.move;

	// 天井に当たったら停止
	if (collisionMapInfo.ceiling) {
		velocity_.y = 0;
	}

	// 壁・床との当たり判定
	UpdateOnWall(collisionMapInfo);
	UpdateOnGround(collisionMapInfo);

	// ワールド行列更新
	WorldTransformUpdate(worldTransform_);

	// 攻撃開始トライ（Space押下）
	TryStartAttack();

	// 攻撃タイマー更新
	if (attackTimer_ > 0.0f) {
		attackTimer_ -= 1.0f / 60.0f; // 60fps前提
		if (attackTimer_ < 0.0f)
			attackTimer_ = 0.0f;
	}

	UpdateBounce();
}

void Player::Draw() {
	// Player 本体
	model_->Draw(worldTransform_, *camera_);

	// 攻撃範囲（OBJで可視化）
	DrawAttackHitboxObj();

	// ★ 上方向の攻撃範囲モデル
	DrawAttackHitboxObj_Upper();

	// 既存のAABB描画（デバッグライン用）
	DrawAttackAABB();
}

void Player::DrawAttackAABB() {
	// 攻撃中かつ上下移動中でないときのみデバッグ表示
	if (!IsAttacking() || isMoving_)
		return;

	AABB atk = GetAttackAABB();

	// ImGui::Begin("Debug");
	// ImGui::GetWindowDrawList()->AddRect(ImVec2(atk.min.x * 50 + 640, -atk.min.y * 50 + 360), ImVec2(atk.max.x * 50 + 640, -atk.max.y * 50 + 360), IM_COL32(255, 0, 0, 255), 0.0f, 0, 2.0f);
	// ImGui::End();
}

// 02_10 10枚目
Vector3 Player::GetWorldPosition() {

	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}
// 02_10 14枚目
AABB Player::GetAABB() {

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

// 02_10 21枚目
void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;
	// ジャンプ初速
	velocity_ += Vector3(0, kJumpAcceleration / 60.0f, 0);

	// 02_12 12枚目 書き換え
	isDead_ = true;
}

void Player::DrawAttackHitboxObj() {
	// 攻撃範囲のAABBを取得
	AABB attackAABB = GetAttackAABB();

	// AABBの中心を計算
	Vector3 center = {
	    (attackAABB.min.x + attackAABB.max.x) * 0.5f,
	    (attackAABB.min.y + attackAABB.max.y) * 0.5f,
	    (attackAABB.min.z + attackAABB.max.z) * 0.5f,
	};

	// ★ 下レーン固定（プレイヤーが動いてもY座標は変わらない）
	center.y = 3.0f;

	attackWorldTransform_.Initialize();
	attackWorldTransform_.scale_ = {1.5f, 1.5f, 1.5f};
	attackWorldTransform_.translation_ = center;
	attackWorldTransform_.rotation_ = worldTransform_.rotation_;

	attackWorldTransform_.matWorld_ = MakeAffineMatrix(attackWorldTransform_.scale_, attackWorldTransform_.rotation_, attackWorldTransform_.translation_);
	attackWorldTransform_.TransferMatrix();

	// ★ laneModel_を使用して描画
	if (laneModel_) {
		laneModel_->Draw(attackWorldTransform_, *camera_);
	} else {
		model_->Draw(attackWorldTransform_, *camera_);
	}

	OutputDebugStringA("Drawing LANE model (fixed lower lane)\n");
}


// 上方向の攻撃判定をモデルで描画
void Player::DrawAttackHitboxObj_Upper() {

	// ★ 上方向の攻撃AABBを取得
	AABB upperAABB = GetAttackAABB_Upper();

	// AABBの中心を計算
	Vector3 upperCenter = {
	    (upperAABB.min.x + upperAABB.max.x) * 0.5f,
	    (upperAABB.min.y + upperAABB.max.y) * 0.5f,
	    (upperAABB.min.z + upperAABB.max.z) * 0.5f,
	};

	// ワールド変換を設定
	attackWorldTransform_Upper_.Initialize();
	attackWorldTransform_Upper_.scale_ = {1.5f, 1.5f, 1.5f};
	attackWorldTransform_Upper_.translation_ = upperCenter;
	attackWorldTransform_Upper_.rotation_ = worldTransform_.rotation_;

	attackWorldTransform_Upper_.matWorld_ = MakeAffineMatrix(attackWorldTransform_Upper_.scale_, attackWorldTransform_Upper_.rotation_, attackWorldTransform_Upper_.translation_);

	attackWorldTransform_Upper_.TransferMatrix();

	// ★ laneModel_を使用して描画（既存と同じ）
	if (laneModel_) {
		laneModel_->Draw(attackWorldTransform_Upper_, *camera_);
	} else {
		model_->Draw(attackWorldTransform_Upper_, *camera_);
	}

	// デバッグ出力
	OutputDebugStringA("Drawing UPPER LANE model\n");
}

void Player::AddBounce() {
	// X,Zはその場のまま
	bounceStartY_ = worldTransform_.translation_.y;

	// バウンド前の高さを固定（攻撃用）
	attackBaseY_ = bounceStartY_;

	// バウンド中でも再バウンド可能（位置を元に戻して再開始）
	worldTransform_.translation_.y = bounceStartY_;

	isBouncing_ = true;
	bounceTimer_ = 0.0f;
}

void Player::UpdateBounce() {
	if (!isBouncing_)
		return;

	bounceTimer_ += 1.0f / 60.0f; // 60fps前提
	float t = bounceTimer_ / bounceDuration_;

	if (t >= 1.0f) {
		// 完了：元の位置に戻す
		worldTransform_.translation_.y = bounceStartY_;
		isBouncing_ = false;
		return;
	}

	// 上昇→下降のイージング（sin波形）
	float offset = std::sin(t * std::numbers::pi_v<float>) * bounceHeight_;
	worldTransform_.translation_.y = bounceStartY_ + offset;
}

void Player::CancelAttack() {
	// 攻撃タイマーを強制終了
	attackTimer_ = 0.0f;
}
