#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "screen.h"
#include "util.h"
#include <stdbool.h>
#include <conio.h>
#define MAP_WIDTH 72
#define MAP_HEIGHT 22

int score = 0; // 점수 변수 추가
int appleEaten = 0;
int startScreen = 1; // 시작 화면 플래그
bool isPaused = false;
int Initialize();
int Release();
int DrawBuffer(int DeltaTime);
int gameOver = 0; // 게임 오버 플래그
enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};
enum Direction currentDirection = RIGHT;
enum Direction newDirection = RIGHT;
//게임스테이트가 0일때 무조건 게임 종료
int GameState;
int GameStartSnakeLength;
int ActiveSnakeNum;

void setCursorVisibility(int visibility)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(consoleHandle, &cursorInfo);
	cursorInfo.bVisible = (visibility == 0) ? FALSE : TRUE;
	SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

#define MAX_SNAKE_LENGTH 1400
struct GameObject
{
	int posX;
	int posY;
	int isActive;
};
void SetConsoleColor(int color) // 색상 설정 함수
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}
//0번이 머리
struct GameObject snakeSkin[MAX_SNAKE_LENGTH];
struct GameObject apple;
enum Direction currentDirection;

int IsSnakeCollidingWithWall()
{
	int headX = snakeSkin[0].posX;
	int headY = snakeSkin[0].posY;

	if (headX == 0 || headX == MAP_WIDTH - 1 || headY == 0 || headY == MAP_HEIGHT - 1)
		return 1; // 뱀이 맵의 벽과 충돌

	return 0; // 뱀이 맵의 벽과 충돌하지 않음
}

void DrawGameOver()
{
	int gameOverOffsetX = (MAP_WIDTH - 14) / 2; // 게임 오버 문구 가로 중앙 정렬을 위한 X 오프셋 계산
	int gameOverOffsetY = (MAP_HEIGHT - 1) / 2; // 게임 오버 문구 세로 중앙 정렬을 위한 Y 오프셋 계산

	SetConsoleColor(12);
	setCursorPos(gameOverOffsetX, gameOverOffsetY);
	printf("=== Game Over ===");

	setCursorPos(gameOverOffsetX - 5, gameOverOffsetY + 2);
	printf("Press SPACEBAR to finish\n");
	SetConsoleColor(15); // 색상 원래대로 변경
}

void CheckCollision()
{
	// 뱀의 머리 좌표
	int headX = snakeSkin[0].posX;
	int headY = snakeSkin[0].posY;

	// 맵 벽에 닿았는지 검사
	if (headX == 0 || headX == MAP_WIDTH - 1 || headY == 0 || headY == MAP_HEIGHT - 1)
	{
		gameOver = 1; // 게임 오버 상태로 변경
		return;
	}

	// 뱀의 몸통에 닿았는지 검사
	for (int i = 1; i < GameStartSnakeLength; i++)
	{
		if (headX == snakeSkin[i].posX && headY == snakeSkin[i].posY)
		{
			gameOver = 1; // 게임 오버 상태로 변경
			return;
		}
	}
}

int main()
{
	CHECKERROR(Initialize());

	unsigned long time_end = GetTickCount();
	unsigned long deltaTime = GetTickCount();
	
	int restartGame = 0;
	setCursorVisibility(1); // 커서 보임/숨김

	while (GameState)
	{
		deltaTime = GetTickCount() - time_end;
		time_end = GetTickCount();
		CheckCollision(); // 충돌 검사 추가

		// 시작 화면
		if (startScreen)
		{
			// 키 입력 처리
			if (_kbhit())
			{
				int key = _getch();
				if (key == ' ') // 스페이스바 입력 시 게임 시작
				{
					startScreen = 0; // 시작 화면 종료
				}
			}

			// 시작 화면 그리기
			setCursorPos(0, 0);
			SetColor(0b1111, 0b0000);
			printf("=== Snake Game ===\n\n");
			printf("Press SPACEBAR to start");

			Sleep(100); // 시작 화면에 대기 시간을 추가해 화면 깜빡임을 방지
			continue; // 시작 화면에서는 게임 루프를 건너뜁니다.
		}

		if (gameOver)
		{
			DrawGameOver();

			// 키 입력 처리
			if (_kbhit())
			{
				int key = _getch();
				if (key == ' ') // 스페이스바 입력 시 종료
				{
					GameState = 0;
					break;
				}
				
			}

			continue;  // 게임 오버 시 게임 루프를 건너뜁니다.
		}

		if (_kbhit())
		{
			int key = _getch();
			switch (key)
			{
			case 72: // 상 방향키
				if (currentDirection != DOWN)
					newDirection = UP;  // 새로운 방향 설정
				break;
			case 80: // 하 방향키
				if (currentDirection != UP)
					newDirection = DOWN;  // 새로운 방향 설정
				break;
			case 75: // 좌 방향키
				if (currentDirection != RIGHT)
					newDirection = LEFT;  // 새로운 방향 설정
				break;
			case 77: // 우 방향키
				if (currentDirection != LEFT)
					newDirection = RIGHT;  // 새로운 방향 설정
				break;
			}

		}

		DrawBuffer(deltaTime);

		if (deltaTime < 33)
			Sleep(33 - deltaTime);

		// 점수 표시
		setCursorPos(0, 24); // 점수 표시 위치 설정
		SetColor(0b1111, 0b0000);
		printf("Score: %d", score);

	}

	CHECKERROR(Release());

	exit(1);
	return 0;
}

void InitApple()
{
	//apple 초기화
	srand(time(NULL));
	do {
		apple.posX = rand() % (MAP_WIDTH - 2) + 1; // 1부터 MAP_WIDTH - 2 사이의 값을 선택
		apple.posY = rand() % (MAP_HEIGHT - 2) + 1; // 1부터 MAP_HEIGHT - 2 사이의 값을 선택
		apple.isActive = 1;
	} while (apple.posX == 0 || apple.posX == MAP_WIDTH - 1 || apple.posY == 0 || apple.posY == MAP_HEIGHT - 1);
}

int Initialize()
{
	GameState = 1;
	GameStartSnakeLength = 3;
	ActiveSnakeNum = GameStartSnakeLength;
	setScreenSize(70, 20);
	setCursorVisibility(0);
	SetColor(0b1000, 0b1111);
	ClearBuffer();

	//GameObject 초기화

	//사과 초기화
	InitApple();
	int i = 0;
	while (i < GameStartSnakeLength)
	{
		if (snakeSkin[i].posX == apple.posX
			&& snakeSkin[i].posY == apple.posY)
		{
			InitApple();
			i = 0;
		}
		else
		{
			i = i + 1;
		}
	}

	//뱀 몸뚱아리 초기화
	i = 0;
	while (i < MAX_SNAKE_LENGTH)
	{
		snakeSkin[i].posX = 35;
		snakeSkin[i].posY = 10;
		snakeSkin[i].isActive = 0;
		i = i + 1;
	}

	i = 0;
	while (i < GameStartSnakeLength)
	{
		snakeSkin[i].posX = 35 - i;
		snakeSkin[i].posY = 10;
		snakeSkin[i].isActive = 1;
		i = i + 1;
	}

	return 0;
}

int Release()
{
	return 0;
}

//dir : 좌우
void MoveSnake(int dir)
{
	// 뱀의 머리 위치 저장
	int prevX = snakeSkin[0].posX;
	int prevY = snakeSkin[0].posY;

	// 뱀의 현재 방향에 따라 수평 또는 수직으로 이동
	switch (newDirection)
	{
	case UP:
		if (currentDirection != DOWN)
			snakeSkin[0].posY -= 1;
		break;
	case DOWN:
		if (currentDirection != UP)
			snakeSkin[0].posY += 1;
		break;
	case LEFT:
		if (currentDirection != RIGHT)
			snakeSkin[0].posX -= 1;
		break;
	case RIGHT:
		if (currentDirection != LEFT)
			snakeSkin[0].posX += 1;
		break;
	}

	// 뱀의 머리 위치에 사과가 있는지 확인
	if (snakeSkin[0].posX == apple.posX && snakeSkin[0].posY == apple.posY)
	{
		// 사과를 먹었으므로 뱀의 길이를 1만큼 증가
		GameStartSnakeLength++;
		ActiveSnakeNum++;
		// 사과 먹어서 점수 업
		score += 10;
		// 먹은 사과를 새로운 위치에 재생성
		InitApple();
	}

	// 뱀의 몸통 이동
	for (int i = 1; i < GameStartSnakeLength; i++)
	{
		// 현재 위치 저장
		int tempX = snakeSkin[i].posX;
		int tempY = snakeSkin[i].posY;

		// 이전 위치로 이동
		snakeSkin[i].posX = prevX;
		snakeSkin[i].posY = prevY;

		// 이전 위치 갱신
		prevX = tempX;
		prevY = tempY;
	}

	// 뱀의 방향 업데이트
	currentDirection = newDirection;

	
}

int DeltaTimeSum = 0;

int DrawBuffer(int DeltaTime)
{
	setCursorPos(0, 0);
	SetColor(0b1111, 0b0000);
	printf("%s", ScreenBuffer);

	DeltaTimeSum = DeltaTimeSum + DeltaTime;
	if (DeltaTimeSum >= 70)
	{
		MoveSnake(0);
		DeltaTimeSum = 0;
	}


	//apple
	setCursorPos(apple.posX, apple.posY);
	SetColor(0b1111, 0b0100);
	printf("*");

	//snake
	int i = 0;
	while (i < GameStartSnakeLength)
	{
		setCursorPos(snakeSkin[i].posX, snakeSkin[i].posY);
		SetColor(0b1111, 0b0010);
		printf("O");
		i = i + 1;
	}

	SetColor(0b0000, 0b1111);
	setCursorPos(0, 22);
	if (DeltaTime != 0)
		printf("time : %d                 \nfps :%d                  \n", DeltaTime, 1000 / DeltaTime);

	return 0;
}