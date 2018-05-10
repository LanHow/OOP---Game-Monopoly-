/*
* mygame.h: 本檔案儲遊戲本身的class的interface
* Copyright (C) 2002-2008 Woei-Kae Chen <wkc@csie.ntut.edu.tw>
*
* This file is part of game, a free game development framework for windows.
*
* game is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* game is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*	 2004-03-02 V4.0
*      1. Add CGameStateInit, CGameStateRun, and CGameStateOver to
*         demonstrate the use of states.
*   2005-09-13
*      Rewrite the codes for CBall and CEraser.
*   2005-09-20 V4.2Beta1.
*   2005-09-29 V4.2Beta2.
*   2006-02-08 V4.2
*      1. Rename OnInitialUpdate() -> OnInit().
*      2. Replace AUDIO_CANYON as AUDIO_NTUT.
*      3. Add help bitmap to CGameStateRun.
*   2006-09-09 V4.3
*      1. Rename Move() and Show() as OnMove and OnShow() to emphasize that they are
*         event driven.
*   2008-02-15 V4.4
*      1. Add namespace game_framework.
*      2. Replace the demonstration of animation as a new bouncing ball.
*      3. Use ShowInitProgress(percent) to display loading progress.
*/

#include "CEraser.h"
#include "CBall.h"
#include "CBouncingBall.h"

namespace game_framework {
	/////////////////////////////////////////////////////////////////////////////
	// Constants
	/////////////////////////////////////////////////////////////////////////////

	enum AUDIO_ID {			// 定義各種音效的編號
		AUDIO_DING,			// 0
		AUDIO_LAKE,				// 1
		AUDIO_BGM,				// 2
		AUDIO_DICE,				// 3
		AUDIO_WAN,				// 4
		AUDIO_CHOOSE,		// 5
		AUDIO_BIU,				// 6
		AUDIO_WAN1,			// 7
		AUDIO_BUILD,			// 8
		AUDIO_SAD				// 9
	};

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的遊戲開頭畫面物件
	// 每個Member function的Implementation都要弄懂
	/////////////////////////////////////////////////////////////////////////////

	class CGameStateInit : public CGameState {
	public:
		CGameStateInit(CGame *g);
		void OnInit();  								// 遊戲的初值及圖形設定
		void OnBeginState();							// 設定每次重玩所需的變數
		void OnKeyUp(UINT, UINT, UINT); 				// 處理鍵盤Up的動作
		void OnLButtonDown(UINT nFlags, CPoint point);  // 處理滑鼠的動作

	protected:
		void OnShow();
		void OnMove();
		// 顯示這個狀態的遊戲畫面
	private:
		CMovingBitmap logo;								// csie的logo
		CAnimation  run;
		CMovingBitmap  s;
		CPoint  point;


	};

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的遊戲執行物件，主要的遊戲程式都在這裡
	// 每個Member function的Implementation都要弄懂
	/////////////////////////////////////////////////////////////////////////////

	struct charactor
	{
		int mapX;					//玩家與地圖左上角相對的X座標
		int mapY;					//玩家與地圖左上角相對的Y座標
		int X;						//玩家顯示的X座標
		int Y;						//玩家顯示的Y座標
		int position;				//玩家在地圖的哪格
		int step;					//玩家所要移動的步數
		int end;						//玩家移動完的終點
		int money;					//玩家的金錢
		bool ismoving;			//玩家是否在移動中
		int playernum;			//玩家的編號
		int inprison;				//玩家暫停的回合數
	};

	class Dice;
	class GameMap;

	class Land {
	public:
		Land(int b, int r, int p) : block(b), rent(r), price(p), belongsto(0), house(0) {};
		int   Rent() { return rent; };												//回傳土地過路費
		int   Price() { return price; };											//回傳土地價格
		int   Belongsto() { return belongsto; };								//回傳土地歸屬
		int   House() { return house; };											//回傳土地房子數量
		void Buy(int playernum) { belongsto = playernum; };			//購買土地
		void BuildHouse() { house++; };											//蓋房子
		void Init() { house = 0; belongsto = 0; };							//初始化
	protected:
		int block;									//格數
		int rent;									//過路費				
		int price;									//購買金額
		int belongsto;							//歸屬
		int house;									//房子數
	};

	class Buy {
	public:
		void Initial();							//初始化landarray
		void OnShow(charactor*, int);			//顯示購買視窗、事件視窗
		void SetTopLeft();							//設定各視窗座標
		void Buyland(charactor*);					//購買土地
		void AddBitmap();								//載入圖片
		void SetboolFalse();							//將running改為false
		void SetboolTrue();							//將running改為true
		bool ReturnBool();							//回傳running
		bool Decision();									//回傳decision
		void SetDecisionTrue();					//將decision改為true
		void SetDecisionFalse();					//將decision改為false
		bool Checkbelong(int p);					//回傳土地是否有人擁有
		void Paymoney(charactor*);				//扣錢
		void Getmoney(charactor*);				//加錢
		void SetBmap(GameMap&b);				//設定*bmap
		void Showbelong();							//顯示每塊地的歸屬
		void Showhouse();								//顯示每塊地的房子
		int findbelongx(int);							//計算歸屬圖片的X座標
		int findbelongy(int);							//計算歸屬圖片的Y座標
		int findhousex(int);							//計算房子圖片的X座標
		int findhousey(int);							//計算房子圖片的Y座標
		int Belongsto(int);							//回傳土地擁有人
		void Buildhouse(int);							//蓋房子
		void GB(charactor*);						//機會命運
		void setn(int i) { n = i; };					//設定n
		int houselevel(int);							//回傳房子數
	protected:						
		bool running;										//人物是否移動中
		bool decision;									//選項
		int n;												//機會命運事件編號
		CMovingBitmap Q,B,F,G,H,V;
		CMovingBitmap choice,belong, hot,fire,build;
		CMovingBitmap HH,SH,H1,H2,H3;
		GameMap * bmap;								//指到gamestate中的GameMap以使用地圖坐標
		Land L1 = Land(1, 25, 70);					//以下為設定每一格的過路費與價格
		Land L2 = Land(2, 0, 0);
		Land L3 = Land(3, 30, 80);
		Land L4 = Land(4, 200, 0);
		Land L5 = Land(5, 100, 250);
		Land L6 = Land(6, 35, 90);
		Land L7 = Land(7, 0, 0);
		Land L8 = Land(8, 35, 90);
		Land L9 = Land(9, 40, 110);
		Land L10 = Land(10, 0, 0);
		Land L11 = Land(11, 50, 130);
		Land L12 = Land(12, 70, 175);
		Land L13 = Land(13, 50, 130);
		Land L14 = Land(14, 60, 150);
		Land L15 = Land(15, 100, 250);
		Land L16 = Land(16, 65, 170);
		Land L17 = Land(17, 0, 0);
		Land L18 = Land(18, 65, 170);
		Land L19 = Land(19, 75, 190);
		Land L20 = Land(20, 70, 175);
		Land L21 = Land(21, 85, 210);
		Land L22 = Land(22, 0, 0);
		Land L23 = Land(23, 85, 210);
		Land L24 = Land(24, 90, 230);
		Land L25 = Land(25, 100, 250);
		Land L26 = Land(26, 100, 250);
		Land L27 = Land(27, 100, 250);
		Land L28 = Land(28, 70, 175);
		Land L29 = Land(29, 105, 270);
		Land L30 = Land(30, 0, 0);
		Land L31 = Land(31, 115, 290);
		Land L32 = Land(32, 115, 290);
		Land L33 = Land(33, 0, 0);
		Land L34 = Land(34, 70, 310);
		Land L35 = Land(35, 100, 250);
		Land L36 = Land(36, 150, 375);
		Land L37 = Land(37, 0, 0);
		Land L38 = Land(38, 75, 0);
		Land L39 = Land(39, 170, 425);
		Land L40 = Land(40, 0, -200);
		Land	landarray[40] = { L1,L2,L3,L4,L5,L6,L7,L8,L9,L10,L11,L12,				//將所有格子裝進array方便使用
											L13,L14,L15,L16,L17,L18,L19,L20,L21,L22,	
											L23,L24,L25,L26,L27,L28,L29,L30,L31,
											L32,L33,L34,L35,L36,L37,L38,L39,L40 };
		CMovingBitmap gb1, gb2, gb3, gb4, gb5, gb6, gb7, gb8, gb9, gb10, gb11,gb12, gb13, gb14;
		CMovingBitmap C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11;
	};

	class GameMap
	{
	public:
		GameMap();											//地圖模組
		void LoadBitmap();								//載入地圖
		void showmap(int p);								//顯示第p格
		void OnShow();										//顯示地圖
		void OnMove();										//控制地圖移動
		void Setmoving(bool flag);					//設定是否在移動的flag
		void Setp1(charactor & player1);			//設定*p1
		void Setdice(Dice & dd);						//設定*d
		void Setland(Buy & land);						//設定*l
		bool ifmoving();										//回傳地圖是否在移動中
		int RX();												//回傳地圖左上角的X座標
		int RY();												//回傳地圖左上角的Y座標
		void Setpnum(int & num);						//設定*pnum
		int findx(int p);									//回傳第p格與地圖左上角對應的X座標
		int findy(int p);										//回傳第p格與地圖左上角對應的Y座標
	protected:
		CMovingBitmap game_map;
		int map[13][13];									//地圖陣列
		int X, Y;												//地圖X,Y座標
		const int MW, MH;								//地圖的長寬
		bool ismoving;										//地圖是否在移動中的flag
		int count;												//計算OnMove中地圖移動的次數
		charactor * p1;										//指向gamestaterun中的charactor
		Dice * d;												//指向gamestaterun中的Dice
		Buy * l;													//指向gamestaterun中的Buy
		int * pnum;											//指向gamestaterun中的pnum
	};

	class Dice
	{
	public:
		int dice();						//骰子隨機生成1~6數字
		void AddBitmap();				//骰子動畫加入1~6的圖
		void Onshow();
		void OnMove();					
		void SetTopLeft();
		void SetboolFalse();			//設定角色靜止
		void SetboolTrue();				//設定角色正在移動
		bool RetrunBool();				//回傳角色是否在移動
		void SetNum(int n);				//設num為骰子擲出來的值n
	protected:
		CAnimation da;
		CMovingBitmap d1, d2, d3, d4, d5, d6;
		bool running;
		int num;
	};

	class SmallMap
	{
	public:
		void Setbooltrue();							//將isshow設為true
		void Setboolfalse();							//將isshow設為false
		void AddBitMap();								
		int findx(int p);								//計算小地圖的人物X座標
		int findy(int p);									//計算小地圖的人物Y座標
		int findbx(int p);								//計算小地圖上歸屬頭像的X座標
		int findby(int p);								//計算小地圖上歸屬頭像的Y座標
		int findhx(int p);								//計算小地圖上房子的X座標
		int findhy(int p);								//計算小地圖上房子的X座標
		void SetTopLeft(int location_of_p1, int location_of_p2);		//設定小地圖上兩個玩家的座標
		void OnShow(Buy land);					//顯示小地圖及人物
		void Showbelong(Buy);						//顯示歸屬頭像
		void Showhouse(Buy);						//顯示房子
	protected:
		bool isshow;										//是否顯示
		CMovingBitmap smap, sp1, sp2, spb1, spb2, sh1, sh2, sh3;
	};

	class CGameStateRun : public CGameState {
	public:
		CGameStateRun(CGame *g);
		~CGameStateRun();
		void OnBeginState();								// 設定每次重玩所需的變數
		void OnInit();  											// 遊戲的初值及圖形設定
		void OnKeyDown(UINT, UINT, UINT);	// 處理鍵盤Down的動作
		void OnKeyUp(UINT, UINT, UINT);		// 處理鍵盤Up的動作
	protected:
		void OnMove();											// 移動遊戲元素
		void OnShow();											// 顯示這個狀態的遊戲畫面
	private:
		CMovingBitmap	background;						// 背景圖
		CMovingBitmap	help;									// 說明圖
		CMovingBitmap	corner;								// 角落圖
		CMovingBitmap  husky;								// 角色哈士奇圖
		CMovingBitmap	sibainu;								// 角色柴犬圖
		GameMap			gamemap;							// 地圖
		charactor			player1;							// 玩家1
		charactor			player2;							// 玩家2
		CInteger			money;								// 金錢
		int					playernum;						// 玩家號碼
		Dice				dd;										// 骰子
		CMovingBitmap		roll_pic;						// 擲出的骰子圖
		Buy					land;									// 購買
		SmallMap			smallmap;							// 小地圖
	};

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的結束狀態(Game Over)
	// 每個Member function的Implementation都要弄懂
	/////////////////////////////////////////////////////////////////////////////

	class CGameStateOver : public CGameState {
	public:
		CGameStateOver(CGame *g);
		void OnBeginState();							// 設定每次重玩所需的變數
		void OnInit();									// 遊戲的初值及圖形設定
		void OnLButtonDown(UINT nFlags, CPoint point);  // 處理滑鼠的動作
	protected:
		void OnShow();									// 顯示這個狀態的遊戲畫面
	private:
		int counter;									// 倒數之計數器
		CMovingBitmap  over;							//game over圖
	};

	class CGameStateHelp : public CGameState {
	public:
		CGameStateHelp(CGame *g);
		void OnInit();									// 遊戲的初值及圖形設定
		void OnBeginState();							// 設定每次重玩所需的變數
		void OnLButtonDown(UINT nFlags, CPoint point);  // 處理滑鼠的動作
	protected:
		void OnShow();									// 顯示這個狀態的遊戲畫面
	private:
		int click = 0;
		CMovingBitmap  b1;								//說明頁第一頁
		CMovingBitmap  b2;								//說明頁第二頁
		CMovingBitmap  b3;								//說明頁第三頁
	};
}