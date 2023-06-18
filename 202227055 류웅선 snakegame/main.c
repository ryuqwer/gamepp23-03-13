#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "screen.h"
#include "util.h"
#include <stdbool.h>
#include <conio.h>
#define MAP_WIDTH 72
#define MAP_HEIGHT 22

int score = 0; // ���� ���� �߰�
int appleEaten = 0;
int startScreen = 1; // ���� ȭ�� �÷���
bool isPaused = false;
int Initialize();
int Release();
int DrawBuffer(int DeltaTime);
int gameOver = 0; // ���� ���� �÷���
enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};
enum Direction currentDirection = RIGHT;
enum Direction newDirection = RIGHT;
//���ӽ�����Ʈ�� 0�϶� ������ ���� ����
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
void SetConsoleColor(int color) // ���� ���� �Լ�
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}
//0���� �Ӹ�
struct GameObject snakeSkin[MAX_SNAKE_LENGTH];
struct GameObject apple;
enum Direction currentDirection;

int IsSnakeCollidingWithWall()
{
	int headX = snakeSkin[0].posX;
	int headY = snakeSkin[0].posY;

	if (headX == 0 || headX == MAP_WIDTH - 1 || headY == 0 || headY == MAP_HEIGHT - 1)
		return 1; // ���� ���� ���� �浹

	return 0; // ���� ���� ���� �浹���� ����
}

void DrawGameOver()
{
	int gameOverOffsetX = (MAP_WIDTH - 14) / 2; // ���� ���� ���� ���� �߾� ������ ���� X ������ ���
	int gameOverOffsetY = (MAP_HEIGHT - 1) / 2; // ���� ���� ���� ���� �߾� ������ ���� Y ������ ���

	SetConsoleColor(12);
	setCursorPos(gameOverOffsetX, gameOverOffsetY);
	printf("=== Game Over ===");

	setCursorPos(gameOverOffsetX - 5, gameOverOffsetY + 2);
	printf("Press SPACEBAR to finish\n");
	SetConsoleColor(15); // ���� ������� ����
}

void CheckCollision()
{
	// ���� �Ӹ� ��ǥ
	int headX = snakeSkin[0].posX;
	int headY = snakeSkin[0].posY;

	// �� ���� ��Ҵ��� �˻�
	if (headX == 0 || headX == MAP_WIDTH - 1 || headY == 0 || headY == MAP_HEIGHT - 1)
	{
		gameOver = 1; // ���� ���� ���·� ����
		return;
	}

	// ���� ���뿡 ��Ҵ��� �˻�
	for (int i = 1; i < GameStartSnakeLength; i++)
	{
		if (headX == snakeSkin[i].posX && headY == snakeSkin[i].posY)
		{
			gameOver = 1; // ���� ���� ���·� ����
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
	setCursorVisibility(1); // Ŀ�� ����/����

	while (GameState)
	{
		deltaTime = GetTickCount() - time_end;
		time_end = GetTickCount();
		CheckCollision(); // �浹 �˻� �߰�

		// ���� ȭ��
		if (startScreen)
		{
			// Ű �Է� ó��
			if (_kbhit())
			{
				int key = _getch();
				if (key == ' ') // �����̽��� �Է� �� ���� ����
				{
					startScreen = 0; // ���� ȭ�� ����
				}
			}

			// ���� ȭ�� �׸���
			setCursorPos(0, 0);
			SetColor(0b1111, 0b0000);
			printf("=== Snake Game ===\n\n");
			printf("Press SPACEBAR to start");

			Sleep(100); // ���� ȭ�鿡 ��� �ð��� �߰��� ȭ�� �������� ����
			continue; // ���� ȭ�鿡���� ���� ������ �ǳʶݴϴ�.
		}

		if (gameOver)
		{
			DrawGameOver();

			// Ű �Է� ó��
			if (_kbhit())
			{
				int key = _getch();
				if (key == ' ') // �����̽��� �Է� �� ����
				{
					GameState = 0;
					break;
				}
				
			}

			continue;  // ���� ���� �� ���� ������ �ǳʶݴϴ�.
		}

		if (_kbhit())
		{
			int key = _getch();
			switch (key)
			{
			case 72: // �� ����Ű
				if (currentDirection != DOWN)
					newDirection = UP;  // ���ο� ���� ����
				break;
			case 80: // �� ����Ű
				if (currentDirection != UP)
					newDirection = DOWN;  // ���ο� ���� ����
				break;
			case 75: // �� ����Ű
				if (currentDirection != RIGHT)
					newDirection = LEFT;  // ���ο� ���� ����
				break;
			case 77: // �� ����Ű
				if (currentDirection != LEFT)
					newDirection = RIGHT;  // ���ο� ���� ����
				break;
			}

		}

		DrawBuffer(deltaTime);

		if (deltaTime < 33)
			Sleep(33 - deltaTime);

		// ���� ǥ��
		setCursorPos(0, 24); // ���� ǥ�� ��ġ ����
		SetColor(0b1111, 0b0000);
		printf("Score: %d", score);

	}

	CHECKERROR(Release());

	exit(1);
	return 0;
}

void InitApple()
{
	//apple �ʱ�ȭ
	srand(time(NULL));
	do {
		apple.posX = rand() % (MAP_WIDTH - 2) + 1; // 1���� MAP_WIDTH - 2 ������ ���� ����
		apple.posY = rand() % (MAP_HEIGHT - 2) + 1; // 1���� MAP_HEIGHT - 2 ������ ���� ����
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

	//GameObject �ʱ�ȭ

	//��� �ʱ�ȭ
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

	//�� ���׾Ƹ� �ʱ�ȭ
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

//dir : �¿�
void MoveSnake(int dir)
{
	// ���� �Ӹ� ��ġ ����
	int prevX = snakeSkin[0].posX;
	int prevY = snakeSkin[0].posY;

	// ���� ���� ���⿡ ���� ���� �Ǵ� �������� �̵�
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

	// ���� �Ӹ� ��ġ�� ����� �ִ��� Ȯ��
	if (snakeSkin[0].posX == apple.posX && snakeSkin[0].posY == apple.posY)
	{
		// ����� �Ծ����Ƿ� ���� ���̸� 1��ŭ ����
		GameStartSnakeLength++;
		ActiveSnakeNum++;
		// ��� �Ծ ���� ��
		score += 10;
		// ���� ����� ���ο� ��ġ�� �����
		InitApple();
	}

	// ���� ���� �̵�
	for (int i = 1; i < GameStartSnakeLength; i++)
	{
		// ���� ��ġ ����
		int tempX = snakeSkin[i].posX;
		int tempY = snakeSkin[i].posY;

		// ���� ��ġ�� �̵�
		snakeSkin[i].posX = prevX;
		snakeSkin[i].posY = prevY;

		// ���� ��ġ ����
		prevX = tempX;
		prevY = tempY;
	}

	// ���� ���� ������Ʈ
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