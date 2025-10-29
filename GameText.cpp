#include "GameText.h"
#include "WinMain.h"
#include "DxPlus/DxPlus.h"
extern int nextScene;
int back_ENDID;
int knojoID;
int messageIndex = 0;
DxPlus::TextAnimator textAnimator1;
int prevTime;
extern int fontID1;
float hue1 = 0.0f;
float hue2 = 0.0f;
constexpr float HUE_ADD = 0.005f;
unsigned int fillColor;
unsigned int outlineColor;
DxPlus::TextAnimator textAnimator2;
constexpr size_t MESSAGE_MAX = 5;
const wchar_t messageLines[MESSAGE_MAX][256] = {
    L"今日はありがとう\n"
    "\n\n-Push Enter Key-",
    L"いきなり君に声をかけられたときは驚いたけど、意外に良い人なんだね\n"
    "\n\n-Push Enter Key-",
    L"また君が良ければ、一緒に遊んでくれないかな？"
    
};

const wchar_t messageText[] =
L"やっと2人になれた…嬉しいよね？そうだよね？君、私のことずっと見てくれてたもんね！\n"
"君から好きって言ってくれたんだよ？それなのにどうして逃げるの？！責任取ってよ？！\n"
"私君のためにすっごく頑張ったんだよ？朝も昼も夜もずーっと君のこと考えてた！君の姿形、\n"
"歩き方、喋り方、ご飯の食べ方、仕草、何もかもが全部ぜーんぶ愛しくて仕方がないの！\n"
"でも君に私の愛は大きすぎるんだね…じゃあもう仕方ないよね！大好きだよ…私もすぐ一緒に行くからね\n"
"…愛してる……好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き\n"
"好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き好き";

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

    textAnimator2.SetFont(fontID1); // 例
    textAnimator2.SetPosition(100, 400);
    textAnimator2.SetText(messageLines[0]);
    textAnimator2.SetAreaSize(1100, 200);
    textAnimator2.SetSpeed(70);
    textAnimator2.SetColor(GetColor(255, 255, 255));
    textAnimator2.SetLineSpacing(2);
    textAnimator2.SetAlignment(DxPlus::TextAnimatorAlign::Center);
    textAnimator2.SetShowFrame(true);

    back_ENDID = DxPlus::Sprite::Load(L"./Data/Images/End_back.png");
    knojoID = DxPlus::Sprite::Load(L"./Data/Images/Character_Jiraichan1Normal.png");
}

void GameText_Reset()
{
    messageIndex = 0;
    textAnimator1.Draw();

    textAnimator2.Draw();

}

void GameText_Update()
{
}

void GameText_END_Render()
{
    int now = GetNowCount();
    int delta = now - prevTime;
    prevTime = now;

    textAnimator1.Update(delta);
    textAnimator1.Draw();
    if (textAnimator1.IsFinished())
    {
        DxLib_End();
        exit(0);
    }
}

void GameText_CLIER_Render()
{
    DxPlus::Sprite::Draw(back_ENDID);
    DxPlus::Sprite::Draw(knojoID, { 300,100 }, { 0.5,0.5 });
    DxPlus::Primitive2D::DrawRect({ 100,400 }, { 1100,200 }, GetColor(0, 0, 0));
    int now = GetNowCount();
    int delta = now - prevTime;
    prevTime = now;
    textAnimator2.Update(delta);
    textAnimator2.Draw();


}

void GameText_End()
{
}
