#include<iostream>
#include<vector>
#include<stdio.h>
#include<conio.h>
#include<string>
#include<random>
#include<Windows.h>
#include<thread>
#include<fstream>
using namespace std;
#define Black          0
#define Blue           1
#define Green          2
#define Cyan           3
#define Red            4
#define Magenta        5
#define Brown          6
#define LightGray      7
#define DarkGray       8
#define LightBlue      9
#define LightGreen    10
#define LightCyan     11
#define LightRed      12
#define Light Magenta  13
#define Yellow        14
#define White         15


/*struct POINT {
	int x;
	int y;
};

struct COORD {
	int X;
	int Y;
};*/

//Hằng số
#define MAX_CAR 17
#define MAX_CAR_LENGTH 15
#define MAX_SPEED 3
#define MAX_VOLUME 10

//Biến toàn cục
POINT** X = NULL; //Mảng chứa MAX_CAR xe
POINT Y; // Đại diện người qua đường
POINT *Y_FINISH = NULL;
int Yfinish = 0;
int height = 31, width = 101; // Đọ rộng và độ cao của màn hình console
int cnt = 0;//Biến hỗ trợ trong quá trình tăng tốc độ xe di chuyển
int MOVING;//Biến xác định hướng di chuyển của người
int SPEED = 1;// Tốc độ xe chạy (xem như level)
int HEIGHT_GAME = 21, WIDTH_GAME = 101;// Độ rộng và độ cao của màn hình game
bool STATE; // Trạng thái sống/chết của người qua đường
int score = 0; // Điểm
int lastSPEED; // Biến nhận giá trị SPEED tại thời điểm bắt đầu chơi
string name; bool savegame = 1;
int saveX = 10; // Tọa độ X của Save

int timeCD = 10;
std::atomic<bool> timeStopLeft(false), timeStopRight(false);

//.....................................Cố định màn hình Console....................................//
void FixConsoleWindow()
{
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);
}

//.....................................Xóa con trỏ chuột trên màn hình Console....................................//
void Nocursortype()
{
	CONSOLE_CURSOR_INFO Info;
	Info.bVisible = FALSE;
	Info.dwSize = 20;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Info);
}
/*typedef struct _SMALL_RECT {
	short Left;
	short Top;
	short Right;
	short Bottom;
}SMALL_RECT;*/

//.....................................Điều chỉnh kích thước của màn hình console....................................//
void SetWindowSize(const short& width, const short& height)
{
	HWND console = GetConsoleWindow();
	SMALL_RECT WindowSize;
	WindowSize.Top = 0;
	WindowSize.Left = 0;
	WindowSize.Right = width;
	WindowSize.Bottom = height;
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), 1, &WindowSize);
}

//.....................................Xóa thanh cuộn....................................//
void removeScrollBar()
{
	HANDLE hOut;
	CONSOLE_SCREEN_BUFFER_INFO SBInfo;
	COORD NewSBSize;
	int Status;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hOut, &SBInfo);
	NewSBSize.X = SBInfo.dwSize.X - 2;
	NewSBSize.Y = SBInfo.dwSize.Y;

	Status = SetConsoleScreenBufferSize(hOut, NewSBSize);
	if (Status == 0)
	{
		Status = GetLastError();
		cout << "SetConsoleScreenBufferSize() failed! Reason : " << Status << endl;
		exit(Status);
	}

	GetConsoleScreenBufferInfo(hOut, &SBInfo);

	cout << "Screen Buffer Size : ";
	cout << SBInfo.dwSize.X << " x ";
	cout << SBInfo.dwSize.Y << endl;
}

//.....................................Đổi màu....................................//
void SetColor(const int& x)
{
	HANDLE color;
	color = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(color, x);
}

//......................................TOA DO TREN MAN HINH.......................//
void GotoXY(int x, int y)
{

	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

//.....................................Reset Dữ liệu như ban đầu....................................//
/* Chức năng: 
- Cung cấp vùng nhớ cho X để chứ xe
- Reset dữ liệu về lúc ban đầu
*/
//..................................................................................................//
void ResetData()
{
	MOVING = 'D';
	SPEED = 1;
	lastSPEED = 1;
	score = 0;
	Y = { 18,19 };
	if (Y_FINISH == NULL)
	{
		Y_FINISH = new POINT[WIDTH_GAME - 2];
	}
	if (X == NULL) // tạo ra mảng Xe
	{
		X = new POINT * [MAX_CAR];
		for (int i = 0; i < MAX_CAR; i++)
			X[i] = new POINT[MAX_CAR_LENGTH];
		for (int i = 0; i < MAX_CAR; i++)
		{
			int temp = (rand() % (WIDTH_GAME - MAX_CAR_LENGTH)) + 1; // chiếc xe tạo ra nằm trong vùng màn hình game. 
			for (int j = 0; j < MAX_CAR_LENGTH; j++) {
				// [i] là chiếc xe thứ i. [j] là cái dấu "=" do MaxCarLength tạo ra.
				X[i][j].x = temp + j; // vị trí tọa độ x dấu "=" thứ j của xe thứ i .
				X[i][j].y = 2 + i; // vị trí tọa độ y dấu "=" thứ j của xe thứ i .
			}
		}
	}
}

//.....................................Hàm dữ liệu sau khi người dùng chỉnh Level mình mong muốn....................................//
/*
// Giống như ResetData, dữ liệu tùy thuộc vào người chơi
*/
//..................................................................................................................................//
void DataAfterOption()
{
	MOVING = 'D';
	lastSPEED = SPEED;
	Y = { 18,19 };
	if (Y_FINISH == NULL)
	{
		Y_FINISH = new POINT[WIDTH_GAME - 2];
	}
	if (X == NULL)
	{
		X = new POINT * [MAX_CAR];
		for (int i = 0; i < MAX_CAR; i++)
			X[i] = new POINT[MAX_CAR_LENGTH];
		for (int i = 0; i < MAX_CAR; i++)
		{
			int temp = (rand() % (WIDTH_GAME - MAX_CAR_LENGTH)) + 1;
			for (int j = 0; j < MAX_CAR_LENGTH; j++) {
				X[i][j].x = temp + j;
				X[i][j].y = 2 + i;
			}
		}
	}
}
//...........................................Vẽ bảng điểm.......................................................//
void DrawScoreBoard(int x, int y, int width_score, int height_score) // x,y là tọa độ bắt đầu vẽ, width(height)_score là vị trí dừng
{
	SetColor(14);
	//Vẽ cột bên trái
	for (int i = 0; i < height_score; i++)
	{
		GotoXY(x, y + i); cout << char(219);
	}
	//Vẽ cột trên cùng
	for (int i = 0; i < width_score; i++)
	{
		GotoXY(x + i, y); cout << char(220);
	}
	//Vẽ cột bên phải
	for (int i = 1; i < height_score; i++)
	{
		GotoXY(x + width_score - 1, y + i); cout << char(219);
	}
	//Vẽ cột dưới cùng
	for (int i = 1; i < width_score - 1; i++)
	{
		GotoXY(x + i, y + height_score - 1); cout << char(220);
	}
}

void GetScore()
{
	if (SPEED == lastSPEED + 1)
	{
		score += 100;
		GotoXY(2, HEIGHT_GAME + 3); SetColor(Yellow);
		cout << "YOUR SCORE: " << score;
		lastSPEED++;
	}
	if (lastSPEED > MAX_SPEED)
	{
		STATE = 0;
	}
}

void DrawBoardGame()
{
	// Vẽ màn hình chơi game
	GotoXY(0, 0);
	SetColor(Yellow);
	for (int i = 0; i < WIDTH_GAME; i++)
	{
		GotoXY(i, 0); cout << char(254);
		GotoXY(i, HEIGHT_GAME-1); cout << char(254);
	}
	for (int i = 0; i < HEIGHT_GAME; i++)
	{
		GotoXY(0, i); cout << char(179);
		GotoXY(WIDTH_GAME, i); cout << char(179);
	}

	// Các phím chức năng
	GotoXY(0, HEIGHT_GAME + 7); cout << "Press Y to PLAY AGAIN.";
	GotoXY(0, HEIGHT_GAME + 8); cout << "Press N to LEAVE game when game over.";
	GotoXY(0, HEIGHT_GAME + 9); cout << "Press P to PAUSE Game.";
	GotoXY(0, HEIGHT_GAME + 10); cout << "Press V to SAVE Game.";
	GotoXY(WIDTH_GAME - 35, HEIGHT_GAME + 3);
	SetColor(Red); cout << "Using W-A-S-D to play the game.";
	DrawScoreBoard(0, HEIGHT_GAME + 1, 20, 5);
	GotoXY(2, HEIGHT_GAME + 3);
	SetColor(Yellow);
	cout << "YOUR SCORE: " << score;
}
/*void DrawBoard(int x, int y, int width, int height, int curPosX=0, int curPosY=0)
{
	GotoXY(x, y); cout << 'X';
	for (int i = 1; i < width; i++)cout << 'X';
	cout << 'X';
	GotoXY(x, height + y); cout << 'X';
	for (int i = 1; i < width; i++)cout << 'X';
	cout << 'X';
	for (int i = y + 1; i < height + y; i++)
	{
		GotoXY(x, i); cout << 'X';
		GotoXY(x + width, i); cout << 'X';
	}
	GotoXY(curPosX, curPosY);
}*/

/*................. Chức năng...............................
* NewGame
* Reset Game từ đầu
...........................................................*/
void StartGame()
{
	system("cls");
	ResetData();
	DrawBoardGame();
	STATE = true;
}

void StartGameAfterOption()
{
	system("cls");
	lastSPEED = SPEED;
	DataAfterOption();
	DrawBoardGame();
	STATE = true;
}

void GabageCollect(POINT**& X)
{
	memset(X, 0, sizeof(X[0][0]) * MAX_CAR * MAX_CAR_LENGTH);
}

void ExitGame(HANDLE t)
{
	system("cls"); 
	TerminateThread(t, 0);
	GabageCollect(X);
}

void PauseGame(HANDLE t)
{
	SuspendThread(t);
}

void ProcessDead()
{
	STATE = 0;
	GotoXY(WIDTH_GAME/2, 19);
	cout << "GAME OVER";
}

void ProcessFinish(POINT& p)
{
	if (SPEED == MAX_SPEED) {
		system("cls");
			STATE = 0;
			for (int j = 0; j < 2; j++)
			{
				for (int i = 9; i < 12; i++)
				{
					GotoXY(10, 15); SetColor(i); cout << "   :::    :::    ::::::::      :::     :::     :::       :::  ::::::::::   :::::      :::";
					GotoXY(10, 16); SetColor(i); cout << "  :+:    :+:   :+:      :+:   :+:     :+:     :+:       :+:     :+:       :+:+:      :+: ";
					GotoXY(10, 17); SetColor(i); cout << "  +:+  +:+    +:+      +:+   +:+     +:+     +:+       +:+     +:+       :+:+:+    +:+   ";
					GotoXY(10, 18); SetColor(i); cout << "  +#++:      +#+      +:+   +#+     +:+     +#+  +:+  +#+     +#+       +#+  +:+  +#+    ";
					GotoXY(10, 19); SetColor(i); cout << "  +#+       +#+      +#+   +#+     +#+     +#+ +#+#+ +#+     +#+      +#+    +#+ #+#     ";
					GotoXY(10, 20); SetColor(i); cout << " #+#       #+#      #+#   #+#     #+#      #+#+# #+#+#      #+#      #+#     #+#+#       ";
					GotoXY(10, 21); SetColor(i); cout << "###         ########       ########         ###   ###   ########### ###      ####        ";
					Sleep(200);
				}
			}
			GotoXY(20, 29); SetColor(Yellow); cout << "Press N to leave game.";

	}
	else {
		SPEED++;
		p = { 18,19 };
		MOVING = 'D';
	}
}

void DrawCars(const char* s)
{
	for (int i = 0; i < MAX_CAR; i++) {
		for (int j = 0; j < MAX_CAR_LENGTH; j++) {
			GotoXY(X[i][j].x, X[i][j].y); // đi tới vị trí "=" thứ j của xe thứ i ( xe thứ i nằm trên dòng thứ i từ trên xuống)
			SetColor(3);
			cout << "=";
		}
	}
}

void DrawCarsManually(POINT** X)
{
	for (int i = 0; i < MAX_CAR; i++) {
		for (int j = 0; j < MAX_CAR_LENGTH; j++) {
			GotoXY(X[i][j].x, X[i][j].y);
			SetColor(3);
			cout << "=";
		}
	}
}

void DrawSticker(const POINT& p, const char* s)
{
	GotoXY(p.x, p.y);
	SetColor(12);
	cout << s;
}

bool IsImpact(const POINT& p, int d) // p =Y; d =Y.y
{
	if (d == 1 || d == 19)return false;
	for (int i = 0; i < MAX_CAR_LENGTH; i++)
	{
		if (p.x == X[d -2][i].x && p.y == X[d -2][i].y)return true; // trừ 2 do thanh ngang trên d=0 và dòng d=1 (dòng chữ Y finish)
	}
	return false;
}

void saveY(POINT*& a)
{
	// bien dem so Y da ve dich.
	a[Yfinish].x = Y.x; 
	a[Yfinish].y = Y.y;
	Yfinish++;
}

bool IsImpactY(const POINT& p,POINT* a)
{
	for (int i = 0; i <= Yfinish; i++)
	{
		if (p.x == a[i].x && p.y == 2)
			return true;
	}
	return false;
}
// ........................................... Hàm chạy xe ..........................................
/*
*  PPTX nhớ vẽ chỗ này 
*/
void MoveCars()
{
	// xe chạy từ trái qua
	if (timeStopLeft == false) {
		for (int i = 1; i < MAX_CAR; i += 2)
		{
			cnt = 0;
			do {
				cnt++;
				for (int j = 0; j < MAX_CAR_LENGTH - 1; j++) {
					X[i][j] = X[i][j + 1];
				}
				// Kiểm tra xe đụng tường thì trả lại vị trí bên kia tường.
				// X[i] ...... = WIDTH_GAME : Sát vách tường bên phải
				X[i][MAX_CAR_LENGTH - 1].x + 1 == WIDTH_GAME ? X[i][MAX_CAR_LENGTH - 1].x = 1 : X[i][MAX_CAR_LENGTH - 1].x++;

			} while (cnt < SPEED); // SPEED =1 thì cách "=" nhích 1 ô, SPEED =2 thì "="nhích qua 1 ô xong nhích tiếp tục 1 ô nữa => di chuyển 1 lần 2 ô
		}
	}
	if (timeStopRight == false) {
		// xe chạy từ phải qua
		for (int i = 0; i < MAX_CAR; i += 2)
		{
			cnt = 0;
			do {
				cnt++;
				for (int j = MAX_CAR_LENGTH - 1; j > 0; j--)
				{
					X[i][j] = X[i][j - 1];
				}
				X[i][0].x - 1 == 0 ? X[i][0].x = WIDTH_GAME - 1 : X[i][0].x--;

			} while (cnt < SPEED);
		}
	}
}
void EraseCars()
{
	for (int i = 0; i < MAX_CAR; i += 2) {
		cnt = 0;
		do {
			GotoXY(X[i][MAX_CAR_LENGTH - 1 - cnt].x, X[i][MAX_CAR_LENGTH - 1 - cnt].y);
			cout <<" ";
			cnt++;
		} while (cnt < SPEED); //  cnt =2 xóa 2 ô 1 lần
	}
	for (int i = 1; i < MAX_CAR; i += 2) {
		cnt = 0;
		do {
			GotoXY(X[i][0 + cnt].x, X[i][0 + cnt].y);
			cout << (" ");
			cnt++;
		} while (cnt < SPEED);
	}
}
void MoveRight()
{
	if (Y.x < WIDTH_GAME - 1)
	{
		DrawSticker(Y, " ");
		Y.x++;
		DrawSticker(Y, "Y");
	}
}
void MoveLeft()
{
	if (Y.x > 1) {
		DrawSticker(Y, "  ");
		Y.x--;
		DrawSticker(Y, "Y");
	}
}
void MoveDown()
{
	if (Y.y < HEIGHT_GAME - 2)
	{
		DrawSticker(Y, " ");
		Y.y++;
		DrawSticker(Y, "Y");
	}
}
void MoveUp()
{
	if (Y.y > 1) {
		DrawSticker(Y, " ");
		Y.y--;
		DrawSticker(Y, "Y");
	}
}

void SubThreadCDStart() {
	while (STATE == true) {
		Sleep(3000);
		timeStopRight = false;
		timeStopLeft = true;
		Sleep(3000);
		timeStopLeft = false;
		timeStopRight = true;
	}
}

void SubThread()
{
	thread thd(SubThreadCDStart);
	while (1) {
		if (STATE)
		{
			switch (MOVING)
			{
			case'A':MoveLeft();
				break;
			case'D':MoveRight();
				break;
			case'W':
			{
				if (IsImpactY(Y, Y_FINISH))
				{

					GotoXY(WIDTH_GAME - 75, HEIGHT_GAME + 1); cout << "Your character is impact another character. Please go to another lane";
					Sleep(50);
					GotoXY(WIDTH_GAME - 75, HEIGHT_GAME + 2); cout << "Press anykey to continue                                             ";
					_getch();
					break;
				}
				else {
					MoveUp();
					break;
				}
			}
			case'S':MoveDown();
				break;
			}
			MOVING = ' ';

			EraseCars();
			MoveCars();
			DrawCarsManually(X);

			if (IsImpact(Y, Y.y)) // Va chạm = Thua
			{
				ProcessDead();
			}
			if (Y.y == 1)
			{
				saveY(Y_FINISH);
				ProcessFinish(Y);
				GetScore();
			}
			Sleep(25); // Tốc độ quay lại vòng lập trễ 25 giây.
		}
	}
	TerminateThread(thd.native_handle(), 0);
	thd.detach();
}

void save()
{
	string data;
	ofstream ofs1; // Mở file lưu data của người chơi
	ofstream ofs2; // Mở file lưu để in trong phần LOAD GAME
	SetColor(3);
	for (int i = 0; i < 40; i++) {
		GotoXY(20 + i, 22); cout << char(220);
	}
	for (int i = 0; i < 40; i++) {
		GotoXY(20 + i, 24); cout << char(220);
	}
	GotoXY(20, 23); cout << "ENTER YOUR FILE NAME:"; getline(cin, data);
	GotoXY(20, 25);
	cout << "SAVE COMPLETE!";
	_getch();
	ofs1.open("Save\\" + data, ios::out);
	ofs2.open("Save\\Name.txt", ios::app);
	ofs2 << data << "       " << score << "       " << SPEED << endl;
	//ofs1 << data; ofs1 << endl;
	ofs1 << Y.x << " " << Y.y; // vị trí của người
	ofs1 << endl;
	for (int i = 0; i < MAX_CAR; i++) { // toa do x y cua xe
		for (int j = 0; j < MAX_CAR_LENGTH; j++) {
			ofs1 << X[i][j].x << " " << X[i][j].y << " ";
		}
	}
	ofs1 << endl;
	ofs1 << STATE << endl;
	ofs1 << SPEED << endl;
	ofs1 << score << endl;
	ofs1 << lastSPEED << endl;
	ofs1.close();
	ofs2.close();
}

void loadG(string data)
{
	ifstream ifs;
	ifs.open("Save\\" + data, ios::in);
	if (!ifs) {
		// That bai.
		savegame = false;
	}
	else
	{
		savegame = 1; //ifs >> name;
		ifs >> Y.x >> Y.y;
		for (int i = 0; i < MAX_CAR; i++) {
			for (int j = 0; j < MAX_CAR_LENGTH; j++) {
				ifs >> X[i][j].x >> X[i][j].y;
			}
		}
		ifs >> STATE;
		ifs >> SPEED;
		ifs >> score;
		ifs >> lastSPEED;
	}
}
void ReadNameFile() // xuất ra các file save mà bạn lưu
{
	SetColor(3);
	GotoXY(40, 1);
	cout << "LIST FILE NAME";
	int i = 2;
	ifstream ifs;
	string s;
	ifs.open("Save\\Name.txt", ios::in);

	while (!ifs.eof())
	{
		if (i == height) { // nếu = độ dài màn hình thì chuyển sang ngang 25 đơn vị
			saveX += 25;
		}
		getline(ifs, s); // nhận dấu " "
		GotoXY(saveX, i);
		cout << s;
		i++;
	}
	ifs.close();
}

void load2()
{
	system("cls");
	ReadNameFile();
	string data;
	GotoXY(40, 28);
	cout << "ENTER FILE NAME: ";
	getline(cin,data); // Nhập vào tên trùng với file save. Enter để chạy loadG
	system("cls");
	// Sau bước Enter của dòng cin.getline, sẽ vào trò chơi.
	loadG(data);
	if (savegame) {
		DrawBoardGame();
		DrawSticker(Y, "Y");
		DrawCarsManually(X);
	}
	else {
		system("cls");
		GotoXY(WIDTH_GAME - 35, HEIGHT_GAME + 4);
		cout << "\n FILE KHONG TON TAI!" << endl;
		GotoXY(WIDTH_GAME - 35, HEIGHT_GAME + 5);
		cout << "\n Press anykey to Menu" << endl;
	}
}

void HuongDan()
{
	GotoXY(0, 15);
	cout << "Using W-A-S-D to control the character.";
	GotoXY(0, 16);
	cout << "Press Enter key to return to Menu.";
	GotoXY(0, 17);
	cout << "How to play?";
	GotoXY(0, 18);
	cout << "1. Control your character through the obstacles and reach the destination (the cars).";
	GotoXY(0, 19);
	cout << "2. Every time the character reaches the finish line, the next level will be harder than the one ";
	GotoXY(0, 20);
	cout << "you just played.";
	GotoXY(0, 21);
	SetColor(13);
	cout << "Press Enter to return the Menu's Game";
}

void Option() // Tùy chỉnh trong game ( LEVEL, VOLUME )
{
	system("cls");
	int Set[3] = { 12,7,14 };
	int Clevel = 1; // Level mà bạn đang chọn vd: Level 1, level 2, level 3
	int Cvolume = 1;
	int counter = 1; // Biến định vị ví trí của mục bạn đang chọn
	char key;
	SetColor(12);
	GotoXY(WIDTH_GAME / 2 + 2, 9); cout << "OPTION"; // topic
	//
	for (int i = 0;;) // vòng lập vô hạn để được nhập liên tục từ bàn phím
	{
		// In ra: LEVEL:  1
		//        VOLUME: 1
		GotoXY((WIDTH_GAME / 2) + 1, 10); SetColor(Set[0]); cout << "LEVEL ";
		GotoXY((WIDTH_GAME / 2) + 10, 10); SetColor(7); cout << Clevel;

		GotoXY((WIDTH_GAME / 2) + 1, 11); SetColor(Set[1]); cout << "VOLUME ";
		GotoXY((WIDTH_GAME / 2) + 10, 11); SetColor(7); cout << Cvolume;



		key = _getch(); // key nhận 1 giá trị được nhập từ bàn phím
		// key nhận giá trị là w
		if (((key == 'W') || (key == 'w') || (key == 72)) && (counter == 2)) // 72 = up arrow key.
		{
			counter--;
		}
		// key nhận giá trị là s
		if (((key == 'S') || (key == 's') || (key == 80)) && (counter == 1))  // 80 = down arrow key.
		{
			counter++;
		}
		//key nhận giá trị là d
		if (key == 'D' || key == 'd') // tăng level và volume
		{
			if (counter == 1) // Level
			{
				if (Clevel < MAX_SPEED && Clevel >= 1)
				{
					Clevel++;
				}
			}
			if (counter == 2)
			{
				if (Cvolume < MAX_VOLUME && Cvolume >= 0)
					Cvolume++;
			}
		}
		if (key == 'A' || key == 'a') // giảm level và volume
		{
			if (counter == 1) // Level
			{
				if (Clevel <= MAX_SPEED && Clevel > 1)
				{
					// GotoXY((WIDTH_GAME / 2) + 11, 10); cout << " "; // ===== sử dụng nếu maxspeed >=10 =====
					Clevel--;
				}
			}
			if (counter == 2)
			{
				if (Cvolume <= MAX_VOLUME && Cvolume > 0)
					GotoXY((WIDTH_GAME / 2) + 11, 11); cout << " ";
				Cvolume--;
			}
		}
		Set[0] = 7; Set[1] = 7;
		if (counter == 1)
		{
			Set[0] = 12; // nếu chọn ô Level ( counter == 1 ) thì sẽ đổi màu.
		}
		if (counter == 2)
		{
			Set[1] = 12;
		}
		if (key == '\r') // key nhận giá trị là Enter
		{
			lastSPEED = SPEED = Clevel; // level thay đổi 
			GotoXY((WIDTH_GAME / 2) - 5, 17);
			SetColor(11);
			cout << "Your changes are accept!";
			GotoXY((WIDTH_GAME / 2) - 5, 18); SetColor(Set[2]); cout << "Enter to return the Menu's Game";
			break;
		}
	}
}

void GAME()
{
	SetColor(11);
	GotoXY(40, 3);
	cout << "  _____          __  __ ______ ";
	GotoXY(40, 4);
	cout << " / ____|   /\\   |  \\/  |  ____|";
	GotoXY(40, 5);
	cout << "| |  __   /  \\  | \\  / | |__   ";
	GotoXY(40, 6);
	cout << "| | |_ | / /\\ \\ | |\\/| |  __|  ";
	GotoXY(40, 7);
	cout << "| |__| |/ ____ \\| |  | | |____ ";
	GotoXY(40, 8);
	cout << " \\_____/_/    \\_|_|  |_|______|";

	GotoXY(1, 10);
	cout << "  _____ _____   ____   _____ _____ _____ _   _  _____    _____ _______ _____  ______ ______ _______ ";
	GotoXY(1, 11);
	cout << " / ____|  __ \\ / __ \\ / ____/ ____|_   _| \\ | |/ ____|  / ____|__   __|  __ \\|  ____|  ____|__   __|";
	GotoXY(1, 12);
	cout << "| |    | |__) | |  | | (___| (___   | | |  \\| | |  __  | (___    | |  | |__) | |__  | |__     | |   ";
	GotoXY(1, 13);
	cout << "| |    |  _  /| |  | |\___ \\ \\___ \ \\ | | | . ` | | |_ |  \\___\\ |  | |  |  _  /|  __| |  __|    | |   ";
	GotoXY(1, 14);
	cout << "| |____| | \\ \\| |__| |____) ____) |_| |_| |\\  | |__| |  ____) |  | |  | | \\ \\| |____| |____   | |   ";
	GotoXY(1, 15);
	cout << " \\_____|_|  \\_\\\\____/|_____|_____/|_____|_| \\_|\\_____| |_____/   |_|  |_|  \\_|______|______|  |_|   ";

	SetColor(14);
	GotoXY(40, 17);
	cout << " __  __ ______ _   _ _    _ ";
	GotoXY(40, 18);
	cout << "|  \\/  |  ____| \\ | | |  | |";
	GotoXY(40, 19);
	cout << "| \\  / | |__  |  \\| | |  | |";
	GotoXY(40, 20);
	cout << "| |\\/| |  __| | . ` | |  | |";
	GotoXY(40, 21);
	cout << "| |  | | |____| |\\  | |__| |";
	GotoXY(40, 22);
	cout << "|_|  |_|______|_| \\_|\\____/ ";
	int Set[] = { 12,7,7,7,7 };
	int counter = 1;
	char key;
	int temp;
	for (int i = 0;;)
	{

		GotoXY(47, 24);
		SetColor(Set[0]);
		cout << "1. Start Game.";

		GotoXY(47, 25);
		SetColor(Set[1]);
		cout << "2. How to play.";

		GotoXY(47, 26);
		SetColor(Set[2]);
		cout << "3. Load Game.";

		GotoXY(47, 27);
		SetColor(Set[3]);
		cout << "4. Option.";

		GotoXY(47, 28);
		SetColor(Set[4]);
		cout << "5. Exit Game.";
		// Gioi thieu nhom
		SetColor(6);
		GotoXY(0, 29);
		cout << "The Game is made by Group 2\tGV: TRUONG TOAN THINH\tIT - HCMUS";
		GotoXY(0, 30);
		cout << "HUYNH TUAN NAM-20120136\tTRAN HOANG ANH PHI-20120158";
		GotoXY(0, 31);
		cout << "LE THI THUY DUONG-20120063\tLE MINH NHUT-20120154\tNGUYEN VIET AN-20120031";

		key = _getch();
		if (((key == 'W') || (key == 'w') || (key == 72)) && (counter >= 2) && (counter <= 5)) // 72 = up arrow key.
		{
			counter--;
		}
		if (((key == 'S') || (key == 's') || (key == 80)) && (counter >= 1) && (counter <= 4)) // 80 = down arrow key.
		{
			counter++;
		}
		if (key == '\r') // Enter
		{
			if (counter == 1) // StartGame
			{
				haha:
				system("cls");
				if (SPEED == 1)
					StartGame();
				else StartGameAfterOption();
				Game:
				thread t1(SubThread);
				while (1)
				{
					temp = toupper(_getch());
					if (STATE == 1)
					{
						if (temp == 'P') { // dừng trò chơi
							PauseGame(t1.native_handle());
						}
						else if (temp == 'V'|| temp == 'v') {
							PauseGame(t1.native_handle());
							save();
							//GabageCollect(X);
							system("cls");
							GAME();
						}
						else {
							while (ResumeThread((HANDLE)t1.native_handle())); // tiếp tục trò chơi sau khi dừng
							if (temp == 'D' || temp == 'A' || temp == 'W' || temp == 'S')
							{
								MOVING = temp;
							}
						}
					}
					else
					{
						if (temp == 'Y') {
							ResetData();
							goto haha; // Y sẽ Reset trò chơi khi thua
						}
						else if ((temp == 'N') || (temp == 'n')) { // N thoát game
							ExitGame(t1.native_handle());
							t1.detach();
							ResetData();
							break;
						}
					}
				}
			}
			if (counter == 2) // How to play
			{
				system("cls");
				char key1;
				HuongDan();
				for (int i = 0;;) {
				key1 = _getch();
				if (key1 == '\r')
				{
					system("cls");
					GAME();
				}
				}
			}
			if (counter == 3)
			{
				load2();
				if (savegame) {
					system("cls");
					DataAfterOption();
					goto Game; // di chuyển tới Game: ( ở trên counter ==1)
				}
				else  {
					_getch();
					system("cls");
					GAME();
				}

			}
			if (counter == 4)
			{
				char key1;
				system("cls");
				Option();
				for (int i = 0;;) {
					key1 = _getch();
					if (key1 == '\r')
					{
						system("cls");
						GAME();
					}
				}
			}
			if (counter == 5) // Exit Game
			{
				system("cls");
				GabageCollect(X);
				return;
			}
		}
		Set[0] = 7; Set[1] = 7; Set[2] = 7; Set[3] = 7; Set[4] = 7;

		if (counter == 1)
		{
			Set[0] = 12;
		}
		if (counter == 2)
		{
			Set[1] = 12;
		}
		if (counter == 3)
		{
			Set[2] = 12;
		}
		if (counter == 4)
		{
			Set[3] = 12;
		}
		if (counter == 5)
		{
			Set[4] = 12;
		}
	}
}

void main()
{
	
	Nocursortype();
	SetWindowSize(width, height);
	removeScrollBar();
	FixConsoleWindow();
	system("cls");
	srand(time(NULL));
	GAME();
	return;
}

