#pragma once
#include <Windows.h>
#include <cstddef>
#include <iostream>
#include <tchar.h>
#include <conio.h>
#include <strsafe.h>
#include <cassert>

class ConsoleWindow
{
private:
	int screenWidth = 120;			// Console Screen Size X (columns)
	int screenHeight = 40;			// Console Screen Size Y (rows)

	wchar_t *screenBuffer;
	HANDLE hConsole;
	DWORD dwBytesWritten;

public:
	ConsoleWindow(int width, int height) : screenWidth(width), screenHeight(height)
	{
		screenBuffer = new wchar_t[screenWidth * screenHeight];
		hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		SetConsoleActiveScreenBuffer(hConsole);
		dwBytesWritten = 0;

		SetConsoleMode(hConsole, ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	}

	~ConsoleWindow()
	{
		delete screenBuffer;
	}

	void setTitle()
	{
		TCHAR szOldTitle[MAX_PATH];
		TCHAR szNewTitle[MAX_PATH];

		// Save current console title.

		if (GetConsoleTitle(szOldTitle, MAX_PATH))
		{
			// Build new console title string.

			StringCchPrintf(szNewTitle, MAX_PATH, TEXT("Raytracer"), szOldTitle);

			// Set console title to new title
			if (!SetConsoleTitle(szNewTitle))
			{
				_tprintf(TEXT("SetConsoleTitle failed (%d)\n"), GetLastError());
				return;
			}
			else
			{
				_tprintf(TEXT("SetConsoleTitle succeeded.\n"));
			}
		}
	}

	void setPixel(int x, int y, char c)
	{
		//if (x < 0 || x > screenWidth - 1 || y < 0 || y > screenHeight - 1)
		//{
		//	//std::cerr << "NO!";
		//	return;
		//}
		assert(x >= 0 || x < screenWidth || y >= 0 || y < screenHeight);

		screenBuffer[y * screenWidth + x] = c;
	}

	wchar_t* getBuffer() { return screenBuffer; }

	void clear()
	{
		for (int i = 0; i < screenWidth * screenHeight; i++)
		{
			screenBuffer[screenWidth * screenHeight - 1] = NULL;
		}
	}

	void draw()
	{
		//swprintf_s(screenBuffer, 40, L"Console Raytracer by Nathan MacAdam ");

		// Display Frame
		screenBuffer[screenWidth * screenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screenBuffer, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
	}
};
