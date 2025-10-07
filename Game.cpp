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
        if (dist == 0.0f) {
            localNormal = { 0, -1 };
        }
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
// 円と障害物の当たり判定
// ---------------------------------------------------------------------
bool CheckCollision(DxPlus::Vec2 circle2,float radius, DxPlus::Vec2 ObstaclePosition)
{
    constexpr float hitboxsize = 10;

    float dx2 = std::abs(circle2.x - ObstaclePosition.x);
    float dy2 = std::abs(circle2.y - ObstaclePosition.y);

    return (dx2 < hitboxsize && dy2 < hitboxsize);

}
//----------------------------------------------------------------------
// 変数
//----------------------------------------------------------------------
//障害物
struct Obstacle {
    DxPlus::Vec2 Position;

};
std::vector<Obstacle>Obstacles;
int pinBall_lafID;//背景
// 棒
DxPlus::Vec2 bouCenter = { 500,650 };  // ← 中心座標（ここを動かせば棒全体が動く）
DxPlus::Vec2 bouCenter2 = { 780,650 };
float bouWidth = 200.0f;
float bouHeight = 50.0f;
float posY = 0.0f;
float angle= 0.25f; // 入力で角度を変える用
float angle2 = -0.25f;
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
    Obstacles.clear();
    Obstacles.push_back({ {400, 400} });
    Obstacles.push_back({ {600, 300} });
    Obstacles.push_back({ {700, 500} });
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
    static float prevAngle2 = -0.25f;

    static DxPlus::Vec2 prevBouPosition = bouCenter;
    static DxPlus::Vec2 prevBouPosition2 = bouCenter2;
    float angularVelocity = angle - prevAngle;
    float angularVelocity2 = -angle2 - prevAngle2;
    DxPlus::Vec2 bouVelocity = bouCenter - prevBouPosition;
    DxPlus::Vec2 bouVelocity2 = bouCenter2 - prevBouPosition2;
    DxPlus::Vec2 normal;
    DxPlus::Vec2 normal2;
    prevAngle = angle;
    prevAngle2 = angle2;
    prevBouPosition = bouCenter;
    prevBouPosition2 = bouCenter2;
    float penetration;
    float penetration2;
    static bool wasPressed = false;  // 前フレームのAキー状態
    bool isPressed = (DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_LEFT);
    static bool wasPressed2 = false;
    bool isPressed2 = (DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_RIGHT);

    // デフォルトの角度
    float defaultAngle = 0.25f;
    float pressedAngle = 0.25f - 0.5f;
    float defaultAngle2 = -0.25f;
    float pressedAngle2 = -0.25f + 0.5f;

    // 今回の入力で角度を決める
    if (isPressed) {
        angle = pressedAngle;
    }
    else {
        angle = defaultAngle;
    }
    if (isPressed2) {
        angle2 = pressedAngle2;
    }
    else {
        angle2 = defaultAngle2;
    }
    // 「押していたのに離した瞬間」を検知
    bool pressedNow = (!wasPressed && isPressed);  // 押した瞬間
    bool releasedNow = (wasPressed && !isPressed);  // 離した瞬間
    bool pressedNow2 = (!wasPressed2 && isPressed2);  // 押した瞬間
    bool releasedNow2 = (wasPressed2 && !isPressed2);  // 離した瞬間

    wasPressed = isPressed;
    wasPressed2 = isPressed2;

    if (CircleOBBCollision(ballPosition, ballRadius, bouCenter, bouWidth, bouHeight, angle, normal, penetration))
    {
        // --- めり込み解消 ---
        ballPosition = ballPosition + normal * penetration;

        // 法線方向の速度成分
        float vn = ballVelocity.x * normal.x + ballVelocity.y * normal.y;

        // 「押した瞬間」を特別扱い
        if (pressedNow && vn < 0)
        {
            // --- 強制的に飛ばす ---
            DxPlus::Vec2 tangent = { -normal.y, normal.x };

            // ヒット位置によって方向をずらす
            float c = cos(-angle);
            float s = sin(-angle);
            DxPlus::Vec2 local;
            local.x = (ballPosition.x - bouCenter.x) * c - (ballPosition.y - bouCenter.y) * s;
            local.y = (ballPosition.x - bouCenter.x) * s + (ballPosition.y - bouCenter.y) * c;

            float relativeX = local.x / (bouWidth * 0.5f);
            relativeX = std::max(-1.0f, std::min(1.0f, relativeX));

            DxPlus::Vec2 hitDir = normal + tangent * relativeX * 0.8f;
            float len = sqrt(hitDir.x * hitDir.x + hitDir.y * hitDir.y);
            if (len > 0.0001f) {
                hitDir.x /= len;
                hitDir.y /= len;
            }

            // 飛ばす強さ（数値調整可能）
            ballVelocity = hitDir * 20.0f;
        }
        else if (vn < 0)
        {
            // 普通に反射／転がし処理
            ballVelocity = ballVelocity - normal * vn;
        }
    }
    if (CircleOBBCollision(ballPosition, ballRadius, bouCenter2, bouWidth, bouHeight, angle2, normal2, penetration2))
    {
        ballPosition = ballPosition + normal2 * penetration2;

        float vn2 = ballVelocity.x * normal2.x + ballVelocity.y * normal2.y;
        if (pressedNow2 && vn2 < 0)
        {
            DxPlus::Vec2 tangent2 = { -normal2.y, normal2.x };

            // --- ローカル座標に変換 ---
            float c2 = cos(-angle2);
            float s2 = sin(-angle2);
            DxPlus::Vec2 local2;
            local2.x = (ballPosition.x - bouCenter2.x) * c2 - (ballPosition.y - bouCenter2.y) * s2;
            local2.y = (ballPosition.x - bouCenter2.x) * s2 + (ballPosition.y - bouCenter2.y) * c2;

            float relativeX2 = local2.x / (bouWidth * 0.5f);
            relativeX2 = std::max(-1.0f, std::min(1.0f, relativeX2));

            // --- 方向を決める ---
            DxPlus::Vec2 hitDir2 = normal2 + tangent2 * relativeX2 * 0.8f;
            float len2 = sqrt(hitDir2.x * hitDir2.x + hitDir2.y * hitDir2.y);
            if (len2 > 0.0001f) {
                hitDir2.x /= len2;
                hitDir2.y /= len2;
            }

            // --- 飛ばす強さ（調整可能） ---
            ballVelocity = hitDir2 * 20.0f;
        }
        else if (vn2 < 0)
        {
            ballVelocity = ballVelocity - normal2 * vn2;
        }
    }
    for (auto& obs : Obstacles)
    {
        if (CheckCollision(ballPosition, ballRadius, obs.Position))
        {
            DxPlus::Vec2 dir = ballPosition - obs.Position;
            float len = sqrt(dir.x * dir.x + dir.y * dir.y);
            if (len > 0.0001f)
            {
                dir.x /= len;
                dir.y /= len;
            }
            ballPosition = obs.Position + dir * (ballRadius+10.0f);

            float vn = ballVelocity.x * dir.x + ballVelocity.y * dir.y;
            if (vn < 0)
            {
                ballVelocity = ballVelocity - dir * (2 * vn);
            }
        }
    }
        
    

    
   
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
    DxPlus::Primitive2D::DrawRect(
        bouCenter2, { bouWidth,bouHeight },
        GetColor(0, 0, 0),
        1.0f,
        { bouWidth * 0.5f,bouHeight * 0.5f },
        angle2
    );
    for (auto& obs : Obstacles)
    {
        DxPlus::Primitive2D::DrawCircle(obs.Position, 10, GetColor(255, 0, 0));
    }
    // 玉を描画
    DxPlus::Primitive2D::DrawCircle(ballPosition, 10, GetColor(0, 0, 0));
}

//----------------------------------------------------------------------
// 終了処理
//----------------------------------------------------------------------
void Game_End()
{
}
