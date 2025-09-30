#include "Title.h"
#include "DxPlus/DxPlus.h"
#include "WinMain.h"

//----------------------------------------------------------------------
// �萔
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// �ϐ�
//----------------------------------------------------------------------
// �t���[���J�E���g�p�ϐ�
int frameCount;

// nextScene��extern�錾
extern int nextScene;

//�@ �^�C�g�����̏��
int titleState;
//�A �t�F�[�h�C�� / �A�E�g�p�ϐ�
float titleFadeTimer;

//----------------------------------------------------------------------
// �����ݒ�
//----------------------------------------------------------------------
void Title_Init()
{
    DxLib::SetBackgroundColor(255, 128, 0);

    Title_Reset();
}

//----------------------------------------------------------------------
// ���Z�b�g
//----------------------------------------------------------------------
void Title_Reset()
{
    frameCount = 0;

    //�B �ϐ��̏����ݒ�
    titleState = 0;
    titleFadeTimer = 1.0f;
}

//----------------------------------------------------------------------
// �X�V����
//----------------------------------------------------------------------
void Title_Update()
{
    //�C titleState�ɂ�镪��
    switch (titleState)
    {
    case 0: // �t�F�[�h�C����
    {
        titleFadeTimer -= 1 / 60.0f;
        if (titleFadeTimer < 0.0f)
        {
            titleFadeTimer = 0.0f;
            titleState++;
        }
        break;
    }

    case 1: // �ʏ펞
    {
        // Enter�L�[��Game�V�[����
        int input = DxPlus::Input::GetButtonDown(DxPlus::Input::PLAYER1);
        if (input & DxPlus::Input::BUTTON_START)
        {
            titleState++;
        }

        break;
    }

    case 2: // �t�F�[�h�A�E�g��
    {
        titleFadeTimer += 1 / 60.0f;
        if (titleFadeTimer > 1.0f)
        {
            titleFadeTimer = 1.0f;
            nextScene = SceneGame;
        }
        break;
    }
    }


    frameCount++;
}

//----------------------------------------------------------------------
// �`�揈��
//----------------------------------------------------------------------
void Title_Render()
{
    // �^�C�g���̕`��
    DxPlus::Text::DrawString(L"2D GameProgramming I",
        { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.33f },
        DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
        { 3.0f, 3.0f });

    // Push Enter�̓_��
    if (frameCount & 0x20)
    {
        DxPlus::Text::DrawString(L"Push Enter",
            { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.75f },
            DxLib::GetColor(255, 255, 0), DxPlus::Text::TextAlign::MIDDLE_CENTER,
            { 2.0f, 2.0f });
    }

    //�D �t�F�[�h�C�� / �t�F�[�h�A�E�g�p
    if (titleFadeTimer > 0.0f)
    {
        DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * titleFadeTimer));
        DxPlus::Primitive2D::DrawRect({ 0,0 },
            { DxPlus::CLIENT_WIDTH, DxPlus::CLIENT_HEIGHT }, DxLib::GetColor(0, 0, 0));
        DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
    }
}

//----------------------------------------------------------------------
// �I������
//----------------------------------------------------------------------
void Title_End()
{
}
