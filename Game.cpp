#include "Game.h"
#include "DxPlus/DxPlus.h"
#include "WinMain.h"

//----------------------------------------------------------------------
// 円と回転矩形の当たり判定
//----------------------------------------------------------------------
bool CircleOBBCollision(
    DxPlus::Vec2 circle, float radius,
    DxPlus::Vec2 rectCenter, float rectW, float rectH, float angle,
    DxPlus::Vec2& normal, float& penetration)
{
    float c = cos(-angle);
    float s = sin(-angle);

    // ローカル座標に変換
    DxPlus::Vec2 local;
    local.x = (circle.x - rectCenter.x) * c - (circle.y - rectCenter.y) * s;
    local.y = (circle.x - rectCenter.x) * s + (circle.y - rectCenter.y) * c;

    float hx = rectW * 0.5f;
    float hy = rectH * 0.5f;
    float closestX = std::max(-hx, std::min(local.x, hx));
    float closestY = std::max(-hy, std::min(local.y, hy));

    float dx = local.x - closestX;
    float dy = local.y - closestY;
    float dist2 = dx * dx + dy * dy;

    if (dist2 <= radius * radius)
    {
        float dist = sqrt(dist2);
        penetration = radius - dist;

        DxPlus::Vec2 localNormal = { dx, dy };
        if (dist == 0.0f)
            localNormal = { 0, -1 };
        else {
            localNormal.x /= dist;
            localNormal.y /= dist;
        }

        normal.x = localNormal.x * c + localNormal.y * s;
        normal.y = -localNormal.x * s + localNormal.y * c;
        return true;
    }
    return false;
}

//----------------------------------------------------------------------
// 変数
//----------------------------------------------------------------------
int pinBall_lafID;

// 棒
DxPlus::Vec2 bouCenter = { 500,600 };  // ← 中心座標（ここを動かせば棒全体が動く）
float bouWidth = 200.0f;
float bouHeight = 50.0f;
float posY = 0.0f;
float angle= 0.25f; // 入力で角度を変える用


// 玉
float ballRadius = 5.0f;
float GRAVITY = 0.5f;
DxPlus::Vec2 ballPosition = { 500,100 };
DxPlus::Vec2 ballVelocity = { 0.0f, 0.0f };

// nextScene の extern 宣言
extern int nextScene;
int gameState;
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
void Game_Reset() { gameState = 0; gameFadeTimer = 1.0f; }

//----------------------------------------------------------------------
// 更新処理
//----------------------------------------------------------------------
void Game_Update()
{
    switch (gameState)
    {
    case 0: // フェードイン中
        gameFadeTimer -= 1 / 60.0f;
        if (gameFadeTimer < 0.0f) {
            gameFadeTimer = 0.0f;
            gameState++;
        }
        break;

    case 1: // 通常時
    {
        int input = DxPlus::Input::GetButtonDown(DxPlus::Input::PLAYER1);
        if (input & DxPlus::Input::BUTTON_SELECT) {
            gameState++;
        }
        break;
    }

    case 2: // フェードアウト中
        gameFadeTimer += 1 / 60.0f;
        if (gameFadeTimer > 1.0f) {
            gameFadeTimer = 1.0f;
            nextScene = SceneTitle;
        }
        break;
    }

    // --- 玉の更新 ---
    ballVelocity.y += GRAVITY;
    ballPosition.x += ballVelocity.x;
    ballPosition.y += ballVelocity.y;

    // 棒との当たり判定
    static float prevAngle = 0.25f;
    static DxPlus::Vec2 prevBouPosition = bouCenter;
    float angularVelocity = angle - prevAngle;
    DxPlus::Vec2 bouVelocity = bouCenter - prevBouPosition;
    DxPlus::Vec2 normal;
    prevAngle = angle;
    prevBouPosition = bouCenter;
    float penetration;
    if (CircleOBBCollision(ballPosition, ballRadius, bouCenter, bouWidth, bouHeight, angle, normal, penetration))
    {
        // めり込みを解消（最低限の押し戻し）
        ballPosition = ballPosition + normal * penetration;

        float vn = ballVelocity.x * normal.x + ballVelocity.y * normal.y;

        // 棒が動いているかどうかを判定
        bool isMoving = (fabs(angularVelocity) > 0.001f) || (fabs(bouVelocity.x) > 0.01f || fabs(bouVelocity.y) > 0.01f);

        if (isMoving && vn < 0) {
            // 棒が動いているときだけ弾く
            ballVelocity = ballVelocity - normal * (1.5f * vn);

            // 棒の移動分を加える
            ballVelocity = ballVelocity + bouVelocity * 0.5f;

            // 回転によるはじき
            DxPlus::Vec2 tangent = { -normal.y, normal.x };
            ballVelocity = ballVelocity + tangent * (angularVelocity * 80.0f);
        }
        else {
            // 棒が止まっている場合 → 法線方向速度は反射せず、押し戻しのみ
            // つまりそのまま転がる
            if (vn < 0) {
                // 法線成分だけカット（棒にめり込まないように）
                ballVelocity = ballVelocity - normal * vn;
            }
        }
    }

    // 入力で角度を変える
    if (DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_LEFT)
        angle  = 0.25f -0.5f;
    else
        angle = 0.25f;
}

//----------------------------------------------------------------------
// 描画処理
//----------------------------------------------------------------------
void Game_Render()
{
    // フェードイン / フェードアウト用
    if (gameFadeTimer > 0.0f) {
        DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * gameFadeTimer));
        DxPlus::Primitive2D::DrawRect({ 0,0 }, { DxPlus::CLIENT_WIDTH, DxPlus::CLIENT_HEIGHT },
            DxLib::GetColor(0, 0, 0));
        DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
    }

    DxPlus::Sprite::Draw(pinBall_lafID);

    // 棒を描画（中心を基準に回転）
    DxPlus::Vec2 bouTopLeft = {
    bouCenter.x - bouWidth * 0.5f,
    bouCenter.y - bouHeight * 0.5f
    };

    DxPlus::Primitive2D::DrawRect(
        bouCenter, { bouWidth, bouHeight },
        GetColor(0, 0, 0),
        1.0f,
        { bouWidth * 0.5f, bouHeight * 0.5f },
        angle 
    );

    // 玉を描画
    DxPlus::Primitive2D::DrawCircle(ballPosition, 10, GetColor(0, 0, 0));
}

//----------------------------------------------------------------------
// 終了処理
//----------------------------------------------------------------------
void Game_End()
{
}
