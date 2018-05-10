/*
* mygame.cpp: ���ɮ��x�C��������class��implementation
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
				int y = i * -147;							//�C�@�檺�e�׬O147����
				int x = j * -147;	
				if (map[i][j] == a) {						//�T�w��p��barray������m
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
				if (map[i][j] == p)							//�T�w��p��barray������m
					nx = x;
			}
		}
		return nx;											//�^��X�y��
	}

	int GameMap::findy(int p)	
	{
		int ny;
		for (int i = 0; i < 13; i++) {
			for (int j = 0; j < 13; j++) {
				int y = i * 147;
				if (map[i][j] == p)							//�T�w��p��barray������m
					ny = y;
			}
		}
		return ny;											//�^��X�y��
	}

	void GameMap::OnMove()	
	{
		if (ismoving == true) {										//�Y�a�Ϧb���ʤ�
			int pos = p1->position;
			int end = p1->end;
			if (end <= 40) {										//�Y���a���ʪ��ت��a��Ƥp��40
				if (p1->position < p1->end) {						//�Y���a�ثe�Ҧb����Ƥp��ت��a
					if (p1->position < 11 && p1->position >= 1) {	//���a����1~11�沾�ʧ��ܪ��y��
						if (count != 49) {							//OnMove������49���Ϯy�в���147����(����@�檺�e��)
							X += 3;
							Sleep(1);
							count++;
						}
						else {
							p1->position++;							//���ʮy�в֭p�F1��N�H����m+1
							count = 0;
						}
					}
					if (p1->position < 21 && p1->position >= 11) {	//���a����11~21�沾�ʧ��ܪ��y��
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
					if (p1->position < 31 && p1->position >= 21) {	//���a����21~31�沾�ʧ��ܪ��y��
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
					if (p1->position < 40 && p1->position >= 31) {	//���a����31~40�沾�ʧ��ܪ��y��
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
				else {															//�Y���a�w�g���ʨ�ت��a
					ismoving = false;											//�a�ϰ����
					p1->ismoving = false;										//�H�����ʪ��A�]��false
					d->SetboolFalse();											//��l�T�w���A�]��false
					l->SetboolTrue();											//�ʶR���A�]��true
					if (l->Checkbelong(p1->end) == true && l->Belongsto(p1->end) != *pnum)		//�Y�g�a���H�֦��B�D��e���a�Ҿ֦�
						CAudio::Instance()->Play(AUDIO_WAN, false);				//���񭵮�
					else if (p1->end == 5 || p1->end == 39)						//�Y�ت��a���d�b��4��β�38��(���D�M���w)
						CAudio::Instance()->Play(AUDIO_SAD, false);
					else if (p1->end == 31)										//�Y�ت��a���d�b��30��(�i�ʺ�)
						CAudio::Instance()->Play(AUDIO_SAD, false);
					else
						CAudio::Instance()->Play(AUDIO_WAN1, false);
				}
			}
			else {																//�Y�H���쥻��ƥ[�W�B�ƶW�L40
				int newend = p1->end - 40;										//�q�L���I�᪺�ت��a���
				if (p1->position < 40 && p1->position >= 31) {					//�q�L���I�e���y�Ч���
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
				if (p1->position == 40) {										//�q�L���I��
					if (count != 49) {
						Y -= 3;
						Sleep(1);
						count++;
					}
					else {
						p1->position = 1;										//�N�H����m���]��1(���I)
						p1->end = newend;										//�N�H���ت��a�]���q�L���I��s�ت��a
						p1->money = p1->money + 200;							//�q�L���I��o200��
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
		da.AddBitmap(6);         //��l�ʵe�[�J1~6����
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
			switch (num)                    //�P�_��l�Y�X�Ӫ��Ȭ�1~6���
			{                               //���O����Y�X���Ʀr
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
		else                                  //�p�G���ⰱ��
			da.OnShow();					  //�h�~���l���ʵe
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
		for (int i = 0; i < 40; i++)			//�Narray���C��land��l��
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
		for (int i = 0; i <= 39; i++) {								//��1����40��
			if (landarray[i].Belongsto() == 1) {					//�Y��i���ݩ󪱮a1
				x = findbelongx(i + 1) + bmap->RX();				//�P�a�ϥ��W���۹�y�� + �a�ϧ���
				y = findbelongy(i + 1) + bmap->RY();
				HH.SetTopLeft(x, y);
				HH.ShowBitmap();									//��ܪ��a1���Y��
			}
			if (landarray[i].Belongsto() == 2) {					//�Y��i���ݩ󪱮a2
				x = findbelongx(i + 1) + bmap->RX();				
				y = findbelongy(i + 1) + bmap->RY();
				SH.SetTopLeft(x, y);
				SH.ShowBitmap();									//��ܪ��a2���Y��
			}
		}
	}

	void Buy::Showhouse() 
	{
		int x, y;
		for (int i = 0; i <= 39; i++) {								//��1����40��
			if (landarray[i].House() == 1) {						//�Y��i�檺�Фl�� = 1
				x = findhousex(i + 1) + bmap->RX();					//�P�a�ϥ��W���۹�y�� + �a�ϧ���
				y = findhousey(i + 1) + bmap->RY();
				H1.SetTopLeft(x+10, y+10);
				H1.ShowBitmap();									//���1�ũФl�Ϥ�
			}
			if (landarray[i].House() == 2) {						//�Y��i�檺�Фl�� = 2
				x = findhousex(i + 1) + bmap->RX();
				y = findhousey(i + 1) + bmap->RY();
				H2.SetTopLeft(x+10, y+10);
				H2.ShowBitmap();									//���2�ũФl�Ϥ�
			}
			if (landarray[i].House() == 3) {						//�Y��i�檺�Фl�� = 3
				x = findhousex(i + 1) + bmap->RX();
				y = findhousey(i + 1) + bmap->RY();
				H3.SetTopLeft(x+10, y+10);
				H3.ShowBitmap();									//���3�ũФl�Ϥ�
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
		switch (n) {								//�P�_�ƥ�
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
			player->inprison = 1;				//�𮧤@�^�X
			break;
		case 8:
			player->money += 200;
			break;
		case 9:
			player->position = 24;				//�e�i������y������
			break;
		case 10:
			player->position += 2;
			break;
		case 11:
			player->inprison = 3;				//���i�ʺ�
			player->position = 11;
			break;
		default:
			break;
		}
	}

	void Buy::OnShow(charactor * pplayer, int belongto) 
	{
		if (running == true) {					//�Y�ʶR�}�l
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
				if (belongto == 0) {			//�Y�g�a�L�H�֦��h��ܬO�_�ʶR����
					Q.ShowBitmap();
					choice.ShowBitmap();
				}
				else if (belongto == pplayer->playernum) {			//�Y�g�a���ۤv�֦���ܬO�_�\�Фl
					if (landarray[pplayer->end - 2].House() < 3) {	//�Фl�W����3��
						build.ShowBitmap();
						choice.ShowBitmap();
					}
				}
				else
					belong.ShowBitmap();		//�g�a�w���H�֦���ܦ���
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
			case 2:					//�ƥ��
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
			case 4:					//���D��
				F.ShowBitmap();
				break;
			case 38:				//���w��
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
		player->money = player->money - landarray[player->end - 2].Price();			//����
		landarray[player->end - 2].Buy(player->playernum);							//�R�a
	}

	void Buy::Paymoney(charactor * player)
	{
		int n = landarray[player->end - 2].House() * 30;							//�p��Фl��
		player->money = player->money - landarray[player->end - 2].Rent() - n;		//����
	}

	void Buy::Getmoney(charactor * player)
	{
		int n = landarray[player->end - 2].House() * 30;							//�p��Фl��
		player->money = player->money + landarray[player->end - 2].Rent() + n;		//�[��
	}

	bool Buy::Checkbelong(int p)
	{
		if (landarray[p - 2].Belongsto() == 0)				//�Y�Фl�S�H�֦�
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
	// �o��class���C�����C���}�Y�e������
	/////////////////////////////////////////////////////////////////////////////

	CGameStateInit::CGameStateInit(CGame *g)
		: CGameState(g)
	{

	}

	void CGameStateInit::OnInit()
	{
		ShowInitProgress(0);	// �@�}�l��loading�i�׬�0%
								//
								// �}�l���J���
								//
		logo.LoadBitmap(IDB_LOGO);
		run.AddBitmap(7, RGB(255, 255, 255));                 //�p���]�B�ʵe�[�J7,8���Ϥ�
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
			GotoGameState(GAME_STATE_RUN);						// ������GAME_STATE_RUN
		else if (nChar == KEY_ESC)								// Demo �����C������k
			PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);	// �����C��
	}

	void CGameStateInit::OnLButtonDown(UINT nFlags, CPoint point)
	{
		if (point.x <= 210 && point.x >= 60 && point.y <= 390 && point.y >= 340)       //Start����
		{
			GotoGameState(GAME_STATE_RUN);		                                       // ������GAME_STATE_RUN
		}

		if (point.x <= 586 && point.x >= 440 && point.y <= 390 && point.y >= 340) {    //Exit����
			PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);	                   // �����C��
		}

		if (point.x <= 400 && point.x >= 250 && point.y <= 390 && point.y >= 340) {    //Help����
			GotoGameState(GAME_STATE_HELP);		                                       // ������GAME_STATE_HELP ��������
		}
	}

	void CGameStateInit::OnMove()
	{
		run.OnMove();                              //�p���]�B�ʵe
	}

	void CGameStateInit::OnShow()
	{
		//
		// �K�Wlogo�P�p���ʵe
		//
		run.SetTopLeft(200, 380);
		logo.SetTopLeft(0, 0);
		logo.ShowBitmap();
		//
		// Demo�ù��r�����ϥΡA���L�}�o�ɽкɶq�קK�����ϥΦr���A���CMovingBitmap����n
		//
		run.OnShow();
	}

	/////////////////////////////////////////////////////////////////////////////
	// �o��class���C�����������A(Game Over)
	/////////////////////////////////////////////////////////////////////////////

	CGameStateOver::CGameStateOver(CGame *g)
		: CGameState(g)
	{
	}
	void CGameStateOver::OnBeginState()
	{
	}

	void CGameStateOver::OnLButtonDown(UINT nFlags, CPoint point) {
		if (point.x <= 391 && point.x >= 242 && point.y <= 377 && point.y >= 325)       //Restart����
		{
			GotoGameState(GAME_STATE_INIT);		                                        // ������GAME_STATE_RUN ���s�}�l�C��
		}
		if (point.x <= 391 && point.x >= 242 && point.y <= 453 && point.y >= 400) {     //Exit����
			PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);	                    // �����C��
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
	// �o��class���C�����C�����檫��A�D�n���C���{�����b�o��
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
		CAudio::Instance()->Play(AUDIO_DING, false);		// ���� WAVE
		CAudio::Instance()->Play(AUDIO_BGM, true);			// ���� MIDI
		player1.money = 3000;                               //player1������l��
		player2.money = 3000;                               //player2������l��
		player1.position = 1;                               //player1����m��l��
		player2.position = 1;                               //player2����m��l��
		player1.step = 0;									//player1�n�����B��
		player2.step = 0;									//player2�n�����B��
		gamemap.Setp1(player1);								//��p1����player1
		gamemap.showmap(player1.position);					//���a����ܦbplayer1����m
		money.SetTopLeft(0, 0);								//������ܦb���W��
		money.SetInteger(player1.money);					//�������player1�������B
		playernum = 1;										//��F�{�b�����a��player1
		gamemap.Setdice(dd);								//��gamemap�̭���d����dd
		gamemap.Setland(land);								//��gamemap�̭���l����land
		player1.X = 200;									//player1��X�y��	
		player1.Y = 250;									//player1��Y�y��	
		player2.X = 200;									//player2��X�y��	
		player2.Y = 250;									//player2��Y�y��	
		player1.ismoving = false;							//�]�wplayer1�Pplayer2���R��
		player2.ismoving = false;							//
		husky.SetTopLeft(player1.X, player1.Y);				//�]�wplayer1�Pplayer2�Ϫ���m
		sibainu.SetTopLeft(player2.X, player2.Y);			//
		player1.mapX = gamemap.RX();						//���a����ܦbplayer1����m
		player1.mapY = gamemap.RY();
		player2.mapX = gamemap.RX();						//���a����ܦbplayer1����m
		player2.mapY = gamemap.RY();
		player1.playernum = 1;								//�]�wplayer1�Pplayer2�Hplayernum���
		player2.playernum = 2;
		player1.inprison = 0;								//�]�wplayer1�Pplayer2�ʺ�����l��
		player2.inprison = 0;
		land.Initial();										//�]�w�g�a����l��
	}

	void CGameStateRun::OnMove()							// ���ʹC������
	{
		if (gamemap.ifmoving() == false)					//�p�G�a�ϵ������ɬ��R�
		{
			if (playernum == 1)
				gamemap.showmap(player1.position);			//�a����ܦbplayer1����m
			if (playernum == 2)
				gamemap.showmap(player2.position);			//�a����ܦbplayer2����m
		}
		gamemap.OnMove();									//�ҥΦa�ϡB��l�B�g�a
		dd.OnMove();
		if (playernum == 1)														//�{�b�����a��player1
		{	
			money.SetInteger(player1.money);									//���player1���l�B
			player2.mapX = gamemap.RX() + gamemap.findx(player2.position);		//player2�b�����W��ܷ|�H�ۦa�ϲ��ʦӧ���
			player2.mapY = gamemap.RY() + gamemap.findy(player2.position);
		}
		if (playernum == 2)														//�{�b�����a��player2
		{
			money.SetInteger(player2.money);									//���player2���l�B
			player1.mapX = gamemap.RX() + gamemap.findx(player1.position);		//player1�b�����W��ܷ|�H�ۦa�ϲ��ʦӧ���
			player1.mapY = gamemap.RY() + gamemap.findy(player1.position);
		}

		if (player1.money <= 0) {												//
			GotoGameState(GAME_STATE_OVER);										//
		}																		//���@���a�l�B�p��0�ɹC���Y����
		if (player2.money <= 0) {												//����game over�e��
			GotoGameState(GAME_STATE_OVER);										//
		}																		//
	}

	void CGameStateRun::OnInit()  								// �C������Ȥιϧγ]�w
	{
		background.LoadBitmap(IDB_BACKGROUND);					       // ���J�I�����ϧ�
		CAudio::Instance()->Load(AUDIO_DING, "sounds\\ding.wav");	   // ���J�s��0���n��ding.wav
		CAudio::Instance()->Load(AUDIO_BGM, "sounds\\BGM.mp3");	       // ���J�I������BGM.mp3
		CAudio::Instance()->Load(AUDIO_DICE, "sounds\\dice.mp3");	   // ���J�Y��l������dice.mp3
		CAudio::Instance()->Load(AUDIO_WAN, "sounds\\wan.mp3");		   // ���J���s�L�L�L������wan.mp3
		CAudio::Instance()->Load(AUDIO_CHOOSE, "sounds\\click.mp3");   // ���J��ܪ�����click.mp3
		CAudio::Instance()->Load(AUDIO_BIU, "sounds\\biu.mp3");        // ���J�T�{���䪺����biu.mp3
		CAudio::Instance()->Load(AUDIO_WAN1, "sounds\\wan1.mp3");      // ���J���s�L������wan1.mp3
		CAudio::Instance()->Load(AUDIO_BUILD, "sounds\\build.mp3");    // ���J�\�Фl������build.mp3
		CAudio::Instance()->Load(AUDIO_SAD, "sounds\\sad.wav");        // ���J�s�˪�����sad.mp3
		husky.LoadBitmap(IDB_HUSKY, RGB(0,0,0));		        	   //���J���⫢�h�_����
		sibainu.LoadBitmap(IDB_SIBAINU, RGB(255, 255, 255));		   //���J����������
		gamemap.LoadBitmap();										   //���J�a��
		money.LoadBitmap();											   //���J������ܹ�
		dd.AddBitmap();												   
		dd.SetTopLeft();											   
		land.AddBitmap();											   
		land.SetTopLeft();											   
		smallmap.AddBitMap();										   
		smallmap.Setboolfalse();									   //�]�w���S����ܤp�a��
		land.SetBmap(gamemap);										   //��land�̭���bmap����gamemap
	}

	void CGameStateRun::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		const char KEY_SPACE = 0x20;// keyboard�ť���
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
		const char KEY_LEFT = 0x25; // keyboard���b�Y
		const char KEY_UP = 0x26; 	// keyboard�W�b�Y
		const char KEY_RIGHT = 0x27;// keyboard�k�b�Y
		const char KEY_DOWN = 0x28; // keyboard�U�b�Y
		charactor *pplayer;
		charactor *pplayer2;
		if (playernum == 1)										//
		{														//
			pplayer = &player1;									//�̷�ɪ����a����
			pplayer2 = &player2;								//�h�]�w���P������
		}														//pplayer��ɽ��쪺���a
		if (playernum == 2)										//pplayer2�h�O�t�@��
		{														//
			pplayer = &player2;									//	
			pplayer2 = &player1;								//
		}
		if (nChar == KEY_ENTER)
		{
			if (land.ReturnBool() == true)
			{
				if (land.Checkbelong(pplayer->end) == false)		//�Y�g�a�S�H�֦�
				{
					switch (pplayer->end - 1) {
					case 1:										    //�g�a��
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
							land.Buyland(pplayer);					//�ʶR�g�a�禡
						}
						break;
					case 4:											//�����ƥ��
					case 38:
						land.Paymoney(pplayer);						//��������
						CAudio::Instance()->Play(AUDIO_BIU, false);
						break;
					case 2:											//���|�R�B��
					case 7:
					case 17:
					case 22:
					case 33:
					case 37:
						land.GB(pplayer);							//����GB�禡(�H���ͦ����|�R�B�ƥ�)
						break;
					case 30:										//�J����
						pplayer->position = 11;
						pplayer->inprison = 3;
						break;
					}
				}
				else if (playernum != land.Belongsto(pplayer->end))								//�Y�g�a���H�֦�
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
						land.Paymoney(pplayer);						//��������
						land.Getmoney(pplayer2);					//�t�@�쪱�a�[��
						break;
					}
				}
				else if (land.houselevel(pplayer->end) < 3)		    //�Y�Фl���Ťp��3
				{
					if (land.Decision() == true)					//���a����ʶR
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
						pplayer2->inprison--;						//�P�_���a�O�_�F���J������
					break;											//�Y�O�h�����t�@�쪱�a
				case 2:												//����J���ѤU���^�X�Ƭ��s
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
				gamemap.Setpnum(playernum);								//�]�w�{�b���a����
				pplayer->ismoving = true;
				pplayer->step = dd.dice();								//�N��l�Y�X�Ӫ��I�ƫ��쪱�a���ʨB��
				pplayer->end = pplayer->position + pplayer->step;		//���a�̫᪺��m���쥻����m�[�W���ʨB��
				dd.SetNum(pplayer->step);
				dd.SetboolTrue();
				gamemap.Setmoving(true);
				land.SetboolFalse();
				money.SetInteger(pplayer->money);						//������ܲ{�b���a���l�B
				if (pplayer->end == 3 || pplayer->end == 8 || pplayer->end == 18 || pplayer->end == 23 || pplayer->end == 34 || pplayer->end == 38)
				{														//�Y���a������|�R�B��
					srand((unsigned)time(NULL));						//�H������1~11���N�Xn
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
					CAudio::Instance()->Play(AUDIO_CHOOSE, false);		//��ܽT�{������
				}
			}
		if (nChar == KEY_RIGHT)
			if (land.ReturnBool() == true)
			{
				if (pplayer->end != 3 && pplayer->end != 5 && pplayer->end != 8 && pplayer->end != 18 && pplayer->end != 23 &&
					pplayer->end != 34 && pplayer->end != 38 && pplayer->end != 39 && pplayer->end != 1)
				{
					land.SetDecisionFalse();
					CAudio::Instance()->Play(AUDIO_CHOOSE, false);		//��ܽT�{������
				}
			}
		if (nChar == KEY_UP)											//
			pplayer->money = pplayer->money + 100;						//*�K��*
		if (nChar == KEY_DOWN)											//�Q�ΤW�U��i�H�W�[��ֿ�
			pplayer->money = pplayer->money - 100;						//
		if (nChar == KEY_M)												//���UM��i�H��ܤp�a��
			smallmap.Setbooltrue();										//
		if (nChar == KEY_1 || nChar == KEY_2 || nChar == KEY_3 || nChar == KEY_4 || nChar == KEY_5 || nChar == KEY_6)
		{
			int n;
			switch (nChar)												//*�K��*
			{															//���U1~6���Ʀr��
			case KEY_1:													//�i�H�����]�w��l���I��
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
				pplayer->position = 11;											//*�K��*
		if (nChar == KEY_8)														//���U7~0�r��
			if (gamemap.ifmoving() == false && land.ReturnBool() == false)		//�i���O�����a�������ʨ��10,20,30,�_�I��
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
		const char KEY_LEFT = 0x25; // keyboard���b�Y
		const char KEY_UP = 0x26; // keyboard�W�b�Y
		const char KEY_RIGHT = 0x27; // keyboard�k�b�Y
		const char KEY_DOWN = 0x28; // keyboard�U�b�Y
		const char KEY_M = 0x4D;
		const char KEY_ESC = 27;
		if (nChar == KEY_M)
			smallmap.Setboolfalse();
		if (nChar == KEY_ESC)								// Demo �����C������k
			PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);	// �����C��
	}

	void CGameStateRun::OnShow()
	{
		//
		//  �`�N�GShow�̭��d�U���n���ʥ��󪫥󪺮y�СA���ʮy�Ъ��u�@����Move���~��A
		//        �_�h��������sø�Ϯ�(OnDraw)�A����N�|���ʡA�ݰ_�ӷ|�ܩǡC���ӳN�y
		//        ���AMove�t�dMVC����Model�AShow�t�dView�A��View�������Model�C
		//
		gamemap.OnShow();
		land.Showbelong();										//����k��
		land.Showhouse();										//��ܩФl
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
		money.ShowBitmap();										//��ܪ���
		dd.Onshow();											//��ܻ�l
		charactor * pplayer;
		if (playernum == 1)
			pplayer = &player1;
		if (playernum == 2) 
			pplayer = &player2;
		land.OnShow(pplayer, land.Belongsto(pplayer->end));		//����k��
		land.SetTopLeft();
		smallmap.SetTopLeft(player1.position, player2.position);
		smallmap.OnShow(land);									//��ܤp�a��


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
		if (point.x <= 55 && point.x >= 10 && point.y <= 55 && point.y >= 7) {					//��^��
			GotoGameState(GAME_STATE_INIT);														//���ܥD�e��
		}
		if (point.x <= 590 && point.x >= 565 && point.y <= 468 && point.y >= 444) {				//����e�@��
			if (click>0)
				click -= 1;
		}
		if (point.x <= 620 && point.x >= 595 && point.y <= 468 && point.y >= 444) {				//�k��U�@��
			if (click <2)
				click += 1;
		}
	}

	void CGameStateHelp::OnShow() {
		switch (click)							//click�x�s�ƹ��I�X��
		{
		case 0:									//��ܲĴX��
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
