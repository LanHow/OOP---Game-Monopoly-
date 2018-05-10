/*
* mygame.cpp: 本檔案儲遊戲本身的class的implementation
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
* History:
*   2002-03-04 V3.1
*          Add codes to demostrate the use of CMovingBitmap::ShowBitmap(CMovingBitmap &).
*	 2004-03-02 V4.0
*      1. Add CGameStateInit, CGameStateRun, and CGameStateOver to
*         demonstrate the use of states.
*      2. Demo the use of CInteger in CGameStateRun.
*   2005-09-13
*      Rewrite the codes for CBall and CEraser.
*   2005-09-20 V4.2Beta1.
*   2005-09-29 V4.2Beta2.
*      1. Add codes to display IDC_GAMECURSOR in GameStateRun.
*   2006-02-08 V4.2
*      1. Revise sample screens to display in English only.
*      2. Add code in CGameStateInit to demo the use of PostQuitMessage().
*      3. Rename OnInitialUpdate() -> OnInit().
*      4. Fix the bug that OnBeginState() of GameStateInit is not called.
*      5. Replace AUDIO_CANYON as AUDIO_NTUT.
*      6. Add help bitmap to CGameStateRun.
*   2006-09-09 V4.3
*      1. Rename Move() and Show() as OnMove and OnShow() to emphasize that they are
*         event driven.
*   2006-12-30
*      1. Bug fix: fix a memory leak problem by replacing PostQuitMessage(0) as
*         PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE,0,0).
*   2008-02-15 V4.4
*      1. Add namespace game_framework.
*      2. Replace the demonstration of animation as a new bouncing ball.
*      3. Use ShowInitProgress(percent) to display loading progress.
*   2010-03-23 V4.6
*      1. Demo MP3 support: use lake.mp3 to replace lake.wav.
*/

#include "stdafx.h"
#include "Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "audio.h"
#include "gamelib.h"
#include "mygame.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h> // RAND_MAX


namespace game_framework {
	GameMap::GameMap()
		:X(0), Y(0), MW(640), MH(480), count(0)
	{
		int map_initial[13][13] = { { 21,22,23,24,25,26,27,28,29,30,31,0,0 },
		{ 20,0,0,0,0,0,0,0,0,0,32,0,0 },
		{ 19,0,0,0,0,0,0,0,0,0,33,0,0 },
		{ 18,0,0,0,0,0,0,0,0,0,34,0,0 },
		{ 17,0,0,0,0,0,0,0,0,0,35,0,0 },
		{ 16,0,0,0,0,0,0,0,0,0,36,0,0 },
		{ 15,0,0,0,0,0,0,0,0,0,37,0,0 },
		{ 14,0,0,0,0,0,0,0,0,0,38,0,0 },
		{ 13,0,0,0,0,0,0,0,0,0,39,0,0 },
		{ 12,0,0,0,0,0,0,0,0,0,40,0,0 },
		{ 11,10,9,8,7,6,5,4,3,2,1,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0 } };
		for (int i = 0; i < 13; i++)
			for (int j = 0; j < 13; j++)
				map[i][j] = map_initial[i][j];
	}

	void GameMap::LoadBitmap()	
	{
		game_map.LoadBitmap(IDB_BOARD);
	}

	void GameMap::OnShow()	
	{
		game_map.SetTopLeft(X, Y);
		game_map.ShowBitmap();
	}

	void GameMap::Setp1(charactor & player)	
	{
		p1 = &player;
	}

	void GameMap::Setdice(Dice & dd)
	{
		d = &dd;
	}

	void GameMap::Setland(Buy & land)
	{
		l = &land;
	}

	void GameMap::Setpnum(int & num)	
	{
		pnum = &num;
	}

	bool GameMap::ifmoving()	
	{
		return ismoving;
	}

	void GameMap::showmap(const int p)	
	{
		int a = p;
		for (int i = 0; i < 13; i++) {
			for (int j = 0; j < 13; j++) {
				int y = i * -147;							//每一格的寬度是147像素
				int x = j * -147;	
				if (map[i][j] == a) {						//確定第p格在array中的位置
					X = x + 30;
					Y = y + 70;
				}
			}
		}
	}

	int GameMap::findx(int p)	
	{
		int nx;
		int x;
		for (int i = 0; i < 13; i++) {
			for (int j = 0; j < 13; j++) {
				x = j * 147;
				if (map[i][j] == p)							//確定第p格在array中的位置
					nx = x;
			}
		}
		return nx;											//回傳X座標
	}

	int GameMap::findy(int p)	
	{
		int ny;
		for (int i = 0; i < 13; i++) {
			for (int j = 0; j < 13; j++) {
				int y = i * 147;
				if (map[i][j] == p)							//確定第p格在array中的位置
					ny = y;
			}
		}
		return ny;											//回傳X座標
	}

	void GameMap::OnMove()	
	{
		if (ismoving == true) {										//若地圖在移動中
			int pos = p1->position;
			int end = p1->end;
			if (end <= 40) {										//若玩家移動的目的地格數小於40
				if (p1->position < p1->end) {						//若玩家目前所在的格數小於目的地
					if (p1->position < 11 && p1->position >= 1) {	//玩家介於1~11格移動改變的座標
						if (count != 49) {							//OnMove應執行49次使座標移動147像素(等於一格的寬度)
							X += 3;
							Sleep(1);
							count++;
						}
						else {
							p1->position++;							//移動座標累計達1格將人物位置+1
							count = 0;
						}
					}
					if (p1->position < 21 && p1->position >= 11) {	//玩家介於11~21格移動改變的座標
						if (count != 49) {
							Y += 3;
							Sleep(1);
							count++;
						}
						else {
							p1->position++;
							count = 0;
						}
					}
					if (p1->position < 31 && p1->position >= 21) {	//玩家介於21~31格移動改變的座標
						if (count != 49) {
							X -= 3;
							Sleep(1);
							count++;
						}
						else {
							p1->position++;
							count = 0;
						}
					}
					if (p1->position < 40 && p1->position >= 31) {	//玩家介於31~40格移動改變的座標
						if (count != 49) {
							Y -= 3;
							Sleep(1);
							count++;
						}
						else {
							p1->position++;
							count = 0;
						}
					}
				}
				else {															//若玩家已經移動到目的地
					ismoving = false;											//地圖停止移動
					p1->ismoving = false;										//人物移動狀態設為false
					d->SetboolFalse();											//骰子固定狀態設為false
					l->SetboolTrue();											//購買狀態設為true
					if (l->Checkbelong(p1->end) == true && l->Belongsto(p1->end) != *pnum)		//若土地有人擁有且非當前玩家所擁有
						CAudio::Instance()->Play(AUDIO_WAN, false);				//撥放音效
					else if (p1->end == 5 || p1->end == 39)						//若目的地停留在第4格或第38格(跳蚤和打針)
						CAudio::Instance()->Play(AUDIO_SAD, false);
					else if (p1->end == 31)										//若目的地停留在第30格(進監獄)
						CAudio::Instance()->Play(AUDIO_SAD, false);
					else
						CAudio::Instance()->Play(AUDIO_WAN1, false);
				}
			}
			else {																//若人物原本格數加上步數超過40
				int newend = p1->end - 40;										//通過原點後的目的地格數
				if (p1->position < 40 && p1->position >= 31) {					//通過原點前的座標改變
					if (count != 49) {
						Y -= 3;
						Sleep(1);
						count++;
					}
					else {
						p1->position++;
						count = 0;
					}
				}
				if (p1->position == 40) {										//通過原點時
					if (count != 49) {
						Y -= 3;
						Sleep(1);
						count++;
					}
					else {
						p1->position = 1;										//將人物位置重設為1(原點)
						p1->end = newend;										//將人物目的地設為通過原點後新目的地
						p1->money = p1->money + 200;							//通過原點獲得200元
						count = 0;
					}
				}
				if (p1->position < newend) {
					if (count != 49) {
						X += 3;
						Sleep(1);
						count++;
					}
					else {
						p1->position++;
						count = 0;
					}
				}
			}
		}
	}

	void GameMap::Setmoving(bool flag)
	{
		ismoving = flag;
	}

	int GameMap::RX()
	{
		return X;
	}

	int GameMap::RY()
	{
		return Y;
	}

	int Dice::dice()      
	{
		int number;
		srand((unsigned)time(NULL));
		number = rand() % 6;
		number += 1;
		return number;
	}

	void Dice::AddBitmap() {
		da.AddBitmap(1);
		da.AddBitmap(2);
		da.AddBitmap(3);
		da.AddBitmap(4);
		da.AddBitmap(5);
		da.AddBitmap(6);         //骰子動畫加入1~6的圖
		d1.LoadBitmapA(1);
		d2.LoadBitmapA(2);
		d3.LoadBitmapA(3);
		d4.LoadBitmapA(4);
		d5.LoadBitmapA(5);
		d6.LoadBitmapA(6);     
	}

	void Dice::SetboolFalse() {
		running = false;          
	}

	void Dice::SetboolTrue() {
		running = true;           
	}

	bool Dice::RetrunBool() {
		return running;           
	}

	void Dice::SetNum(int n) {
		num = n;                 
	}

	void Dice::Onshow() {
		if (running == true)      
		{ 
			switch (num)                    //判斷骰子擲出來的值為1~6何者
			{                               //分別顯示擲出的數字
			case 1:
				d1.ShowBitmap();
				break;
			case 2:
				d2.ShowBitmap();
				break;
			case 3:
				d3.ShowBitmap();
				break;
			case 4:
				d4.ShowBitmap();
				break;
			case 5:
				d5.ShowBitmap();
				break;
			case 6:
				d6.ShowBitmap();
				break;
			default:
				break;
			}
		}
		else                                  //如果角色停止
			da.OnShow();					  //則繼續骰子的動畫
	}

	void Dice::OnMove() {
		da.OnMove();						  
	}

	void Dice::SetTopLeft() {
		da.SetTopLeft(530, 360);
		d1.SetTopLeft(530, 360);
		d2.SetTopLeft(530, 360);
		d3.SetTopLeft(530, 360);
		d4.SetTopLeft(530, 360);
		d5.SetTopLeft(530, 360);
		d6.SetTopLeft(530, 360);
	}

	void Buy::Initial()                      
	{
		for (int i = 0; i < 40; i++)			//將array中每個land初始化
			landarray[i].Init();
	}

	int Buy::findbelongx(int p)
	{
		int x;
		if (p >= 1 && p <= 10)
			x = 148 * p;
		if (p >= 11 && p <= 20)
			x = 148 * 10 - 90;
		if (p >= 21 && p <= 30)
			x = 148 * 10 - 148 * (p - 20);
		if (p >= 31 && p <= 40)
			x = 100;
		return 1665 - x;
	}

	int Buy::findbelongy(int p)
	{
		int y;
		if (p >= 1 && p <= 10)
			y = 35;
		if (p >= 11 && p <= 19)
			y = 146 * (p - 10) - 50;
		if (p >= 20 && p <= 30)
			y = 1460 - 150;
		if (p >= 31 && p <= 40)
			y = 1460 - 146 * (p - 30) - 50;
		return 1585 - y;
	}

	int Buy::findhousex(int p)
	{
		int x;
		if (p >= 1 && p <= 10)
			x = 148 * p;
		if (p >= 11 && p <= 20)
			x = 148 * 10 - 10;
		if (p >= 21 && p <= 30)
			x = 148 * 10 - 148 * (p - 20);
		if (p >= 31 && p <= 40)
			x = 40;
		return 1665 - x;
	}

	int Buy::findhousey(int p)
	{
		int y;
		if (p >= 1 && p <= 10)
			y = -30;
		if (p >= 11 && p <= 19)
			y = 146 * (p - 10) - 50;
		if (p >= 20 && p <= 30)
			y = 1460 - 90;
		if (p >= 31 && p <= 40)
			y = 1460 - 146 * (p - 30) - 50;
		return 1585 - y;
	}

	void Buy::SetBmap(GameMap &b) 
	{
		bmap = &b;
	}

	void Buy::Showbelong() 
	{
		int x, y;
		for (int i = 0; i <= 39; i++) {								//第1格到第40格
			if (landarray[i].Belongsto() == 1) {					//若第i格屬於玩家1
				x = findbelongx(i + 1) + bmap->RX();				//與地圖左上角相對座標 + 地圖坐標
				y = findbelongy(i + 1) + bmap->RY();
				HH.SetTopLeft(x, y);
				HH.ShowBitmap();									//顯示玩家1的頭像
			}
			if (landarray[i].Belongsto() == 2) {					//若第i格屬於玩家2
				x = findbelongx(i + 1) + bmap->RX();				
				y = findbelongy(i + 1) + bmap->RY();
				SH.SetTopLeft(x, y);
				SH.ShowBitmap();									//顯示玩家2的頭像
			}
		}
	}

	void Buy::Showhouse() 
	{
		int x, y;
		for (int i = 0; i <= 39; i++) {								//第1格到第40格
			if (landarray[i].House() == 1) {						//若第i格的房子數 = 1
				x = findhousex(i + 1) + bmap->RX();					//與地圖左上角相對座標 + 地圖坐標
				y = findhousey(i + 1) + bmap->RY();
				H1.SetTopLeft(x+10, y+10);
				H1.ShowBitmap();									//顯示1級房子圖片
			}
			if (landarray[i].House() == 2) {						//若第i格的房子數 = 2
				x = findhousex(i + 1) + bmap->RX();
				y = findhousey(i + 1) + bmap->RY();
				H2.SetTopLeft(x+10, y+10);
				H2.ShowBitmap();									//顯示2級房子圖片
			}
			if (landarray[i].House() == 3) {						//若第i格的房子數 = 3
				x = findhousex(i + 1) + bmap->RX();
				y = findhousey(i + 1) + bmap->RY();
				H3.SetTopLeft(x+10, y+10);
				H3.ShowBitmap();									//顯示3級房子圖片
			}
		}
	}

	void Buy::SetboolFalse() 
	{
		running = false;
	}

	void Buy::SetboolTrue()
	{
		running = true;
	}

	bool Buy::ReturnBool() 
	{
		return running;
	}

	bool Buy::Decision() 
	{
		return decision;
	}

	void Buy::SetDecisionFalse()
	{
		decision = false;
	}

	void Buy::SetDecisionTrue() 
	{
		decision = true;
	}

	void Buy::GB(charactor *  player)
	{
		switch (n) {								//判斷事件
		case 1:
			player->money -= 200;
			break;
		case 2:
			player->money -= 500;
			break;
		case 3:
			player->money += 500;
			break;
		case 4:
			player->position -= 1;
			break;
		case 5:
			player->money -= 300;
			break;
		case 6:
			if (player->position == 38)
				player->position = 1;
			else
				player->position += 3;
			break;
		case 7:
			player->inprison = 1;				//休息一回合
			break;
		case 8:
			player->money += 200;
			break;
		case 9:
			player->position = 24;				//前進到黃金獵犬那格
			break;
		case 10:
			player->position += 2;
			break;
		case 11:
			player->inprison = 3;				//關進監獄
			player->position = 11;
			break;
		default:
			break;
		}
	}

	void Buy::OnShow(charactor * pplayer, int belongto) 
	{
		if (running == true) {					//若購買開始
			switch (pplayer->end - 1) {
			case 1:	
			case 3:
			case 6:
			case 8:
			case 9:
			case 11:
			case 13:
			case 14:
			case 16:
			case 18:
			case 19:
			case 21:
			case 23:
			case 24:
			case 26:
			case 27:
			case 29:
			case 31:
			case 32:
			case 34:
			case 36:
			case 39:
				if (belongto == 0) {			//若土地無人擁有則顯示是否購買視窗
					Q.ShowBitmap();
					choice.ShowBitmap();
				}
				else if (belongto == pplayer->playernum) {			//若土地為自己擁有顯示是否蓋房子
					if (landarray[pplayer->end - 2].House() < 3) {	//房子上限為3級
						build.ShowBitmap();
						choice.ShowBitmap();
					}
				}
				else
					belong.ShowBitmap();		//土地已有人擁有顯示扣錢
				break;
			case 5:
			case 15:
			case 25:
			case 35:
				if (belongto == 0) {
					H.ShowBitmap();
					choice.ShowBitmap();
				}
				else if (belongto == pplayer->playernum) {
					build.ShowBitmap();
					choice.ShowBitmap();
				}
				else
					belong.ShowBitmap();
				break;
			case 12:
				if (belongto == 0) {
					fire.ShowBitmap();
					choice.ShowBitmap();
				}
				else if (belongto == pplayer->playernum) {
					build.ShowBitmap();
					choice.ShowBitmap();
				}
				else
					belong.ShowBitmap();
				break;
			case 28:
				if (belongto == 0) {
					hot.ShowBitmap();
					choice.ShowBitmap();
				}
				else if (belongto == pplayer->playernum) {
					build.ShowBitmap();
					choice.ShowBitmap();
				}
				else
					belong.ShowBitmap();
				break;
			case 2:					//事件格
			case 17:
			case 33:
			case 7:
			case 22:
			case 37:
				switch (n) {
				case 1:
					C1.ShowBitmap();
					break;
				case 2:
					C2.ShowBitmap();
					break;
				case 3:
					C3.ShowBitmap();
					break;
				case 4:
					C4.ShowBitmap();
					break;
				case 5:
					C5.ShowBitmap();
					break;
				case 6:
					C6.ShowBitmap();
					break;
				case 7:
					C7.ShowBitmap();
					break;
				case 8:
					C8.ShowBitmap();
					break;
				case 9:
					C9.ShowBitmap();
					break;
				case 10:
					C10.ShowBitmap();
					break;
				case 11:
					C11.ShowBitmap();
					break;
				}
				break;
			case 4:					//跳蚤格
				F.ShowBitmap();
				break;
			case 38:				//打針格
				V.ShowBitmap();
				break;
			}
		}
	}

	void Buy::SetTopLeft() 
	{
		Q.SetTopLeft(130, 150);
		if (decision == false)
			choice.SetTopLeft(342, 288);
		if (decision == true)
			choice.SetTopLeft(181, 288);
		B.SetTopLeft(50, 20);
		F.SetTopLeft(130, 150);
		G.SetTopLeft(50, 20);
		H.SetTopLeft(130, 150);
		hot.SetTopLeft(130, 150);
		fire.SetTopLeft(130, 150);
		V.SetTopLeft(130, 150);
		belong.SetTopLeft(100, 50);
		C1.SetTopLeft(100, 120);
		C2.SetTopLeft(100, 120);
		C3.SetTopLeft(100, 120);
		C4.SetTopLeft(100, 120);
		C5.SetTopLeft(100, 120);
		C6.SetTopLeft(100, 120);
		C7.SetTopLeft(100, 120);
		C8.SetTopLeft(100, 120);
		C9.SetTopLeft(100, 120);
		C10.SetTopLeft(100, 120);
		C11.SetTopLeft(100, 120);
		build.SetTopLeft(130, 150);
	}

	void Buy::AddBitmap() 
	{
		Q.LoadBitmap(IDB_BUY);
		B.LoadBitmap(IDB_BAD);
		F.LoadBitmap(IDB_FLEA);
		G.LoadBitmap(IDB_GOOD, RGB(0, 0, 0));
		choice.LoadBitmap(IDB_CHOICE, RGB(255, 255, 255));
		belong.LoadBitmap(IDB_PASSPAY, RGB(0, 0, 0));
		H.LoadBitmap(IDB_HOUSE);
		hot.LoadBitmap(IDB_HOTDOG);
		fire.LoadBitmap(IDB_FIRE);
		V.LoadBitmap(IDB_VET);
		HH.LoadBitmap(IDB_HH, RGB(0, 0, 0));
		SH.LoadBitmap(IDB_SH, RGB(0, 0, 0));
		C1.LoadBitmap(IDB_C1);
		C2.LoadBitmap(IDB_C2);
		C3.LoadBitmap(IDB_C3);
		C4.LoadBitmap(IDB_C4);
		C5.LoadBitmap(IDB_C5);
		C6.LoadBitmap(IDB_C6);
		C7.LoadBitmap(IDB_C7);
		C8.LoadBitmap(IDB_C8);
		C9.LoadBitmap(IDB_C9);
		C10.LoadBitmap(IDB_C10);
		C11.LoadBitmap(IDB_C11);
		build.LoadBitmap(IDB_BUILD);
		H1.LoadBitmap(IDB_H1,RGB(0,0,0));
		H2.LoadBitmap(IDB_H2,RGB(0,0,0)); 
		H3.LoadBitmap(IDB_H3,RGB(0,0,0));
	}

	void Buy::Buyland(charactor * player)
	{
		player->money = player->money - landarray[player->end - 2].Price();			//扣錢
		landarray[player->end - 2].Buy(player->playernum);							//買地
	}

	void Buy::Paymoney(charactor * player)
	{
		int n = landarray[player->end - 2].House() * 30;							//計算房子數
		player->money = player->money - landarray[player->end - 2].Rent() - n;		//扣錢
	}

	void Buy::Getmoney(charactor * player)
	{
		int n = landarray[player->end - 2].House() * 30;							//計算房子數
		player->money = player->money + landarray[player->end - 2].Rent() + n;		//加錢
	}

	bool Buy::Checkbelong(int p)
	{
		if (landarray[p - 2].Belongsto() == 0)				//若房子沒人擁有
			return false;
		else
			return true;
	}

	int Buy::Belongsto(int p)
	{
		return landarray[p - 2].Belongsto();
	}

	void Buy::Buildhouse(int p)
	{
		landarray[p - 2].BuildHouse();
	}

	int Buy::houselevel(int p)
	{
		return landarray[p - 2].House();
	}

	void SmallMap::Setbooltrue()
	{
		isshow = true;
	}

	void SmallMap::Setboolfalse()
	{
		isshow = false;
	}

	void SmallMap::AddBitMap()
	{
		smap.LoadBitmap(IDB_SMALLMAP);
		sp1.LoadBitmap(IDB_SMALLP1, RGB(255, 255, 255));
		sp2.LoadBitmap(IDB_SMALLP2, RGB(255, 255, 255));
		spb1.LoadBitmap(IDB_SMALLHUSKYHEAD, RGB(0, 0, 0));
		spb2.LoadBitmap(IDB_SMALLSHIBAHEAD, RGB(0, 0, 0));
		sh1.LoadBitmap(IDB_SMALLHOUSE1, RGB(0, 0, 0));
		sh2.LoadBitmap(IDB_SMALLHOUSE2, RGB(0, 0, 0));
		sh3.LoadBitmap(IDB_SMALLHOUSE3, RGB(0, 0, 0));
	}

	int SmallMap::findx(int p)
	{
		int x;
		if (p >= 1 && p <= 11)
			x = 36 * p;
		if (p >= 11 && p <= 21)
			x = 396;
		if (p >= 22 && p <= 30)
			x = 396 - 36 * (p - 21);
		if (p >= 31 && p <= 40)
			x = 36;
		return 517 - x;
	}

	int SmallMap::findy(int p)
	{
		int y;
		if (p >= 1 && p <= 11)
			y = 0;
		if (p >= 12 && p <= 20)
			y = 35 * (p - 11);
		if (p >= 21 && p <= 31)
			y = 350;
		if (p >= 32 && p <= 40)
			y = 350 - 35 * (p - 31);
		return 398 - y;
	}

	int SmallMap::findbx(int p)
	{
		int x;
		if (p >= 1 && p <= 11)
			x = 36 * p;
		if (p >= 11 && p <= 21)
			x = 380;
		if (p >= 22 && p <= 30)
			x = 396 - 36 * (p - 21);
		if (p >= 31 && p <= 40)
			x = 55;
		return 525 - x;
	}

	int SmallMap::findby(int p)
	{
		int y;
		if (p >= 1 && p <= 11)
			y = 10;
		if (p >= 12 && p <= 20)
			y = 35 * (p - 11);
		if (p >= 21 && p <= 31)
			y = 330;
		if (p >= 32 && p <= 40)
			y = 350 - 35 * (p - 31);
		return 398 - y;
	}

	int SmallMap::findhx(int p)
	{
		int x;
		if (p >= 1 && p <= 11)
			x = 36 * p;
		if (p >= 11 && p <= 21)
			x = 405;
		if (p >= 22 && p <= 30)
			x = 396 - 36 * (p - 21);
		if (p >= 31 && p <= 40)
			x = 36;
		return 525 - x;
	}

	int SmallMap::findhy(int p)
	{
		int y;
		if (p >= 1 && p <= 11)
			y = -10;
		if (p >= 12 && p <= 20)
			y = 35 * (p - 11);
		if (p >= 21 && p <= 31)
			y = 350;
		if (p >= 32 && p <= 40)
			y = 350 - 35 * (p - 31);
		return 398 - y;
	}

	void SmallMap::SetTopLeft(int location_of_p1, int location_of_p2)
	{
		smap.SetTopLeft(89, 9);
		int p1x, p1y, p2x, p2y;
		p1x = findx(location_of_p1);
		p1y = findy(location_of_p1);
		p2x = findx(location_of_p2);
		p2y = findy(location_of_p2);
		sp1.SetTopLeft(p1x, p1y);
		sp2.SetTopLeft(p2x, p2y);
	}

	void SmallMap::Showbelong(Buy land)
	{
		int x, y;
		for (int i = 0; i <= 39; i++) {
			if (land.Belongsto(i + 2) == 1)
			{
				x = findbx(i + 2);
				y = findby(i + 2);
				spb1.SetTopLeft(x, y);
				spb1.ShowBitmap();
			}
			if (land.Belongsto(i + 2) == 2)
			{
				x = findbx(i + 2);
				y = findby(i + 2);
				spb2.SetTopLeft(x, y);
				spb2.ShowBitmap();
			}
		}
	}

	void SmallMap::Showhouse(Buy land)
	{
		int x, y;
		for (int i = 0; i <= 39; i++) {
			if (land.houselevel(i + 2) == 1) {
				x = findhx(i + 2);
				y = findhy(i + 2);
				sh1.SetTopLeft(x, y);
				sh1.ShowBitmap();
			}
			if (land.houselevel(i + 2) == 2) {
				x = findhx(i + 2);
				y = findhy(i + 2);
				sh2.SetTopLeft(x, y);
				sh2.ShowBitmap();
			}
			if (land.houselevel(i + 2) == 3) {
				x = findhx(i + 2);
				y = findhy(i + 2);
				sh3.SetTopLeft(x, y);
				sh3.ShowBitmap();
			}
		}
	}

	void SmallMap::OnShow(Buy land)
	{
		if (isshow == TRUE) {
			smap.ShowBitmap();
			Showbelong(land);
			Showhouse(land);
			sp1.ShowBitmap();
			sp2.ShowBitmap();
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的遊戲開頭畫面物件
	/////////////////////////////////////////////////////////////////////////////

	CGameStateInit::CGameStateInit(CGame *g)
		: CGameState(g)
	{

	}

	void CGameStateInit::OnInit()
	{
		ShowInitProgress(0);	// 一開始的loading進度為0%
								//
								// 開始載入資料
								//
		logo.LoadBitmap(IDB_LOGO);
		run.AddBitmap(7, RGB(255, 255, 255));                 //小狗跑步動畫加入7,8的圖片
		run.AddBitmap(8, RGB(255, 255, 255));
	}

	void CGameStateInit::OnBeginState()
	{
	}

	void CGameStateInit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		const char KEY_ESC = 27;
		const char KEY_SPACE = ' ';
		if (nChar == KEY_SPACE)
			GotoGameState(GAME_STATE_RUN);						// 切換至GAME_STATE_RUN
		else if (nChar == KEY_ESC)								// Demo 關閉遊戲的方法
			PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);	// 關閉遊戲
	}

	void CGameStateInit::OnLButtonDown(UINT nFlags, CPoint point)
	{
		if (point.x <= 210 && point.x >= 60 && point.y <= 390 && point.y >= 340)       //Start按鍵
		{
			GotoGameState(GAME_STATE_RUN);		                                       // 切換至GAME_STATE_RUN
		}

		if (point.x <= 586 && point.x >= 440 && point.y <= 390 && point.y >= 340) {    //Exit按鍵
			PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);	                   // 關閉遊戲
		}

		if (point.x <= 400 && point.x >= 250 && point.y <= 390 && point.y >= 340) {    //Help按鍵
			GotoGameState(GAME_STATE_HELP);		                                       // 切換至GAME_STATE_HELP 說明頁面
		}
	}

	void CGameStateInit::OnMove()
	{
		run.OnMove();                              //小狗跑步動畫
	}

	void CGameStateInit::OnShow()
	{
		//
		// 貼上logo與小狗動畫
		//
		run.SetTopLeft(200, 380);
		logo.SetTopLeft(0, 0);
		logo.ShowBitmap();
		//
		// Demo螢幕字型的使用，不過開發時請盡量避免直接使用字型，改用CMovingBitmap比較好
		//
		run.OnShow();
	}

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的結束狀態(Game Over)
	/////////////////////////////////////////////////////////////////////////////

	CGameStateOver::CGameStateOver(CGame *g)
		: CGameState(g)
	{
	}
	void CGameStateOver::OnBeginState()
	{
	}

	void CGameStateOver::OnLButtonDown(UINT nFlags, CPoint point) {
		if (point.x <= 391 && point.x >= 242 && point.y <= 377 && point.y >= 325)       //Restart按鍵
		{
			GotoGameState(GAME_STATE_INIT);		                                        // 切換至GAME_STATE_RUN 重新開始遊戲
		}
		if (point.x <= 391 && point.x >= 242 && point.y <= 453 && point.y >= 400) {     //Exit按鍵
			PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);	                    // 關閉遊戲
		}
	}

	void CGameStateOver::OnInit()
	{
		over.LoadBitmap(IDB_OVER);
	}

	void CGameStateOver::OnShow()
	{
		over.ShowBitmap();
	}

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的遊戲執行物件，主要的遊戲程式都在這裡
	/////////////////////////////////////////////////////////////////////////////

	CGameStateRun::CGameStateRun(CGame *g)
		: CGameState(g)
	{
	}

	CGameStateRun::~CGameStateRun()
	{
	}

	void CGameStateRun::OnBeginState()
	{
		CAudio::Instance()->Play(AUDIO_DING, false);		// 撥放 WAVE
		CAudio::Instance()->Play(AUDIO_BGM, true);			// 撥放 MIDI
		player1.money = 3000;                               //player1的錢初始值
		player2.money = 3000;                               //player2的錢初始值
		player1.position = 1;                               //player1的位置初始值
		player2.position = 1;                               //player2的位置初始值
		player1.step = 0;									//player1要走的步數
		player2.step = 0;									//player2要走的步數
		gamemap.Setp1(player1);								//把p1指到player1
		gamemap.showmap(player1.position);					//讓地圖顯示在player1的位置
		money.SetTopLeft(0, 0);								//金錢顯示在左上角
		money.SetInteger(player1.money);					//金錢顯示player1錢的金額
		playernum = 1;										//表達現在的玩家為player1
		gamemap.Setdice(dd);								//把gamemap裡面的d指到dd
		gamemap.Setland(land);								//把gamemap裡面的l指到land
		player1.X = 200;									//player1的X座標	
		player1.Y = 250;									//player1的Y座標	
		player2.X = 200;									//player2的X座標	
		player2.Y = 250;									//player2的Y座標	
		player1.ismoving = false;							//設定player1與player2為靜止
		player2.ismoving = false;							//
		husky.SetTopLeft(player1.X, player1.Y);				//設定player1與player2圖的位置
		sibainu.SetTopLeft(player2.X, player2.Y);			//
		player1.mapX = gamemap.RX();						//讓地圖顯示在player1的位置
		player1.mapY = gamemap.RY();
		player2.mapX = gamemap.RX();						//讓地圖顯示在player1的位置
		player2.mapY = gamemap.RY();
		player1.playernum = 1;								//設定player1與player2以playernum表示
		player2.playernum = 2;
		player1.inprison = 0;								//設定player1與player2監獄的初始值
		player2.inprison = 0;
		land.Initial();										//設定土地的初始值
	}

	void CGameStateRun::OnMove()							// 移動遊戲元素
	{
		if (gamemap.ifmoving() == false)					//如果地圖視窗此時為靜止的
		{
			if (playernum == 1)
				gamemap.showmap(player1.position);			//地圖顯示在player1的位置
			if (playernum == 2)
				gamemap.showmap(player2.position);			//地圖顯示在player2的位置
		}
		gamemap.OnMove();									//啟用地圖、骰子、土地
		dd.OnMove();
		if (playernum == 1)														//現在的玩家為player1
		{	
			money.SetInteger(player1.money);									//顯示player1的餘額
			player2.mapX = gamemap.RX() + gamemap.findx(player2.position);		//player2在視窗上顯示會隨著地圖移動而改變
			player2.mapY = gamemap.RY() + gamemap.findy(player2.position);
		}
		if (playernum == 2)														//現在的玩家為player2
		{
			money.SetInteger(player2.money);									//顯示player2的餘額
			player1.mapX = gamemap.RX() + gamemap.findx(player1.position);		//player1在視窗上顯示會隨著地圖移動而改變
			player1.mapY = gamemap.RY() + gamemap.findy(player1.position);
		}

		if (player1.money <= 0) {												//
			GotoGameState(GAME_STATE_OVER);										//
		}																		//任一玩家餘額小於0時遊戲即結束
		if (player2.money <= 0) {												//跳到game over畫面
			GotoGameState(GAME_STATE_OVER);										//
		}																		//
	}

	void CGameStateRun::OnInit()  								// 遊戲的初值及圖形設定
	{
		background.LoadBitmap(IDB_BACKGROUND);					       // 載入背景的圖形
		CAudio::Instance()->Load(AUDIO_DING, "sounds\\ding.wav");	   // 載入編號0的聲音ding.wav
		CAudio::Instance()->Load(AUDIO_BGM, "sounds\\BGM.mp3");	       // 載入背景音樂BGM.mp3
		CAudio::Instance()->Load(AUDIO_DICE, "sounds\\dice.mp3");	   // 載入擲骰子的音效dice.mp3
		CAudio::Instance()->Load(AUDIO_WAN, "sounds\\wan.mp3");		   // 載入狗叫汪汪汪的音效wan.mp3
		CAudio::Instance()->Load(AUDIO_CHOOSE, "sounds\\click.mp3");   // 載入選擇的音效click.mp3
		CAudio::Instance()->Load(AUDIO_BIU, "sounds\\biu.mp3");        // 載入確認按鍵的音效biu.mp3
		CAudio::Instance()->Load(AUDIO_WAN1, "sounds\\wan1.mp3");      // 載入狗叫汪的音效wan1.mp3
		CAudio::Instance()->Load(AUDIO_BUILD, "sounds\\build.mp3");    // 載入蓋房子的音效build.mp3
		CAudio::Instance()->Load(AUDIO_SAD, "sounds\\sad.wav");        // 載入哀傷的音效sad.mp3
		husky.LoadBitmap(IDB_HUSKY, RGB(0,0,0));		        	   //載入角色哈士奇的圖
		sibainu.LoadBitmap(IDB_SIBAINU, RGB(255, 255, 255));		   //載入角色柴犬的圖
		gamemap.LoadBitmap();										   //載入地圖
		money.LoadBitmap();											   //載入金錢顯示圖
		dd.AddBitmap();												   
		dd.SetTopLeft();											   
		land.AddBitmap();											   
		land.SetTopLeft();											   
		smallmap.AddBitMap();										   
		smallmap.Setboolfalse();									   //設定為沒有顯示小地圖
		land.SetBmap(gamemap);										   //把land裡面的bmap指到gamemap
	}

	void CGameStateRun::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		const char KEY_SPACE = 0x20;// keyboard空白鍵
		const char KEY_ENTER = 0xD;	// keyboardEnter
		const char KEY_M = 0x4D;	// keyboard	M
		const char KEY_1 = 0x31;	// keyboard1
		const char KEY_2 = 0x32;	// keyboard2
		const char KEY_3 = 0x33;	// keyboard3
		const char KEY_4 = 0x34;	// keyboard4
		const char KEY_5 = 0x35;	// keyboard5
		const char KEY_6 = 0x36;	// keyboard6
		const char KEY_7 = 0x37;	// keyboard7
		const char KEY_8 = 0x38;	// keyboard8
		const char KEY_9 = 0x39;	// keyboard9
		const char KEY_0 = 0x30;	// keyboard0
		const char KEY_LEFT = 0x25; // keyboard左箭頭
		const char KEY_UP = 0x26; 	// keyboard上箭頭
		const char KEY_RIGHT = 0x27;// keyboard右箭頭
		const char KEY_DOWN = 0x28; // keyboard下箭頭
		charactor *pplayer;
		charactor *pplayer2;
		if (playernum == 1)										//
		{														//
			pplayer = &player1;									//依當時的玩家為何
			pplayer2 = &player2;								//去設定不同的指標
		}														//pplayer當時輪到的玩家
		if (playernum == 2)										//pplayer2則是另一位
		{														//
			pplayer = &player2;									//	
			pplayer2 = &player1;								//
		}
		if (nChar == KEY_ENTER)
		{
			if (land.ReturnBool() == true)
			{
				if (land.Checkbelong(pplayer->end) == false)		//若土地沒人擁有
				{
					switch (pplayer->end - 1) {
					case 1:										    //土地格
					case 3:
					case 5:
					case 6:
					case 8:
					case 9:
					case 11:
					case 12:
					case 13:
					case 14:
					case 15:
					case 16:
					case 18:
					case 19:
					case 21:
					case 23:
					case 24:
					case 25:
					case 26:
					case 27:
					case 28:
					case 29:
					case 31:
					case 32:
					case 34:
					case 35:
					case 36:
					case 39:
						if (land.Decision() == true)
						{
							CAudio::Instance()->Play(AUDIO_BIU, false);
							land.Buyland(pplayer);					//購買土地函式
						}
						break;
					case 4:											//扣錢事件格
					case 38:
						land.Paymoney(pplayer);						//直接扣錢
						CAudio::Instance()->Play(AUDIO_BIU, false);
						break;
					case 2:											//機會命運格
					case 7:
					case 17:
					case 22:
					case 33:
					case 37:
						land.GB(pplayer);							//執行GB函式(隨機生成機會命運事件)
						break;
					case 30:										//入獄格
						pplayer->position = 11;
						pplayer->inprison = 3;
						break;
					}
				}
				else if (playernum != land.Belongsto(pplayer->end))								//若土地有人擁有
				{
					switch (pplayer->end - 1) {
					case 1:
					case 3:
					case 5:
					case 6:
					case 8:
					case 9:
					case 11:
					case 12:
					case 13:
					case 14:
					case 15:
					case 16:
					case 18:
					case 19:
					case 21:
					case 23:
					case 24:
					case 25:
					case 26:
					case 27:
					case 28:
					case 29:
					case 31:
					case 32:
					case 34:
					case 35:
					case 36:
					case 39:
					case 4:
					case 38:
						land.Paymoney(pplayer);						//直接扣錢
						land.Getmoney(pplayer2);					//另一位玩家加錢
						break;
					}
				}
				else if (land.houselevel(pplayer->end) < 3)		    //若房子等級小於3
				{
					if (land.Decision() == true)					//玩家選擇購買
					{
						CAudio::Instance()->Play(AUDIO_BUILD, false);
						land.Buildhouse(pplayer->end);
						pplayer->money = pplayer->money - 50;
					}
				}
				land.SetDecisionFalse();
				switch (playernum)
				{
				case 1:
					if (pplayer2->inprison == 0)					//
						playernum = 2;								//
					else											//
						pplayer2->inprison--;						//判斷玩家是否達成入獄條件
					break;											//若是則切換另一位玩家
				case 2:												//直到入獄剩下的回合數為零
					if (pplayer2->inprison == 0)					//
						playernum = 1;								//	
					else											//
						pplayer2->inprison--;						//
					break;
				default:
					break;
				}
				land.SetboolFalse();
			}
		}
		if (nChar == KEY_SPACE)
		{
			if (gamemap.ifmoving() == false && land.ReturnBool() == false)
			{
				CAudio::Instance()->Play(AUDIO_DICE, false);
				gamemap.Setp1(*pplayer);
				gamemap.Setpnum(playernum);								//設定現在玩家為何
				pplayer->ismoving = true;
				pplayer->step = dd.dice();								//將骰子擲出來的點數指到玩家移動步數
				pplayer->end = pplayer->position + pplayer->step;		//玩家最後的位置為原本的位置加上移動步數
				dd.SetNum(pplayer->step);
				dd.SetboolTrue();
				gamemap.Setmoving(true);
				land.SetboolFalse();
				money.SetInteger(pplayer->money);						//金錢顯示現在玩家的餘額
				if (pplayer->end == 3 || pplayer->end == 8 || pplayer->end == 18 || pplayer->end == 23 || pplayer->end == 34 || pplayer->end == 38)
				{														//若玩家走到機會命運格
					srand((unsigned)time(NULL));						//隨機產生1~11的代碼n
					int n = rand() % 11;
					land.setn(n + 1);
				}
			}
		}
		if (nChar == KEY_LEFT)
			if (land.ReturnBool() == true)
			{
				if(pplayer->end != 3 && pplayer->end != 5 && pplayer->end != 8 && pplayer->end != 18 && pplayer->end != 23 &&
					pplayer->end != 34 && pplayer->end != 38 && pplayer->end != 39 && pplayer->end != 1)
				{
					land.SetDecisionTrue();								
					CAudio::Instance()->Play(AUDIO_CHOOSE, false);		//選擇確認的音效
				}
			}
		if (nChar == KEY_RIGHT)
			if (land.ReturnBool() == true)
			{
				if (pplayer->end != 3 && pplayer->end != 5 && pplayer->end != 8 && pplayer->end != 18 && pplayer->end != 23 &&
					pplayer->end != 34 && pplayer->end != 38 && pplayer->end != 39 && pplayer->end != 1)
				{
					land.SetDecisionFalse();
					CAudio::Instance()->Play(AUDIO_CHOOSE, false);		//選擇確認的音效
				}
			}
		if (nChar == KEY_UP)											//
			pplayer->money = pplayer->money + 100;						//*密技*
		if (nChar == KEY_DOWN)											//利用上下鍵可以增加減少錢
			pplayer->money = pplayer->money - 100;						//
		if (nChar == KEY_M)												//按下M鍵可以顯示小地圖
			smallmap.Setbooltrue();										//
		if (nChar == KEY_1 || nChar == KEY_2 || nChar == KEY_3 || nChar == KEY_4 || nChar == KEY_5 || nChar == KEY_6)
		{
			int n;
			switch (nChar)												//*密技*
			{															//按下1~6的數字鍵
			case KEY_1:													//可以直接設定骰子的點數
				n = 1;
				break;
			case KEY_2:
				n = 2;
				break;
			case KEY_3:
				n = 3;
				break;
			case KEY_4:
				n = 4;
				break;
			case KEY_5:
				n = 5;
				break;
			case KEY_6:
				n = 6;
				break;
			default:
				break;
			}
			if (gamemap.ifmoving() == false && land.ReturnBool() == false)
			{
				gamemap.Setp1(*pplayer);
				gamemap.Setpnum(playernum);
				pplayer->ismoving = true;
				pplayer->step = n;
				pplayer->end = pplayer->position + pplayer->step;
				dd.SetNum(pplayer->step);
				dd.SetboolTrue();
				gamemap.Setmoving(true);
				land.SetboolFalse();
				money.SetInteger(pplayer->money);
				if (pplayer->end == 3 || pplayer->end == 8 || pplayer->end == 18 || pplayer->end == 23 || pplayer->end == 34 || pplayer->end == 38)
				{
					srand((unsigned)time(NULL));
					int n = rand() % 11;
					land.setn(n + 1);
				}
			}
		}
		if (nChar == KEY_7)
			if (gamemap.ifmoving() == false && land.ReturnBool() == false)		//
				pplayer->position = 11;											//*密技*
		if (nChar == KEY_8)														//按下7~0字鍵
			if (gamemap.ifmoving() == false && land.ReturnBool() == false)		//可分別讓玩家直接移動到第10,20,30,起點格
				pplayer->position = 21;											//
		if (nChar == KEY_9)
			if (gamemap.ifmoving() == false && land.ReturnBool() == false)
				pplayer->position = 31;
		if (nChar == KEY_0)
			if (gamemap.ifmoving() == false && land.ReturnBool() == false)
				pplayer->position = 1;
	}
	void CGameStateRun::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		const char KEY_LEFT = 0x25; // keyboard左箭頭
		const char KEY_UP = 0x26; // keyboard上箭頭
		const char KEY_RIGHT = 0x27; // keyboard右箭頭
		const char KEY_DOWN = 0x28; // keyboard下箭頭
		const char KEY_M = 0x4D;
		const char KEY_ESC = 27;
		if (nChar == KEY_M)
			smallmap.Setboolfalse();
		if (nChar == KEY_ESC)								// Demo 關閉遊戲的方法
			PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);	// 關閉遊戲
	}

	void CGameStateRun::OnShow()
	{
		//
		//  注意：Show裡面千萬不要移動任何物件的座標，移動座標的工作應由Move做才對，
		//        否則當視窗重新繪圖時(OnDraw)，物件就會移動，看起來會很怪。換個術語
		//        說，Move負責MVC中的Model，Show負責View，而View不應更動Model。
		//
		gamemap.OnShow();
		land.Showbelong();										//顯示歸屬
		land.Showhouse();										//顯示房子
		if (playernum == 1)
		{
			husky.SetTopLeft(player1.X, player1.Y);
			sibainu.SetTopLeft(player2.mapX + player2.X - 30, player2.mapY + player2.Y - 70);
		}
		if (playernum == 2)
		{
			husky.SetTopLeft(player1.mapX + player1.X - 30, player1.mapY + player1.Y - 70);
			sibainu.SetTopLeft(player2.X, player2.Y);
		}
		husky.ShowBitmap();
		sibainu.ShowBitmap();
		money.ShowBitmap();										//顯示金錢
		dd.Onshow();											//顯示骰子
		charactor * pplayer;
		if (playernum == 1)
			pplayer = &player1;
		if (playernum == 2) 
			pplayer = &player2;
		land.OnShow(pplayer, land.Belongsto(pplayer->end));		//顯示歸屬
		land.SetTopLeft();
		smallmap.SetTopLeft(player1.position, player2.position);
		smallmap.OnShow(land);									//顯示小地圖


	}


	CGameStateHelp::CGameStateHelp(CGame *g)
		: CGameState(g)
	{
	}

	void CGameStateHelp::OnBeginState() {
	}

	void CGameStateHelp::OnInit() {
		b1.LoadBitmap(IDB_B1);
		b2.LoadBitmap(IDB_B2);
		b3.LoadBitmap(IDB_B3);
	}

	void CGameStateHelp::OnLButtonDown(UINT nFlags, CPoint point) {
		if (point.x <= 55 && point.x >= 10 && point.y <= 55 && point.y >= 7) {					//返回鍵
			GotoGameState(GAME_STATE_INIT);														//跳至主畫面
		}
		if (point.x <= 590 && point.x >= 565 && point.y <= 468 && point.y >= 444) {				//左鍵前一頁
			if (click>0)
				click -= 1;
		}
		if (point.x <= 620 && point.x >= 595 && point.y <= 468 && point.y >= 444) {				//右鍵下一頁
			if (click <2)
				click += 1;
		}
	}

	void CGameStateHelp::OnShow() {
		switch (click)							//click儲存滑鼠點幾次
		{
		case 0:									//顯示第幾頁
			b1.ShowBitmap();
			break;
		case 1:
			b2.ShowBitmap();
			break;
		case 2:
			b3.ShowBitmap();
			break;

		default:
			break;
		}
	}
}
