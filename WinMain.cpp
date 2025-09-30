#include "DxPlus/DxPlus.h"
#include"WinMain.h"
#include"Title.h"
#include"Game.h"

int currentScene = SceneNone;
int nextScene = SceneTitle;

//------------------------------------------------------
//�v���g�^�C�v�錾
//------------------------------------------------------
static LRESULT CALLBACK CustomWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//------------------------------------------------------
//wWinMain�֐�
//------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
	//DxPlus(Dx���C�u����)�̏�����
	constexpr bool WINDOWED = TRUE;//�E�B���h�E���[�h
	if (DxPlus::Initialize(DxPlus::CLIENT_WIDTH, DxPlus::CLIENT_HEIGHT, WINDOWED) == -1)
	{
		return -1; //���������s
	}

	//�E�B���h�E�v���V�[�W����ݒ�
	DxLib::SetHookWinProc(CustomWinProc);

	//Game_Init();

	//�Q�[�����[�v
	while (DxPlus::GameLoop())
	{
		if (nextScene != SceneNone)
		{
			switch (currentScene)
			{
			case SceneTitle:
				Title_End();
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

			case SceneGame:
				Game_Init();
				break;
			}

			currentScene = nextScene;
			nextScene = SceneNone;
		}

		//���͂̏�Ԃ��X�V����
		DxPlus::Input::Update();

		switch (currentScene)
		{
		case SceneTitle:
			Title_Update();
			Title_Render();
			break;

		case SceneGame:
			Game_Update();
			Game_Render();
			break;
		}

		//�f�o�b�N����`�悷��
		DxPlus::Debug::Draw();

		//��ʂ��N���A����
		DxLib::ScreenFlip();
		DxLib::ClearDrawScreen();

	}
	//Game_End();
	switch (currentScene)
	{
	case SceneTitle:
		Title_End();
		break;

	case SceneGame:
		Game_End();
		break;
	}

	//DxPlus�̏I������
	DxPlus::ShutDown();

	//�߂�l��0�ł悢
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
