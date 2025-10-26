#include "Game.h"
#include "DxPlus/DxPlus.h"
#include "WinMain.h"
#include "GameText.h"
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
static bool soundPlayed = false;
static int seHorror = -1;
static int timer = 0;
static int state = 0; // 0=暗転, 1=背景表示, 2=フェードアウ
//音声
int seHit = -1;
int seHit2 = -1;
int seHit3;
int bgm;
extern int bgm3;
int END_SE;
int END_SE2;
int polisSE;
//ゲームシーン
enum GameScene {
    Gamepause,
    Game,
    GameOver,
    GameEnd_A,
    GameEnd_B,
    GameEnd_C,
    GameEnd_D,
    GameEnd_E,
    GameEnd_F,
    GameClear,
};
GameScene Scene = Gamepause;
//背景・画像
int gimikkuIDs[12];
int backID;
int girlPointID;
int EndIDs[6];
int sloopID;
int sloop2ID;
int polisID;
DxPlus::Vec2 polisPosition = { 1280,400 };
// 棒
DxPlus::Vec2 bouCenter = { 530,663 };  // ← 右の中心座標（ここを動かせば棒全体が動く）
DxPlus::Vec2 bouCenter2 = { 750,663 };// 左
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
// 左
DxPlus::Vec2 slopeCenter2 = { 1045, 545 };  // 位置
float slopeWidth2 = 600.0f;
float slopeHeight2 = 60.0f;
float slopeAngle2 = -0.4f; 
// 壁(左)
struct wallObject {
    DxPlus::Vec2 wallCenter;
    float wallWidth;
    float wallHeight;
    float wallAngle;
};
DxPlus::Vec2 wallCenter = { 0, 300 };  // 位置
float wallWidth = 600.0f;
float wallHeight = 60.0f;
float wallAngle = 1.6f;
// 壁(右)
DxPlus::Vec2 wallCenter2 = { 1280, 300 };  // 位置
float wallWidth2 = 600.0f;
float wallHeight2 = 60.0f;
float wallAngle2 = -1.6f;
// 天井
DxPlus::Vec2 wallCenter3 = { 700, 0 };  // 位置
float wallWidth3 = 1500.0f;
float wallHeight3 = 60.0f;
float wallAngle3= 3.15f;
// 棒
DxPlus::Vec2 objectPosition{ 200,300 };
float objectWidth = 200.0f;
float objectHeight = 20;
float objectAngle = -0.23f;
//円の周りの棒
DxPlus::Vec2 objectPosition1{ 1000,300 };
float objectWidth1= 120.0f;
float objectHeight1= 12;
float objectAngle1= 0.0f;
DxPlus::Vec2 objectPosition2{ 1000,300 };
float objectWidth2 = 60.0f;
float objectHeight2 = 6;
float objectAngle2 = 1.55f;
float f = 0;
// 玉
int ballHP = 2;
float ballRadius = 10.0f;
float GRAVITY = 0.5f;
DxPlus::Vec2 ballPosition = { 80,350 };
DxPlus::Vec2 ballVelocity = { 0.0f, 0.0f };
//ハート
int heartID;
int heartPoint = 0;
DxPlus::Vec2 HeartPosition = { 1000,300};
float HeartRadius = 50.0f;
// ハートと連動する障害物
bool Heart = false;
DxPlus::Vec2 HeartObjectPosition = { 630,70 };
float HeartObjectRadius = 40.0f;
// 発射装置
DxPlus::Vec2 shotBallPosition = { 100,200};
float shotBallRadius = 70.0f;
// 障害物
DxPlus::Vec2 ObjectPosition = { 630,300 };
float ObjectRadius = 15.0f;
// ポイント
bool pointTipe;
float PointRadius = 30.0f;
DxPlus::Vec2 PointPosition = { 1050,100 };
DxPlus::Vec2 PointPosition2 = { 400,100 };
static int PointType1 = 1;
static int PointType2 = 2;
// タイマー
float currentTime = 0.0f;
float timeLimit = 60.0f; // 60秒制限
// nextScene の extern 宣言
extern int nextScene;
int gameState;
float gameFadeTimer;
// ---------------------------------------------------------
// 汎用フェードアウト処理
// ---------------------------------------------------------
bool DoFadeOut()
{
    static int localFadeCount = 0;
    static const int localMaxFade = 180;
    localFadeCount++;

    int alpha = (255 * localFadeCount) / localMaxFade;
    if (alpha > 255) alpha = 255;

    // 徐々に黒を重ねていく
    DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
    DxLib::DrawBox(0, 0, 1280, 720, DxLib::GetColor(0, 0, 0), TRUE);
    DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    if (localFadeCount >= localMaxFade)
    {
        localFadeCount = 0;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------
// 初期設定
//----------------------------------------------------------------------
void Game_Init()
{
    DxLib::SetBackgroundColor(0, 128, 255);
    Scene = GameScene::Gamepause;
    EndIDs[0]= DxPlus::Sprite::Load(L"./Data/Images/Background1_Hole.png");
    EndIDs[1]= DxPlus::Sprite::Load(L"./Data/Images/back.png");
    EndIDs[2]= DxPlus::Sprite::Load(L"./Data/Images/Background2.png");
    EndIDs[3]= DxPlus::Sprite::Load(L"./Data/Images/Character_JiraichanGameEnd5099_Image.png");
    EndIDs[4]= DxPlus::Sprite::Load(L"./Data/Images/Jiraichan_GameClear_Rough.png");
    EndIDs[5] = DxPlus::Sprite::Load(L"./Data/Images/Character_JiraichanGameEnd149_Image.png");
    heartID = DxPlus::Sprite::Load(L"./Data/Images/heart.png");
    girlPointID = DxPlus::Sprite::Load(L"./Data/Images/Character_Jiraichan1_2_Normal.png");
    gimikkuIDs[0]= DxPlus::Sprite::Load(L"./Data/Images/¥boundbou.png");
    gimikkuIDs[1]= DxPlus::Sprite::Load(L"./Data/Images/¥Changebotann_on.png");
    gimikkuIDs[2]= DxPlus::Sprite::Load(L"./Data/Images/¥Changebotann_off.png");
    gimikkuIDs[3]= DxPlus::Sprite::Load(L"./Data/Images/hukitobasi.png");
    gimikkuIDs[4]= DxPlus::Sprite::Load(L"./Data/Images/Changebotann_point.png");
    backID= DxPlus::Sprite::Load(L"./Data/Images/toriaezuhaikei.png");
    sloopID = DxPlus::Sprite::Load(L"./Data/Images/sloop.png");
    sloop2ID = DxPlus::Sprite::Load(L"./Data/Images/sloop2.png");
    polisID= DxPlus::Sprite::Load(L"./Data/Images/polis.png");
    seHit = DxLib::LoadSoundMem(L"./Data/Sounds/gakon.mp3");
    seHit2 = DxLib::LoadSoundMem(L"./Data/Sounds/PlayerShot.wav");
    END_SE = DxLib::LoadSoundMem(L"./Data/Sounds/ショック1.mp3");
    END_SE2;
    polisSE = DxLib::LoadSoundMem(L"./Data/Sounds/Police_Car-Siren03-01(Close).mp3");
    //seHit3= DxLib::LoadSoundMem(L"./Data/Sounds/");
    bgm = LoadSoundMem(L"./Data/Sounds/heal_me.wav");
    srand((unsigned int)time(NULL));  // 乱数の初期化
    PointType1 = rand() % 2 + 1;
    PointType2 = rand() % 2 + 1;

    soundPlayed = false;
    Game_Reset();
}

//----------------------------------------------------------------------
// リセット
//----------------------------------------------------------------------
void Game_Reset() 
{ 
    ballHP = 2;
    gameState = 0; 
    heartPoint = 0;
    Scene = GameScene::Gamepause;
    gameFadeTimer = 1.0f; 
    currentTime = 0.0f;
    timeLimit = 60.0f;
    Heart = false;
    ballPosition = { 1150,400};
    ballVelocity = { 0.0f, 0.0f };
    polisPosition = { 1280,400 };

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
    case Gamepause:
    {
        if (DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_START)
        {
            Scene = GameScene::Game;
            ballHP = 2;
            gameState = 0;
            heartPoint = -40;
            gameFadeTimer = 1.0f;
            currentTime = 0.0f;
            timeLimit = 60.0f;
            ballPosition = { 1150,400 };
            ballVelocity = { 0.0f, 0.0f };
            PlaySoundMem(bgm, DX_PLAYTYPE_LOOP, TRUE);
            ChangeVolumeSoundMem(300, bgm);
        }
        break;
    }
    //　ゲーム画面
    case Game:
    {
       
        // エンディング
      currentTime += 1.0f / 60.0f;
if (currentTime >= timeLimit)
{
    if (heartPoint <= 0)
    {
        gameFadeTimer += 1 / 60.0f;
        if (gameFadeTimer > 1.0f) {
            gameFadeTimer = 1.0f;
           
            Scene = GameScene::GameEnd_A;
        }
       
    }
    else if (heartPoint < 50)
    {
        gameFadeTimer += 1 / 60.0f;
        if (gameFadeTimer > 1.0f) {
            gameFadeTimer = 1.0f;
            Scene = GameScene::GameEnd_B;
        }
       
    }
    else if (heartPoint <= 99)
    {
        gameFadeTimer += 1 / 60.0f;
        if (gameFadeTimer > 1.0f) {
            gameFadeTimer = 1.0f;
            
            Scene = GameScene::GameEnd_C;
        }
       
    }
    else if (heartPoint >= 100)
    {
        Scene = GameScene::GameClear;
    }

}
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
                DxLib::PlaySoundMem(seHit, DX_PLAYTYPE_BACK);
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
                DxLib::PlaySoundMem(seHit, DX_PLAYTYPE_BACK);
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
        for (int i = 0;i <= 1;i++)
        {
            if (CircleOBBCollision(ballPosition, ballRadius, { wallCenter.x+i*95,wallCenter.y+i*340 }, wallWidth, wallHeight, wallAngle, wallNormal, wallPenetration))
            {
                ballPosition = ballPosition + wallNormal * wallPenetration;

                float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
                if (vn < 0)
                {
                    // 反射
                    ballPosition += wallNormal * wallPenetration;
                    ballVelocity = ballVelocity - wallNormal * (2 * vn);
                }
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
                ballPosition += wallNormal * wallPenetration;
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
                ballPosition += wallNormal * wallPenetration;
                ballVelocity = ballVelocity - wallNormal * (2 * vn);
            }
        }
        // ---強く反射する障害物---
        if (CircleOBBCollision(ballPosition, ballRadius, { objectPosition.x,objectPosition.y }, objectWidth, objectHeight, objectAngle * 1 - 0.17, wallNormal, wallPenetration))
        {
            ballPosition = ballPosition + wallNormal * wallPenetration;

            float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
            if (vn < 0)
            {
                // 反射
                
                ballVelocity = ballVelocity - wallNormal * (2.0 * vn);
            }
        }
        if (CircleOBBCollision(ballPosition, ballRadius, { objectPosition.x-100,objectPosition.y+50 }, objectWidth, objectHeight, objectAngle * 1 - 0.17, wallNormal, wallPenetration))
        {
            ballPosition = ballPosition + wallNormal * wallPenetration;

            float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
            if (vn < 0)
            {
                // 反射
                ballPosition += wallNormal * wallPenetration;
                ballVelocity = ballVelocity - wallNormal * (2.0 * vn);
            }
        }

        if (CircleOBBCollision(ballPosition, ballRadius, { objectPosition.x+800,objectPosition.y +180 }, objectWidth, objectHeight, objectAngle-0.19, wallNormal, wallPenetration))
        {
            ballPosition = ballPosition + wallNormal * wallPenetration;

            float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
            // めり込み解消
            if (vn < 0)
            {
                ballPosition += wallNormal * wallPenetration;
                // 反射
                ballVelocity = ballVelocity - wallNormal * (2.0 * vn);
            }
        }
        if (CircleOBBCollision(ballPosition, ballRadius, { objectPosition.x+100 ,objectPosition.y + 220 }, objectWidth, objectHeight, objectAngle * -1, wallNormal, wallPenetration))
        {
            ballPosition = ballPosition + wallNormal * wallPenetration;

            float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
            if (vn < 0)
            {
                // 反射
                ballPosition += wallNormal * wallPenetration;
                ballVelocity = ballVelocity - wallNormal * (2.0 * vn);
            }
        }


        
        //円の斜めの棒
        if (CircleOBBCollision(ballPosition, ballRadius, { objectPosition.x+550 ,objectPosition.y - 190 }, objectWidth, objectHeight, objectAngle -0.64, wallNormal, wallPenetration))
        {
            ballPosition = ballPosition + wallNormal * wallPenetration;

            float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
            if (vn < 0)
            {
                // 反射
                ballPosition += wallNormal * wallPenetration;
                ballVelocity = ballVelocity - wallNormal * ( vn);
            }
        }
        if (CircleOBBCollision(ballPosition, ballRadius, { objectPosition.x + 300 ,objectPosition.y - 190 }, objectWidth, objectHeight, objectAngle +1.23, wallNormal, wallPenetration))
        {
            ballPosition = ballPosition + wallNormal * wallPenetration;

            float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
            if (vn < 0)
            {
                // 反射
                ballPosition += wallNormal * wallPenetration;
                ballVelocity = ballVelocity - wallNormal * (vn);
            }
        }
        // ---円の周りの棒---
            if (CircleOBBCollision(ballPosition, ballRadius, { objectPosition1.x+20,objectPosition1.y - 55 }, objectWidth1, objectHeight1, objectAngle1/* + f*/, wallNormal, wallPenetration))
            {
                ballPosition = ballPosition + wallNormal * wallPenetration;

                float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
                if (vn < 0)
                {
                    // 反射
                    ballPosition += wallNormal * wallPenetration;
                    ballVelocity = ballVelocity - wallNormal * (vn);
                }
            }
        if (CircleOBBCollision(ballPosition, ballRadius, {objectPosition1.x+20,objectPosition1.y + 55}, objectWidth1, objectHeight1, objectAngle1, wallNormal, wallPenetration))
        {
            ballPosition = ballPosition + wallNormal * wallPenetration;

            float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
            if (vn < 0)
            {
                // 反射
                ballPosition += wallNormal * wallPenetration;
                ballVelocity = ballVelocity - wallNormal * (vn);
            }
        }
        if (CircleOBBCollision(ballPosition, ballRadius, {objectPosition1.x + 75,objectPosition1.y}, objectWidth1, objectHeight1, objectAngle1 + 1.11, wallNormal, wallPenetration))
        {
            ballPosition = ballPosition + wallNormal * wallPenetration;

            float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
            if (vn < 0)
            {
                // 反射
                ballPosition += wallNormal * wallPenetration;
                ballVelocity = ballVelocity - wallNormal * (2*vn);
            }
        }
        if (CircleOBBCollision(ballPosition, ballRadius, { objectPosition1.x - 930,objectPosition1.y + 120 }, objectWidth1, objectHeight1, objectAngle1-0.23/* + f*/, wallNormal, wallPenetration))
        {
            ballPosition = ballPosition + wallNormal * wallPenetration;

            float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
            if (vn < 0)
            {
                // 反射
                ballPosition += wallNormal * wallPenetration;
                ballVelocity = ballVelocity - wallNormal * (2*vn);
            }
        }
        if (CircleOBBCollision(ballPosition, ballRadius, { objectPosition1.x - 930,objectPosition1.y + 180 }, objectWidth1, objectHeight1, objectAngle1 - 0.23/* + f*/, wallNormal, wallPenetration))
        {
            ballPosition = ballPosition + wallNormal * wallPenetration;

            float vn = ballVelocity.x * wallNormal.x + ballVelocity.y * wallNormal.y;
            if (vn < 0)
            {
                // 反射
                ballPosition += wallNormal * wallPenetration;
                ballVelocity = ballVelocity - wallNormal * (2*vn);
            }
        }

        // ---反射しない障害物---
        DxPlus::Vec2 Objectdelta = { ballPosition.x - ObjectPosition.x, ballPosition.y - ObjectPosition.y };
        float Objectdist = Objectdelta.x * Objectdelta.x + Objectdelta.y * Objectdelta.y;
        float ObjectradiusSum = ballRadius + ObjectRadius;
        bool isCollidingObject = (Objectdist <= ObjectradiusSum * ObjectradiusSum);
        if (isCollidingObject)
        {
            float dist = sqrt(Objectdist);
            if (dist == 0.0f) {
                Objectdelta = { 0.0f, -1.0f };
                dist = 1.0f;
            }
            DxPlus::Vec2 normal = { Objectdelta.x / dist, Objectdelta.y / dist };
            // めり込み解消
            float Objectpenetration = ObjectradiusSum - dist;
            ballPosition = ballPosition + normal * Objectpenetration;
            float vn = ballVelocity.x * normal.x + ballVelocity.y * normal.y;
            if (vn < 0.0f)
            {
                ballPosition += wallNormal * wallPenetration;
                ballVelocity = ballVelocity - normal * (vn);

            }
        }

        // --- ハート ---
        DxPlus::Vec2 delta = {ballPosition.x - HeartPosition.x, ballPosition.y - HeartPosition.y};
        float dist = delta.x * delta.x + delta.y * delta.y;
        float radiusSum = ballRadius + HeartRadius;
        bool isCollidingHeart1 = (dist <= radiusSum * radiusSum);
        if (isCollidingHeart1)
        {
            DxLib::PlaySoundMem(seHit2, DX_PLAYTYPE_BACK);
            float dist2 = sqrt(dist);
            if (dist2 == 0.0f) {
                delta = { 0.0f, -1.0f };
                dist2 = 1.0f;
            }
            DxPlus::Vec2 normal = { delta.x / dist2, delta.y / dist2 };

            // めり込み解消
            float penetration = radiusSum - dist2;
            ballPosition = ballPosition + normal * penetration;

            float vn = ballVelocity.x * normal.x + ballVelocity.y * normal.y;
            if (vn < 0.0f)
            {
                if (Heart)
                {
                    Heart = false;
                }
                else
                {
                    Heart = true;
                }
                ballVelocity = ballVelocity - normal * (1.6f * vn); 
            }

        }
        // ハートと連動する障害物
        DxPlus::Vec2 delta2 = {ballPosition.x - HeartObjectPosition.x, ballPosition.y - HeartObjectPosition.y};
        float dist3 = delta2.x * delta2.x + delta2.y * delta2.y;
        float radiusSum2 = ballRadius + HeartObjectRadius;
        bool isCollidingHeart = (dist3 <= radiusSum2 * radiusSum2);
        if (isCollidingHeart)
        {
            DxLib::PlaySoundMem(seHit2, DX_PLAYTYPE_BACK);
            float dist4 = sqrt(dist3);
            if (dist4 == 0.0f) {
                delta2 = { 0.0f, -1.0f };
                dist4 = 1.0f;
            }
            DxPlus::Vec2 normal2 = { delta2.x / dist4, delta2.y / dist4 };

            // めり込み解消
            float penetration2 = radiusSum2 - dist4;
            ballPosition = ballPosition + normal2 * penetration2;

            float vn = ballVelocity.x * normal2.x + ballVelocity.y * normal2.y;
            if (vn < 0.0f)
            {
                if (Heart)
                {
                    heartPoint += 30;
                    Heart = false;
                }
                ballVelocity = ballVelocity - normal2 * (1.6f * vn);
            }

        }
        // フレーム間で当たっていたかを記録
        static bool wasCollidingHeart1 = false;
        static bool wasCollidingHeart2 = false;
        // --- ポイント ---
        DxPlus::Vec2 Pointdelta = {ballPosition.x - PointPosition.x, ballPosition.y - PointPosition.y};
        float dist2 = Pointdelta.x * Pointdelta.x + Pointdelta.y * Pointdelta.y;
        float PointradiusSum = ballRadius + PointRadius;
        bool isCollidingPoint1 = (dist2 <= PointradiusSum * PointradiusSum);
        if (isCollidingPoint1)
        {
            DxLib::PlaySoundMem(seHit2, DX_PLAYTYPE_BACK);
            float dist = sqrt(dist2);
            if (dist == 0.0f) {
                Pointdelta = { 0.0f, -1.0f };
                dist = 1.0f;
            }
            DxPlus::Vec2 normal = { Pointdelta.x / dist, Pointdelta.y / dist };
            // めり込み解消
            float Pointpenetration = PointradiusSum - dist;
            ballPosition = ballPosition + normal * Pointpenetration;
            float vn = ballVelocity.x * normal.x + ballVelocity.y * normal.y;
            if (vn < 0.0f)
            {
                ballVelocity = ballVelocity - normal * (2 * vn);

                if (!wasCollidingHeart1)
                {
                    if (PointType1 == 1)
                        heartPoint += 20;
                    else
                        heartPoint -= 10;

                    PointType1 = rand() % 2 + 1;
                    PointType2 = rand() % 2 + 1;// 次の色にランダム変更
                }
            }
        }
        wasCollidingHeart1 = isCollidingPoint1;
        // --- ポイント2 ---
        DxPlus::Vec2 Pointdelta2 = { ballPosition.x - PointPosition2.x, ballPosition.y - PointPosition2.y };
        float Pointdist3 = Pointdelta2.x * Pointdelta2.x + Pointdelta2.y * Pointdelta2.y;
        bool isCollidingPoint2 = (Pointdist3 <= PointradiusSum * PointradiusSum);
        if (isCollidingPoint2)
        {
            DxLib::PlaySoundMem(seHit2, DX_PLAYTYPE_BACK);
            float dist4 = sqrt(Pointdist3);
            if (dist4 == 0.0f) {
                Pointdelta2 = { 0.0f, -1.0f };
                dist4 = 1.0f;
            }
            DxPlus::Vec2 normal2 = { Pointdelta2.x / dist4, Pointdelta2.y / dist4 };
            float penetration2 = PointradiusSum - dist4;
            ballPosition = ballPosition + normal2 * penetration2;
            float vn2 = ballVelocity.x * normal2.x + ballVelocity.y * normal2.y;
            if (vn2 < 0.0f)
            {
                ballVelocity = ballVelocity - normal2 * (2 * vn2);

                if (!wasCollidingHeart2)
                {
                    if (PointType2 == 1)
                        heartPoint += 20;
                    else
                        heartPoint -= 10;

                    PointType2 = rand() % 2 + 1;
                    PointType1 = rand() % 2 + 1;
                }
            }
        }
        wasCollidingHeart2 = isCollidingPoint2;
         // ボールを打ち出す障害物
DxPlus::Vec2 shotBalldelta = { ballPosition.x - shotBallPosition.x, ballPosition.y - shotBallPosition.y };
float shotBalldist = shotBalldelta.x * shotBalldelta.x + shotBalldelta.y * shotBalldelta.y;
float shotBallradiusSum = shotBallRadius + shotBallRadius;
bool isCollidingshotBallRadius = (shotBalldist <= shotBallradiusSum * shotBallradiusSum);
if (isCollidingshotBallRadius)
{
    float shotBalldist2 = sqrt(shotBalldist);
    if (shotBalldist2 == 0.0f) {
        shotBalldelta = { 0.0f, -1.0f };
        shotBalldist2 = 1.0f;
    }
    DxPlus::Vec2 normal = { shotBalldelta.x / shotBalldist2, shotBalldelta.y / shotBalldist2 };
    // めり込み解消
    float penetration = shotBallradiusSum - shotBalldist2;
    ballPosition = ballPosition + normal * penetration;
    float vn = ballVelocity.x * normal.x + ballVelocity.y * normal.y;
    if (vn < 0.0f)
    {
        ballPosition = { 300, 440 };
        ballVelocity = { 15.0f,-15.0f };
    }

}  
        // ゲームオーバーに切り替え
        if (ballPosition.y > DxPlus::CLIENT_HEIGHT)
        {
            ballHP -=1;
            if (ballHP < 0)
            {
                GameText_Init();
                DxLib::PlaySoundMem(END_SE, DX_PLAYTYPE_BACK);
                Scene = GameScene::GameOver;
            }
            else
            {
                ballPosition = { 1150,400 };
                ballVelocity = { 0.0f,0.0f };
            }
            
        }
    }
    break;
    case GameClear:
    {
    if (!soundPlayed)
{
    seHorror = DxLib::LoadSoundMem(L"./Data/Sounds/ショック1.mp3");
    DxLib::PlaySoundMem(seHorror, DX_PLAYTYPE_BACK);
    soundPlayed = true;
    Game_Reset();
}

    }
   break;
    case GameEnd_A:
    {
   if (!soundPlayed)
{
    seHorror = DxLib::LoadSoundMem(L"./Data/Sounds/ショック1.mp3");
    DxLib::PlaySoundMem(seHorror, DX_PLAYTYPE_BACK);
    soundPlayed = true;
    Game_Reset();
}
break;
    }
  
    case GameEnd_B:
    {
     if (!soundPlayed)
{
    seHorror = DxLib::LoadSoundMem(L"./Data/Sounds/ショック1.mp3");
    DxLib::PlaySoundMem(seHorror, DX_PLAYTYPE_BACK);
    soundPlayed = true;
    Game_Reset();
}

    }
   break;
    case GameEnd_C:
    {
       if (!soundPlayed)
{
    seHorror = DxLib::LoadSoundMem(L"./Data/Sounds/ショック1.mp3");
    DxLib::PlaySoundMem(seHorror, DX_PLAYTYPE_BACK);
    soundPlayed = true;
    Game_Reset();
}

    }
    break;
    case GameEnd_D:
    {
      if (!soundPlayed)
{
    seHorror = DxLib::LoadSoundMem(L"./Data/Sounds/ショック1.mp3");
    DxLib::PlaySoundMem(seHorror, DX_PLAYTYPE_BACK);
    soundPlayed = true;
    Game_Reset();
}

    }
    break;
    case GameEnd_E:
    {
       if (!soundPlayed)
{
    seHorror = DxLib::LoadSoundMem(L"./Data/Sounds/ショック1.mp3");
    DxLib::PlaySoundMem(seHorror, DX_PLAYTYPE_BACK);
    soundPlayed = true;
    Game_Reset();
}
break;
    }
   
    //　ゲームオーバー画面
    case GameOver:
    {
    if (!soundPlayed)
{
    seHorror = DxLib::LoadSoundMem(L"./Data/Sounds/ショック1.mp3");
    DxLib::PlaySoundMem(seHorror, DX_PLAYTYPE_BACK);
    soundPlayed = true;
    Game_Reset();
}
break;
    
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
    case Gamepause:
    {
            DxPlus::Sprite::Draw(backID, { 0,0 });

            for (int i = 0;i <= 1;i++)
            {
                DxPlus::Sprite::Draw(gimikkuIDs[0], { 300.0f + i * 800,500.0f + i * 30 }, { 0.6,0.6 }, { 640,320 }, DxPlus::Deg2Rad * 15.0f + i * 15);
            }
            DxPlus::Sprite::Draw(gimikkuIDs[3], { 0,0 });
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
            //反射する棒
            for (int i = 0;i <= 1;i++)
            {
                DxPlus::Primitive2D::DrawRect(
                    { objectPosition.x + i * -100,objectPosition.y + i * 50 },
                    { objectWidth, objectHeight },
                    GetColor(0, 80, 0),
                    1.0f,
                    { objectWidth * 0.5f, objectHeight * 0.5f },
                    objectAngle * 1 - 0.17
                );
                DxPlus::Primitive2D::DrawRect(
                    {wallCenter.x+i*95, wallCenter.y+i*340},
                    { wallWidth, wallHeight },
                    GetColor(80, 80, 80),
                    1.0f,
                    { wallWidth * 0.5f, wallHeight * 0.5f },
                    wallAngle
                );
            }
            DxPlus::Primitive2D::DrawRect(
                { objectPosition.x + 800,objectPosition.y + 180 },
                { objectWidth, objectHeight },
                GetColor(0, 80, 0),
                1.0f,
                { objectWidth * 0.5f, objectHeight * 0.5f },
                objectAngle - 0.19
            );
            DxPlus::Primitive2D::DrawRect(
                { objectPosition.x+100 ,objectPosition.y + 220 },
                { objectWidth, objectHeight }, GetColor(0, 80, 0),
                1.0f,
                { objectWidth * 0.5f, objectHeight * 0.5f },
                objectAngle * -1
            );

            DxPlus::Primitive2D::DrawRect(
                { objectPosition1.x - 930,objectPosition1.y + 120 },
                { objectWidth1, objectHeight1 },
                GetColor(0, 80, 0),
                1.0f,
                { objectWidth1 * 0.5f, objectHeight1 * 0.5f },
                objectAngle1 - 0.23
            );
            DxPlus::Primitive2D::DrawRect(
                { objectPosition1.x - 930,objectPosition1.y + 180 },
                { objectWidth1, objectHeight1 },
                GetColor(0, 80, 0),
                1.0f,
                { objectWidth1 * 0.5f, objectHeight1 * 0.5f },
                objectAngle1 - 0.23
            );


            //円の周りの斜めの棒
            for (int i = 0;i <= 1;i++)
            {
                DxPlus::Sprite::Draw(gimikkuIDs[0], { 520.0f + i * 200,90.0f }, { 0.7,0.7 }, { 640,320 }, DxPlus::Deg2Rad * 48.0f + i * 17.15f);

            }
            DxPlus::Primitive2D::DrawRect(
                { objectPosition.x + 550 ,objectPosition.y - 190 },
                { objectWidth, objectHeight }, GetColor(0, 80, 0),
                1.0f,
                { objectWidth * 0.5f, objectHeight * 0.5f },
                objectAngle - 0.64
            );
            DxPlus::Primitive2D::DrawRect(
                { objectPosition.x + 300 ,objectPosition.y - 190 },
                { objectWidth, objectHeight }, GetColor(0, 80, 0),
                1.0f,
                { objectWidth * 0.5f, objectHeight * 0.5f },
                objectAngle + 1.11
            );
            //円の周りの棒
            DxPlus::Primitive2D::DrawRect(
                { objectPosition1.x + 20,objectPosition1.y - 55 },
                { objectWidth1, objectHeight1 },
                GetColor(0, 80, 0),
                1.0f,
                { objectWidth1 * 0.5f, objectHeight1 * 0.5f },
                objectAngle1
            );
            DxPlus::Primitive2D::DrawRect(
                { objectPosition1.x + 20,objectPosition1.y + 55 },
                { objectWidth1, objectHeight1 },
                GetColor(0, 80, 0),
                1.0f,
                { objectWidth1 * 0.5f, objectHeight1 * 0.5f },
                objectAngle1
            );
            DxPlus::Primitive2D::DrawRect(
                { objectPosition1.x + 75,objectPosition1.y },
                { objectWidth1, objectHeight1 },
                GetColor(0, 80, 0),
                1.0f,
                { objectWidth1 * 0.5f, objectHeight1 * 0.5f },
                objectAngle1 + 1.55
            );
            //反射しない障害物
            DxPlus::Primitive2D::DrawCircle(ObjectPosition, ObjectRadius, GetColor(255, 255, 255));
            // ボールを飛ばすギミック
            DxPlus::Primitive2D::DrawCircle(shotBallPosition, shotBallRadius, GetColor(0, 0, 255));
            // 得点
            if (PointType1 == 1)
            {
                DxPlus::Primitive2D::DrawCircle(PointPosition, PointRadius, GetColor(255, 0, 0));
            }
            else
            {
                DxPlus::Primitive2D::DrawCircle(PointPosition, PointRadius, GetColor(0, 0, 0));
            }
            // 得点２
            if (PointType2 == 1)
            {
                DxPlus::Primitive2D::DrawCircle(PointPosition2, PointRadius, GetColor(255, 0, 0));

            }
            else
            {
                DxPlus::Primitive2D::DrawCircle(PointPosition2, PointRadius, GetColor(0, 0, 0));
            }
            DxPlus::Primitive2D::DrawCircle(HeartPosition, HeartRadius, GetColor(0, 255, 0));
            DxPlus::Sprite::Draw(gimikkuIDs[2], { 930,200 }, { 0.5,0.5 });
            DxPlus::Sprite::Draw(gimikkuIDs[4], { 520,30 }, { 0.5,0.5 });
            DxPlus::Primitive2D::DrawCircle({ HeartObjectPosition.x,HeartObjectPosition.y }, HeartRadius, GetColor(0, 0, 0));
            // 時間の描画
            std::wstring Time = std::to_wstring(timeLimit) + L"";
            DxPlus::Text::DrawString(Time.c_str(), 200, 650, GetColor(255, 255, 255));
            // 得点の描画
            DxPlus::Vec2 girlPointPosition = { 1100,530 };
            DxPlus::Vec2 girlPointScale = { 0.4,0.4 };
            DxPlus::Sprite::Draw(girlPointID, girlPointPosition, girlPointScale);
            DxPlus::Sprite::Draw(heartID, { 920,580 });
            std::wstring scoreText = std::to_wstring(heartPoint) + L"%";
            DxPlus::Text::DrawString(scoreText.c_str(), 970, 650, GetColor(255, 255, 255));
            DxPlus::Sprite::Draw(sloopID, { 775 , 685 }, { 0.5f, 0.5f }, { 228, 108 });
            DxPlus::Sprite::Draw(sloop2ID, { 500, 632 }, { 0.5f, 0.5f }, { 1, 1 });
            // 玉を描画
            DxPlus::Primitive2D::DrawCircle({ 1150, 400 }, ballRadius, GetColor(0, 0, 0));

     
    
    case Game:
    {
        DxPlus::Sprite::Draw(backID, { 0,0 });

        for (int i = 0;i <= 1;i++)
        {
            DxPlus::Sprite::Draw(gimikkuIDs[0], { 300.0f + i * 800,500.0f + i * 30 }, { 0.6,0.6 }, { 640,320 }, DxPlus::Deg2Rad * 15.0f + i * 15);
        }
        DxPlus::Sprite::Draw(gimikkuIDs[3], { 0,0 });
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
        //反射する棒
        for (int i = 0;i <= 1;i++)
        {
            DxPlus::Primitive2D::DrawRect(
                { objectPosition.x+i*-100,objectPosition.y+i*50 },
                { objectWidth, objectHeight },
                GetColor(0, 80, 0),
                1.0f,
                { objectWidth * 0.5f, objectHeight * 0.5f },
                objectAngle * 1 - 0.17
            );
            DxPlus::Primitive2D::DrawRect(
                { objectPosition.x - 80 - i,objectPosition.y + 10 + i * 250 },
                { objectWidth, objectHeight },
                GetColor(0, 80, 0),
                1.0f,
                { objectWidth * 0.5f, objectHeight * 0.5f },
                objectAngle - 1.3
            );
            DxPlus::Primitive2D::DrawRect(
                { wallCenter.x + i * 95, wallCenter.y + i * 340 },
                { wallWidth, wallHeight },
                GetColor(80, 80, 80),
                1.0f,
                { wallWidth * 0.5f, wallHeight * 0.5f },
                wallAngle
            );

        }
        DxPlus::Primitive2D::DrawRect(
            { objectPosition.x + 800,objectPosition.y + 180 },
            { objectWidth, objectHeight },
            GetColor(0, 80, 0),
            1.0f,
            { objectWidth * 0.5f, objectHeight * 0.5f },
            objectAngle - 0.19
        );
        DxPlus::Primitive2D::DrawRect(
            { objectPosition.x+100 ,objectPosition.y + 220 },
            { objectWidth, objectHeight }, GetColor(0, 80, 0),
            1.0f,
            { objectWidth * 0.5f, objectHeight * 0.5f },
            objectAngle * -1
        );
        
            DxPlus::Primitive2D::DrawRect(
                { objectPosition1.x - 930,objectPosition1.y + 120},
                { objectWidth1, objectHeight1 },
                GetColor(0, 80, 0),
                1.0f,
                { objectWidth1 * 0.5f, objectHeight1 * 0.5f },
                objectAngle1 - 0.23
            );
            DxPlus::Primitive2D::DrawRect(
                { objectPosition1.x - 930,objectPosition1.y + 180 },
                { objectWidth1, objectHeight1 },
                GetColor(0, 80, 0),
                1.0f,
                { objectWidth1 * 0.5f, objectHeight1 * 0.5f },
                objectAngle1 - 0.23
            );

        
        //円の周りの斜めの棒
            for (int i = 0;i <= 1;i++)
            {
                DxPlus::Sprite::Draw(gimikkuIDs[0], { 520.0f+i*200,90.0f }, { 0.7,0.7 }, { 640,320 }, DxPlus::Deg2Rad * 48.0f+i*17.15f);

            }
            

        DxPlus::Primitive2D::DrawRect(
            { objectPosition.x + 550 ,objectPosition.y - 190 },
            { objectWidth, objectHeight }, GetColor(0, 80, 0),
            1.0f,
            { objectWidth * 0.5f, objectHeight * 0.5f },
            objectAngle - 0.64
        );
        DxPlus::Primitive2D::DrawRect(
            { objectPosition.x + 300 ,objectPosition.y - 190 },
            { objectWidth, objectHeight }, GetColor(0, 80, 0),
            1.0f,
            { objectWidth * 0.5f, objectHeight * 0.5f },
            objectAngle + 1.11
        );
        //円の周りの棒
        DxPlus::Primitive2D::DrawRect(
            { objectPosition1.x + 20,objectPosition1.y - 55 },
            { objectWidth1, objectHeight1 },
            GetColor(0, 80, 0),
            1.0f,
            { objectWidth1 * 0.5f, objectHeight1 * 0.5f },
            objectAngle1
        );
        DxPlus::Primitive2D::DrawRect(
            { objectPosition1.x + 20,objectPosition1.y + 55 },
            { objectWidth1, objectHeight1 },
            GetColor(0, 80, 0),
            1.0f,
            { objectWidth1 * 0.5f, objectHeight1 * 0.5f },
            objectAngle1
        );
        DxPlus::Primitive2D::DrawRect(
            { objectPosition1.x + 75,objectPosition1.y },
            { objectWidth1, objectHeight1 },
            GetColor(0, 80, 0),
            1.0f,
            { objectWidth1 * 0.5f, objectHeight1 * 0.5f },
            objectAngle1 + 1.55
        );
        //反射しない障害物
        DxPlus::Primitive2D::DrawCircle(ObjectPosition, ObjectRadius, GetColor(255, 255, 255));
        // ボール
        DxPlus::Primitive2D::DrawCircle(shotBallPosition, shotBallRadius, GetColor(0, 0, 255));
        // 得点
        if (PointType1 == 1)
        {
            DxPlus::Primitive2D::DrawCircle(PointPosition, PointRadius, GetColor(255, 0, 0));

        }
        else
        {
            DxPlus::Primitive2D::DrawCircle(PointPosition, PointRadius, GetColor(0, 0, 0));

        }

        // 得点２
        if (PointType2 == 1)
            DxPlus::Primitive2D::DrawCircle(PointPosition2, PointRadius, GetColor(255, 0, 0));
        else
            DxPlus::Primitive2D::DrawCircle(PointPosition2, PointRadius, GetColor(0, 0, 0));

        DxPlus::Primitive2D::DrawCircle(HeartPosition, HeartRadius, GetColor(0, 255, 0));
        if (Heart)
        {
            DxPlus::Sprite::Draw(gimikkuIDs[1], { 930,230 }, { 0.5,0.5 });
            DxPlus::Sprite::Draw(gimikkuIDs[4], { 520,30 }, { 0.5,0.5 });
            DxPlus::Primitive2D::DrawCircle({HeartObjectPosition.x,HeartObjectPosition.y}, HeartRadius, GetColor(255, 0, 0));
        }
        else
        {
            DxPlus::Sprite::Draw(gimikkuIDs[2], { 930,200 }, { 0.5,0.5 });
            DxPlus::Sprite::Draw(gimikkuIDs[4], { 520,30 }, { 0.5,0.5 });
            DxPlus::Primitive2D::DrawCircle({ HeartObjectPosition.x,HeartObjectPosition.y }, HeartRadius, GetColor(0, 0, 0));

        }
        // 時間の描画
        std::wstring Time = std::to_wstring(timeLimit) + L"";
        DxPlus::Text::DrawString(Time.c_str(), 200, 650, GetColor(255, 255, 255));
        // 得点の描画
        DxPlus::Vec2 girlPointPosition = { 1100,530 };
        DxPlus::Vec2 girlPointScale = { 0.4,0.4 };
        DxPlus::Sprite::Draw(girlPointID, girlPointPosition, girlPointScale);
        DxPlus::Sprite::Draw(heartID, { 920,580 });
        std::wstring scoreText = std::to_wstring(heartPoint) + L"%";
        DxPlus::Text::DrawString(scoreText.c_str(), 970, 650, GetColor(255, 255, 255));
        static float angle = 0.0f;
        static float posY = 0.0f;
        static float posX = 0.0f;
        if (DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_RIGHT)
        {
            angle = 0.52;
            posY = 5;
            posX = -10;
        }
        else
        {
            angle = 0.15;
            posY = 0;
            posX = 0;
        }
        DxPlus::Sprite::Draw(sloopID, { 775+posX, 685 + posY }, { 0.5f, 0.5f }, { 228, 108 }, angle);
        static float angle2 = 0.0f;
        static float posY2 = 0.0f;
        if (DxPlus::Input::GetButton(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_LEFT)
        {
            angle2 = -0.5;
            posY2 = 5;
        }
        else
        {
            angle2 = -0.15;
            posY2 = 0;
        }
        DxPlus::Sprite::Draw(sloop2ID, { 500, 632+ posY2 }, { 0.5f, 0.5f }, { 1, 1 }, angle2);

        // 玉を描画
        DxPlus::Primitive2D::DrawCircle(ballPosition, ballRadius, GetColor(0, 0, 0));

              if (gameFadeTimer > 0.0f)
   {
       // α値を算出（0～255）
       int alpha = (int)(255 * gameFadeTimer);
       if (alpha > 255) alpha = 255;

       // --- 描画をすべて隠す（黒フェード） ---
       DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
       DxPlus::Primitive2D::DrawRect(
           { 0, 0 },
           { DxPlus::CLIENT_WIDTH, DxPlus::CLIENT_HEIGHT },
           DxLib::GetColor(0, 0, 0)
       );
       DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
   }

   // --- 完全にフェードアウトしたら描画しない ---
   if (gameFadeTimer >= 1.0f)
   {
       DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
       return; // ← 全ての描画をスキップ
   }
    }
    break;
    }
    break;
    case GameEnd_A:
    {
        DxPlus::Sprite::Draw(EndIDs[1]);
    }
    break;
    case GameEnd_B:
    {
        DxPlus::Sprite::Draw(EndIDs[2]);
    }
    break;
    case GameEnd_C:
    {
        DxPlus::Sprite::Draw(EndIDs[3]);
    }
    break;
    case GameEnd_D:
    {
        DxPlus::Sprite::Draw(EndIDs[0]);
    }
    break;
    case GameClear:
    {
        DxPlus::Sprite::Draw(EndIDs[4]);
    }
    break;
    case GameEnd_E:
    {
        DxPlus::Sprite::Draw(polisID, polisPosition,{0.4,0.4});
        if (polisPosition.x < 0)
        {
            DxPlus::Sprite::Draw(EndIDs[5]);
        }
    }
    break;
    case GameOver:
    {
        DxPlus::Sprite::Draw(EndIDs[5]);
        GameText_Render();
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






