// integrated.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <windows.h>
#include <wincon.h>
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include "SimpleIni.h"
#include <iostream>
#include <direct.h>
#include <map>
#include <Wincon.h>
#include <iterator>
#define ELPP_DISABLE_DEFAULT_CRASH_HANDLING
#include "easylogging++.h"
#include <sys/stat.h>
#include <regex>
INITIALIZE_EASYLOGGINGPP
/*
Global declarations. May need cleanup.
*/

PROCESS_INFORMATION hConsole1, hConsole2;//consoles being used.Referencing the host console
const int BUFFSIZE = 128;//global buffer size for arrays in and out.
DWORD g_dwCurrentProc;

STARTUPINFO sj;//startup information structure for the guest process
PROCESS_INFORMATION pj;//structure to receive the target process information

HANDLE  hStdIn; // standard input handle to the process
HANDLE  hStdOut; //standard output handle to the process
HANDLE  hStdErr; //standard error handle to the process

CONSOLE_SCREEN_BUFFER_INFO csbiInfo; //stores the information about created console buffer information like dimensions
CHAR_INFO chiBuffer[3200]; // large character information structiure to capture the console output.
CHAR_INFO pchiBuffer[3200];//previous buffer data
CHAR_INFO chiFill;//to fill the screen
SMALL_RECT srctReadRect; //console screen rectangle.
SMALL_RECT prctReadRect;//rectangle to read previous buffer info

COORD coordBufSize;  //coordinate BuffSize
COORD coordBufCoord; //coordinate Buff
COORD origin;

//hist global
static COORD start;
static COORD histCord;
static int readLcount = 1;//track the repeatation of console read. for future use.

std::string old_history = "";
std::string history = "";
std::string prompt = "";

//readfile decl
CSimpleIniA ini;
CSimpleIniA::TNamesDepend keys;
std::string pgname;
std::string args;

int t_old = 0;
bool exitFlg = false;

/*******************************************************************
FILE EXISTS
********************************************************************/

inline bool exists_test3(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}



/*******************************************************************
SIMULATES RETURN
********************************************************************/
void enter(void)
{
	DWORD dwTmp;
	INPUT_RECORD ir[2];
	Sleep(1000);
	ir[0].EventType = KEY_EVENT;
	ir[0].Event.KeyEvent.bKeyDown = TRUE;
	ir[0].Event.KeyEvent.dwControlKeyState = 0;
	ir[0].Event.KeyEvent.uChar.UnicodeChar = VK_RETURN;
	ir[0].Event.KeyEvent.wRepeatCount = 1;
	ir[0].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
	ir[0].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);

	ir[1].EventType = KEY_EVENT;
	ir[1].Event.KeyEvent.bKeyDown = FALSE;
	ir[1].Event.KeyEvent.dwControlKeyState = 0;
	ir[1].Event.KeyEvent.uChar.UnicodeChar = VK_RETURN;
	ir[1].Event.KeyEvent.wRepeatCount = 1;
	ir[1].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
	ir[1].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);

	dwTmp = 0;
	WriteConsoleInput(hStdIn, ir, 2, &dwTmp);
	Sleep(1000);
	return;
}

void esc(void)
{
	DWORD dwTmp;
	INPUT_RECORD ir[2];
	Sleep(1000);
	ir[0].EventType = KEY_EVENT;
	ir[0].Event.KeyEvent.bKeyDown = TRUE;
	ir[0].Event.KeyEvent.dwControlKeyState = 0;
	ir[0].Event.KeyEvent.uChar.UnicodeChar = VK_ESCAPE;
	ir[0].Event.KeyEvent.wRepeatCount = 1;
	ir[0].Event.KeyEvent.wVirtualKeyCode = VK_ESCAPE;
	ir[0].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_ESCAPE, MAPVK_VK_TO_VSC);

	ir[1].EventType = KEY_EVENT;
	ir[1].Event.KeyEvent.bKeyDown = FALSE;
	ir[1].Event.KeyEvent.dwControlKeyState = 0;
	ir[1].Event.KeyEvent.uChar.UnicodeChar = VK_ESCAPE;
	ir[1].Event.KeyEvent.wRepeatCount = 1;
	ir[1].Event.KeyEvent.wVirtualKeyCode = VK_ESCAPE;
	ir[1].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_ESCAPE, MAPVK_VK_TO_VSC);

	dwTmp = 0;
	WriteConsoleInput(hStdIn, ir, 2, &dwTmp);
	Sleep(1000);
	return;
}



void ctrl(int a)
{
	try {
		GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
	}
	catch (std::exception &e)
	{
		LOG(INFO) << e.what() << std::endl;
		return;
	}

}

void ctrlChar(std::string a)
{
	WCHAR a1 = a.at(1);
	//send ctrl down
	//then char down
	//ctrl up
	//char up

	DWORD dwTmp;
	INPUT_RECORD ir[4];
	WCHAR con = '^';
	Sleep(1000);
	ir[0].EventType = KEY_EVENT;
	ir[0].Event.KeyEvent.bKeyDown = TRUE;
	ir[0].Event.KeyEvent.dwControlKeyState = 0;
	ir[0].Event.KeyEvent.uChar.UnicodeChar = con;
	ir[0].Event.KeyEvent.wRepeatCount = 1;

	ir[1].EventType = KEY_EVENT;
	ir[1].Event.KeyEvent.bKeyDown = TRUE;
	ir[1].Event.KeyEvent.dwControlKeyState = 0;
	ir[1].Event.KeyEvent.uChar.UnicodeChar = a1;
	ir[1].Event.KeyEvent.wRepeatCount = 1;

	ir[2].EventType = KEY_EVENT;
	ir[2].Event.KeyEvent.bKeyDown = FALSE;
	ir[2].Event.KeyEvent.dwControlKeyState = 0;
	ir[2].Event.KeyEvent.uChar.UnicodeChar = a1;
	ir[2].Event.KeyEvent.wRepeatCount = 1;

	ir[3].EventType = KEY_EVENT;
	ir[3].Event.KeyEvent.bKeyDown = FALSE;
	ir[3].Event.KeyEvent.dwControlKeyState = 0;
	ir[3].Event.KeyEvent.uChar.UnicodeChar = con;
	ir[3].Event.KeyEvent.wRepeatCount = 1;

	dwTmp = 0;
	WriteConsoleInput(hStdIn, ir, 4, &dwTmp);
	Sleep(1000);
	return;
}



/*******************************************************************
STRING CHECK
********************************************************************/
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

static inline std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}

std::string check(std::string str) {

	return trim(str);
}

/*******************************************************************
EXECUTES CONSOLE INPUT FROM A STRING
********************************************************************/
void send(std::string a)
{
	Sleep(100);
	if (a == "")
		return;
	if (a == "{ENTER}")
	{
		enter();
		return;
	}

	if (a.find("{}") != std::string::npos)
	{
		ctrl(1);
		return;
	}

	if (a.find("^") != std::string::npos)
	{
		//ctrlChar(a);
		return;
	}

	if (a == "{ESC}")
	{
		esc();
		return;
	}

	DWORD dwTmp;

	char tab2[1024];

	strncpy(tab2, a.c_str(), sizeof(tab2));
	tab2[sizeof(tab2) - 1] = 0; //add an end of string incase needed

	short key = 0;
	for (int i = 0; i<sizeof(tab2); i++)
	{
		if (tab2[i] == '\0')//leve when end of string
			break;

		INPUT_RECORD ir[2];
		//keystroke simulation.Directly goes to console input stream whichever the guest program using
		ir[0].EventType = KEY_EVENT;
		ir[0].Event.KeyEvent.bKeyDown = TRUE;
		ir[0].Event.KeyEvent.dwControlKeyState = 0;
		ir[0].Event.KeyEvent.uChar.UnicodeChar = tab2[i];
		ir[0].Event.KeyEvent.wRepeatCount = 1;

		ir[1].EventType = KEY_EVENT;
		ir[1].Event.KeyEvent.bKeyDown = FALSE;
		ir[1].Event.KeyEvent.dwControlKeyState = 0;
		ir[1].Event.KeyEvent.uChar.UnicodeChar = tab2[i];
		ir[1].Event.KeyEvent.wRepeatCount = 1;

		dwTmp = 0;
		WriteConsoleInput(hStdIn, ir, 2, &dwTmp);
		Sleep(10);

	}
	enter();
	return;

}

/*******************************************************************
READS CONSOLE OUTPUT
********************************************************************/
void readConsole(void)
{

	if (!GetConsoleScreenBufferInfo(hStdOut, &csbiInfo))
	{
		printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
		return;
	}


	// Set the source rectangle for current prompt.  

	srctReadRect.Top = csbiInfo.dwCursorPosition.Y;
	srctReadRect.Left = 0;
	srctReadRect.Bottom = csbiInfo.dwCursorPosition.Y;
	srctReadRect.Right = csbiInfo.dwCursorPosition.X;

	coordBufSize.X = csbiInfo.srWindow.Right;
	coordBufSize.Y = csbiInfo.srWindow.Bottom;
	coordBufCoord.X = csbiInfo.srWindow.Left;
	coordBufCoord.Y = csbiInfo.srWindow.Top;

	char c[3200] = { 0 }; //initialize char array for curret prompt
						  // Clear the CHAR_INFO buffer:
	for (int i = 0; i<3200; ++i) {
		// Fill it with Line terminators
		if (chiBuffer[i].Char.AsciiChar == '\0')
		{
			break;
		}
		else
		{
			chiBuffer[i].Char.AsciiChar = '\0';
			c[i] = '\0';
		}
	}



	//for current prompt

			ReadConsoleOutput(
				hStdOut,        // screen buffer to read from 
				chiBuffer,      // buffer to copy into 
				coordBufSize,   // col-row size of chiBuffer 
				coordBufCoord,  // top left dest. cell in chiBuffer 
				&srctReadRect);

		

	

	for (int i = 0; i < 3200; i++)
	{
		//c[i]=chiBuffer[i].Char.AsciiChar;
		if (chiBuffer[i].Char.AsciiChar == '\0')
		{
			//c[i]='\0';
			break;
		}
		else
		{
			c[i] = chiBuffer[i].Char.AsciiChar;
		}
	}


	std::string str(c);
	str = check(str);
	prompt = str;

	std::wstring stemp = std::wstring(str.begin(), str.end());
	LPCWSTR sw = stemp.c_str();

	histCord = origin;
	DWORD numcharread = 0;
	SHORT x = csbiInfo.dwSize.X;
	SHORT char2read;
	SHORT y = (csbiInfo.dwCursorPosition.Y - 1);//determine the start position to read for the second loop
	if (y <= 0)
	{
		char2read = (x);
	}
	else
	{
		char2read = (x*y);
	}
	LPWSTR histBuff = new TCHAR[char2read];
	//MessageBox(NULL,(LPCWSTR) char2read,L"Char2Read",MB_OK | MB_SYSTEMMODAL);
	ReadConsoleOutputCharacter(hStdOut, histBuff, char2read, histCord, &numcharread);
	std::wstring histStr = histBuff;
	histStr = histStr.substr(0, numcharread);
	history = std::string(histStr.begin(), histStr.end());

	start.X = csbiInfo.dwCursorPosition.X;
	start.Y = csbiInfo.dwCursorPosition.Y + 1;

	SMALL_RECT srctScrollRect;//declaration of scrollRect
	SMALL_RECT srctClipRect;//clipping rectangle
	COORD coordDest;//destination coordinate
	COORD coordCur;//coordinates to move the cursor

	srctScrollRect.Top = 0;
	srctScrollRect.Bottom = csbiInfo.dwSize.Y;
	srctScrollRect.Left = 0;
	srctScrollRect.Right = csbiInfo.dwSize.X;//scrolling rectangle

	coordDest.X = 0;
	coordDest.Y = -(csbiInfo.dwCursorPosition.Y);//scrolling destination coordinate

	coordCur.X = csbiInfo.dwCursorPosition.X;
	coordCur.Y = 0;//coordinates to move the cursor

	srctClipRect = srctScrollRect; //clipping rectangle and scrolling rectangle are the same.

	if (!ScrollConsoleScreenBuffer(
		hStdOut,         // screen buffer handle 
		&srctScrollRect, // scrolling rectangle 
		&srctClipRect,   // clipping rectangle 
		coordDest,       // top left destination cell 
		&chiFill))       // fill character and color
	{
		LOG(INFO) << "Could not scroll the screen buffer!" << GetLastError() << std::endl;
		return;
	}
	SetConsoleCursorPosition(hStdOut, coordCur);//move the cursor to top
	COORD coordFill;
	coordFill.X = 0;
	coordFill.Y = 1;
	FillConsoleOutputCharacter(hStdOut, '\0', csbiInfo.dwSize.X*csbiInfo.dwSize.Y + 1, coordFill, &numcharread);//and fill the rest of the screen with line terminators
	++readLcount;
	old_history = history;//for future use.previous history page
	return;
}

//****************************************************************************************************
//CONSOLE WINDOW SETUP
//******************************************************************************************************

//window setup
void SetConsoleWindowSize(int x, int y, HANDLE h)
{
	//HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	if (h == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Unable to get stdout handle.");

	// If either dimension is greater than the largest console window we can have,
	// there is no point in attempting the change.
	{
		COORD largestSize = GetLargestConsoleWindowSize(h);
		if (x > largestSize.X)
			throw std::invalid_argument("The x dimension is too large.");
		if (y > largestSize.Y)
			throw std::invalid_argument("The y dimension is too large.");
	}


	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	if (!GetConsoleScreenBufferInfo(h, &bufferInfo))
		throw std::runtime_error("Unable to retrieve screen buffer info.");

	SMALL_RECT& winInfo = bufferInfo.srWindow;
	COORD windowSize = { winInfo.Right - winInfo.Left + 1, winInfo.Bottom - winInfo.Top + 1 };

	if (windowSize.X > x || windowSize.Y > y)
	{
		// window size needs to be adjusted before the buffer size can be reduced.
		SMALL_RECT info =
		{
			0,
			0,
			x < windowSize.X ? x - 1 : windowSize.X - 1,
			y < windowSize.Y ? y - 1 : windowSize.Y - 1
		};

		if (!SetConsoleWindowInfo(h, TRUE, &info))
			throw std::runtime_error("Unable to resize window before resizing buffer.");
	}

	COORD size = { x, y };
	if (!SetConsoleScreenBufferSize(h, size))
		throw std::runtime_error("Unable to resize screen buffer.");


	SMALL_RECT info = { 0, 0, x - 1, y - 1 };
	if (!SetConsoleWindowInfo(h, TRUE, &info))
		throw std::runtime_error("Unable to resize window after resizing buffer.");
}



void ShowLastSystemError()
{
	LPSTR messageBuffer;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		0,          // source
		GetLastError(),
		0,          // lang
		(LPSTR)&messageBuffer,
		0,
		NULL);

	std::cerr << messageBuffer << '\n';
	LocalFree(messageBuffer);
}

COORD QueryUserForConsoleSize()
{
	COORD size = { 0, 0 };

	std::cout << "New console size: ";
	std::cin >> size.X >> size.Y;

	return size;
}


//set screen buffer.
void ScreenSize(int x, int y, HANDLE Handle) {

	COORD coord;
	coord.X = x;
	coord.Y = y;

	_SMALL_RECT Rect;
	Rect.Top = 0;
	Rect.Left = 0;
	Rect.Bottom = x - 1;
	Rect.Right = y - 1;

	//HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleScreenBufferSize(Handle, coord);
	SetConsoleWindowInfo(Handle, TRUE, &Rect);
	SetConsoleWindowSize(x, y, Handle);

}



/*
*********************************************************************************************
END CONSOLE WINDOW SETUP

*********************************************************************************************
*/


/*******************************************************************
STARTS THE DESIRED PROCESS
********************************************************************/
void startProcess(std::string cmd)
{

	hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdErr = GetStdHandle(STD_ERROR_HANDLE);

	std::wstring stemp = std::wstring(cmd.begin(), cmd.end());
	LPCWSTR comm = stemp.c_str();


	ZeroMemory(&sj, sizeof(sj));
	ZeroMemory(&pj, sizeof(pj));
	sj.cb = sizeof(sj);
	sj.dwFlags = STARTF_USESTDHANDLES;
	sj.hStdInput = hStdIn;
	sj.hStdOutput = hStdOut;
	sj.hStdError = hStdErr;
	sj.wShowWindow = SW_MINIMIZE;

	if (!CreateProcess(comm, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &sj, &pj))
	{
		printf("CreateProcess failed (%d)\n", GetLastError());
		//_getch();
		hConsole1 = pj;
		return;
	}
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbiInfo))
	{
		printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
		return;
	}
	/*need to make buffer size = current console size to prevent scrolling*/
	//COORD scrsize;
	//scrsize.X = 100;//csbiInfo.srWindow.Right+1;
	//scrsize.Y = 36;//csbiInfo.srWindow.Bottom+1;
	//SetConsoleScreenBufferSize(hStdOut, scrsize);
	ScreenSize(100, 36, hStdOut);


}
//overloaded , just for running with arguments.
void startProcess(std::string cmd, std::string param)
{
	hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdErr = GetStdHandle(STD_ERROR_HANDLE);
	if (param != "") {
		std::string comm(cmd + " " + param);
		std::wstring stemp = std::wstring(comm.begin(), comm.end());
		LPWSTR params = (LPWSTR)stemp.c_str();

		ZeroMemory(&sj, sizeof(sj));
		sj.cb = sizeof(sj);
		ZeroMemory(&pj, sizeof(pj));
		sj.dwFlags = STARTF_USESTDHANDLES;
		sj.hStdInput = hStdIn;
		sj.hStdOutput = hStdOut;
		sj.hStdError = hStdErr;


		if (!CreateProcess(NULL, params, NULL, NULL, TRUE, 0, NULL, NULL, &sj, &pj))
		{
			printf("CreateProcess failed (%d)\n", GetLastError());
			hConsole1 = pj;
			return;
		}
	}
	else
	{
		std::string comm(cmd);
		std::wstring stemp = std::wstring(comm.begin(), comm.end());
		LPWSTR params = (LPWSTR)stemp.c_str();

		ZeroMemory(&sj, sizeof(sj));
		sj.cb = sizeof(sj);
		ZeroMemory(&pj, sizeof(pj));
		sj.dwFlags = STARTF_USESTDHANDLES;
		sj.hStdInput = hStdIn;
		sj.hStdOutput = hStdOut;
		sj.hStdError = hStdErr;

		if (!CreateProcess(NULL, params, NULL, NULL, TRUE, 0, NULL, NULL, &sj, &pj))
		{
			printf("CreateProcess failed (%d)\n", GetLastError());
			//_getch();
			hConsole1 = pj;
			return;
		}

	}
	LPWSTR gc = GetCommandLine();
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbiInfo))
	{
		printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
		return;
	}
	/*need to make buffer size = current console size to prevent scrolling*/
	//COORD scrsize;
	//scrsize.X = 100;//csbiInfo.srWindow.Right+1;
	//scrsize.Y = 36;//csbiInfo.srWindow.Bottom+1;
	//SetConsoleScreenBufferSize(hStdOut, scrsize);

	ScreenSize(100,36, hStdOut);

}

void init(std::string pgm)
{
	//coordinate init
	start.X = 0;
	start.Y = 0;
	origin.X = 0;
	origin.Y = 0;

	if (CreateDirectory(L"log", NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
		// CopyFile(...)
	}
	else
	{
		// Failed to create directory.
		std::cout << "could not create log directory!" << std::endl;
	}

	//logging init 
	el::Configurations defaultConf;
	defaultConf.setToDefault();
	// Values are always std::string
	defaultConf.set(el::Level::Info, el::ConfigurationType::ToStandardOutput, "false");
	defaultConf.set(el::Level::Info, el::ConfigurationType::ToFile, "true");
	defaultConf.set(el::Level::Info, el::ConfigurationType::Filename, "log\\" + pgm + "_%datetime{%d-%M-%Y_%h%m%s}.log");
	el::Loggers::reconfigureLogger("default", defaultConf);
	el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);
	//logger ends


}

//readFile functions
void readIni(std::string filename)
{
	ini.SetUnicode();
	ini.LoadFile(filename.c_str());
	return;
}

std::map<std::string, std::string> readVals(const char* sect)
{
	std::map<std::string, std::string> staging_map;
	const char * pszValue = "";
	for (CSimpleIniA::TNamesDepend::iterator it = keys.begin(); it != keys.end(); ++it) {
		const char *val = it->pItem;
		std::string sym(val);
		const char* val1 = sym.c_str();
		pszValue = ini.GetValue(sect, val1, "NA" /*default*/);
		std::string pzv(pszValue);
		staging_map[sym] = pzv;

	}
	return staging_map;

}


std::list<std::pair<std::string, std::string>> readSeqVals(const char* sect)
{
	std::list<std::pair<std::string, std::string>> staging_list;
	const char * pszValue = "";
	for (CSimpleIniA::TNamesDepend::iterator it = keys.begin(); it != keys.end(); ++it) {
		const char *val = it->pItem;
		std::string sym(val);
		const char* val1 = sym.c_str();
		pszValue = ini.GetValue(sect, val1, "NA" /*default*/);
		std::string pzv(pszValue);
		staging_list.push_back(std::make_pair(sym, pzv));

	}
	return staging_list;

}


std::list<std::pair<std::string, std::string>> readSeqKeys(std::string sect)
{
	ini.GetAllKeys(sect.c_str(), keys);
	keys.sort(CSimpleIniA::Entry::LoadOrder());
	return readSeqVals(sect.c_str());
}


std::map<std::string, std::string> readKeys(std::string sect)
{
	ini.GetAllKeys(sect.c_str(), keys);
	return readVals(sect.c_str());
}

int cmpStr(std::string iniStr, std::string actStr)
{
	if (iniStr == actStr)
	{
		return 1;
	}

	return 0;
}

//processor functions

void processPgm(std::map<std::string, std::string> tmap)
{
	std::map<std::string, std::string>::iterator it = tmap.begin();
	while (it != tmap.end())
	{
		////std::cout<<it->first<<" --> "<<it->second<<std::endl;
		if (it->second != "" && it->first != "")
		{
			if (it->first == "ProgramName")
			{
				pgname = it->second;
			}
			else
			{
				args = it->second;
			}
		}
		else
		{
			if (it->first == "Arguments")
			{
				args = "";
			}
			else
			{
				//std::cout<<"Program Name is missing..Exiting!"<<std::endl;
				return;
			}

		}
		it++;
	}
}

int processCwd(std::map<std::string, std::string> tmap)
{
	std::map<std::string, std::string>::iterator it = tmap.begin();
	while (it != tmap.end())
	{
		if (it->second != "" && it->first != "")
		{
			if (_chdir(it->second.c_str()) != 0)
			{
				LOG(INFO) << "Could not change working directory to: '" << it->second << "'" << std::endl;
				return 1;
			}
			else
			{
				////std::cout<<"Directory Changed to:"<<it->second<<std::endl;
				LOG(INFO) << "Changed working directory to: '" << it->second << "'" << std::endl;
				return 0;
			}
		}
		it++;
	}
}

int processEnvVar(std::map<std::string, std::string> tmap)
{
	std::map<std::string, std::string>::iterator it = tmap.begin();
	while (it != tmap.end())
	{
		if (it->first != "" && it->second != "")
		{
			LPCWSTR a, b;
			std::wstring stemp = std::wstring(it->first.begin(), it->first.end());
			a = stemp.c_str();

			stemp = std::wstring(it->second.begin(), it->second.end());
			b = stemp.c_str();
			if (SetEnvironmentVariable(a, b) != 0)
			{
				//Log the failure to set the env variable.
				//combination failure can happen , so single return wont work
				//std::cout<<"Env set for : "<<it->first<<std::endl;
				LOG(INFO) << "Set Env Var: '" << a << "' and value : '" << b << "'" << std::endl;

			}
			else
			{
				//log success;
				//std::cout<<"Could not set the variable : "<<GetLastError()<<std::endl;
				LOG(INFO) << "Could not set Env Var: '" << a << "' and value : '" << b << "'" << std::endl;
			}

		}
		else
		{
			//std::cout<<"Env Skipping :"<<it->first<<" as blank values found!"<<std::endl;
			LOG(INFO) << "Skipping blank Env Var: '" << it->first << "' and value : '" << it->second << "'" << std::endl;
		}
		it++;
	}

	return 0;
}

int findPos(std::list<std::pair<std::string, std::string>> cmds, std::string tstep)
{
	int t_count = 0;
	for (auto t_it = cmds.begin(); t_it != cmds.end(); ++t_it)
	{
		if (t_it->first == tstep)
		{
			int tmp = t_count;
			t_count -= t_old;
			t_old = tmp;
			return t_count;
		}
		++t_count;
	}
	return -15000; //impossible value, no match found
}


void ExecComm(std::list<std::pair<std::string, std::string>> cmds)
{
	//std::string vars[5]={""};
	std::string nStep = cmds.begin()->first;
	auto it = cmds.begin();
	do {
		std::string currKey = it->first;
		std::string currLine = it->second;
		std::string dbgMsg = "Current Step : " + it->first + " & its params :" + it->second;
		//std::cout<<"Executing: "<<currKey<<" -> "<<currLine<<std::endl;
		LOG(INFO) << "Current Step is: " << it->first << " and it's parameters are : " << it->second;
		if (exitFlg == true)
		{
			LOG(INFO) << "Exit Signal!...Ending the execution!" << std::endl;
			int next = findPos(cmds, "END");
			std::advance(it, next);
			std::string endcmd = it->second;
			if (it->second != "")
				send(trim(it->second));
			break;
		}
		auto commands = split(currLine, '|');
		int default_timeout = 25;
		if (commands.size() == 5 && (commands[4] != "" || commands[4] != "NA"))
		{
			default_timeout = stoi(commands[4]);
		}
		/* new code starts */
		int timeout = 0;
		std::string m_chk = "";
		if (currKey.find("#") != std::string::npos)//checking for history content
		{
			while (1)
			{   //Sleep(10000);
				readConsole();
				m_chk = history;//read history value
				LOG(INFO) << "Checking '" << commands[0] << "' in screen buffer history : " << std::endl << history << std::endl;
				if (std::regex_match(m_chk, std::regex(commands[0])))//got a match m_chk.find(commands[0]) != std::string::npos
				{
					send(commands[1]);//execute success command
									  //call if condition
					LOG(INFO) << "History Match Succeeded!...sending command: '" << commands[1] << "'" << std::endl;
					if (commands.size()>2 && (commands[2] != "" || commands[2] != "NA"))//if we have post success step
					{
						int next = findPos(cmds, commands[2]);//find that step
						if (next == -15000)//if not found
						{
							LOG(INFO) << "Could not find step : '" << commands[2] << "' , exiting..." << std::endl;//notify that
							exitFlg = true;
							break;
						}
						std::advance(it, next);
						LOG(INFO) << "Moving to step : '" << commands[2] << "' ; '" << it->first << "' -> '" << it->second << "'" << std::endl;
						if (commands[2] == "END")
						{
							exitFlg = true;
							//int next=findPos(cmds,"END");
							//std::advance(it,next);
							LOG(INFO) << "End command found !...Setting exit flag..." << std::endl;
						}
					}
					else
					{
						++it;
						LOG(INFO) << "Could not find success step..Moving to next step '" << it->first << "' --> '" << it->second << std::endl;
					}
					break;
				}
				else
				{
					LOG(INFO) << "History did not match..re-trying to reach max time " << default_timeout << " , trial : " << (timeout + 1) << std::endl;
					if (timeout >= default_timeout/*stoi(commands[4])*/)
					{
						//call if condition but with command[4] instead of 3
						LOG(INFO) << "Trying to get the failure step ...." << std::endl;
						if (commands.size()>3 && (commands[3] != "" || commands[3] != "NA"))
						{
							int next = findPos(cmds, commands[3]);
							if (next == -15000)
							{
								LOG(INFO) << "Could not find step : '" << commands[3] << "' ... Setting Exit Flag!" << std::endl;
								exitFlg = true;
								break;
							}
							std::advance(it, next);
							LOG(INFO) << "Moving to step : '" << commands[3] << "' ; " << it->first << " -> " << it->second << std::endl;
							if (commands[3] == "END")
							{
								exitFlg = true;
								//int next=findPos(cmds,"END");
								//std::advance(it,next);
								LOG(INFO) << "Hit END command .. Setting Exit Flag!" << std::endl;
							}
						}
						else
						{
							LOG(INFO) << "No failure step is provided in the runfile ...Setting Exit flag!" << std::endl;
							exitFlg = true;
							//++it;
						}
						break;
					}
					else
					{
						Sleep(1000);
						timeout += 1;
						LOG(INFO) << "Sleeping for one second ..." << std::endl;
						continue;
					}
				}//
			}
		}
		else
		{
			while (1)//Loop incase of timeout rechecking 
			{
				//Sleep(3000);
				readConsole();
				m_chk = prompt; //read the prompt value
								//bool tst=(m_chk==commands[0]);
				LOG(INFO) << "Checking : '" << commands[0] << "' with prompt '" << prompt << "'" << std::endl;
				if (std::regex_match(m_chk, std::regex(commands[0])))// got a match with expected value  m_chk==commands[0]
				{
					send(commands[1]); //execute corresponding command
									   //call if condition
					LOG(INFO) << "Prompt matched .. sending command '" << commands[1] << "'" << std::endl;
					if (commands.size()>2 && (commands[2] != "" || commands[2] != "NA"))//if there is a post success step
					{
						int next = findPos(cmds, commands[2]);
						if (next == -15000)//no such step then
						{
							LOG(INFO) << "Could not find post success step : '" << commands[2] << "' .. Setting exit flag" << std::endl;//tell that
							exitFlg = true;//throw exit flag
							break;//break the while loop
						}
						std::advance(it, next);//move cursor to that
						LOG(INFO) << "Moving to step : '" << commands[2] << "' ; '" << it->first << "' -> '" << it->second << std::endl;
						if (commands[2] == "END")//if post success step was end, then notify that 
						{
							LOG(INFO) << "END step found ... setting exit flag!" << std::endl;
							//int next=findPos(cmds,"END");
							//std::advance(it,next);
							exitFlg = true;
						}
					}
					else
					{
						++it;
						LOG(INFO) << "No post success step then just go to next step: '" << it->first << "' -> '" << it->second << std::endl;
					}
					break;//now cpme of out while //optimization needed.
				}
				else//did not get the expected prompt
				{
					//call if condition but with command[4] instead of 3
					LOG(INFO) << "Prompt did not match..re-trying to reach max time " << default_timeout << " , trial : " << (timeout + 1) << std::endl;
					if (timeout >= default_timeout/*stoi(commands[4])*/)//if completed the max wait time
					{
						//call if condition but with command[4] instead of 3
						LOG(INFO) << "Trying to get the failure step ...." << std::endl;
						if (commands.size()>3 && (commands[3] != "" || commands[3] != "NA"))//and there is a valid post failure step
						{
							int next = findPos(cmds, commands[3]);//search the index of post failure step
							if (next == -15000) //if post failure step is not existing
							{
								//std::cout<<"Could not find step :"<<commands[3];//tell that
								LOG(INFO) << "Could not find step : '" << commands[3] << "' ... Setting Exit Flag!" << std::endl;
								exitFlg = true;
								break;//come out of while loop and throw that 
							}
							std::advance(it, next);//if post failure step is found move to that
												   //std::cout<<"Moving to step :"<<commands[3]<<" ; "<<it->first<<" -> "<<it->second<<std::endl;
							LOG(INFO) << "Moving to step : '" << commands[3] << "' ; " << it->first << " -> " << it->second << std::endl;
							if (commands[3] == "END")//if the step key is END exit , as it is the last step.
							{
								exitFlg = true;
								//int next=findPos(cmds,"END");
								//std::advance(it,next);
								LOG(INFO) << "Hit END command .. Setting Exit Flag!" << std::endl;
							}
						}
						else
						{
							//++it;//if no post failure step , end
							LOG(INFO) << "No failure step is provided in the runfile ...Setting Exit flag!" << std::endl;
							exitFlg = true;

						}
						break;//come out of loop
					}
					else//not yet reached timeout
					{
						Sleep(1000);//poll every second
						timeout += 1;//increment timeout
						LOG(INFO) << "Sleeping for one second ..." << std::endl;
						continue;//loop again
					}
				}//
			}
		}
	} while (it != cmds.end());
}


int _tmain(int argc, _TCHAR* argv[])
{
	//try {
		std::list<std::pair<std::string, std::string>> Seq_cmds;
		std::map<std::string, std::string> Path;
		std::map<std::string, std::string> EnvironmentVariables;
		std::map<std::string, std::string> ProgramSection;
		std::map<std::string, std::string> Configuration;

		if (argc != 2)
		{
			LOG(INFO) << "No ini file was passed as an argument..trying with default ini" << std::endl;
			if (exists_test3("runfile.ini"))
			{
				readIni("runfile.ini");
			}
			else
			{
				LOG(INFO) << "No runfile.ini file was found in the working directory..exiting" << std::endl;
				return 1;
			}
		}
		else
		{
			std::wstring runfile;
			runfile = argv[1];
			std::string runfile1(runfile.begin(), runfile.end());
			readIni(runfile1);
			LOG(INFO) << "Data read from ini file '" << runfile1 << "'" << std::endl;

		}

		Seq_cmds = readSeqKeys("SequenceCommands");
		Path = readKeys("Path");
		EnvironmentVariables = readKeys("EnvironmentVariables");
		ProgramSection = readKeys("ProgramSection");
		Configuration = readKeys("Configuration");

		processPgm(ProgramSection);
		processCwd(Path);
		processEnvVar(EnvironmentVariables);
		Sleep(1000);
		std::size_t found = pgname.find_last_of("/\\");

		init(pgname.substr(found + 1));
		LOG(INFO) << "Starting Program :" + pgname << std::endl;
		startProcess(pgname, args);
		Sleep(3000);
		ExecComm(Seq_cmds);
		//_getch();
		LOG(INFO) << "Program Ended";
		CloseHandle(pj.hProcess);
		CloseHandle(pj.hThread);
	//}
	/*catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;

	}*/
	return 0;
}

