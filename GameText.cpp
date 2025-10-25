#include "GameText.h"
#include "WinMain.h"
#include "DxPlus/DxPlus.h"
int messageIndex = 0;
DxPlus::TextAnimator textAnimator1;
int prevTime;
extern int fontID1;
float hue1 = 0.0f;
float hue2 = 0.0f;
constexpr float HUE_ADD = 0.005f;
unsigned int fillColor;
unsigned int outlineColor;

const wchar_t messageText[] =
L"すぐに消せ。a消せ。aすぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。\n"
"すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。すぐに消せ。";

void GameText_Init()
{
    textAnimator1.SetFont(fontID1); // 例
    textAnimator1.SetPosition(0, 0);
    textAnimator1.SetText(messageText);
    textAnimator1.SetAreaSize(1280, 720);
    textAnimator1.SetColor(GetColor(255, 0, 0));
    textAnimator1.SetSpeed(3);
    textAnimator1.SetLineSpacing(5);
    textAnimator1.SetAlignment(DxPlus::TextAnimatorAlign::Left);
    textAnimator1.SetShowFrame(true);
    prevTime = GetNowCount();
}

void GameText_Reset()
{
    messageIndex = 0;
    textAnimator1.Draw();
    textAnimator1.Reset();
}

void GameText_Update()
{
}

void GameText_Render()
{
    // 現在の時刻（ミリ秒）を取得
    int now = GetNowCount();
    // 前回の時刻との差を計算（＝経過時間）
    int delta = now - prevTime;
    // 今の時刻を次回用に保存
    prevTime = now;
    // アニメーションを進める
    textAnimator1.Update(delta);
    textAnimator1.Draw();
    if (textAnimator1.IsFinished())
    {
        DxLib_End();
        exit(0);
    }
}

void GameText_End()
{
}
