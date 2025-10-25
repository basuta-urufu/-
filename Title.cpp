#include "Title.h"
#include "DxPlus/DxPlus.h"
#include "WinMain.h"

//----------------------------------------------------------------------
// 定数
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// 変数
//----------------------------------------------------------------------
// フレームカウント用変数
int frameCount;
//BGM
extern int bgm3;
// nextSceneのextern宣言
extern int nextScene;

// タイトル中の状態
int titleState;
// セレクト
int selectedIndex = 0;
// フェードイン / アウト用変数
float titleFadeTimer;

//----------------------------------------------------------------------
// 初期設定
//----------------------------------------------------------------------
void Title_Init()
{
    DxLib::SetBackgroundColor(255, 128, 0);
    Title_Reset();
}

//----------------------------------------------------------------------
// リセット
//----------------------------------------------------------------------
void Title_Reset()
{
    frameCount = 0;

    // 変数の初期設定
    titleState = 0;
    titleFadeTimer = 1.0f;
}

//----------------------------------------------------------------------
// 更新処理
//----------------------------------------------------------------------
void Title_Update()
{
    // titleStateによる分岐
    switch (titleState)
    {
        if (CheckSoundMem(bgm3) == FALSE) 
        { 
            StopSoundMem(bgm3);
        }
    case 0: // フェードイン中
    {
        titleFadeTimer -= 1 / 60.0f;
        if (titleFadeTimer < 0.0f)
        {
            titleFadeTimer = 0.0f;
            titleState++;
            

        }
        break;
    }

    case 1: // 通常時
    {
        
        int input = DxPlus::Input::GetButtonDown(DxPlus::Input::PLAYER1);
        if (input & DxPlus::Input::BUTTON_RIGHT)
        {
            selectedIndex++;
            if (selectedIndex > 1)
            {
                selectedIndex = 1;
            }
        }
        if (input & DxPlus::Input::BUTTON_LEFT)
        {
            selectedIndex--;
            if (selectedIndex <-2)
            {
                selectedIndex = -2;
            }

        }
        if (input & DxPlus::Input::BUTTON_START && selectedIndex == -2)
        {
            exit(0);
        }
        if (input & DxPlus::Input::BUTTON_START && selectedIndex == -1)
        {
            nextScene = SceneRule;
        }
        if (input & DxPlus::Input::BUTTON_START&&selectedIndex==0)
        {
            StopSoundMem(bgm3);
            nextScene = SceneGame;
        }
        if (input & DxPlus::Input::BUTTON_START && selectedIndex == 1)
        {
            nextScene= SceneGallery;
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
    DxPlus::Text::DrawString(L"落とせ地雷ちゃん",
        { DxPlus::CLIENT_WIDTH * 0.5f, DxPlus::CLIENT_HEIGHT * 0.33f },
        DxLib::GetColor(255, 255, 255), DxPlus::Text::TextAlign::MIDDLE_CENTER,
        { 3.0f, 3.0f });
    if (selectedIndex == -2) {
        if (frameCount & 0x20)
        {
            DxPlus::Text::DrawString(L"EXIT",
                { DxPlus::CLIENT_WIDTH * 0.15f, DxPlus::CLIENT_HEIGHT * 0.75f },
                DxLib::GetColor(0, 0, 0), DxPlus::Text::TextAlign::MIDDLE_CENTER,
                { 2.0f, 2.0f });
        }
    }
    else
        DxPlus::Text::DrawString(L"EXIT",
            { DxPlus::CLIENT_WIDTH * 0.15f, DxPlus::CLIENT_HEIGHT * 0.75f },
            DxLib::GetColor(0, 0, 0), DxPlus::Text::TextAlign::MIDDLE_CENTER,
            { 2.0f, 2.0f });
    if (selectedIndex == -1) {
        if (frameCount & 0x20)
        {
            DxPlus::Text::DrawString(L"ルール",
                { DxPlus::CLIENT_WIDTH * 0.35f, DxPlus::CLIENT_HEIGHT * 0.75f },
                DxLib::GetColor(0, 0, 0), DxPlus::Text::TextAlign::MIDDLE_CENTER,
                { 2.0f, 2.0f });
        }
    }
    else
        DxPlus::Text::DrawString(L"ルール",
            { DxPlus::CLIENT_WIDTH * 0.35f, DxPlus::CLIENT_HEIGHT * 0.75f },
            DxLib::GetColor(0, 0, 0), DxPlus::Text::TextAlign::MIDDLE_CENTER,
            { 2.0f, 2.0f });
    if (selectedIndex == 0) {
        if (frameCount & 0x20)
        {
            DxPlus::Text::DrawString(L"START",
                { DxPlus::CLIENT_WIDTH * 0.55f, DxPlus::CLIENT_HEIGHT * 0.75f },
                DxLib::GetColor(0, 0, 0), DxPlus::Text::TextAlign::MIDDLE_CENTER,
                { 2.0f, 2.0f });
        }
    }
    else
        DxPlus::Text::DrawString(L"START",
            { DxPlus::CLIENT_WIDTH * 0.55f, DxPlus::CLIENT_HEIGHT * 0.75f },
            DxLib::GetColor(0, 0, 0), DxPlus::Text::TextAlign::MIDDLE_CENTER,
            { 2.0f, 2.0f });
    if (selectedIndex == 1) {
        if (frameCount & 0x20)
        {
            DxPlus::Text::DrawString(L"ギャラリー",
                { DxPlus::CLIENT_WIDTH * 0.80f, DxPlus::CLIENT_HEIGHT * 0.75f },
                DxLib::GetColor(0, 0, 0), DxPlus::Text::TextAlign::MIDDLE_CENTER,
                { 2.0f, 2.0f });
        }
    }
    else
        DxPlus::Text::DrawString(L"ギャラリー",
            { DxPlus::CLIENT_WIDTH * 0.80f, DxPlus::CLIENT_HEIGHT * 0.75f },
            DxLib::GetColor(0, 0, 0), DxPlus::Text::TextAlign::MIDDLE_CENTER,
            { 2.0f, 2.0f });
    
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
    
}
