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
// 変数
//----------------------------------------------------------------------

//ゲームシーン
enum GameScene {
    Game,
    GameOver,
    GameClear,
};
GameScene Scene = Game;
int pinBall_lafID;//背景
// 棒
DxPlus::Vec2 bouCenter = { 530,660 };  // ← 中心座標（ここを動かせば棒全体が動く）
DxPlus::Vec2 bouCenter2 = { 750,660 };
float bouWidth = 170.0f;
float bouHeight = 50.0f;
float posY = 0.0f;
float angle= 0.25f; // 入力で角度を変える用
float angle2 = -0.25f;
// 斜めの床（バーの先に続く）
DxPlus::Vec2 slopeCenter = { 230, 595 };  // 位置
float slopeWidth = 600.0f;
float slopeHeight = 60.0f;
float slopeAngle = 0.23f;  
DxPlus::Vec2 slopeCenter2 = { 1045, 595 };  // 位置
float slopeWidth2 = 600.0f;
float slopeHeight2 = 60.0f;
float slopeAngle2 = -0.23f; 
//壁
DxPlus::Vec2 wallCenter = { 0, 300 };  // 位置
float wallWidth = 600.0f;
float wallHeight = 60.0f;
float wallAngle = 1.6f;
DxPlus::Vec2 wallCenter2 = { 1280, 300 };  // 位置
float wallWidth2 = 600.0f;
float wallHeight2 = 60.0f;
float wallAngle2 = -1.6f;
DxPlus::Vec2 wallCenter3 = { 700, 0 };  // 位置
float wallWidth3 = 1500.0f;
float wallHeight3 = 60.0f;
float wallAngle3= 3.15f;
// 玉
float ballRadius = 10.0f;
float GRAVITY = 0.5f;
DxPlus::Vec2 ballPosition = { 300,100 };
DxPlus::Vec2 ballVelocity = { 0.0f, 0.0f };
//ハート
int heartID;
float heartRadius = 50.0f;
DxPlus::Vec2 heartPosition = { 845,300 };
DxPlus::Vec2 heartPosition2 = { 300,100 };
int heartPoint = 0;
// ★変更：それぞれのハートに独立したタイプを持たせる
static int heartType1 = 1;
static int heartType2 = 2;
//ポイント
int pointCounter;
bool pointTipe;
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
    Scene = Game;
    pinBall_lafID = DxPlus::Sprite::Load(L"./Data/Images/pinBall_laf.png");
    heartID = DxPlus::Sprite::Load(L"./Data/Images/heart.png");
    srand((unsigned int)time(NULL));  // 乱数の初期化
    heartType1 = rand() % 2 + 1;
    heartType2 = rand() % 2 + 1;
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
    //　現在のゲームシーン
    switch (Scene)
    {
    //　ゲーム画面
    case Game:
    {
        //　バーの当たり判定（左）
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
                ballVelocity = hitDir * 25.0f;
            }
            else if (vn < 0)
            {
                // 普通に反射／転がし処理
                ballVelocity = ballVelocity - normal * vn;
            }
        }
        //　バーの当たり判定（右）
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
                ballVelocity = hitDir2 * 25.0f;
            }
            else if (vn2 < 0)
            {
                ballVelocity = ballVelocity - normal2 * vn2;
            }
        }
        //　斜めの床の当たり判定（左）
        DxPlus::Vec2 slopeNormal;
        float slopePenetration;
        if (CircleOBBCollision(ballPosition, ballRadius, slopeCenter, slopeWidth, slopeHeight, slopeAngle, slopeNormal, slopePenetration))
        {
            ballPosition = ballPosition + slopeNormal * slopePenetration;

            float vn = ballVelocity.x * slopeNormal.x + ballVelocity.y * slopeNormal.y;
            if (vn < 0)
            {
                ballVelocity = ballVelocity - slopeNormal * vn;
                ballVelocity.x *= 0.99f;
                ballVelocity.y *= 0.99f;
            }
        }
        //　斜めの床の当たり判定（右）
        DxPlus::Vec2 slopeNormal2;
        float slopePenetration2;
        if (CircleOBBCollision(ballPosition, ballRadius, slopeCenter2, slopeWidth2, slopeHeight2, slopeAngle2, slopeNormal2, slopePenetration2))
        {
            ballPosition = ballPosition + slopeNormal2 * slopePenetration2;

            float vn2 = ballVelocity.x * slopeNormal2.x + ballVelocity.y * slopeNormal2.y;
            if (vn2 < 0)
            {
                ballVelocity = ballVelocity - slopeNormal2 * vn2;
                ballVelocity.x *= 0.99f;
                ballVelocity.y *= 0.99f;
            }
        }
        //　壁の当たり判定（左）
        DxPlus::Vec2 wallNormal;
        float wallPenetration;
        if (CircleOBBCollision(ballPosition, ballRadius, wallCenter, wallWidth, wallHeight, wallAngle, wallNormal, wallPenetration))
        {
            ballPosition = ballPosition + wallNormal * wallPenetration;

            float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
            if (vn < 0)
            {
                // 反射
                ballVelocity = ballVelocity - wallNormal * (2 * vn);
            }
        }
        //　壁の当たり判定（右）
        if (CircleOBBCollision(ballPosition, ballRadius, wallCenter2, wallWidth2, wallHeight2, wallAngle2, wallNormal, wallPenetration))
        {
            ballPosition = ballPosition + wallNormal * wallPenetration;

            float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
            if (vn < 0)
            {
                // 反射
                ballVelocity = ballVelocity - wallNormal * (2 * vn);
            }
        }
        //　天井の当たり判定
        if (CircleOBBCollision(ballPosition, ballRadius, wallCenter3, wallWidth3, wallHeight3, wallAngle3, wallNormal, wallPenetration))
        {
            ballPosition = ballPosition + wallNormal * wallPenetration;

            float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
            if (vn < 0)
            {
                // 反射
                ballVelocity = ballVelocity - wallNormal * (2 * vn);
            }
        }
        // フレーム間で当たっていたかを記録
        static bool wasCollidingHeart1 = false;
        static bool wasCollidingHeart2 = false;

        // --- ハート1 ---
        DxPlus::Vec2 delta = { ballPosition.x - heartPosition.x, ballPosition.y - heartPosition.y };
        float dist2 = delta.x * delta.x + delta.y * delta.y;
        float radiusSum = ballRadius + heartRadius;
        bool isCollidingHeart1 = (dist2 <= radiusSum * radiusSum);

        if (isCollidingHeart1)
        {
            float dist = sqrt(dist2);
            if (dist == 0.0f) {
                delta = { 0.0f, -1.0f };
                dist = 1.0f;
            }
            DxPlus::Vec2 normal = { delta.x / dist, delta.y / dist };

            // めり込み解消
            float penetration = radiusSum - dist;
            ballPosition = ballPosition + normal * penetration;

            float vn = ballVelocity.x * normal.x + ballVelocity.y * normal.y;

            if (vn < 0.0f)
            {
                ballVelocity = ballVelocity - normal * (2 * vn);

                if (!wasCollidingHeart1)
                {
                    if (heartType1 == 1)
                        heartPoint += 100;
                    else
                        heartPoint += 5;

                    heartType1 = rand() % 2 + 1; // 次の色にランダム変更
                }
            }
        }
        wasCollidingHeart1 = isCollidingHeart1;


        // --- ハート2 ---
        DxPlus::Vec2 delta2 = { ballPosition.x - heartPosition2.x, ballPosition.y - heartPosition2.y };
        float dist3 = delta2.x * delta2.x + delta2.y * delta2.y;
        bool isCollidingHeart2 = (dist3 <= radiusSum * radiusSum);

        if (isCollidingHeart2)
        {
            float dist4 = sqrt(dist3);
            if (dist4 == 0.0f) {
                delta2 = { 0.0f, -1.0f };
                dist4 = 1.0f;
            }
            DxPlus::Vec2 normal2 = { delta2.x / dist4, delta2.y / dist4 };

            float penetration2 = radiusSum - dist4;
            ballPosition = ballPosition + normal2 * penetration2;

            float vn2 = ballVelocity.x * normal2.x + ballVelocity.y * normal2.y;

            if (vn2 < 0.0f)
            {
                ballVelocity = ballVelocity - normal2 * (2 * vn2);

                if (!wasCollidingHeart2)
                {
                    if (heartType2 == 1)
                        heartPoint += 100;
                    else
                        heartPoint += 5;

                    heartType2 = rand() % 2 + 1;
                }
            }
        }
        wasCollidingHeart2 = isCollidingHeart2;



        
        //　ゲームクリアに切り替え
        if (heartPoint >= 100)
        {
            Scene = GameScene::GameClear;
        }
        // ゲームオーバーに切り替え
        /*if (ballPosition.y > DxPlus::CLIENT_HEIGHT)
        {
            Scene = GameScene::GameOver;
        }*/
    }
    break;
    case GameClear:
    {
        
    }
    break;
    //　ゲームオーバー画面
    case GameOver:
    {
        
    }
    break;
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

    switch (Scene)
    {
    case Game:
    {
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
        //斜めの棒
        DxPlus::Primitive2D::DrawRect(
            slopeCenter,
            { slopeWidth, slopeHeight },
            GetColor(80, 80, 80),
            1.0f,
            { slopeWidth * 0.5f, slopeHeight * 0.5f },
            slopeAngle
        );
        DxPlus::Primitive2D::DrawRect(
            slopeCenter2,
            { slopeWidth2, slopeHeight2 },
            GetColor(80, 80, 80),
            1.0f,
            { slopeWidth2 * 0.5f, slopeHeight2 * 0.5f },
            slopeAngle2
        );
        //壁
        DxPlus::Primitive2D::DrawRect(
            wallCenter,
            { wallWidth, wallHeight },
            GetColor(80, 80, 80),
            1.0f,
            { wallWidth * 0.5f, wallHeight * 0.5f },
            wallAngle
        );
        DxPlus::Primitive2D::DrawRect(
            wallCenter2,
            { wallWidth2, wallHeight2 },
            GetColor(80, 80, 80),
            1.0f,
            { wallWidth2 * 0.5f, wallHeight2 * 0.5f },
            wallAngle2
        );
        DxPlus::Primitive2D::DrawRect(
            wallCenter3,
            { wallWidth3, wallHeight3 },
            GetColor(80, 80, 80),
            1.0f,
            { wallWidth3 * 0.5f, wallHeight3 * 0.5f },
            wallAngle3
        );
        //ハート
        if (heartType1 == 1)
            DxPlus::Primitive2D::DrawCircle(heartPosition, heartRadius, GetColor(0, 0, 0));
        else
            DxPlus::Primitive2D::DrawCircle(heartPosition, heartRadius, GetColor(255, 0, 0));

        // ハート2
        if (heartType2 == 1)
            DxPlus::Primitive2D::DrawCircle(heartPosition2, heartRadius, GetColor(0, 0, 0));
        else
            DxPlus::Primitive2D::DrawCircle(heartPosition2, heartRadius, GetColor(255, 0, 0));
        DxPlus::Sprite::Draw(heartID, { 570,230 });
        // 玉を描画
        DxPlus::Primitive2D::DrawCircle(ballPosition, ballRadius, GetColor(0, 0, 0));

    }
    break;
    case GameClear:
    {
        DxPlus::Sprite::Draw(pinBall_lafID);
        DxPlus::Primitive2D::DrawRect(
            wallCenter,
            { wallWidth, wallHeight },
            GetColor(80, 80, 80),
            1.0f,
            { wallWidth * 0.5f, wallHeight * 0.5f },
            wallAngle
        );
        DxPlus::Primitive2D::DrawRect(
            wallCenter2,
            { wallWidth2, wallHeight2 },
            GetColor(80, 80, 80),
            1.0f,
            { wallWidth2 * 0.5f, wallHeight2 * 0.5f },
            wallAngle2
        );
        DxPlus::Primitive2D::DrawRect(
            wallCenter3,
            { wallWidth3, wallHeight3 },
            GetColor(80, 80, 80),
            1.0f,
            { wallWidth3 * 0.5f, wallHeight3 * 0.5f },
            wallAngle3
        );
        //ハート
        DxPlus::Primitive2D::DrawCircle(heartPosition, heartRadius, GetColor(0, 0, 0));
        DxPlus::Sprite::Draw(heartID, { 570,230 });

    }
    break;
    case GameOver:
    {
        DxPlus::Sprite::Draw(pinBall_lafID);
    }
    break;
   }
    
}

//----------------------------------------------------------------------
// 終了処理
//----------------------------------------------------------------------
void Game_End()
{
}
