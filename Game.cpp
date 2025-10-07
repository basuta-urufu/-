#include "Game.h"
#include "DxPlus/DxPlus.h"
#include "WinMain.h"
#include <vector>
#include <cmath>

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
// 円と障害物（点）当たり判定
//----------------------------------------------------------------------
bool CheckCollision(DxPlus::Vec2 circle, float radius, DxPlus::Vec2 point)
{
    constexpr float hitboxsize = 10.0f;
    float dx = std::abs(circle.x - point.x);
    float dy = std::abs(circle.y - point.y);
    return (dx < hitboxsize && dy < hitboxsize);
}

//----------------------------------------------------------------------
// 円とポリゴン（ハート壁）当たり判定
//----------------------------------------------------------------------
bool CirclePolygonCollision(
    const DxPlus::Vec2& circlePos, float radius,
    const std::vector<DxPlus::Vec2>& vertices,
    const DxPlus::Vec2& center, float scale,
    DxPlus::Vec2& outNormal, float& outPenetration)
{
    bool hit = false;
    float minPenetration = 1e9f;
    DxPlus::Vec2 bestNormal = { 0,0 };

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        DxPlus::Vec2 p1 = center + vertices[i] * scale;
        DxPlus::Vec2 p2 = center + vertices[(i + 1) % vertices.size()] * scale;
        DxPlus::Vec2 edge = p2 - p1;
        DxPlus::Vec2 normal = { -edge.y, edge.x };
        float len = sqrt(normal.x * normal.x + normal.y * normal.y);
        if (len == 0) continue;
        normal.x /= len; normal.y /= len;

        float dist = (circlePos.x - p1.x) * normal.x + (circlePos.y - p1.y) * normal.y;
        if (dist < radius)
        {
            hit = true;
            float pen = radius - dist;
            if (pen < minPenetration)
            {
                minPenetration = pen;
                bestNormal = normal;
            }
        }
    }

    if (hit)
    {
        outNormal = bestNormal;
        outPenetration = minPenetration;
    }
    return hit;
}

//----------------------------------------------------------------------
// 変数
//----------------------------------------------------------------------
struct Obstacle { DxPlus::Vec2 Position; };
std::vector<Obstacle> Obstacles;

int pinBall_lafID;
DxPlus::Vec2 bouCenter = { 500,650 };
DxPlus::Vec2 bouCenter2 = { 780,650 };
float bouWidth = 200.0f;
float bouHeight = 50.0f;
float angle = 0.25f;
float angle2 = -0.25f;
float ballRadius = 5.0f;
float GRAVITY = 0.5f;
DxPlus::Vec2 ballPosition = { 640,100 };
DxPlus::Vec2 ballVelocity = { 0.0f, 0.0f };

extern int nextScene;
int gameState;
float gameFadeTimer;

//----------------------------------------------------------------------
// ハート形壁（画像に合わせて再設計）
//----------------------------------------------------------------------
// ピンクのふっくらハートに近い形状
std::vector<DxPlus::Vec2> heartVertices = {
    { 0, -140 },
    { 50, -180 },
    { 90, -60 },
    { 120, -10 },
    { 130, 60 },
    { 100, 130 },
    { 50, 180 },
    { 0, 200 },
    { -50, 180 },
    { -100, 130 },
    { -130, 60 },
    { -120, -10 },
    { -90, -60 },
    { -50, -100 },
};
DxPlus::Vec2 heartCenter = { 640, 360 };
float heartScale = 2.4f;

//----------------------------------------------------------------------
// 初期設定
//----------------------------------------------------------------------
void Game_Init()
{
    DxLib::SetBackgroundColor(180, 255, 255);
    pinBall_lafID = DxPlus::Sprite::Load(L"./Data/Images/pinBall_laf.png");

    Obstacles.clear();
    Obstacles.push_back({ {400, 400} });
    Obstacles.push_back({ {600, 300} });
    Obstacles.push_back({ {700, 500} });

    Game_Reset();
}

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
    // --- フェード管理 ---
    switch (gameState)
    {
    case 0:
        gameFadeTimer -= 1 / 60.0f;
        if (gameFadeTimer < 0.0f) { gameFadeTimer = 0.0f; gameState++; }
        break;
    case 1:
        if (DxPlus::Input::GetButtonDown(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_SELECT)
            gameState++;
        break;
    case 2:
        gameFadeTimer += 1 / 60.0f;
        if (gameFadeTimer > 1.0f) { gameFadeTimer = 1.0f; nextScene = SceneTitle; }
        break;
    }

    // --- 玉の物理更新 ---
    ballVelocity.y += GRAVITY;
    ballPosition += ballVelocity;

    // フリッパー入力
    bool leftPressed = DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_LEFT;
    bool rightPressed = DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_RIGHT;
    angle = leftPressed ? -0.25f : 0.25f;
    angle2 = rightPressed ? 0.25f : -0.25f;

    DxPlus::Vec2 normal, normal2;
    float penetration, penetration2;

    // --- フリッパー衝突 ---
    if (CircleOBBCollision(ballPosition, ballRadius, bouCenter, bouWidth, bouHeight, angle, normal, penetration))
    {
        ballPosition += normal * penetration;
        float vn = ballVelocity.x * normal.x + ballVelocity.y * normal.y;
        if (vn < 0) ballVelocity -= normal * (2 * vn);
    }
    if (CircleOBBCollision(ballPosition, ballRadius, bouCenter2, bouWidth, bouHeight, angle2, normal2, penetration2))
    {
        ballPosition += normal2 * penetration2;
        float vn2 = ballVelocity.x * normal2.x + ballVelocity.y * normal2.y;
        if (vn2 < 0) ballVelocity -= normal2 * (2 * vn2);
    }

    // --- ハート壁との衝突 ---
    DxPlus::Vec2 normalH;
    float penetrationH;
    if (CirclePolygonCollision(ballPosition, ballRadius, heartVertices, heartCenter, heartScale, normalH, penetrationH))
    {
        ballPosition += normalH * penetrationH;
        float vn = ballVelocity.x * normalH.x + ballVelocity.y * normalH.y;
        if (vn < 0) ballVelocity -= normalH * (2 * vn);
    }

    // --- 障害物との衝突 ---
    for (auto& obs : Obstacles)
    {
        if (CheckCollision(ballPosition, ballRadius, obs.Position))
        {
            DxPlus::Vec2 dir = ballPosition - obs.Position;
            float len = sqrt(dir.x * dir.x + dir.y * dir.y);
            if (len > 0.0001f)
            {
                dir.x /= len; dir.y /= len;
            }
            ballPosition = obs.Position + dir * (ballRadius + 10.0f);
            float vn = ballVelocity.x * dir.x + ballVelocity.y * dir.y;
            if (vn < 0) ballVelocity -= dir * (2 * vn);
        }
    }
}

//----------------------------------------------------------------------
// 描画処理
//----------------------------------------------------------------------
void Game_Render()
{
    DxPlus::Sprite::Draw(pinBall_lafID);

    // ハート形壁
    std::vector<DxPlus::Vec2> drawVerts;
    for (auto& v : heartVertices)
        drawVerts.push_back(heartCenter + v * heartScale);

    for (size_t i = 0; i < drawVerts.size(); ++i)
    {
        size_t j = (i + 1) % drawVerts.size();
        DxPlus::Primitive2D::DrawLine(drawVerts[i], drawVerts[j], GetColor(255, 120, 200), 3.0f);
    }

    // フリッパー
    DxPlus::Primitive2D::DrawRect(bouCenter, { bouWidth, bouHeight }, GetColor(0, 0, 0), 1.0f,
        { bouWidth * 0.5f, bouHeight * 0.5f }, angle);
    DxPlus::Primitive2D::DrawRect(bouCenter2, { bouWidth, bouHeight }, GetColor(0, 0, 0), 1.0f,
        { bouWidth * 0.5f, bouHeight * 0.5f }, angle2);

    // 障害物
    for (auto& obs : Obstacles)
        DxPlus::Primitive2D::DrawCircle(obs.Position, 10, GetColor(255, 0, 0));

    // 玉
    DxPlus::Primitive2D::DrawCircle(ballPosition, 10, GetColor(0, 0, 0));
}

//----------------------------------------------------------------------
// 終了処理
//----------------------------------------------------------------------
void Game_End() {}
