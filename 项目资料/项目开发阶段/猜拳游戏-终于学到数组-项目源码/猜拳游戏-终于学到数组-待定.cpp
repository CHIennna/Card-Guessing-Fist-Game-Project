#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <tchar.h>
#include <conio.h>
#include <easyx.h>
#include <graphics.h>
#include <windows.h>
#include <Mmsystem.h>
#pragma comment(lib,"winmm.lib")

// -------------------- 数据设计 --------------------
// 卡牌结构体
struct Card {
	int type;             // 0=攻击,1=防御
	int value;            // 效果数值（卡牌的攻击/防御卡牌)
	bool isCard;          // true 在牌堆里面， false就不在
};

// 玩家结构体
struct Player {
	int hp;               // 当前血量
	int attack;           // 基础攻击力
	struct Card hand[10]; // 手牌数组
	int handCount;          // 手牌数量
};

// 人机结构体
struct Robot {
	int hp;               // 当前血量
	int attack;           // 固定攻击力
	int difficulty;       // 难度（0简单、1中等、2困难）
};
//小组成员结构体
struct TeamMember {
	LPCTSTR name;
	LPCTSTR role;
};
TeamMember members[10] = {
	{_T("阿飘"),   _T("发牌，展示当前手牌")},
	{_T("阿通"),   _T("结束界面,数据统筹")},
	{_T("Itachi"), _T("初始玩家与人机")},
	{_T("李叫"),   _T("玩家方出牌，打印游戏场景/状态")},
	{_T("衔鱼"),   _T("生成猜拳结果，游戏主体循环")},
	{_T("小天"),   _T("初始化牌堆，难度选择")},
	{_T("小学长"), _T("架构搭建，幕后完善")},
	{_T("小西瓜"), _T("菜单，团队介绍")},
	{_T("小元"),   _T("战斗结果及后续")},
	{_T("小占"),   _T("此次未参加")}
};

// -------------------- 全局变量 --------------------

struct Player player;      // 玩家
struct Robot enemy;        // 人机
struct Card cards[25];      // 牌堆
int MaxHp;				// 最大血量
const int cardsMax = 7;     // 卡牌最大数量
int cardNum = 16;    // 牌堆数量
int curRounds = 0;  // 当前回合数
const int maxRounds = 10;       // 最大回合数（可自定义）
int difficulty = 1;         // 难度（默认为1）
int soundEffect = 1;       // 音效开关（1开，0关）
int volume = 50;           // 音量大小（0-100）


// -------------------- 数据设计 --------------------


// -------------------- service --------------------
/*
	负责人：小天
	功能：初始化牌堆，并且打乱
	参数：void
	返回值：void
*/
void initCards();

/*
	负责人：itachi
	功能：初始化玩家与人机
	参数：void
	返回值：void
*/
void initPlayer();

/*
	负责人：小天
	功能：难度选择
	参数：void
	返回值：void
*/
void difficultyChoose();

/*
	负责人：阿飘
	功能：从牌堆里面取二张牌发放到玩家手里
	参数：void
	返回值：void
*/
void dealCards();

/*
	负责人：李叫
	功能：玩家方出牌
	参数：
		cardIndex：卡牌下标
	返回值：void
*/
void playerPlayCard(int cardIndex);


/*
	负责人：李叫
	功能：玩家方弃牌
	参数：
		cardIndex：卡牌下标
	返回值：void
*/
void disCard();

/*
	负责人：衔鱼
	功能：生成猜拳结果
	参数：void
	返回值： 0 石头  1 剪刀  2 布
*/
int guessFist();

/*
	负责人：小元
	功能：猜拳战斗的结果及后续处理
	参数：
		playerChoice：玩家的猜拳结果
		enemyChoice：敌人的猜拳结果
	返回值：void
*/
void handleFightResult(int playerChoice, int enemyChoice);

// -------------------- service --------------------
void drawAlpha(int  picture_x, int picture_y, IMAGE* picture);

// -------------------- view --------------------

/*
	负责人：阿飘
	功能：展示玩家当前手牌
	参数：
		player: 玩家
	返回值：void
*/
void printHandCards();

/*
	负责人：李叫
	功能：打印游戏场景/状态
	参数：
		player: 玩家
	返回值：void
*/
void printGameScene();

/*
	负责人：阿通
	功能：游戏结束界面及结果展示
	参数：void
	返回值：void
*/
void gameOverView();

/*
	负责人：小西瓜
	功能：游戏主菜单
	参数：void
	返回值：void
*/
void menuView();

/*
	负责人：小西瓜
	功能：团队介绍
	参数：void
	返回值：void
*/
void introduceTeam();

/*
	负责人：
	功能：音效和音量设置
	参数：void
	返回值：void
*/
void setting();

/*
	负责人：衔鱼
	功能：游戏的主体循环
	参数：void
	返回值：void
*/
void gameView();

void playBGM(const char* music, bool repeat, int volume = -1);
// -------------------- view --------------------

int main()
{
	mciSendString(_T("open music\\bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("play bgm repeat"), NULL, 0, NULL);
	mciSendString(_T("setaudio bgm volume to 200"), NULL, 0, NULL);
	menuView();
	return 0;
}

void initCards() {
	int index = 0;

	// 创建攻击牌（类型0）
	// 5点攻击牌 * 5
	for (int i = 0; i < 5; i++) {
		cards[index].type = 0;
		cards[index].value = 5;
		cards[index].isCard = true;
		index++;
	}
	// 10点攻击牌 * 2
	for (int i = 0; i < 2; i++) {
		cards[index].type = 0;
		cards[index].value = 10;
		cards[index].isCard = true;
		index++;
	}
	// 20点攻击牌 * 1
	cards[index].type = 0;
	cards[index].value = 20;
	cards[index].isCard = true;
	index++;

	// 创建防御牌（类型1）
	// 3点防御牌 * 5
	for (int i = 0; i < 5; i++) {
		cards[index].type = 1;
		cards[index].value = 3;
		cards[index].isCard = true;
		index++;
	}
	// 5点防御牌 * 2
	for (int i = 0; i < 2; i++) {
		cards[index].type = 1;
		cards[index].value = 5;
		cards[index].isCard = true;
		index++;
	}
	// 10点防御牌 * 1
	cards[index].type = 1;
	cards[index].value = 10;
	cards[index].isCard = true;
	index++;

	// 设置牌堆数量
	cardNum = 16;

	// 洗牌
	for (int i = 0; i < 100; i++) {
		int a = rand() % cardNum;
		int b = rand() % cardNum;
		struct Card temp = cards[a];
		cards[a] = cards[b];
		cards[b] = temp;
	}
}
void initPlayer() {
	player.hp = 50;          // 玩家初始血量
	player.attack = 10;       // 玩家初始攻击力
	player.handCount = 0;     // 玩家手牌数量初始化为0
	// 初始化玩家手牌为空
	for (int i = 0; i < 10; i++) {
		player.hand[i].type = -1; // -1表示无效卡牌
		player.hand[i].value = 0;
		player.hand[i].isCard = false; // 初始状态为不在牌堆中
	}
	// 初始化敌人属性
	enemy.difficulty = 1;      // 默认难度为中等
	if (enemy.difficulty == 0) {
		enemy.hp = 50;
	}
	else if (enemy.difficulty == 1) {
		enemy.hp = 75;
	}
	else if (enemy.difficulty == 2) {
		enemy.hp = 100;
	}
	MaxHp = enemy.hp;
	enemy.attack = 5;         // 敌人初始攻击力
}

void dealCards()
{
	int x = 0, y = 0;
	srand(time(0));

	//抽第一张牌，如果卡牌已被抽走则继续抽
	x = rand() % 16;
	while (cards[x].isCard == false)
	{
		x = rand() % 16;
	}

	//卡牌进入玩家牌库并标记，卡牌数加一
	player.hand[player.handCount] = cards[x];
	cards[x].isCard = false;
	player.handCount++;

	//同理抽第二张
	y = rand() % 16;
	while (cards[y].isCard == false)
	{
		y = rand() % 16;
	}
	player.hand[player.handCount] = cards[y];
	player.handCount++;
	cards[y].isCard = false;
}

void playerPlayCard(int cardIndex) {

	//记录牌的效果
	if (player.hand[cardIndex].type) {
		enemy.attack -= player.hand[cardIndex].value;
		if (enemy.attack < 0)
		{
			enemy.attack = 0;
		}
	}
	else {
		player.attack += player.hand[cardIndex].value;
	}
	for (int k = cardIndex; k < 7; k++) {
		player.hand[k] = player.hand[k + 1];//出的牌被下一张覆盖，后面的牌依次迁移

	}
	player.handCount--;
}

void disCard()
{
	void printHandCards();
	int discardCount = player.handCount - 5;  // 需要弃牌的数量
	if (discardCount <= 0) return;  // 不需要弃牌时直接返回

	settextstyle(40, 0, _T("微软雅黑"));
	settextcolor(BLACK);
	outtextxy(700, 550, _T("请弃牌，回合结束时手牌数不得超过五张"));
	printHandCards();//打印卡牌

	int cnt = 0;  // 已弃牌数

	while (cnt < discardCount)
	{
		MOUSEMSG m;  // 声明鼠标消息变量
		if (MouseHit())
		{
			m = GetMouseMsg();
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				int selectedCard = -1;  // 初始化为无效值

				// 查找选中的卡牌
				for (int i = 0; i < player.handCount; i++)
				{
					if (m.x >= 80 + i * 260 && m.x <= 260 + i * 260 &&
						m.y >= 640 && m.y <= 880)
					{
						selectedCard = i;
						break;
					}
				}

				if (selectedCard != -1)  // 确保选中了有效的卡牌
				{
					// 移动剩余卡牌填补空缺
					for (int k = selectedCard; k < player.handCount - 1; k++)
					{
						player.hand[k] = player.hand[k + 1];
					}

					// 更新手牌数量和弃牌计数
					player.handCount--;
					cnt++;
					printHandCards();//打印卡牌

					// 刷新界面显示（需要实现）
					// refreshHandDisplay();
				}
			}
		}
		//Sleep(50);
	}
}


int guessFist()
{
	return rand() % 3;
}

void handleFightResult(int playerChoice, int enemyChoice)
{
	cleardevice();
	printGameScene();//猜拳选择界面


	// 绘制选项按钮并处理鼠标交互
	settextstyle(50, 0, _T("华文琥珀"));
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	int wSpace = (450 - textwidth(_T("点击选择猜拳类型"))) / 2;
	int hSpace = (300 - textheight(_T("点击选择猜拳类型"))) / 2;
	outtextxy(725 + wSpace, 250 + hSpace, _T("点击选择猜拳类型"));
	IMAGE img;
	loadimage(&img, L"image\\arrow.png", 116, 76);
	drawAlpha(380, 440, &img);
	drawAlpha(380, 315, &img);
	drawAlpha(380, 190, &img);


	// 等待玩家选择猜拳
	int state = 0;
	MOUSEMSG m;

	while (state != 2)
	{
		if (state == 0)
		{
			if (MouseHit())
			{
				m = GetMouseMsg();
				// 处理鼠标点击
				if (m.uMsg == WM_LBUTTONDOWN)
				{
					if (m.x >= 500 && m.x <= 600 && m.y >= 325 && m.y <= 425)
						playerChoice = 0;  // 石头
					else if (m.x >= 500 && m.x <= 600 && m.y >= 450 && m.y <= 550)
						playerChoice = 1;  // 剪刀
					else if (m.x >= 500 && m.x <= 600 && m.y >= 200 && m.y <= 300)
						playerChoice = 2;  // 布
					else
					{
						playerChoice = -1;
					}
					if (playerChoice != -1)
					{
						enemyChoice = guessFist();
						state = 1;
					}
				}
			}

		}
		else if (state == 1)
		{
			
			cleardevice();
			printGameScene();
			setbkmode(TRANSPARENT);
			settextstyle(25, 0, _T("宋体"));

			wchar_t playerChoiceText[50], enemyChoiceText[50];
			swprintf_s(playerChoiceText, _T("你选择了: %s"),
				playerChoice == 0 ? _T("石头") : (playerChoice == 1 ? _T("剪刀") : _T("布")));
			swprintf_s(enemyChoiceText, _T("电脑选择了: %s"),
				enemyChoice == 0 ? _T("石头") : (enemyChoice == 1 ? _T("剪刀") : _T("布")));

			outtextxy(765, 280, playerChoiceText);
			outtextxy(765, 320, enemyChoiceText);
			//mciSendString(_T("open music\\叮.mp3 alias bgm6"), NULL, 0, NULL);
			//mciSendString(_T("play bgm6 "), NULL, 0, NULL);
			//mciSendString(_T("setaudio bgm6 volume to 100"), NULL, 0, NULL);

			// 战斗结果
			int result;
			if (playerChoice == enemyChoice)
			{
				result = 0; // 平局
			}
			else if ((playerChoice == 0 && enemyChoice == 1) ||
				(playerChoice == 1 && enemyChoice == 2) ||
				(playerChoice == 2 && enemyChoice == 0))
			{
				result = 1; // 玩家赢
			}
			else {
				result = -1; // 玩家输
			}
			wchar_t resultText[50];
			settextstyle(30, 0, _T("黑体"));
			settextcolor(RED);

			if (result == 1)
			{
				swprintf_s(resultText, _T("你赢了这一回合！"));
				outtextxy(765, 360, resultText);
			}
			else if (result == 0)
			{
				swprintf_s(resultText, _T("平局！"));
				outtextxy(765, 360, resultText);
			}
			else
			{
				swprintf_s(resultText, _T("你输了这一回合！"));
				outtextxy(765, 360, resultText);
			}

			// 伤害计算与显示
			int damage1 = 0;//对敌方伤害
			int damage2 = 0;//对玩家伤害

			if (result == 1) {
				damage1 = player.attack/*加卡牌数值，等卡牌完成后补充*/;
				enemy.hp -= damage1;
				if (enemy.hp < 0) enemy.hp = 0;
			}
			else if (result == -1) {
				damage2 = enemy.attack/*防御牌数值，等卡牌完成后补充*/;
				player.hp -= damage2;
				if (player.hp < 0) player.hp = 0;
			}
			else
			{
				enemy.hp -= 0;
				player.hp -= 0;
			}

			wchar_t damageText[100];
			settextstyle(30, 0, _T("宋体"));

			if (damage1 > 0)
			{
				swprintf_s(damageText, _T("敌人受到 %d 点伤害！"), damage1);
				outtextxy(765, 400, damageText);
			}
			if (damage2 > 0)
			{
				swprintf_s(damageText, _T("你受到 %d 点伤害！"), damage2);
				outtextxy(765, 400, damageText);
			}
			outtextxy(765, 440, _T("按任意键继续..."));
			ExMessage confirm;
			while (true)
			{
				if (peekmessage(&confirm, EM_KEY))
				{
					if (confirm.message == WM_KEYDOWN)
						break;
				}
			}
			state = 2;
		}
	}
	
	//PlaySound(_T("music\\叮.mp3"), NULL,SND_ASYNC|SND_FILENAME);
}
// 计算猜拳结果
// 玩家胜利的情况
// 敌人胜利的情况
/*void printHandCards()
{
	const int MAX_HAND = 7;  // 最大手牌数
	IMAGE card[MAX_HAND];     // 图片数组

	// 计算动态间距 (屏幕宽度1900，左右边距各80)
	int spacing = (1900 - 160);
	if (player.handCount > 1) {
		spacing /= (player.handCount - 1);
	}

	for (int i = 0; i < player.handCount; i++)
	{
		// 载入卡牌图片
		if (player.hand[i].type == 0) // 攻击卡
		{
			switch (player.hand[i].value)
			{
			case 5:
				loadimage(card + i, _T("image\\attack5.png")); // 实际路径
				break;
			case 10:
				loadimage(card + i, _T("image\\attack10.png"));
				break;
			case 20:
				loadimage(card + i, _T("image\\attack20.png"));
				break;
			default: // 处理意外值
				loadimage(card + i, _T("image\\card_default.png"));
			}
		}
		else // 防御卡
		{
			switch (player.hand[i].value)
			{
			case 5:
				loadimage(card + i, _T("image\\defence5.png"));
				break;
			case 10:
				loadimage(card + i, _T("image\\defence10.png"));
				break;
			case 20:
				loadimage(card + i, _T("image\\defence20.png"));
				break;
			default: // 处理意外值
				loadimage(card + i, _T("image\\card_default.png"));
			}
		}

		// 打印卡牌（动态计算x位置）
		putimage(80 + spacing * i, 640, card + i);
	}
}*/

void printHandCards()
{
	const int MAX_HAND = 7;  // 最大手牌数
	IMAGE card[MAX_HAND];     // 图片数组

	for (int i = 0; i < player.handCount; i++)
	{
		// 载入卡牌图片
		if (player.hand[i].type == 0) // 攻击卡
		{
			switch (player.hand[i].value)
			{
			case 5:
				loadimage(card + i, _T("image\\attack5.png")); // 实际路径
				break;
			case 10:
				loadimage(card + i, _T("image\\attack10.png"));
				break;
			case 20:
				loadimage(card + i, _T("image\\attack20.png"));
				break;
			default: // 处理意外值
				loadimage(card + i, _T("image\\card_default.png"));
			}
		}
		else // 防御卡
		{
			switch (player.hand[i].value)
			{
			case 5:
				loadimage(card + i, _T("image\\defence5.png"));
				break;
			case 10:
				loadimage(card + i, _T("image\\defence10.png"));
				break;
			case 3:
				loadimage(card + i, _T("image\\defence3.png"));
				break;
			default: // 处理意外值
				loadimage(card + i, _T("image\\card_default.png"));
			}
		}

		//int x = 80 + i * (CARD_WIDTH + spacing);
		// 打印卡牌（动态计算x位置）
		putimage(i * 260 + 80, 640, card + i);
	}
}

void printGameScene() {
	setbkcolor(0xFFFFFF);//img后的数字根据坐标表对应行数而定
	cleardevice();//清屏

	//场景绘图
	IMAGE img1;
	loadimage(&img1, L"image\\background.png", 1900, 900);
	putimage(0, 0, &img1);
	setfillcolor(RGB(245, 240, 220));
	fillroundrect(725, 250, 450 + 725, 300 + 250, 20, 20);
	setfillcolor(BLACK);
	fillrectangle(0, 600, 1900, 610);


	IMAGE img2;//设置玩家头像
	loadimage(&img2, L"image\\player.png", 160, 160);
	drawAlpha(0, 0, &img2);
	IMAGE img3;//设置鸽子头像
	loadimage(&img3, L"image\\pigeon1.png", 125, 130);
	drawAlpha(1750, 15, &img3);
	IMAGE img6;//设置设置键
	loadimage(&img6, L"image\\settings.png", 114, 114);
	drawAlpha(893, 4, &img6);
	IMAGE img7;//设置对方实体
	loadimage(&img7, L"image\\pigeoncute.png", 205, 200);
	drawAlpha(1530, 300, &img7);
	IMAGE img8;//设置玩家实体
	loadimage(&img8, L"image\\player.png", 260, 260);
	drawAlpha(114, 270, &img8);
	IMAGE img9;//设置玩家石头
	loadimage(&img9, L"image\\playerrock.png", 100, 100);
	drawAlpha(500, 325, &img9);
	IMAGE img10;//设置对方石头
	loadimage(&img10, L"image\\robotrock.png", 100, 100);
	drawAlpha(1290, 325, &img10);
	IMAGE img11;//设置玩家剪刀
	loadimage(&img11, L"image\\playerscissors.png", 100, 100);
	drawAlpha(500, 450, &img11);
	IMAGE img12;//设置对方剪刀
	loadimage(&img12, L"image\\robotscissors.png", 100, 100);
	drawAlpha(1290, 450, &img12);
	IMAGE img13;//设置玩家布
	loadimage(&img13, L"image\\playerpaper.png", 100, 100);
	drawAlpha(500, 200, &img13);
	IMAGE img14;//设置对方布
	loadimage(&img14, L"image\\robotpaper.png", 100, 100);
	drawAlpha(1290, 200, &img14);
	//设置玩家血条
	setfillcolor(0x4f07db);
	fillrectangle(170, 35, 170 + 375 * (player.hp) / 100, 70);

	fillrectangle(1715, 35, 1715 - 375 * (enemy.hp) / MaxHp, 70);
	setfillcolor(0x4f07db);
	//设置状态文本
	wchar_t playerHP[100] = L"";
	wchar_t playerAt[100] = L"";
	wchar_t enemyHP[100] = L"";
	wchar_t enemyAt[100] = L"";
	wsprintf(playerHP, L"血量：%d", player.hp);
	wsprintf(playerAt, L"攻击力：%d", player.attack);
	wsprintf(enemyHP, L"血量：%d", enemy.hp);
	wsprintf(enemyAt, L"攻击力：%d", enemy.attack);
	settextcolor(0x000000);
	settextstyle(25, 0, L"黑体");
	outtextxy(170, 75, playerHP);
	outtextxy(170, 100, playerAt);
	outtextxy(1600, 75, enemyHP);
	outtextxy(1600, 100, enemyAt);
	//设置回合数展示
	wchar_t Rounds[100] = L"";
	wsprintf(Rounds, L"回合数：%d", curRounds);
	settextstyle(60, 0, L"华文彩云");
	setbkmode(TRANSPARENT);
	outtextxy(810, 140, Rounds);
};


void gameOverView()
{
	/* ==================== 界面初始化 ==================== */
	cleardevice();               // 清除当前屏幕内容
	setbkcolor(WHITE);           // 设置背景颜色为白色
	cleardevice();               // 再次清除屏幕应用背景色

	/* ==================== 音效处理 ==================== */
	if (soundEffect) {
		// 播放游戏结束音效（需要res/gameover.wav文件）
		mciSendString(_T("play res/gameover.wav"), NULL, 0, NULL);
	}

	/* ==================== 显示游戏结束标题 ==================== */
	settextstyle(80, 0, _T("黑体"));  // 设置标题字体：80号黑体
	settextcolor(BLACK);               // 设置标题颜色为黑色
	outtextxy(700, 100, _T("游戏结束")); // 在坐标(700,100)显示标题

	/* ==================== 胜负判定逻辑 ==================== */
	const TCHAR* result = NULL;   // 结果文本指针
	COLORREF resultColor;         // 结果文本颜色

	// 根据游戏状态确定显示内容
	if (player.hp <= 0) {
		// 情况1: 玩家血量耗尽 - 失败
		result = _T("你失败了！");
		resultColor = RED;        // 红色表示失败
		mciSendString(_T("open music\\失败.mp3 alias bgm1"), NULL, 0, NULL);
		mciSendString(_T("play bgm1 "), NULL, 0, NULL);
		mciSendString(_T("setaudio bgm1 volume to 300"), NULL, 0, NULL);
	}
	else if (enemy.hp <= 0) {
		// 情况2: 敌人血量耗尽 - 胜利
		result = _T("你胜利了！");
		resultColor = GREEN;      // 绿色表示胜利
		mciSendString(_T("open music\\胜利.mp3 alias bgm2"), NULL, 0, NULL);
		mciSendString(_T("play bgm2 "), NULL, 0, NULL);
		mciSendString(_T("setaudio bgm2 volume to 300"), NULL, 0, NULL);
	}
	else if (curRounds >= maxRounds) {
		// 情况3: 回合数耗尽 - 平局（根据血量判断优劣）
		if (player.hp > enemy.hp) {
			result = _T("你的血量更高！胜利！");
			mciSendString(_T("open music\\胜利.mp3 alias bgm3"), NULL, 0, NULL);
			mciSendString(_T("play bgm3 "), NULL, 0, NULL);
			mciSendString(_T("setaudio bgm3 volume to 300"), NULL, 0, NULL);
		}
		else if (player.hp < enemy.hp) {
			result = _T("敌人血量更高！失败......");
			mciSendString(_T("open music\\失败.mp3 alias bgm4"), NULL, 0, NULL);
			mciSendString(_T("play bgm4 "), NULL, 0, NULL);
			mciSendString(_T("setaudio bgm volume to 300"), NULL, 0, NULL);
		}
		else {
			result = _T("完美平局！");
			mciSendString(_T("open music\\叮.mp3 alias bgm5"), NULL, 0, NULL);
			mciSendString(_T("play bgm5 "), NULL, 0, NULL);
			mciSendString(_T("setaudio bgm5 volume to 300"), NULL, 0, NULL);
		}
		resultColor = BLUE;     // 黄色表示平局
	}

	/* ==================== 显示胜负结果 ==================== */
	settextstyle(40, 0, _T("楷体"));  // 设置结果字体：40号楷体
	settextcolor(resultColor);        // 应用前面确定的颜色
	outtextxy(750, 230, result);      // 在坐标(750,230)显示结果

	/* ==================== 游戏统计信息显示 ==================== */
	settextstyle(30, 0, _T("宋体"));  // 设置统计信息字体：30号宋体
	settextcolor(BLACK);               // 黑色文本

	TCHAR statsText[100];  // 用于格式化的文本缓冲区

	// 回合数统计
	_stprintf(statsText, _T("回合数: %d/%d"), curRounds, maxRounds);
	outtextxy(750, 300, statsText);  // 在坐标(750,300)显示

	// 玩家血量统计
	_stprintf(statsText, _T("玩家剩余血量: %d"), player.hp);
	outtextxy(750, 350, statsText);  // 在坐标(750,350)显示

	// 敌人血量统计
	_stprintf(statsText, _T("敌人剩余血量: %d"), enemy.hp);
	outtextxy(750, 400, statsText);  // 在坐标(750,400)显示

	// 游戏难度显示（使用三元运算符转换数字为文本）
	_stprintf(statsText, _T("游戏难度: %s"),
		difficulty == 0 ? _T("简单") :
		(difficulty == 1 ? _T("中等") : _T("困难")));
	outtextxy(750, 450, statsText);  // 在坐标(750,450)显示

	// 返回菜单提示
	_stprintf(statsText, _T("按空格键返回菜单"));
	outtextxy(750, 500, statsText);
	settextstyle(25, 0, _T("宋体"));  // 设置提示字体：25号宋体
	outtextxy(720, 550, _T("按 R 键重新开始游戏"));  // 重新开始提示
	outtextxy(720, 600, _T("按 空格 键返回主菜单"));   // 返回菜单提示

	/* ==================== 用户输入处理循环 ==================== */
	/*while (1) {  // 无限循环直到有效输入
		if (_kbhit()) {  // 检测键盘输入
			int key = _getch();  // 获取按键

			// 处理R键：重新开始游戏
			if (key == 'r' || key == 'R') {
				// 重置游戏状态
				player.hp = 100;  // 恢复玩家血量

				// 根据难度设置敌人血量
				enemy.hp = difficulty == 0 ? 150 :
					(difficulty == 1 ? 250 : 300);

				curRounds = 0;    // 重置回合数
				initCards();      // 重新初始化牌堆
				return;           // 退出函数，返回游戏主循环
			}
			// 处理ESC键：返回主菜单
			else if (key == 27) {
				closegraph();     // 关闭图形窗口
				menuView();       // 调用主菜单视图函数
				return;           // 退出函数
			}
		}
		Sleep(50);  // 短暂休眠（50毫秒），降低CPU占用率
	}*/


	ExMessage msg = { 0 };
	while (1)
	{
		getmessage(&msg, EX_KEY);

		if (msg.message == WM_KEYDOWN && msg.vkcode == VK_SPACE)
		{
			//closegraph();
			menuView();
			return;
		}
		else if (msg.vkcode == 'R' || msg.vkcode == 'r')
		{
			gameView();
			return;
		}
	}
}

void menuView() {
	// 主菜单界面
	// 包括开始游戏、设置、团队介绍等选项
	//设置背景颜色为白色
	initgraph(1900, 900);
	setbkcolor(WHITE);
	cleardevice();
	//设置字体格式
	settextstyle(80, 0, _T("宋体"));
	settextcolor(BLACK);
	outtextxy(700, 100, _T("猜 拳 游 戏"));
	//设置一张小鸽子图片
	//loadimage(NULL, L"image\\pigeoncute.png", 300, 300);
	//IMAGE img;
	//putimage(1550, 550, &img);
	//设置菜单
	outtextxy(300, 300, _T("开始游戏"));
	outtextxy(300, 400, _T("团队介绍"));
	outtextxy(300, 500, _T("设置"));
	outtextxy(300, 600, _T("退出"));
	ExMessage msg;
	while (true)
	{
		if (peekmessage(&msg, EM_MOUSE))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				if (msg.x >= 300 && msg.x <= 500 && msg.y >= 300 && msg.y <= 380)
				{
					gameView();
				}
				else if (msg.x >= 300 && msg.x <= 500 && msg.y >= 400 && msg.y <= 480)
				{
					introduceTeam();
					cleardevice();
					settextstyle(80, 0, _T("宋体"));
					settextcolor(BLACK);
					outtextxy(700, 100, _T("猜 拳 游 戏"));
					//设置菜单
					outtextxy(300, 300, _T("开始游戏"));
					outtextxy(300, 400, _T("团队介绍"));
					outtextxy(300, 500, _T("设置"));
					outtextxy(300, 600, _T("退出"));
				}
				/*else if (msg.x >= 300 && msg.x <= 500 && msg.y >= 500 && msg.y <= 580)
				{
					setting();
				}*/
				else if (msg.x >= 300 && msg.x <= 500 && msg.y >= 600 && msg.y <= 680)
				{
					break;
				}
			}

		}
	}
	closegraph();  // 关闭图形窗口
}

void difficultyChoose() {

	setbkcolor(WHITE);
	cleardevice();
	settextstyle(80, 0, _T("黑体"));
	settextcolor(BLACK);
	outtextxy(400, 100, _T("\n========== 难度选择 ==========\n"));
	settextstyle(50, 0, _T("黑体"));
	settextcolor(BLACK);
	outtextxy(100, 200, _T("1. 简单：敌人生命值较低，攻击力较弱，适合新手玩家。\n"));
	outtextxy(100, 300, _T("2. 中等：敌人生命值和攻击力适中，适合有一定经验的玩家。\n"));
	outtextxy(100, 400, _T("3. 困难：敌人生命值高，攻击力强，适合寻求挑战的玩家。\n"));
	outtextxy(620, 500, _T("=============================\n"));
	outtextxy(300, 600, _T("请输入选项(按下1/2/3):"));
	outtextxy(300, 600, _T("请输入选项(按下1/2/3):"));
	// 显示当前选择（初始为空）
	TCHAR statusText[100] = { 0 };
	settextstyle(30, 0, _T("黑体"));
	outtextxy(300, 700, _T("当前选择：未选择"));

	bool flag = false;  // 标志变量，表示是否已选择难度

	while (1) {
		ExMessage msg;
		getmessage(&msg, EX_KEY);
		// 获取按键
		if (msg.message == WM_KEYDOWN  && msg.vkcode >=29 && msg.vkcode <=51 ) {
			int scan = msg.vkcode;  // 获取按键
			// 更新敌人属性
			//printf("按键值：%d\n", scan);  // 调试输出
			switch (scan) {
			case 49:  // 简单
				enemy.difficulty = 0;
				difficulty = 0;
				enemy.hp = 50;
				MaxHp = enemy.hp;
				enemy.attack = 5;
				_stprintf(statusText, _T("当前选择：简单（生命值 %d，攻击力 %d）"), enemy.hp, enemy.attack);
				flag = true;
				break;
			case 50:  // 中等
				enemy.difficulty = 1;
				difficulty = 1;
				enemy.hp = 75;
				MaxHp = enemy.hp;
				enemy.attack = 9;
				_stprintf(statusText, _T("当前选择：中等（生命值 %d，攻击力 %d）"), enemy.hp, enemy.attack);
				flag = true;
				break;
			case 51:  // 困难
				enemy.difficulty = 2;
				difficulty = 2;
				enemy.hp = 100;
				MaxHp = enemy.hp;
				enemy.attack = 10;
				_stprintf(statusText, _T("当前选择：困难（生命值 %d，攻击力 %d）"), enemy.hp, enemy.attack);
				flag = true;
				break;
			}break;
		}
	}
	// 刷新显示
	while (1) {
		cleardevice();
		outtextxy(600, 100, _T("========== 难度选择 =========="));
		outtextxy(600, 200, _T("1. 简单"));
		outtextxy(600, 300, _T("2. 中等"));
		outtextxy(600, 400, _T("3. 困难"));
		outtextxy(600, 500, _T("============================="));
		outtextxy(600, 600, _T("已选择难度！"));
		outtextxy(600, 700, statusText);  // 显示当前选择
		ExMessage msg;
		getmessage(&msg, EX_KEY);


		if (msg.message == WM_KEYDOWN)
			break;  // 退出循环，进入游戏主循环
	}
}


void introduceTeam()
{
	//设置背景为白色
	setbkcolor(WHITE);
	cleardevice();
	//设置字体格式
	settextstyle(80, 0, _T("黑体"));
	settextcolor(BLACK);
	outtextxy(700, 50, _T("团队介绍"));
	settextstyle(30, 0, _T("黑体"));
	outtextxy(700, 700, _T("按空格键返回"));
	for (int i = 0; i < 9; i++) {
		TCHAR info[100];
		_stprintf_s(info, _T("%s: %s"), members[i].name, members[i].role);
		outtextxy(700, 150 + i * 60, info);
	}

	ExMessage msg = { 0 };
	while (1)
	{
		if (peekmessage(&msg, EX_KEY))
		{
			if (msg.vkcode == VK_SPACE)
			{
				return;
			}
		}
	}
}

void drawAlpha(int  picture_x, int picture_y, IMAGE* picture) //x为载入图片的X坐标，y为Y坐标
{

	// 变量初始化
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}

void setting() {}

void gameView()
{
	cleardevice();
	curRounds = 0;
	// 初始化游戏数据
	initCards();
	initPlayer();
	difficultyChoose();
	int save = enemy.attack;
	// 游戏主循环
	while (curRounds < maxRounds && player.hp > 0 && enemy.hp > 0)
	{
		curRounds++;
		cleardevice();
		// 绘制游戏场景和回合信息
		player.attack = 10;
		enemy.attack = save;
		printGameScene();

		// 发牌和显示手牌
		if (curRounds < 9)
		{
			dealCards();
		}
		printHandCards();
		settextstyle(80, 0, _T("华文琥珀"));
		settextcolor(BLACK);
		setbkmode(TRANSPARENT);
		int wSpace = (450 - textwidth(_T("请出牌"))) / 2;
		int hSpace = (300 - textheight(_T("请出牌"))) / 2;
		outtextxy(725 + wSpace, 250 + hSpace, _T("请出牌"));

		// 手牌选择逻辑
		int selectedCard = -1;
		MOUSEMSG m;
		while (selectedCard == -1)
		{
			if (MouseHit())
			{
				m = GetMouseMsg();
				if (m.uMsg == WM_LBUTTONDOWN)
				{
					for (int i = 0; i < player.handCount; i++)
					{
						if (m.x >= 80 + i * 260 && m.x <= 260 + i * 260 &&
							m.y >= 640 && m.y <= 880) {
							selectedCard = i;
							break;
						}
					}
				}
			}
		}
		// 玩家出牌
		playerPlayCard(selectedCard);

		handleFightResult(0, 0);
		disCard();

	}
	// 游戏结束
	gameOverView();

}

