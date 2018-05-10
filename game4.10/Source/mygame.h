/*
* mygame.h: ���ɮ��x�C��������class��interface
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

	enum AUDIO_ID {			// �w�q�U�ح��Ī��s��
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
	// �o��class���C�����C���}�Y�e������
	// �C��Member function��Implementation���n����
	/////////////////////////////////////////////////////////////////////////////

	class CGameStateInit : public CGameState {
	public:
		CGameStateInit(CGame *g);
		void OnInit();  								// �C������Ȥιϧγ]�w
		void OnBeginState();							// �]�w�C�������һݪ��ܼ�
		void OnKeyUp(UINT, UINT, UINT); 				// �B�z��LUp���ʧ@
		void OnLButtonDown(UINT nFlags, CPoint point);  // �B�z�ƹ����ʧ@

	protected:
		void OnShow();
		void OnMove();
		// ��ܳo�Ӫ��A���C���e��
	private:
		CMovingBitmap logo;								// csie��logo
		CAnimation  run;
		CMovingBitmap  s;
		CPoint  point;


	};

	/////////////////////////////////////////////////////////////////////////////
	// �o��class���C�����C�����檫��A�D�n���C���{�����b�o��
	// �C��Member function��Implementation���n����
	/////////////////////////////////////////////////////////////////////////////

	struct charactor
	{
		int mapX;					//���a�P�a�ϥ��W���۹諸X�y��
		int mapY;					//���a�P�a�ϥ��W���۹諸Y�y��
		int X;						//���a��ܪ�X�y��
		int Y;						//���a��ܪ�Y�y��
		int position;				//���a�b�a�Ϫ�����
		int step;					//���a�ҭn���ʪ��B��
		int end;						//���a���ʧ������I
		int money;					//���a������
		bool ismoving;			//���a�O�_�b���ʤ�
		int playernum;			//���a���s��
		int inprison;				//���a�Ȱ����^�X��
	};

	class Dice;
	class GameMap;

	class Land {
	public:
		Land(int b, int r, int p) : block(b), rent(r), price(p), belongsto(0), house(0) {};
		int   Rent() { return rent; };												//�^�Ǥg�a�L���O
		int   Price() { return price; };											//�^�Ǥg�a����
		int   Belongsto() { return belongsto; };								//�^�Ǥg�a�k��
		int   House() { return house; };											//�^�Ǥg�a�Фl�ƶq
		void Buy(int playernum) { belongsto = playernum; };			//�ʶR�g�a
		void BuildHouse() { house++; };											//�\�Фl
		void Init() { house = 0; belongsto = 0; };							//��l��
	protected:
		int block;									//���
		int rent;									//�L���O				
		int price;									//�ʶR���B
		int belongsto;							//�k��
		int house;									//�Фl��
	};

	class Buy {
	public:
		void Initial();							//��l��landarray
		void OnShow(charactor*, int);			//����ʶR�����B�ƥ����
		void SetTopLeft();							//�]�w�U�����y��
		void Buyland(charactor*);					//�ʶR�g�a
		void AddBitmap();								//���J�Ϥ�
		void SetboolFalse();							//�Nrunning�אּfalse
		void SetboolTrue();							//�Nrunning�אּtrue
		bool ReturnBool();							//�^��running
		bool Decision();									//�^��decision
		void SetDecisionTrue();					//�Ndecision�אּtrue
		void SetDecisionFalse();					//�Ndecision�אּfalse
		bool Checkbelong(int p);					//�^�Ǥg�a�O�_���H�֦�
		void Paymoney(charactor*);				//����
		void Getmoney(charactor*);				//�[��
		void SetBmap(GameMap&b);				//�]�w*bmap
		void Showbelong();							//��ܨC���a���k��
		void Showhouse();								//��ܨC���a���Фl
		int findbelongx(int);							//�p���k�ݹϤ���X�y��
		int findbelongy(int);							//�p���k�ݹϤ���Y�y��
		int findhousex(int);							//�p��Фl�Ϥ���X�y��
		int findhousey(int);							//�p��Фl�Ϥ���Y�y��
		int Belongsto(int);							//�^�Ǥg�a�֦��H
		void Buildhouse(int);							//�\�Фl
		void GB(charactor*);						//���|�R�B
		void setn(int i) { n = i; };					//�]�wn
		int houselevel(int);							//�^�ǩФl��
	protected:						
		bool running;										//�H���O�_���ʤ�
		bool decision;									//�ﶵ
		int n;												//���|�R�B�ƥ�s��
		CMovingBitmap Q,B,F,G,H,V;
		CMovingBitmap choice,belong, hot,fire,build;
		CMovingBitmap HH,SH,H1,H2,H3;
		GameMap * bmap;								//����gamestate����GameMap�H�ϥΦa�ϧ���
		Land L1 = Land(1, 25, 70);					//�H�U���]�w�C�@�檺�L���O�P����
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
		Land	landarray[40] = { L1,L2,L3,L4,L5,L6,L7,L8,L9,L10,L11,L12,				//�N�Ҧ���l�˶iarray��K�ϥ�
											L13,L14,L15,L16,L17,L18,L19,L20,L21,L22,	
											L23,L24,L25,L26,L27,L28,L29,L30,L31,
											L32,L33,L34,L35,L36,L37,L38,L39,L40 };
		CMovingBitmap gb1, gb2, gb3, gb4, gb5, gb6, gb7, gb8, gb9, gb10, gb11,gb12, gb13, gb14;
		CMovingBitmap C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11;
	};

	class GameMap
	{
	public:
		GameMap();											//�a�ϼҲ�
		void LoadBitmap();								//���J�a��
		void showmap(int p);								//��ܲ�p��
		void OnShow();										//��ܦa��
		void OnMove();										//����a�ϲ���
		void Setmoving(bool flag);					//�]�w�O�_�b���ʪ�flag
		void Setp1(charactor & player1);			//�]�w*p1
		void Setdice(Dice & dd);						//�]�w*d
		void Setland(Buy & land);						//�]�w*l
		bool ifmoving();										//�^�Ǧa�ϬO�_�b���ʤ�
		int RX();												//�^�Ǧa�ϥ��W����X�y��
		int RY();												//�^�Ǧa�ϥ��W����Y�y��
		void Setpnum(int & num);						//�]�w*pnum
		int findx(int p);									//�^�ǲ�p��P�a�ϥ��W��������X�y��
		int findy(int p);										//�^�ǲ�p��P�a�ϥ��W��������Y�y��
	protected:
		CMovingBitmap game_map;
		int map[13][13];									//�a�ϰ}�C
		int X, Y;												//�a��X,Y�y��
		const int MW, MH;								//�a�Ϫ����e
		bool ismoving;										//�a�ϬO�_�b���ʤ���flag
		int count;												//�p��OnMove���a�ϲ��ʪ�����
		charactor * p1;										//���Vgamestaterun����charactor
		Dice * d;												//���Vgamestaterun����Dice
		Buy * l;													//���Vgamestaterun����Buy
		int * pnum;											//���Vgamestaterun����pnum
	};

	class Dice
	{
	public:
		int dice();						//��l�H���ͦ�1~6�Ʀr
		void AddBitmap();				//��l�ʵe�[�J1~6����
		void Onshow();
		void OnMove();					
		void SetTopLeft();
		void SetboolFalse();			//�]�w�����R��
		void SetboolTrue();				//�]�w���⥿�b����
		bool RetrunBool();				//�^�Ǩ���O�_�b����
		void SetNum(int n);				//�]num����l�Y�X�Ӫ���n
	protected:
		CAnimation da;
		CMovingBitmap d1, d2, d3, d4, d5, d6;
		bool running;
		int num;
	};

	class SmallMap
	{
	public:
		void Setbooltrue();							//�Nisshow�]��true
		void Setboolfalse();							//�Nisshow�]��false
		void AddBitMap();								
		int findx(int p);								//�p��p�a�Ϫ��H��X�y��
		int findy(int p);									//�p��p�a�Ϫ��H��Y�y��
		int findbx(int p);								//�p��p�a�ϤW�k���Y����X�y��
		int findby(int p);								//�p��p�a�ϤW�k���Y����Y�y��
		int findhx(int p);								//�p��p�a�ϤW�Фl��X�y��
		int findhy(int p);								//�p��p�a�ϤW�Фl��X�y��
		void SetTopLeft(int location_of_p1, int location_of_p2);		//�]�w�p�a�ϤW��Ӫ��a���y��
		void OnShow(Buy land);					//��ܤp�a�ϤΤH��
		void Showbelong(Buy);						//����k���Y��
		void Showhouse(Buy);						//��ܩФl
	protected:
		bool isshow;										//�O�_���
		CMovingBitmap smap, sp1, sp2, spb1, spb2, sh1, sh2, sh3;
	};

	class CGameStateRun : public CGameState {
	public:
		CGameStateRun(CGame *g);
		~CGameStateRun();
		void OnBeginState();								// �]�w�C�������һݪ��ܼ�
		void OnInit();  											// �C������Ȥιϧγ]�w
		void OnKeyDown(UINT, UINT, UINT);	// �B�z��LDown���ʧ@
		void OnKeyUp(UINT, UINT, UINT);		// �B�z��LUp���ʧ@
	protected:
		void OnMove();											// ���ʹC������
		void OnShow();											// ��ܳo�Ӫ��A���C���e��
	private:
		CMovingBitmap	background;						// �I����
		CMovingBitmap	help;									// ������
		CMovingBitmap	corner;								// ������
		CMovingBitmap  husky;								// ���⫢�h�_��
		CMovingBitmap	sibainu;								// ��������
		GameMap			gamemap;							// �a��
		charactor			player1;							// ���a1
		charactor			player2;							// ���a2
		CInteger			money;								// ����
		int					playernum;						// ���a���X
		Dice				dd;										// ��l
		CMovingBitmap		roll_pic;						// �Y�X����l��
		Buy					land;									// �ʶR
		SmallMap			smallmap;							// �p�a��
	};

	/////////////////////////////////////////////////////////////////////////////
	// �o��class���C�����������A(Game Over)
	// �C��Member function��Implementation���n����
	/////////////////////////////////////////////////////////////////////////////

	class CGameStateOver : public CGameState {
	public:
		CGameStateOver(CGame *g);
		void OnBeginState();							// �]�w�C�������һݪ��ܼ�
		void OnInit();									// �C������Ȥιϧγ]�w
		void OnLButtonDown(UINT nFlags, CPoint point);  // �B�z�ƹ����ʧ@
	protected:
		void OnShow();									// ��ܳo�Ӫ��A���C���e��
	private:
		int counter;									// �˼Ƥ��p�ƾ�
		CMovingBitmap  over;							//game over��
	};

	class CGameStateHelp : public CGameState {
	public:
		CGameStateHelp(CGame *g);
		void OnInit();									// �C������Ȥιϧγ]�w
		void OnBeginState();							// �]�w�C�������һݪ��ܼ�
		void OnLButtonDown(UINT nFlags, CPoint point);  // �B�z�ƹ����ʧ@
	protected:
		void OnShow();									// ��ܳo�Ӫ��A���C���e��
	private:
		int click = 0;
		CMovingBitmap  b1;								//�������Ĥ@��
		CMovingBitmap  b2;								//�������ĤG��
		CMovingBitmap  b3;								//�������ĤT��
	};
}