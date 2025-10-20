#include "Title.h"
#include "DxPlus/DxPlus.h"
#include "WinMain.h"

//----------------------------------------------------------------------
// 定数
//----------------------------------------------------------------------
int bgm3;
//----------------------------------------------------------------------
// 変数
//----------------------------------------------------------------------
// フレームカウント用変数
int frameCount;

// nextSceneのextern宣言
extern int nextScene;

//① タイトル中の状態
int titleState;
//② フェードイン / アウト用変数
float titleFadeTimer;

//----------------------------------------------------------------------
// 初期設定
//----------------------------------------------------------------------
void Title_Init()
{
    DxLib::SetBackgroundColor(255, 128, 0);
    bgm3 = LoadSoundMem(L"./Data/Sounds/不安（ピアノ演奏）.mp3");
   

    Title_Reset();
}

//----------------------------------------------------------------------
// リセット
//----------------------------------------------------------------------
void Title_Reset()
{
    frameCount = 0;

    //③ 変数の初期設定
    titleState = 0;
    titleFadeTimer = 1.0f;
}

//----------------------------------------------------------------------
// 更新処理
//----------------------------------------------------------------------
void Title_Update()
{
    //④ titleStateによる分岐
    switch (titleState)
    {
    case 0: // フェードイン中
    {
        titleFadeTimer -= 1 / 60.0f;
        if (titleFadeTimer < 0.0f)
        {
           
            titleFadeTimer = 0.0f;
            titleState++;
            PlaySoundMem(bgm3, DX_PLAYTYPE_LOOP, TRUE);
            ChangeVolumeSoundMem(200, bgm3);
        }
        break;
    }

    case 1: // 通常時
    {
        // EnterキーでGameシーンへ
        int input = DxPlus::Input::GetButtonDown(DxPlus::Input::PLAYER1);
        if (input & DxPlus::Input::BUTTON_START)
        {
            titleState++;

        }

        break;
    }

    case 2: // フェードアウト中
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
// 描画処理
//----------------------------------------------------------------------
void Title_Render()
{
    // タイトルの描画
    DxPlus::Text::DrawString(L"2D GameProgramming I",
        { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.33f },
        DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
        { 3.0f, 3.0f });

    // Push Enterの点滅
    if (frameCount & 0x20)
    {
        DxPlus::Text::DrawString(L"Push Enter",
            { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.75f },
            DxLib::GetColor(255, 255, 0), DxPlus::Text::TextAlign::MIDDLE_CENTER,
            { 2.0f, 2.0f });
    }

    //⑤ フェードイン / フェードアウト用
    if (titleFadeTimer > 0.0f)
    {
        DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * titleFadeTimer));
        DxPlus::Primitive2D::DrawRect({ 0,0 },
            { DxPlus::CLIENT_WIDTH, DxPlus::CLIENT_HEIGHT }, DxLib::GetColor(0, 0, 0));
        DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
    }
}

//----------------------------------------------------------------------
// 終了処理
//----------------------------------------------------------------------
void Title_End()
{
    if (CheckSoundMem(bgm3) == TRUE)
    {
        StopSoundMem(bgm3);
    }
}

