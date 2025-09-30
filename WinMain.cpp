//①　WinMain.hとTitele.hをインクルードする
#include"DxPlus/DxPlus.h"
#include"WinMain.h"
#include"Title.h"
#include"Game.h"
//変数
int currentScene = SceneNone;
int nextScene = SceneTitle;

static int sceneTimer = 0;
//プロトタイプ宣言
static LRESULT CALLBACK CustomWinProc(HWND hWnd, UINT msg, WPARAM wPaream, LPARAM lParam);

float fadeTimer = 0.0f;
float fadeDuration = 2.0f; // 1周期（薄→濃→薄）にかかる秒数
float  deltaTime;
int fadeCount = 0;
const int maxFadeCount = 3; // 繰り返したい回数
bool isFadeFinished = false;
int titleImageID;




//wWinmain関数
int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {


	constexpr bool WINDOWED = TRUE;//ウィンドウモード
	if (DxPlus::Initialize(DxPlus::CLIENT_WIDTH, DxPlus::CLIENT_HEIGHT, WINDOWED) == 1) {

		return -1;
	}

	int CLEAR=-1;
	bool hasPlayedClearBgm = false;
	CLEAR = DxLib::LoadSoundMem(L"./Data/Sounds/nc178239_神が土管からコンティニューする効果音.mp3"); // パスは適宜変更
	if (CLEAR == -1) {
		// 読み込み失敗時の処理
		DxPlus::Utils::FatalError(L"Failed to load:./Data/Images/クリアBGMの読み込みに失敗しました");
	}
	//Game_Init();
	int OVER = -1;
	bool hasPlayedClearBgmOVER = false;
	OVER = DxLib::LoadSoundMem(L"./Data/Sounds/ゲームオーバー.wav"); // パスは適宜変更
	if (OVER== -1) {
		// 読み込み失敗時の処理
		DxPlus::Utils::FatalError(L"Failed to load:./Data/Images/ゲームオーバー.wavの読み込みに失敗しました");
	}
	


	//ゲームループ
	while (DxPlus::GameLoop())
	{
		auto input = DxPlus::Input::GetButton(DxPlus::Input::PLAYER1);
		//シーンが切り替わるなら
		if (nextScene != SceneNone) {

			//今のシーンの終了関数を呼ぶ
			switch (currentScene) {
			case SceneTitle:
				Title_End();
				break;
			case SceneGame:
				Game_End();
				break;
			}

			switch (nextScene) {
			case SceneTitle:
				Title_Init();
				break;
			case SceneGame:

				Game_Init();
				break;
			}
			currentScene = nextScene;
			nextScene = SceneNone;
		}

		DxPlus::Input::Update();

		switch (currentScene) {
		case SceneTitle:
			Title_Update();
			Title_Render();
			break;
		case SceneGame:
			/*DxPlus::Text::DrawString(L"特にないよ(*^^*)", { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.33 },
				DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
				{ 3.0f,3.0f });
			if (DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_TRIGGER1) {
				DxPlus::Text::DrawString(L"エンターを押しましょう", { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.7 },
					DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
					{ 3.0f,3.0f });
			}


			if (DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_UP) {
				nextScene = SceneK9;
			}*/

			Game_Update();
			Game_Render();
			Game_Play();
			sceneTimer++;

			if (sceneTimer >= MAX_WAIT_TIME_FRAMES)
			{
				nextScene = SceneOther;  // または次のシーン番号
				sceneTimer = 0;            // タイマーリセット
			}
			break;
			
		case SceneOther:
		{
			
			
			DxPlus::Text::DrawString(L"特にないよ(*^^*)", { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.33 },
				DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
				{ 3.0f,3.0f });
			if (DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_TRIGGER1) {
				DxPlus::Text::DrawString(L"エンターを押しましょう", { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.7 },
					DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
					{ 3.0f,3.0f });
			}
			
			

			if (input & DxPlus::Input::BUTTON_UP)
			{
				nextScene = SceneK9;
			}
			else if (!(input & DxPlus::Input::BUTTON_UP)&&(input != 0)) // 他の何かのボタンが押されていれば
			{
				nextScene = SceneTitle;
			}
			

		}
		
			break;


		case SceneK9:
			
			

			
			DxPlus::Text::DrawString(L"特にないよ(*^^*)", { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.33 },
				DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
				{ 3.0f,3.0f });
			if (DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_TRIGGER1) {
				DxPlus::Text::DrawString(L"エンターを押しましょう", { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.7 },
					DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
					{ 3.0f,3.0f });
			}
if (input & DxPlus::Input::BUTTON_DOWN)
			{
				nextScene = SceneVS;
			}
			else if (
				!(DxPlus::Input::BUTTON_DOWN) &&
				!(DxPlus::Input::BUTTON_UP) &&
				input != 0) // 他の何かのボタンが押されていれば
			{
				nextScene = SceneTitle;
			}
			break;
		case SceneVS:
			
			
			DxPlus::Text::DrawString(L"特にないよ(*^^*)", { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.33 },
				DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
				{ 3.0f,3.0f });
			if (DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_TRIGGER1) {
				DxPlus::Text::DrawString(L"エンターを押しましょう", { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.7 },
					DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
					{ 3.0f,3.0f });
			}
			if (input & DxPlus::Input::BUTTON_LEFT)
			{
				nextScene = SceneRR;
			}
			// 他のボタンで戻る（ただしLEFT, UP, DOWNは除外）
			else if (
				!(input & DxPlus::Input::BUTTON_LEFT) &&
				!(input & DxPlus::Input::BUTTON_UP) &&
				!(input & DxPlus::Input::BUTTON_DOWN) &&
				input != 0
				)
			{
				nextScene = SceneTitle;
			}

			break;
		


		case SceneRR:
			//RIGHT

			if (input & DxPlus::Input::BUTTON_RIGHT)
			{
				nextScene = SceneSR;
			}
			// 他のボタンで戻る（ただしLEFT, UP, DOWNは除外）
			else if (
				!(input & DxPlus::Input::BUTTON_RIGHT)&&
				!(input & DxPlus::Input::BUTTON_LEFT) &&
				!(input & DxPlus::Input::BUTTON_UP) &&
				!(input & DxPlus::Input::BUTTON_DOWN) &&
				input != 0
				)
			{
				nextScene = SceneTitle;
			}

			DxPlus::Text::DrawString(L"特にないよ(*^^*)", { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.33 },
				DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
				{ 3.0f,3.0f });
			if (DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_TRIGGER1) {
				DxPlus::Text::DrawString(L"エンターを押しましょう", { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.7 },
					DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
					{ 3.0f,3.0f });
			}

			break;
		case SceneCLEAR:

			if (!hasPlayedClearBgm && CLEAR != -1)
			{
				hasPlayedClearBgm = true;
				PlaySoundMem(CLEAR, DX_PLAYTYPE_BACK);
				
			}

			
			DxPlus::Text::DrawString(L"GAME  CLEAR!!", { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.33 },
				DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
				{ 3.0f,3.0f });
			if (input != 0) {
				nextScene = SceneTitle;
			}
			break;
case SceneGAMEOVRE :
			DxPlus::Text::DrawString(L"GAME  OVRE!!", { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.33 },
				DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
				{ 3.0f,3.0f });
			if (!hasPlayedClearBgmOVER && OVER != -1)
			{
				hasPlayedClearBgmOVER = true;
				PlaySoundMem(OVER, DX_PLAYTYPE_BACK);

			}
			if (
				!(input & DxPlus::Input::BUTTON_RIGHT) &&
				!(input & DxPlus::Input::BUTTON_LEFT) &&
				!(input & DxPlus::Input::BUTTON_UP) &&
				!(input & DxPlus::Input::BUTTON_DOWN) &&
				input != 0) {
				nextScene = SceneTitle;
			}
			break;
		}
		
		


		DxPlus::Debug::Draw();




		DxLib::ScreenFlip();
		DxLib::ClearDrawScreen();

		
	}

	Game_End();
	switch (currentScene) {
	case SceneTitle:
		Title_End();
		break;
	case SceneGame:
		Game_End();
		break;

	}









	DxPlus::ShutDown();


	return 0;







}

LRESULT CALLBACK CustomWinProc(HWND, UINT mag, WPARAM wParam, LPARAM) {

	switch (mag) {
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {

			PostQuitMessage(0);

		}
		break;
	}
	return 0;




}
//nc178239_神が土管からコンティニューする効果音













