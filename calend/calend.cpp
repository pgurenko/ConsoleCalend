//
// Author: Pavel Gurenko <pgurenko@gmail.com>
// Quiz program to show off the console calendar.
// Implemented under: Visual Studio 2010
// Platform: Windows only (has windows-specific console coloring code)
//

#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <locale.h>
#include <windows.h>

// Forward declarations
void printError();
int getFirstDayOfWeek();
int getMonthStartDay(int year, int month);
int getDaysInMonth(int year, int month);
bool isToday(tm& Value);
bool isTomorrow(tm& Value);

int main(int argc, const char* argv[])
{
	if(argc < 2)
	{
		printError();
		return 0;
	}

	int iMonth = 0;
	int iYear = 0;
	sscanf(argv[1], "%d.%d", &iMonth, &iYear);

	if(iYear <= 1900 || iMonth > 12 || iMonth <= 0)
	{
		printError();
		return 0;
	}

	// Init console colors
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( hstdout, &csbi );
	
	int iFirstWeekDay = getFirstDayOfWeek();

	wchar_t Buf[255];
	tm dtCurrent =
	{
        0,			  /* seconds after the minute - [0,59] */
        0,			  /* minutes after the hour - [0,59] */
        0,			  /* hours since midnight - [0,23] */
        1,            /* day of the month - [1,31] */
        iMonth - 1,   /* months since January - [0,11] */
        iYear - 1900, /* years since 1900 */
        iFirstWeekDay,/* days since Sunday - [0,6] */
        0,			  /* days since January 1 - [0,365] */
        0,			  /* daylight savings time flag */
	};

	int iMonthStartDay = getMonthStartDay(iYear, iMonth);
	int iDaysInMonth = getDaysInMonth(iYear, iMonth);
	
	int iRow = 0;
	bool bFillCalendar = false;
	while(dtCurrent.tm_mday <= iDaysInMonth)
	{
		dtCurrent.tm_wday = iFirstWeekDay;
		for(int i = 0; i < 7; i++)
		{
			if(iRow == 0)
			{
				// Setting white color on black background
				SetConsoleTextAttribute(hstdout, 0xF0);

				wcsftime(Buf, 255, L"%a", &dtCurrent);
				wprintf(L"%5s", Buf);
			}
			else if(iRow == 1)
			{
				// Setting white color on black background
				SetConsoleTextAttribute(hstdout, 0x7F);

				if(iMonthStartDay == dtCurrent.tm_wday)
				{
					bFillCalendar = true;
				}
			}

			// Filling blank space before and after selected month
			if(!bFillCalendar && iRow != 0)
			{
				wprintf(L"%5s", " ");
			}

			if(bFillCalendar)
			{
				// Setting days color
				if(dtCurrent.tm_wday == 0 || dtCurrent.tm_wday == 6)
				{
					// Sundays and saturdays are light red!
					SetConsoleTextAttribute(hstdout, 0x7C);
				}
				else
				{
					// Other days are white
					SetConsoleTextAttribute(hstdout, 0x71);
				}

				if(isToday(dtCurrent))
				{
					// Printing "today" in the brackets
					swprintf(Buf, L"[%2d]", dtCurrent.tm_mday);
					wprintf(L"%6s", Buf);
				}
				else if(isTomorrow(dtCurrent) && i != 0 && i != 6)
				{
					// After "today" interval is shorter
					wprintf(L"%4d", dtCurrent.tm_mday);
				}
				else
				{
					wprintf(L"%5d", dtCurrent.tm_mday);
				}

				dtCurrent.tm_mday++;

				if(dtCurrent.tm_mday > iDaysInMonth)
				{
					bFillCalendar = false;
				}
			}

			// Space for nice formatting:)
			if(i == 6 && !isTomorrow(dtCurrent))
			{
				wprintf(L" ");
			}

			if(dtCurrent.tm_wday == 6)
			{
				dtCurrent.tm_wday = 0;
			}
			else
			{
				dtCurrent.tm_wday++;
			}
		}

		SetConsoleTextAttribute(hstdout, csbi.wAttributes);

		wprintf(L"\r\n");

		iRow++;
	}

	printf("Press any key to continue...");

	getch();

	return 0;
}

void printError()
{
	printf("Calend. The console calendar.\n");
	printf("Invalid parameters. Usage: calend.exe <mm.YYYY>\n");
}

int getFirstDayOfWeek()
{
	// Let's make it's sunday by default
	int result = 0;

	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Control Panel\\International", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
		DWORD dwLength = sizeof(DWORD);
		DWORD dwValue = 0;
		DWORD dwType = 0;
		if(RegQueryValueEx(hKey, L"iFirstDayOfWeek", NULL, &dwType, (LPBYTE)&dwValue, &dwLength) == ERROR_SUCCESS)
		{
			result = dwValue - '0';
			if(result == 6)
			{
				result = 0;
			}
			else
			{
				result++;
			}
		}
    }
	return result;
}

int getMonthStartDay(int year, int month)
{
	// Finding out which day of week does the month start on
	tm dtStart =
	{
        0,			  /* seconds after the minute - [0,59] */
        0,			  /* minutes after the hour - [0,59] */
        0,			  /* hours since midnight - [0,23] */
        1,            /* day of the month - [1,31] */
        month - 1,    /* months since January - [0,11] */
        year - 1900,  /* years since 1900 */
        0,			  /* days since Sunday - [0,6] */
        0,			  /* days since January 1 - [0,365] */
        0,			  /* daylight savings time flag */
	};

	// setting tm_wday and tm_yday
	mktime(&dtStart);

	return dtStart.tm_wday;
}

int getDaysInMonth(int year, int month)
{
	int result = 31;
	if (month == 2)
	{
		bool bLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);

		if (!bLeapYear)
		{
			result = 29;
		}
		else
		{
			result = 28;
		}
	}
	else if (month == 4 || month == 6 || month == 9 || month == 11)
	{
		result = 30;
	}
	return result;
}

bool isToday(tm& Value)
{
	time_t tmRawTime = time(0);
	tm* dtNow = localtime(&tmRawTime);
	return (dtNow->tm_year == Value.tm_year && 
			dtNow->tm_mon == Value.tm_mon && 
			dtNow->tm_mday == Value.tm_mday);
}

bool isTomorrow(tm& Value)
{
	time_t tmRawTime = time(0);
	tm* dtNow = localtime(&tmRawTime);
	return (dtNow->tm_year == Value.tm_year && 
			dtNow->tm_mon == Value.tm_mon && 
			dtNow->tm_mday + 1 == Value.tm_mday);
}
