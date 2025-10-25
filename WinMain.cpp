#include "DxPlus/DxPlus.h"
#include"WinMain.h"
#include"Title.h"
#include"Game.h"
#include"Rule.h"
int currentScene = SceneNone;
int nextScene = SceneTitle;

int bgm3;
// フォント
int fontID1 = -1;
int fontID2 = -1;
//------------------------------------------------------
//プロトタイプ宣言
//------------------------------------------------------
static LRESULT CALLBACK CustomWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//------------------------------------------------------
//wWinMain関数
//------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
	//DxPlus(Dxライブラリ)の初期化
	constexpr bool WINDOWED = TRUE;//ウィンドウモード
	if (DxPlus::Initialize(DxPlus::CLIENT_WIDTH, DxPlus::CLIENT_HEIGHT, WINDOWED) == -1)
	{
		return -1; //初期化失敗
	}

	//ウィンドウプロシージャを設定
	DxLib::SetHookWinProc(CustomWinProc);
	bgm3 = LoadSoundMem(L"./Data/Sounds/BGM.wav");
	PlaySoundMem(bgm3, DX_PLAYTYPE_LOOP, TRUE); 
	//Game_Init();
	if (AddFontResourceExW(L"./Data/Fonts/Belleza/Belleza-Regular.ttf", FR_PRIVATE, 0) == 0)
	{
		DxPlus::Utils::FatalError(L"Failed to add font: ./Data/Fonts/Belleza/Belleza - Regular.ttf");
	}
	fontID1 = DxPlus::Text::InitializeFont(L"Yusei Magic", 25, 5);
	if (fontID1 == -1)
	{
		DxPlus::Utils::FatalError(L"Failed to add font: ./Data/Fonts/Belleza/Belleza-Regular.ttf");
	}
	//ゲームループ
	while (DxPlus::GameLoop())
	{
		if (nextScene != SceneNone)
		{
			switch (currentScene)
			{
			case SceneTitle:
				Title_End();
				break;
			case SceneRule:
				Rule_End();
				break;
			case SceneGame:
				Game_End();
				break;

			}
			switch (nextScene)
			{
			case SceneTitle:
				Title_Init();
				break;
			case SceneRule:
				Rule_Init();
				break;
			case SceneGame:
				Game_Init();
				break;
			}

			currentScene = nextScene;
			nextScene = SceneNone;
		}

		//入力の状態を更新する
		DxPlus::Input::Update();

		switch (currentScene)
		{
		case SceneTitle:
			Title_Update();
			Title_Render();
			break;
		case SceneRule:
			Rule_Update();
			Rule_Render();
			break;
		case SceneGame:
			Game_Update();
			Game_Render();
			break;
		}

		//デバック情報を描画する
		DxPlus::Debug::Draw();

		//画面をクリアする
		DxLib::ScreenFlip();
		DxLib::ClearDrawScreen();

	}
	//Game_End();
	switch (currentScene)
	{
	case SceneTitle:
		Title_End();
		break;
	case SceneRule:
		Rule_End();
		break;
	case SceneGame:
		Game_End();
		break;
	}

	//DxPlusの終了処理
	DxPlus::ShutDown();

	//戻り値は0でよい
	return 0;
	while (DxPlus::GameLoop())
	{
		DxPlus::Text::DrawString(L"2D Game Programming I", { DxPlus::CLIENT_WIDTH * 0.5f,DxPlus::CLIENT_HEIGHT * 0.5f },
			DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER, { 2,2 });

		DxLib::ScreenFlip();
		DxLib::ClearDrawScreen();
	}
	DxPlus::ShutDown();
	return 0;
}
LRESULT CALLBACK CustomWinProc(HWND, UINT msg, WPARAM wParam, LPARAM)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		break;
	}
	return 0;
}
