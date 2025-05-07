#pragma once
#include "KamataEngine.h"

KamataEngine::Matrix4x4 MakeAffineMattrix(KamataEngine::Vector3& scale, 
	KamataEngine::Vector3& rotation, KamataEngine::Vector3& translation);