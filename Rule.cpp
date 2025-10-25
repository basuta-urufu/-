#include "Rule.h"
#include"DxPlus/DxPlus.h"
#include"WinMain.h"
#include"Title.h"
//-------------------------------------------------------------------
//変数
//-------------------------------------------------------------------
// 画像
int Rule1BackID;
int Rule2BackID;
int Rule3BackID;
// 画像の切り替え
int RulePos = 0;
// シーンの切り替え
extern int nextScene;
extern int titleState;
void Rule_Init()
{
	Rule1BackID=DxPlus::Sprite::Load(L"./Data/Images/Character_JiraichanGameEnd5099_Image.png");
	Rule2BackID=DxPlus::Sprite::Load(L"./Data/Images/Jiraichan_GameClear_Rough.png");
	Rule3BackID= DxPlus::Sprite::Load(L"./Data/Images/back.png");
}

void Rule_Reset()
{
	RulePos = 0;
}

void Rule_Update()
{
	if (DxPlus::Input::GetButtonDown(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_RIGHT)
	{
		RulePos++;
		if (RulePos > 2)
		{
			RulePos = 2;
		}
	}
	if (DxPlus::Input::GetButtonDown(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_LEFT)
	{
		RulePos--;
		if (RulePos < 0)
		{
			RulePos = 0;
		}
	}
	if (DxPlus::Input::GetButtonDown(DxPlus::Input::PLAYER1) & DxPlus::Input::BUTTON_START)
	{
		nextScene = SceneTitle;
		Rule_Reset();
	}


}

void Rule_Render()
{
	if (RulePos == 0)
	{
		DxPlus::Sprite::Draw(Rule1BackID);
	}
	else if (RulePos == 1)
	{
		DxPlus::Sprite::Draw(Rule2BackID);
	}
	else
	{
		DxPlus::Sprite::Draw(Rule3BackID);
	}
		
}

void Rule_End()
{
}
