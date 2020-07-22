#include<stdio.h>
#include<fstream.h>
#include<stdlib.h>
#include<graphics.h>
#include<time.h>
#include<conio.h>
#include<dos.h>
#include<string.h>

int myscore=0,v=0;
//score as string
char * scorestr;
//level as string
char * tempstr;
class highscore
{
	char name[20];
	int score;
	public:
	char*getname()
	{
		return name;
	}
	int getscore()
	{
		return score;
	}
	void display()
	{
		setcolor(12);
		settextstyle(6,0,1);
		outtextxy(50,60+v,"Name :");
		outtextxy(120,60+v, name);
		outtextxy(50,80+v,"Score :");
		itoa(score,scorestr,10);
		outtextxy(120,80+v, scorestr);
	}
	void insert()
	{
		settextstyle(6,0,1);
		setcolor(4);
		gotoxy(28,5);
		outtextxy(70,54,"Enter your UserName:");
		gets(name);
	}
	void setscore(int h)
	{
		score=h;
	}
	highscore()
	{
		score=myscore;
	}

};
highscore h,s1;
//The tetris matrix (2d array)
int board[35][13];
//Different versions of T block (rotated)
int t_[8]={1,0,1,1,1,2,2,1};
int t_90[8]={0,1,1,0,1,1,2,1};
int t_180[8]={0,1,1,0,1,1,1,2};
int t_270[8]={0,1,1,1,1,2,2,1};
//Different versions of L block (rotated)
int l_[8]={0,2,1,0,1,1,1,2};
int l_90[8]={0,1,1,1,2,1,2,2};
int l_180[8]={1,0,1,1,1,2,2,0};
int l_270[8]={0,0,0,1,1,1,2,1};
//S block
int isAtBottom();
int s_[8]={0,0,0,1,1,0,1,1};
//Different versions of Z block
int z_[8]={1,1,1,2,2,0,2,1};
int z_90[8]={0,1,1,1,1,2,2,2};
//Different versions of I block

int i_ver[8]={0,1,1,1,2,1,3,1};
int i_hor[8]={1,0,1,1,1,2,1,3};

/*
Numbering for type of blocks:
(values of fallingBlockNumber)
0=T
1=L
2=S
3=Z
4=I
*/
//pointer to current falling block type array
int *blockarray;
int fallingblockNum;
int fallingBlockVersion=0;//values 0 to 4
int fallingBlockRow=0;//the current row of falling block
int fallingBlockCol=0;//the current column of falling block
long double startdelay=200;
long double motiondelay;
int scoreInc=5;//score for each line clearance
char buffer[10];
int X=120,Y=30,H=22,W=22;
int Tl=12,Th=20;
int level=1;
int lines=0;
int scorespeedctrl=0;


void reset_scores();
void credits();
void menu();
void highscores();
void sort();
void lost();

//function to draw in game controls screen
void Intro()
{

	cleardevice();
	setcolor(15);
	settextstyle(0,0,4);
	for(int i=0;i<4;i++)
		outtextxy(260+i,30+i,"CONTROLS");
	settextstyle(0,0,2);
	setcolor(4);
	for(int k=0;k<=3;k++)
	{
		outtextxy(100+k,100+30-k,"Left  Key  : Move Left");
		outtextxy(100+k,130+30-k,"Right Key  : Move Right");
		outtextxy(100+k,160+30-k,"Up    Key  : Rotate");
		outtextxy(100+k,190+30-k,"Down  Key  : Increase Speed");
		outtextxy(100+k,220+30-k,"Q          : Quit the game");
		outtextxy(100+k,380+30-k,"Press up arrow to continue...");
		if(k==2)
			setcolor(12);
	}
	for(int _i=0;_i<1000;_i++)
		putpixel(random(640),random(480),random(16));
}
// function to draw controls
void controls()
{

	cleardevice();
	setcolor(15);
	settextstyle(0,0,4);
	for(int i=0;i<4;i++)
		outtextxy(140+i,5+i,"CONTROLS");
	settextstyle(0,0,2);
	setcolor(4);
	for(int k=0;k<=3;k++)
	{
		outtextxy(100+k,100+30-k,"Left  Key  : Move Left");
		outtextxy(100+k,130+30-k,"Right Key  : Move Right");
		outtextxy(100+k,160+30-k,"Up    Key  : Rotate");
		outtextxy(100+k,190+30-k,"Down  Key  : Increase Speed");
		outtextxy(100+k,220+30-k,"Q          : Quit the game");
		outtextxy(100+k,380+30-k,"Press any key to continue...");
		if(k==2)
			setcolor(12);
	}
	for(int _i=0;_i<1000;_i++)
		putpixel(random(640),random(480),random(16));
	getch();
}
// function to draw 3D text
void Text3D(int a,int b,int depth,int bgcolor,int fgcolor,char*caption)
{
	setcolor(bgcolor);
	for(int i=0;i<depth;i++)
	{
		outtextxy(a+i,b+i,caption);
	}
	setcolor(fgcolor);
	outtextxy(a+i,b+i,caption);
}
//function to draw intro screen
void intro()
{
	settextstyle(0,0,2);
	Text3D(230,200,2,3,11,"Loading..");
	settextstyle(0,0,0);
	for(int i=0;i<1000;i++)
	{
		putpixel(random(640),random(480),random(16));
	}
	delay(1500);
	cleardevice();
	for(int l=0;l<1000;l++)
	{
		putpixel(random(640),random(480),random(16));
	}

	delay(1500);
	cleardevice();

	settextstyle(0,0,2);
	Text3D(230,200,2,3,11,"a game by");
	Text3D(150,240,2,3,11,"KARTHIK,MANAV,PRATHEEK");
	settextstyle(0,0,0);
	for(int k=0;k<1000;k++)
	{
		putpixel(random(640),random(480),random(16));
	}

	delay(1500);
	cleardevice();

	settextstyle(0,0,7);
	Text3D(150,150,4,3,11,"TETRIS");
	settextstyle(0,0,2);
	Text3D(100,300,2,3,11,"(c) 2019 CODEZERA");
	settextstyle(0,0,0);
	for(int j=0;j<2000;j++)
	{
		putpixel(random(640),random(480),random(16));
	}
	delay(1500);
	cleardevice();
}

//spawning next falling block
void NextBlock()
{
	fallingblockNum=random(5);
	if(fallingblockNum==0||fallingblockNum==1)
		fallingBlockVersion=random(4);
	else if(fallingblockNum==4||fallingblockNum==3)
		fallingBlockVersion=random(2);
	else
		fallingBlockVersion=0;
	fallingBlockRow=0;
	fallingBlockCol=5;
}
//store pointer to current falling block's array in *blockarray;
int *getFallingBlockArray()
{
	int a=fallingblockNum*10+fallingBlockVersion;
	switch(a)
	{
		case 0:return (t_);
		case 1:return (t_90);
		case 2:return (t_180);
		case 3:return (t_270);
		case 10:return (l_);
		case 11:return (l_90);
		case 12:return (l_180);
		case 13:return (l_270);
		case 20:return (s_);
		case 30:return (z_);
		case 31:return (z_90);
		case 40:return (i_hor);
		case 41:return (i_ver);
	}
	return (i_ver);
}
/* To check if moving to left or right
or rotation of falling block is feasible.
i.e to check whether there is enough space around it.
*/
int isDrawable(int newrow,int newcol,int blockversion)
{
	int i,tempversion,flag=1;;
	tempversion=fallingBlockVersion;
	fallingBlockVersion=blockversion;
	blockarray=getFallingBlockArray();
	for(i=0;i<8;i+=2)
	{
		//check if block goes out of the matrix (up or down)
		if(newrow+blockarray[i]>34||newrow+blockarray[i]<0)
		{
			flag=0;
			break;
		}
		//check if block goes out of the matrix (left or right)
		if(newcol+blockarray[i+1]>12||newcol+blockarray[i+1]<0)
		{
			flag=0;
			break;
		}
		//check if there is any obstacle on the proposed position
		if(board[(newrow+blockarray[i])][(newcol+blockarray[i+1])]==2)
		{
			flag=0;
			break;
		}
	}
	fallingBlockVersion=tempversion;
	blockarray=getFallingBlockArray();
	return flag;
}

/*
To clear the old position of falling block
after it moves to a new position (row/column change)
or after it is rotated
*/
void clearOldBlockVersion()
{
	int i,r,c;
	for(i=0;i<8;i+=2)
	{
		r=fallingBlockRow+blockarray[i];
		c=fallingBlockCol+blockarray[i+1];
		board[r][c]=0;
		setfillstyle(EMPTY_FILL,BLACK);
		setcolor(BLACK);
		bar(2+c*13,2+r*13,2+c*13+13,2+r*13+13);
		rectangle(2+c*13,2+r*13,2+c*13+13,2+r*13+13);
	}
}
/*

To draw the new picture

after it moves to a new position (row/column change)

or after it is rotated

*/

void drawNewBlockVersion()
{
	int i,r,c;
	for(i=0;i<8;i+=2)
	{
		r=fallingBlockRow+blockarray[i];
		c=fallingBlockCol+blockarray[i+1];
		board[r][c]=1;
		setfillstyle(XHATCH_FILL,CYAN);
		setcolor(GREEN);
		bar(2+c*13,2+r*13,2+c*13+13,2+r*13+13);
		rectangle(2+c*13,2+r*13,2+c*13+13,2+r*13+13);
	}
}

/*

To check whether game is over.

called after every move.

Game is over when tetris matrix is filled.

*/


int isGameOver()
{
	if(isDrawable(0,5,fallingBlockVersion)==0)
		return 1;
	drawNewBlockVersion();
	if(isAtBottom())
		return 1;
	return 0;
}
/*
To check if falling block reached the bottom
*/
int isAtBottom()
{
	int i,max=0,ti,tj;
	for(i=0;i<8;i+=2)
		if(*(blockarray+i)>max)
			max=*(blockarray+i);
	if(fallingBlockRow+max>=34)
		return 1;
	for(i=0;i<8;i+=2)
	{
		ti=*(blockarray+i)+fallingBlockRow;
		tj=*(blockarray+i+1)+fallingBlockCol;

		if(board[ti+1][tj]==2)
			return 1;
	}
	return 0;
}
/*
Draw score on screen
*/
void showScore()
{
	int left,top;
	setcolor(BLACK);
	setfillstyle(EMPTY_FILL,BLACK);
	left=getmaxx()-100;
	top=getmaxy()/2;
	bar3d(left,top,left+60,top+70,2,1);
	setcolor(YELLOW);
	settextstyle(TRIPLEX_FONT, HORIZ_DIR,2);
	setfillstyle(SOLID_FILL,11);
       bar(X+(Tl*W)+7,Y-1,X+(Tl*W)+4*W+(4*W),Y+(5*H));

       setfillstyle(SOLID_FILL,14);
       bar(X+(Tl*W)+7,Y+(5*H)+2,X+(Tl*W)+4*W+(4*W),Y+(10*H));

       setfillstyle(SOLID_FILL,10);
       bar(X+(Tl*W)+7,Y+(10*H)+2,X+(Tl*W)+4*W+(4*W),Y+(15*H));

       setfillstyle(SOLID_FILL,12);
       bar(X+(Tl*W)+7,Y+(15*H)+2,X+(Tl*W)+4*W+(4*W),Y+(20*H)+3);

       setcolor(13);
       rectangle(X+(Tl*W)+5,Y-4,X+(Tl*W)+4*W+4*W+1,Y+20*H+4);

       setcolor(RED);
       settextstyle(SMALL_FONT, HORIZ_DIR,6);
       outtextxy(getmaxx()-230,getmaxy()/2-150,"Press 'Q' to quit.");
       settextstyle(0,0,2);
       setcolor(8);
       outtextxy(X+(Tl*W)+7+2*W,Y+(5*H)+2+H,"Score");
       outtextxy(X+(Tl*W)+7+2*W,Y+(10*H)+2+H,"Level");
       outtextxy(X+(Tl*W)+7+2*W,Y+(15*H)+2+H,"Lines");

       setcolor(0);

       outtextxy(X+(Tl*W)+7+3*W,Y+(5*H)+2+3*H,itoa(myscore,buffer,10));
       outtextxy(X+(Tl*W)+7+3*W,Y+(10*H)+2+3*H,itoa(level,buffer,10));
       outtextxy(X+(Tl*W)+7+3*W,Y+(15*H)+2+3*H,itoa(lines,buffer,10));

       settextstyle(0,0,0);

}


/*

To clear any row that is fully filled and increment score.

Also settles rows over the cleared row down.

*/

void CollapseFullRow()
{
	int i,j,k,sum,copyskipover=0,r;
	for(i=34;i>=0;)
	{
		sum=0;//full flag
		for(j=0;j<13;j++)
			sum+=board[i][j];
		if(sum==2*13)//row full
		{
			myscore+=scoreInc;
			h.setscore(myscore);
			s1.setscore(myscore);
			copyskipover++;
			lines++;
		}
		if(sum==0)
			break;
		i--;
		if(copyskipover>0)
		{
			for(j=0;j<13;j++)
			{
				r=i+copyskipover;
				board[r][j]=board[i][j];
				if(board[i][j]==0)
				{
					setfillstyle(EMPTY_FILL,BLACK);//empty yo hatch
					setcolor(BLACK);
					bar(2+j*13,2+r*13,2+j*13+13,2+r*13+13);
				}
				else
				{
					setfillstyle(XHATCH_FILL,CYAN);
					setcolor(GREEN);
					bar(2+j*13,2+r*13,2+j*13+13,2+r*13+13);
					rectangle(2+j*13,2+r*13,2+j*13+13,2+r*13+13);
				}
			}
		}
	}
	for(k=0;k<copyskipover;k++)
	{
		r=i+k;
		for(j=0;j<13;j++)
		{
			board[r][j]=0;
			setfillstyle(XHATCH_FILL,BLACK);
			setcolor(BLACK);
			bar(2+j*13,2+r*13,2+j*13+13,2+r*13+13);
		}
	}
	showScore();
}
void game()
{
	clrscr();
	char arrowpre,ch,timehalving=0;
	int gd=DETECT,gm,i,j,k=1,spawn=1,tversion;
	initgraph(&gd,&gm,"C:\\TURBOC3\\BGI\\");
	intro();
	Intro();
	motiondelay=startdelay;
	scorestr=(char*)malloc(sizeof(char)*10);
	for(i=0;i<35;i++)
		for(j=0;j<13;j++)
			board[i][j]=0;
/*
to get the ascii characters for up arrow key.
When any of arrow key is pressed, two characters
are sent to buffer.
The second one is of value 72 for up arrow.
*/
	while(k!=72)
	{
		arrowpre=getch();
		if(kbhit())
			k=getch();
	}

	initgraph(&gd,&gm,"C:\\TURBOC3\\BGI\\");
	cleardevice();
	setcolor(RED);
	rectangle(0,0,13*13+6,35*13+6);
	srand(time(NULL));
	showScore();
	setcolor(RED);
	settextstyle(SMALL_FONT, HORIZ_DIR,6);
	while(ch!='q'&&ch!='Q'&&k!='q'&&k!='Q')
	{
		if(kbhit()&&spawn==0)
		{
			ch=getch();
			if(ch==arrowpre)//if an arrow key pressed
			{
				k=getch();
				if(k==72)//if up arrow key is pressed
				{
					if(fallingblockNum==0||fallingblockNum==1)
						tversion=(fallingBlockVersion+1)%4;
					else if(fallingblockNum==4||fallingblockNum==3)
						tversion=(fallingBlockVersion+1)%2;

					if(fallingblockNum!=2&&isDrawable(fallingBlockRow,fallingBlockCol,tversion))
					{
						clearOldBlockVersion();
						fallingBlockVersion=tversion;
						blockarray=getFallingBlockArray();
						drawNewBlockVersion();
					}
				}
			else if(k==75)//if left arrow key is pressed
			{
				if(isDrawable(fallingBlockRow,fallingBlockCol-1,fallingBlockVersion))
				{
					clearOldBlockVersion();
					fallingBlockCol--;
					drawNewBlockVersion();
				}
			}
			else if(k==77)//if right arrow key is pressed
			{
				if(isDrawable(fallingBlockRow,fallingBlockCol+1,fallingBlockVersion))
				{
					clearOldBlockVersion();
					fallingBlockCol++;
					drawNewBlockVersion();
				}
			}
			else if(k==80)//if down arrow key is pressed
			{
				if(isDrawable(fallingBlockRow+1,fallingBlockCol,fallingBlockVersion))
				{
					clearOldBlockVersion();
					fallingBlockRow++;
					drawNewBlockVersion();
				}
			}
			while(kbhit())
				getch();
		}
	}
	if(isAtBottom()&&spawn==0)
	{
		for(i=0;i<8;i+=2)
		{
			board[fallingBlockRow+blockarray[i]][fallingBlockCol+blockarray[i+1]]=2;
		}
		spawn=1;
		CollapseFullRow();
	}
	if(spawn)
	{
		NextBlock();
		blockarray=getFallingBlockArray();
		spawn=0;
		if(isGameOver())
		{
			lost();
			sort();
			lines=0;
			menu();
			return;
		}
	}
	else
	{
		timehalving=(timehalving+1)%3;
		if(timehalving==2)
		{
			clearOldBlockVersion();
			fallingBlockRow++;
			drawNewBlockVersion();
		}
	}
	scorespeedctrl=(scorespeedctrl+1)%280;
	if(scorespeedctrl==0)
	{
		motiondelay-=8;
		scoreInc++;
	}
	if(myscore<=10)
	{
		level=1;
		delay(80);
	}
	else if(myscore<=30)
	{
		level=2;
		delay(75);
	}
	else if(myscore<=50)
	{
		level=3;
		delay(60);
	}
	else if(myscore<=70)
	{
		level=4;
		delay(55);
	}
	else if(myscore<=90)
	{
		level=5;
		delay(50);
	}
	else if(myscore<=110)
	{
		level=6;
		delay(45);
	}
	else if(myscore<=130)
	{
		level=7;
		delay(40);
	}
	else if(myscore<=150)
	{
		level=8;
		delay(35);
	}
	else if(myscore<=170)
	{
		level=9;
		delay(30);
	}
	else if(myscore<=190)
	{
		level=10;
		delay(25);
	}
	else
	{
		level=9999;
		delay(1);
	}
	}
	sort();
	menu();
	lines=0;
}
void menu()
{
	int gd=DETECT,gm;
	static int choice=0;
	char message[10],a;
	initgraph(&gd,&gm,"c:/TC/BGI");
	cleardevice();
	setcolor(1);
	for(int _i=0;_i<1000;_i++)
		putpixel(random(640),random(480),random(16));

	setfillstyle(SOLID_FILL,14);
	setfillstyle(SOLID_FILL,15);
	setcolor(15);
	setcolor(15);
	settextstyle(0,0,4);
	for(int i=0;i<4;i++)
		outtextxy(180+i,100+i,"TETRIS");

	settextstyle(0,0,2);
	setcolor(4);
	sprintf(message, "PLAY GAME");
	outtextxy(200,250, message);
	sprintf(message, "LEADERBOARD");
	outtextxy(200,275, message);
	sprintf(message, "CONTROLS");
	outtextxy(200,300, message);
	sprintf(message, "DEVELOPERS/EXIT");
	outtextxy(200,325, message);
	settextstyle(0,0,1);
	setcolor(3);
	sprintf(message, "Use 'W' and 'S' keys to navigate and enter to select");
	outtextxy(180,420,message);
	setcolor(15);
	line(200,270+choice,450,270+choice);
	while(1)
	{
		a=getch();
		switch(a)
		{
			case 'w':if(choice>0)
				 {
					setcolor(BLACK);
					line(200,270+choice,450,270+choice);
					choice-=25;
					setcolor(WHITE);
					line(200,270+choice,450,270+choice);
				 }
				 break;
			case 's':if(choice<75)
				 {
					setcolor(BLACK);
					line(200,270+choice,450,270+choice);
					choice+=25;
					setcolor(WHITE);
					line(200,270+choice,450,270+choice);
				 }
				break;
			case 13:switch(choice)
				{
					case 0: myscore=0;
						lines=0;
						level=1;
						game();
						break;
					case 25:highscores();
						break;
					case 50:controls();
						menu();
						break;
					case 75:credits();
						closegraph();
						exit(0);
						break;
				}
		}
	}
}
void credits()
{
	clrscr();
	char key;
	int gd=DETECT,gm;
	char message[100];
	initgraph(&gd,&gm,"c:/TC/BGI");


	cleardevice();
	setcolor(GREEN);
	outtextxy(400,400,"To Go back, Press 'B'");
	setcolor(15);
	settextstyle(0,0,4);
	for(int i=0;i<4;i++)
		outtextxy(260+i,30+i,"CREDITS");
	settextstyle(0,0,2);
	setcolor(4);
	for(int k=0;k<=3;k++)
	{
		outtextxy(100+k,100+30-k,"MANAV ANANTHAKUMAR-12A");
		outtextxy(100+k,130+30-k,"PRATHEEK KAUSHIK-12A");
		outtextxy(100+k,160+30-k,"KARTHIK VEKATESH-12A");
		outtextxy(100+k,250+30-k,"PRESS ANY KEY TO EXIT....");
		if(k==2)
			setcolor(12);
	}
	for(int _i=0;_i<1000;_i++)
		putpixel(random(640),random(480),random(16));
	key=getch();
	if(key=='b')
	{
		menu();
	}
}
void ExistingPlayer()
{
	settextstyle(0,0,2);
	setcolor(RED);
	for(int _i=0;_i<1000;_i++)
		putpixel(random(640),random(480),random(16));
	for(int k=0;k<=3;k++)
	{
		outtextxy(50+k,100+30-k,"Since You Are an existing player");
		outtextxy(50+k,130+30-k,"and have scored higher");
		outtextxy(50+k,160+30-k,"that your existing score");
		outtextxy(50+k,190+30-k,"we will update your score");
		if(k==2)
			setcolor(12);
	}
	getch();
	cleardevice();
	for(_i=0;_i<1000;_i++)
		putpixel(random(640),random(480),random(16));
	setcolor(RED);
	for(int b=0;b<4;b++)
	{
		for(int _i=0;_i<1000;_i++)
			putpixel(random(640),random(480),random(16));
		outtextxy(90+b,50-b,"S");
		outtextxy(130+b,50-b,"C");
		outtextxy(170+b,50-b,"O");
		outtextxy(210+b,50-b,"R");
		outtextxy(250+b,50-b,"E");
		outtextxy(290+b,50-b," ");
		outtextxy(310+b,50-b,"U");
		outtextxy(350+b,50-b,"P");
		outtextxy(390+b,50-b,"D");
		outtextxy(430+b,50-b,"A");
		outtextxy(470+b,50-b,"T");
		outtextxy(510+b,50-b,"E");
		outtextxy(550+b,50-b,"D");
		delay(500);
		cleardevice();
		delay(500);
		cleardevice();
	}
	getch();
}
void HighScorer()
{
	cleardevice();
	for(int _i=0;_i<1000;_i++)
		putpixel(random(640),random(480),random(16));
	settextstyle(0,0,3);
	outtextxy(90,50,"C");
	delay(200);
	outtextxy(120,50,"O");
	delay(200) ;
	outtextxy(150,50,"N");
	delay(200) ;
	outtextxy(180,50,"G");
	delay(200)  ;
	outtextxy(210,50,"R");
	delay(200)   ;
	outtextxy(240,50,"A");
	delay(200)    ;
	outtextxy(270,50,"T");
	delay(200)     ;
	outtextxy(300,50,"U");
	delay(200)     ;
	outtextxy(330,50,"L");
	delay(200)     ;
	outtextxy(360,50,"A");
	delay(200)    ;
	outtextxy(390,50,"T");
	delay(200)     ;
	outtextxy(420,50,"I");
	delay(200)     ;
	outtextxy(450,50,"0");
	delay(200)   ;
	outtextxy(480,50,"N");
	delay(200)   ;
	outtextxy(510,50,"S");
	delay(200);
	settextstyle(0,0,2);
	for(int k=0;k<=3;k++)
	{
		outtextxy(90+k,100+30-k,"You have made the high score");
		outtextxy(90+k,130+30-k,"Well played!");
		if(k==2)
			setcolor(12);
	}
	delay(200);
	getch();
	cleardevice();
}

void LowerScore()
{
	cleardevice();
	for(int _i=0;_i<1000;_i++)
		putpixel(random(640),random(480),random(16));
	settextstyle(0,0,2);
	setcolor(RED);
	for(int k=0;k<=3;k++)
	{
		outtextxy(100+k,100+30-k,"You have played this game before");
		outtextxy(100+k,130+30-k,"Nice try but try beating");
		outtextxy(110+k,160+30-k,"Your previous score");
		if(k==2)
			setcolor(12);
	}
	getch();
}
void sort()
{
	cleardevice();
	setcolor(RED);
	settextstyle(0,0,3);
	for(int f=0;f<4;f++)
	{
		for(int _i=0;_i<1000;_i++)
			putpixel(random(640),random(480),random(16));
		outtextxy(150+f,50+30-f,"G");
		outtextxy(200+f,50+30-f,"O");
		outtextxy(250+f,50+30-f,"O");
		outtextxy(300+f,50+30-f,"D");
		outtextxy(350+f,50+30-f," ");
		outtextxy(400+f,50+30-f,"G");
		outtextxy(450+f,50+30-f,"A");
		outtextxy(500+f,50+30-f,"M");
		outtextxy(550+f,50+30-f,"E");
		delay(500);
		cleardevice();
		delay(500);
		cleardevice();
	}
	highscore s4;
	int m=s1.getscore();
	s4.setscore(m);//Sorts the names of players in descending order of their scores
	fstream fin("score10.dat",ios::binary|ios::in);
	int ins=0;
	int flag=0;
	int k=0;
	for(int _i=0;_i<1000;_i++)
			putpixel(random(640),random(480),random(16));
	s4.insert();
	while(fin.read((char*)&s1,sizeof(h)))
	{
		k++;
		if(k==1)
		{
			if(s4.getscore() > s1.getscore())
			{
				HighScorer();
			}
		}
		if(strcmp(s1.getname(),s4.getname())==0)
		{
			ins=1;
			break;
		}
	}
	fin.close();
	fin.open("score10.dat",ios::binary|ios::in);
	fstream fout("temp10.dat",ios::binary|ios::app);
	if(!fout)
	{
		  cout<<"Error!";
		  return;
	}
	while(fin.read((char*)&s1,sizeof(h)))
	{
		if (ins==1)
		{
			if(strcmp(s1.getname(),s4.getname())==0 && flag==0)
			{
				if(s1.getscore()<s4.getscore())
				{
					ExistingPlayer();
					fout.write((char*)&s4,sizeof(h));
					flag=1;
				}
				else
				{
					LowerScore();
					fout.write((char*)&s1,sizeof(h));
					flag=1;
				}
			}
			else if(s1.getscore() < s4.getscore() && flag==0)
			{
				ExistingPlayer();
				fout.write((char*)&s4,sizeof(h));
				fout.write((char*)&s1,sizeof(h));
				flag=1;
			}
			else if(strcmp(s1.getname(),s4.getname())==0 && flag==1)
			{
				goto again;
			}
			else
			{
				fout.write((char*)&s1,sizeof(h));
			}
		}
		else
		{
			if(s1.getscore() < s4.getscore() && flag==0)
			{
				fout.write((char*)&s4,sizeof(h));
				fout.write((char*)&s1,sizeof(h));
				flag=1;
			}
			else
			{
				fout.write((char*)&s1,sizeof(h));
			}
		}
		again:
	}
	if(flag==0)
	{
		fout.write((char*)&s4,sizeof(h));
	}
	fout.close();
	fin.close();
	remove("score10.dat");
	rename("temp10.dat","score10.dat");
}
void highscores()              //Function for displaying highscores
{
	cleardevice();
	for(int _i=0;_i<1000;_i++)
		putpixel(random(640),random(480),random(16));
	int top=0; //variable for top 10 players
	char key;
	ifstream fin("score10.dat",ios::binary|ios::app);
	setcolor(4);
	settextstyle(0,0,5);
	for(int i=0;i<4;i++)
		outtextxy(100+i,5+i,"HIGH SCORES");
	settextstyle(0,0,1);
	setcolor(3);
	outtextxy(400,200,"To Reset Scores, Press 'R'");
	cout<<endl<<endl<<endl;
	while(fin.read((char *) &s1,sizeof(s1)))
	{
		if(top < 10)
		{
			if(fin.eof())
			{
				break;
			}
			else
			{
				s1.display();
				v+=50;
				top++;
			}
		}
	}
	fin.close();
	key=getch();
	if(key=='r')
	{
		reset_scores();
	}
	menu();
}
//function to display game over and end game
void lost()
{
	cleardevice();
	settextstyle(0,0,4);
	setcolor(RED);
	for(int _i=0;_i<1000;_i++)
		putpixel(random(640),random(480),random(16));
	outtextxy(90,50,"L");
	delay(200);
	outtextxy(130,50,"O");
	delay(200) ;
	outtextxy(170,50,"S");
	delay(200) ;
	outtextxy(210,50,"E");
	delay(200)  ;
	outtextxy(250,50,"R");
	delay(200)   ;
	for(int b=0;b<4;b++)
	{
		for(int _i=0;_i<1000;_i++)
			putpixel(random(640),random(480),random(16));
		outtextxy(90,50,"L");
		outtextxy(130,50,"O");
		outtextxy(170,50,"S");
		outtextxy(210,50,"E");
		outtextxy(250,50,"R");
		delay(500);
		cleardevice();
		delay(500);
		cleardevice();
	}
	getch();
}
void reset_scores()
{
	remove("score10.dat");
	menu();
}
void main()
{
	clrscr();
	int gd=DETECT,gm;
	initgraph(&gd,&gm,"C:\\TURBOC3\\BGI\\");
	menu();
}



