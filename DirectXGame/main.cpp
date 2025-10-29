#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include "SelectionScene.h"
#include "ResultScene.h"
#include <Windows.h>

using namespace KamataEngine; // これ書いておくとkamataEngine::書かなくてよい

// 02_12 24枚目
TitleScene* titleScene = nullptr;
SelectionScene* selectionScene = nullptr;
GameScene* gameScene = nullptr;
ResultScene* resultScene = nullptr;

// 02_12 25枚目(Scene sceneまで)
enum class Scene {
	kUnknown = 0,
	kTitle,
	kSelection,
	kGame,
	kResult,
};
// 現在シーン（型）
Scene scene = Scene::kUnknown;

// 02_12 29枚目
void ChangeScene() {

	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kSelection;
			delete titleScene;
			titleScene = nullptr;
			selectionScene = new SelectionScene;
			selectionScene->Initialize();
		}
		break;
	case Scene::kSelection:
		if (selectionScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGame;
			delete selectionScene;
			selectionScene = nullptr;
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		// 02_12 30枚目
		if (gameScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
	case Scene::kResult:
		if (resultScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			delete resultScene;
			resultScene = nullptr;
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
	}
}

// 02_12 31枚目
void UpdateScene() {

	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kSelection:
		selectionScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	case Scene::kResult:
		resultScene->Update();
		break;
	}
}

// 02_12 32枚目
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kSelection:
		selectionScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	case Scene::kResult:
		resultScene->Draw();
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"2165_ネオンレーン");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// 02_12 21枚目 ↓3行
	//	TitleScene *titleScene = nullptr; // 02_12 24枚目でグローバルに引っ越し
	scene = Scene::kTitle; // 02_12 28枚目
	titleScene = new TitleScene;
	titleScene->Initialize();

	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();

		// 02_12 21枚目で変更
		// titleScene->Update(); //02_12 33枚目で削除

		// シーン切り替え
		ChangeScene(); // 02_12 33枚目で追加
		// シーン更新
		UpdateScene(); // 02_12 33枚目で追加

		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();

		// ゲームシーンの描画
		// titleScene->Draw(); // 02_12 33枚目で削除

		// シーンの描画
		DrawScene(); // 02_12 33枚目で追加

		// 軸表示の描画
		AxisIndicator::GetInstance()->Draw();

		// プリミティブ描画のリセット
		PrimitiveDrawer::GetInstance()->Reset();

		// ImGui描画
		imguiManager->Draw();
		imguiManager->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// 02_12 35枚目 各種解放
	delete titleScene;
	delete selectionScene;
	delete gameScene;
	delete resultScene;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}