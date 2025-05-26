#pragma once
#include "KamataEngine.h"
#include<cmath>
#include<numbers>

using namespace KamataEngine;

Matrix4x4 MakeAffineMatrix(Vector3& scale, Vector3& rotation, Vector3& translation);