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
bool isToday(int year, int month, int day);

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

	// Set environment's default locale
	setlocale(LC_ALL, "");

	// Init console colors
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( hstdout, &csbi );
	
	int iFirstWeekDay = getFirstDayOfWeek();
	int iMonthStartWeekDay = getMonthStartDay(iYear, iMonth);
	int iDaysInMonth = getDaysInMonth(iYear, iMonth);
	
	wchar_t Buf[255];

	int iRow = 0;
	bool bFillCalendar = false;
	int iCurrentWeekDay = iFirstWeekDay;
	int iCurrentMonthDay = 1;

	while(iCurrentMonthDay <= iDaysInMonth)
	{
		iCurrentWeekDay = iFirstWeekDay;
		for(int i = 0; i < 7; i++)
		{
			if(iRow == 0)
			{
				// Setting white color on black background
				SetConsoleTextAttribute(hstdout, 0xF0);

				GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME1 + iCurrentWeekDay, Buf, 255);
				wprintf(L"%4.3ls ", Buf);
			}
			else if(iRow == 1)
			{
				// Setting white color on black background
				SetConsoleTextAttribute(hstdout, 0x7F);

				if(iMonthStartWeekDay == iCurrentWeekDay)
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
				if(iCurrentWeekDay == 5 || iCurrentWeekDay == 6)
				{
					// Sundays and saturdays are light red!
					SetConsoleTextAttribute(hstdout, 0x7C);
				}
				else
				{
					// Other days are white
					SetConsoleTextAttribute(hstdout, 0x71);
				}

				if(isToday(iYear, iMonth, iCurrentMonthDay))
				{
					// Printing "today" in the brackets
					swprintf(Buf, L"[%2d]", iCurrentMonthDay);
					wprintf(L"%5.4s", Buf);
				}
				else
				{
					wprintf(L"%4d ", iCurrentMonthDay);
				}

				iCurrentMonthDay++;

				if(iCurrentMonthDay > iDaysInMonth)
				{
					bFillCalendar = false;
				}
			}

			if(iCurrentWeekDay == 6)
			{
				iCurrentWeekDay = 0;
			}
			else
			{
				iCurrentWeekDay++;
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
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, (LPWSTR)&result, 2);

	// GetLocaleInfo returns numeric char '0' - '6'
	// Hence, just subtracting...
	result -= '0';

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

	if(dtStart.tm_wday == 0)
	{
		dtStart.tm_wday = 6;
	}
	else
	{
		dtStart.tm_wday--;
	}

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

bool isToday(int year, int month, int day)
{
	time_t tmRawTime = time(0);
	tm* dtNow = localtime(&tmRawTime);
	return (dtNow->tm_year + 1900 == year && 
			dtNow->tm_mon + 1 == month && 
			dtNow->tm_mday == day);
}
