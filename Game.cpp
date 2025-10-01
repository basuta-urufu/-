#include "Game.h"
#include "DxPlus/DxPlus.h"
#include "WinMain.h"

//----------------------------------------------------------------------
// 定数
//----------------------------------------------------------------------
// 円と回転矩形の当たり判定
bool CircleOBBCollision(DxPlus::Vec2 circle, float radius,
    DxPlus::Vec2 rectCenter, float rectW, float rectH, float angle,
    DxPlus::Vec2& normal)
{
    // 回転矩形の軸を計算
    DxPlus::Vec2 axisX = { cosf(angle), sinf(angle) };
DxPlus::Vec2 axisY = { -sinf(angle), cosf(angle) };

    // ローカル座標に変換
    DxPlus::Vec2 rel = circle - rectCenter;
    float localX = rel.x * axisX.x + rel.y * axisX.y;
    float localY = rel.x * axisY.x + rel.y * axisY.y;

    // 矩形の半サイズ
    float halfW = rectW * 0.5f;
    float halfH = rectH * 0.5f;

    // 矩形内でクランプ（最近傍点）
    float closestX = std::max(-halfW, std::min(localX, halfW));
    float closestY = std::max(-halfH, std::min(localY, halfH));

    // 最近傍点との距離
    float dx = localX - closestX;
    float dy = localY - closestY;
    float dist2 = dx * dx + dy * dy;

    if (dist2 <= radius * radius)
    {
        float dist = sqrtf(dist2);
        if (dist > 0.0f)
            normal = (axisX * (dx / dist)) + (axisY * (dy / dist));
        else
            normal = { 0,-1 }; // めり込み防止
        return true;
    }
    return false;
}
//----------------------------------------------------------------------
// 変数
//----------------------------------------------------------------------
int pinBall_lafID;
//棒
float posY;
DxPlus::Vec2 bouPosition = { 400,600 };
DxPlus::Vec2 flipperPos = { 400,600 }; // 棒の中心
float flipperLength = 200.0f;          // 棒の長さ
float flipperThickness = 20.0f;        // 棒の厚み
float flipperAngle = 0.25f;            // ラジアン角度
float flipperPrevAngle = 0.0f;
//玉
float ballRadius = 5.0f;
float GRAVITY = 0.5f;
DxPlus::Vec2 ballPosition = { 500,100 };
DxPlus::Vec2 ballVelocity = { 0.0f, 0.0f };
// nextSceneのextern宣言
extern int nextScene;

// ゲーム中の状態
int gameState;
// フェードイン / アウト用変数
float gameFadeTimer;

//----------------------------------------------------------------------
// 初期設定
//----------------------------------------------------------------------
void Game_Init()
{
    DxLib::SetBackgroundColor(0, 128, 255);

    pinBall_lafID = DxPlus::Sprite::Load(L"./Data/Images/pinBall_laf.png");

    Game_Reset();
}

//----------------------------------------------------------------------
// リセット
//----------------------------------------------------------------------
void Game_Reset()
{
    gameState = 0;
    gameFadeTimer = 1.0f;
}

//----------------------------------------------------------------------
// 更新処理
//----------------------------------------------------------------------
void Game_Update()
{
    
    switch (gameState)
    {
    case 0: // フェードイン中
    {
        gameFadeTimer -= 1 / 60.0f;
        if (gameFadeTimer < 0.0f)
        {
            gameFadeTimer = 0.0f;
            gameState++;
        }
        break;
    }

    case 1: // 通常時
    {
        // BackSpaceキーが押されたらタイトルに戻す
        int input = DxPlus::Input::GetButtonDown(DxPlus::Input::PLAYER1);
        if (input & DxPlus::Input::BUTTON_SELECT)
        {
            gameState++;
        }
        break;
    }

    case 2: // フェードアウト中
    {
        gameFadeTimer += 1 / 60.0f;
        if (gameFadeTimer > 1.0f)
        {
            gameFadeTimer = 1.0f;
            nextScene = SceneTitle;
        }
        break;
    }
    }
   
    ballPosition.y += ballVelocity.y;
    float flipperAngularVelocity = flipperAngle - flipperPrevAngle;
    flipperPrevAngle = flipperAngle;
    
    //棒の当たり判定
    float bouWidth = 200.0f;
    float bouHeight = 20.0f;
    float bouLeft = bouPosition.x;
    float bouRight = bouPosition.x + bouWidth;
    float bouTop = bouPosition.y;
    float bouBottom = bouPosition.y + bouHeight;
    //玉の当たり判定
    float ballBottom = ballPosition.y + ballRadius;
    float ballCenterX = ballPosition.x;
    ballVelocity.y += GRAVITY;
    ballPosition.x += ballVelocity.x;
    ballPosition.y += ballVelocity.y;
    float bouPowerX = 0.0f;
    float bouPowerY = 0.0f;
    float angle = 0.25f + posY; // 棒の角度
    DxPlus::Vec2 dir = { cosf(flipperAngle), sinf(flipperAngle) };
    DxPlus::Vec2 half = dir * (flipperLength / 2);

    DxPlus::Vec2 p1 = flipperPos - half; // 棒の端
    DxPlus::Vec2 p2 = flipperPos + half; // 棒の端
    DxPlus::Vec2 normal;
    if (CircleOBBCollision(ballPosition, ballRadius,
        flipperPos, flipperLength, flipperThickness, flipperAngle,
        normal))
    {
        // 押し出し
        ballPosition = ballPosition + normal * 2.0f;

        // 反射
        float vn = ballVelocity.x * normal.x + ballVelocity.y * normal.y;
        ballVelocity = ballVelocity - normal * (2 * vn);

        // キー押しで反発力追加
    }
    if (DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_LEFT)
    {
        flipperAngle = -0.25f;
    }
    else
        flipperAngle = 0.25f;

    

}

//----------------------------------------------------------------------
// 描画処理
//----------------------------------------------------------------------
void Game_Render()
{
    // フェードイン / フェードアウト用
    if (gameFadeTimer > 0.0f)
    {
        DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * gameFadeTimer));
        DxPlus::Primitive2D::DrawRect({ 0,0 },
            { DxPlus::CLIENT_WIDTH, DxPlus::CLIENT_HEIGHT }, DxLib::GetColor(0, 0, 0));
        DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
    }
    DxPlus::Sprite::Draw(pinBall_lafID);
    DxPlus::Primitive2D::DrawRect(flipperPos, { flipperLength, flipperThickness }, GetColor(0, 0, 0), 1.0f, { 0.5f,0.5f }, flipperAngle);
    DxPlus::Primitive2D::DrawCircle(ballPosition, 10, GetColor(0, 0, 0));
}

//----------------------------------------------------------------------
// 終了処理
//----------------------------------------------------------------------
void Game_End()
{
}

