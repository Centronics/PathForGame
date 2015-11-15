#include "MainHeader.h"
#define Naked static __declspec(naked)
#define HEALTH 1003
#define ARMOR 1002
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved );
HINSTANCE hInstance = NULL; HHOOK myMouseHook = NULL; HHOOK myKeyboardHook = NULL;
static int Force = 0, PassForce = 0; static bool MinSpread = false; static DWORD baseaddr = 0, FEARMPaddr = 0, GameDataaddr = 0;
LRESULT CALLBACK myMouse ( int nCode, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK myKeyboard ( int nCode, WPARAM wParam, LPARAM lParam );
DWORD WINAPI Start(PVOID Par);
void inline static __cdecl FastWrite(void* DstAddr, void* SrcMas, unsigned int Count);
static void PreparePaths();
void UnHOOK();
bool SetCheat();
void Go(HMODULE hMod);
static void MouseControls();
void inline HF(bool fl);
void FireForce();
void BulletCounter();
void passGOOD();
void GetAutoMessage();
void MyPosValueSet();
void HUD_INIT();
void HUDCount();

enum EGameState
{
	GS_UNDEFINED=0,
	GS_PLAYING,
	GS_EXITINGLEVEL,
	GS_LOADINGLEVEL,
	GS_SPLASHSCREEN,
	GS_MENU,
	GS_SCREEN,
	GS_PAUSED,
	GS_DEMOSCREEN,
	GS_MOVIE,
};

enum EHUDMODES
{
	NOTHING,
	TEXT,
	PATRONS,
	CHAT,
};

static unsigned int __stdcall str_Hash(const wchar_t *pString);
unsigned char SpreadHack [5] = { 0xE9, 0x6C, 0x08, 0x03, 0x00 }; unsigned char SpreadHack_back [5] = { 0xC3, 0xCC, 0xCC, 0xCC, 0xCC };
unsigned int speedhack[4] = { 0x000000B8, 0x00009041, 0x00AC868B, 0x0 };
unsigned int respawnhack[2] = { 0x9090, 0xEB }; unsigned int respawnhack_back[9] = { 0x75, 0xB60F3077, 0x0B136090, 0x9524FF10, 0x100B1350, 0x01307E83, 0x1574, 0x1275, 0x3774 };
unsigned int BulletCounter_back[2] = { 0x38244C8B, 0x2024748B };
static bool HyperFire = true, NoSpreadAlways = true, inviz = false, speedh = true, resph = false, actresp_minus = true, StillFire = false, SCMDWorker = false;
static bool ReadMyPosition = false, MyPositionWrited = false; unsigned char MyPosValueMode=0;
static int TIMER_MAX=800, Timer=800;
DWORD dwOldProtect = 0;
void UnlimitedAmmo();
void WriteDatabaseRecord();
void MsgRead();
void GetHealthOrArmor();
void HAA();
void AutoVote();
void VoteOnOff();
static void SetDefaults();
__declspec(noreturn) inline void ErrorHandler(HMODULE hMod);
static DWORD val_1_FEARMP=0, g_pILTClient = (DWORD)0x1a5418;
static DWORD val_1_GameData=0;
static bool InfiniteAmmo=false, allowInfAmmo = false;
static bool InfiniteHealth=true, InfiniteArmor=true, VoteSpam=false, TurnOnOffKickVotes=false, OnOffKickVotes=true, bAdvancedFunctions=false, VoteLocal = false;
static bool DOitnow=true, dontDOit=false, doitnow=true, dontdoit=false, ServerTerm=false;
static unsigned int AmmoInClips=0, *GameState=0;
static unsigned int InjAddrOfBulletCounter=0, InjAddrHyperFire=0, InjAddrOfSpeedHack=0, InjAddrOfCallSpreadHack=0, InjAddrCommon=0;
static float MyHealth=200.0f;
static wchar_t KeyPath[MAX_PATH + 1] = L"", PassHPath[MAX_PATH + 1] = L"", PassPath[MAX_PATH + 1] = L"";
const size_t HUDTEXT   [] = { 0x04, 0x68, 0x80, 0x88, 0xA4, 0x74, 0x40, 0x1C, 0x54, 0x24, 0x20, 0x90, 0x94, 0x18, 0x9C, 0xA0, 0x14 };
const size_t HUDPATRONS[] = { 0x0C, 0x10, 0x28, 0x2C };
const size_t HUDCHAT      =   0x50;
EHUDMODES HUDMODE = EHUDMODES::TEXT; size_t HUDPTR = 0;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH: DisableThreadLibraryCalls((HMODULE)hModule); Go((HMODULE)hModule); break;
		case DLL_PROCESS_DETACH: UnHOOK(); break;
    }
    return TRUE;
}

void Go(HMODULE hMod)
{
	HANDLE myThr = NULL;
	myThr = CreateThread(NULL, NULL, Start, (PVOID)hMod, NULL, NULL);
	if (myThr != NULL) CloseHandle(myThr); else ErrorHandler(hMod);
}

bool SetCheat()
{
	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
	MODULEENTRY32 me32;
	HANDLE mysnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
	if(mysnap == INVALID_HANDLE_VALUE) return false;
	me32.dwSize = sizeof(MODULEENTRY32);
	if(Module32First(mysnap, &me32)==FALSE)
	{
		CloseHandle(mysnap);
		return false;
	}
	else
	{
		do
		{
			if ( (me32.szModule[0] == 'G') && (me32.szModule[1] == 'a') && (me32.szModule[2] == 'm') )
			{
				if (me32.szModule[3] != 'e') { if (me32.modBaseSize==(DWORD)0x1D2000) baseaddr = (DWORD)me32.modBaseAddr; }
				if ((me32.szModule[3] == 'e') && (me32.szModule[4] == 'D') && (me32.szModule[5] == 'a') && (me32.szModule[6] == 't') && (me32.szModule[7] == 'a')) GameDataaddr = (DWORD)me32.modBaseAddr;
			}
			if ((me32.szModule[0] == 'F') && (me32.szModule[1] == 'E') && (me32.szModule[2] == 'A') && (me32.szModule[3] == 'R') && (me32.szModule[4] == 'M') && (me32.szModule[5] == 'P')) FEARMPaddr = (DWORD)me32.modBaseAddr;
		} while(Module32Next(mysnap, &me32)==TRUE);
	}
	CloseHandle(mysnap);
	if ((baseaddr == 0) || (FEARMPaddr == 0) || (GameDataaddr == 0)) return false;
	val_1_FEARMP = FEARMPaddr+(DWORD)0x16A2A8;
	val_1_GameData = GameDataaddr+(DWORD)0xD118;
	unsigned int Writeaddr=baseaddr+0x32710;
	unsigned int Ins [5] = { 0x000001BF, 0x9090C300, 0x34E005C7, 0x0000101A, 0x90C30000 };                                     //NoRecoil and NoSpread hacks
	if (!WriteToMemory((void*)Writeaddr, &Ins, 20)) return false;
	Writeaddr=baseaddr+0x3271A;
	Ins [0] = baseaddr+0x1A34E0;
	if (!WriteToMemory((void*)Writeaddr, Ins[0])) return false;
	Writeaddr=baseaddr+0x31BD0;
	Ins [0] = 0x90909090; Ins [1] = 0x01B89090; Ins [2] = 0x90000000; Ins [3] = 0x000000C3;
	if (!WriteToMemory((void*)Writeaddr, (void*)&Ins, 13)) return false;                                                       //NoReloadHack
	Writeaddr=baseaddr+0x2E520;
	Ins [0] = 0x000000E8; Ins [1] = 0x9000;
	if (!WriteToMemory((void*)Writeaddr, (void*)&Ins, 6)) return false;
	Writeaddr+=5;
	Ins [0] = (unsigned int)FireForce-Writeaddr;
	Writeaddr-=4;
	if (!WriteToMemory((void*)Writeaddr, Ins[0])) return false;                                                                 //Call HyperFire procedure
	Writeaddr=baseaddr+0x3A850;
	if (!WriteToMemory((void*)Writeaddr, (unsigned int)0x000008C2)) return false;                                               //GravityHack
	BOOL ProtectOperation=FALSE; InjAddrOfCallSpreadHack=baseaddr+0x1EA7;
	ProtectOperation=VirtualProtect((LPVOID) InjAddrOfCallSpreadHack, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);                //CallSpreadHack
	if (ProtectOperation==FALSE) return false; InjAddrOfSpeedHack=baseaddr+0x370A6;
	ProtectOperation=VirtualProtect((LPVOID) InjAddrOfSpeedHack, 6, PAGE_EXECUTE_READWRITE, &dwOldProtect);                     //SpeedHack
	if (ProtectOperation==FALSE) return false; InjAddrHyperFire=baseaddr+0x2E502;
	ProtectOperation=VirtualProtect((LPVOID) InjAddrHyperFire, 2, PAGE_EXECUTE_READWRITE, &dwOldProtect);                       //For HyperFire (HF) procedure
	if (ProtectOperation==FALSE) return false; InjAddrOfBulletCounter = baseaddr+0xBCF81;
	ProtectOperation=VirtualProtect((LPVOID) InjAddrOfBulletCounter, 9, PAGE_EXECUTE_READWRITE, &dwOldProtect);                 //Call BulletCounter procedure
	if (ProtectOperation==FALSE) return false; Writeaddr = FEARMPaddr+0x872426;
	if (!WriteToMemory((void*)Writeaddr, (unsigned int)0x000000B9)) return false; __asm inc Writeaddr                           //BinaryMessage hack
	if (!WriteToMemory((void*)Writeaddr, (unsigned int)HAA)) return false; Writeaddr+=4;
	if (!WriteToMemory((void*)Writeaddr, (unsigned int)0x5BC3E1FF)) return false;
	Writeaddr=baseaddr+0x015442D;																								//GOOD admin password flag
	if (!WriteToMemory((void*)Writeaddr, (unsigned int)0x52FF29EB)) return false;
	Writeaddr=baseaddr+0x0154458;
	if (!WriteToMemory((void*)Writeaddr, (unsigned int)0x00E9C88B)) return false; Writeaddr+=7;
	Ins [0] = (unsigned int)passGOOD-Writeaddr; Writeaddr-=4;
	if (!WriteToMemory((void*)Writeaddr, Ins[0])) return false;
	InjAddrCommon=baseaddr+0x522CC;																								//Remove "Cant quick save"
	if (!WriteToMemory((void*)InjAddrCommon, (unsigned int)0x00008EE9)) return false;
	InjAddrCommon=baseaddr+0x3271E;
	WriteToMemory((void*)InjAddrCommon, (unsigned int)0x3e800000);
	FastWrite((void*)InjAddrOfCallSpreadHack, (void*)&SpreadHack, 5);
	InjAddrCommon=baseaddr+0x3A159;																								//Position Read/Write/Disabled
	WriteToMemory((void*)InjAddrCommon, (unsigned int)0x000000B9); InjAddrCommon++;
	WriteToMemory((void*)InjAddrCommon, (unsigned int)MyPosValueSet); InjAddrCommon+=4;
	WriteToMemory((void*)InjAddrCommon, (unsigned int)0x0000E1FF); InjAddrCommon+=2;
	WriteToMemory((void*)InjAddrCommon, (unsigned int)0x6A168B90);
	InjAddrCommon=baseaddr+0x1A54A0;																							//GameStateGrabber
	GameState=(unsigned int*)InjAddrCommon;
	InjAddrCommon=FEARMPaddr+0xE358;																							//PasswordGenerator
	unsigned int tmpOffset = ((unsigned int)GetAutoMessage) - InjAddrCommon;
	InjAddrCommon-=4;
	WriteToMemory((void*)InjAddrCommon, tmpOffset);
	InjAddrCommon=(Writeaddr=baseaddr+0x006FBB7+5) - 6;                                                                         //HUD_INIT
	WriteToMemory((void*)InjAddrCommon, 0xE890); InjAddrCommon += 2;
	Writeaddr=((unsigned int)&HUD_INIT) - Writeaddr;
	WriteToMemory((void*)InjAddrCommon, Writeaddr);
	InjAddrCommon=Writeaddr=baseaddr+0x006FBF7;                                                                                 //Remove HUD Items
	WriteToMemory((void*)Writeaddr, 0xE8); Writeaddr += 5;
	Writeaddr=((unsigned int)&HUDCount) - Writeaddr;
	WriteToMemory((void*)(InjAddrCommon + 1), Writeaddr);

	g_pILTClient += baseaddr;

	PreparePaths();
	HWND hwnd = FindWindow(NULL, TEXT("F.E.A.R."));
	if (hwnd==NULL) return false;
	DWORD idThread = GetWindowThreadProcessId(hwnd, NULL);
	if (idThread==NULL) return false;
	myKeyboardHook = SetWindowsHookEx(WH_KEYBOARD, myKeyboard, NULL, idThread);
	if (myKeyboardHook==NULL) return false;
	myMouseHook = SetWindowsHookEx(WH_MOUSE, myMouse, NULL, idThread);
	if (myMouseHook==NULL) return false;
	return true;
}

DWORD WINAPI Start(PVOID Par)
{
	if (!SetCheat()) ErrorHandler((HMODULE)Par);
	Sleep(INFINITE);
	return NULL;
}

__declspec(noreturn) static void ErrorHandler(HMODULE hMod)
{
	MessageBox(NULL, TEXT("Не удалось установить чит!"), TEXT("Ошибка!"), MB_OK);
	FreeLibraryAndExitThread(hMod, (DWORD)0);
}

static void SetDefaults()
{
	NoSpreadAlways = true;
	FastWrite((void*)InjAddrOfCallSpreadHack, (void*)&SpreadHack, 5);
	InjAddrCommon=baseaddr+0xBF33;
	WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack_back[8], 2); InjAddrCommon=baseaddr+0x5214A;
	WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack_back[7], 2); InjAddrCommon=baseaddr+0xAF63A;
	WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack_back, 1); InjAddrCommon=baseaddr+0xB1310;
	WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack_back[1], 22);
	resph = false;
	HyperFire = true; HF (false);
	speedh=true; FastWrite((void*)InjAddrOfSpeedHack, (void*)&speedhack[2], 6);
	StillFire=false; InfiniteAmmo=false; allowInfAmmo = false; VoteLocal=false; ServerTerm=false;
	FastWrite((void*)InjAddrOfBulletCounter, (void*)&BulletCounter_back, 8);
	InfiniteHealth=true; InfiniteArmor=true; VoteSpam=false; TurnOnOffKickVotes=false; OnOffKickVotes=true;
	AmmoInClips=0; PassForce=Force=0; TIMER_MAX=800; Timer=800; MinSpread = false; bAdvancedFunctions=false;
	InjAddrCommon=baseaddr+0x3271E;
	WriteToMemory((void*)InjAddrCommon, (unsigned int)0x3e800000);
	MyPosValueMode=0; ReadMyPosition=false;
}

void inline static __cdecl FastWrite(void* DstAddr, void* SrcMas, unsigned int Count)
{
	_asm
	{
		mov edi, DstAddr
		mov esi, SrcMas
		mov ecx, Count
		REP movsb
	}
}

static void MouseControls()
{
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		if (DOitnow)
		{
			unsigned int* MyNakedProc=(unsigned int*)InjAddrOfBulletCounter;
			*MyNakedProc=0xB9;
			_asm inc MyNakedProc
			*MyNakedProc = (unsigned int)BulletCounter; MyNakedProc++;
			*MyNakedProc = (unsigned int)0x8990D1FF;
			if (HyperFire) HF(true);
			if (allowInfAmmo) InfiniteAmmo=true;
			dontDOit=true;
			DOitnow=false;
		}
	}
	else
	{
		if (dontDOit)
		{
			FastWrite((void*)InjAddrOfBulletCounter, (void*)&BulletCounter_back, 8);
			HF(false);
			InfiniteAmmo=false;
			dontDOit=false;
			DOitnow=true;
		}
	}

	if (GetAsyncKeyState(VK_RBUTTON))
	{
		if (doitnow)
		{
			if(!NoSpreadAlways) FastWrite((void*)InjAddrOfCallSpreadHack, (void*)&SpreadHack, 5);
			if (speedh) FastWrite((void*)InjAddrOfSpeedHack, (void*)&speedhack, 6);
			if (StillFire) { if (allowInfAmmo) { InfiniteAmmo=true; AmmoInClips=0; } HF(true); }
			if (ReadMyPosition) MyPosValueMode = 1;
			dontdoit=true;
			doitnow=false;
		}
		return;
	}
	else
	{
		if (dontdoit)
		{
			if(!NoSpreadAlways) FastWrite((void*)InjAddrOfCallSpreadHack, (void*)&SpreadHack_back, 5);
			FastWrite((void*)InjAddrOfSpeedHack, (void*)&speedhack[2], 6);
			if (StillFire) { InfiniteAmmo=false; HF(false); }
			if (ReadMyPosition) MyPosValueMode = 0;
			dontdoit=false;
			doitnow=true;
		}
	}
}

static bool PassGenerator(register wchar_t* str, const size_t Symb)
{
	if(!Symb) return false;
	register size_t numofDigits = Symb;
	const wchar_t start = 0x0021;
	const wchar_t finish = 0x007E;
	str += numofDigits;
	do
	{
		if(*(--str) < finish)
		{
			(*str)++;
			break;
		}
	} while(--numofDigits);
	if(numofDigits)
	{
		numofDigits = Symb - numofDigits;
		wmemset(++str, start, numofDigits);
		return true;
	}
	return false;
}

unsigned int passHash = 0;

static unsigned int WINAPI PassProc(void*)
{
	wchar_t passCurrent[] = L"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	const unsigned int psh = passHash; unsigned char lim = 3;
	for(unsigned char n = 1; n <= 15;)
	{
		wchar_t* const tmp = passCurrent + (15 - n);
		if(PassGenerator(tmp, n))
		{
			if(str_Hash(tmp) == psh)
			{
				const HANDLE myFile = CreateFileW(PassPath, FILE_WRITE_DATA, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if(myFile == INVALID_HANDLE_VALUE) return NULL;
				DWORD nn = SetFilePointer(myFile, 0L, NULL, FILE_END);
				if(nn == INVALID_SET_FILE_POINTER)
				{
					CloseHandle(myFile);
					return NULL;
				}
				if(nn > 2)
				{
					WriteFile(myFile, L"\r\n", 2 * sizeof(wchar_t), &nn, NULL);
				}
				else
				{
					SetFilePointer(myFile, 0L, NULL, FILE_BEGIN);
					wchar_t tsym = 0xFEFF;
					WriteFile(myFile, &tsym, sizeof(wchar_t), &nn, NULL);
				}
				DWORD tsize = static_cast<DWORD>(&passCurrent[15] - tmp);
				WriteFile(myFile, tmp, tsize * sizeof(wchar_t), &nn, NULL);
				CloseHandle(myFile);
				if((--lim, !lim)) return NULL;
			}
		}
		else n++;
	}
	return NULL;
}

unsigned int EnterHash = 0, CheatRepeat = 0;
wchar_t *ReadPassFile = nullptr;
wchar_t passread[(25 * 20) + 2] = L"";
wchar_t *preventer = passread, *passlptr = nullptr;
bool PassStarted = false;

LRESULT CALLBACK myMouse (int nCode, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(nCode);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	MouseControls();
	return NULL;
}

LRESULT CALLBACK myKeyboard (int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode != HC_ACTION) return NULL;
	else
	{
		if (wParam == VK_DIVIDE) if (lParam&0x80000000)
		{
			InjAddrCommon=baseaddr+0x3271E;
			if (MinSpread)
			{
				WriteToMemory((void*)InjAddrCommon, (unsigned int)0x3e800000);
				MinSpread=false;
				return NULL;
			}
			if (!MinSpread)
			{
				WriteToMemory((void*)InjAddrCommon, (unsigned int)0x00000000);
				MinSpread=true;
			}
			return NULL;
		}

		if (wParam == VK_F4) if (lParam&0x80000000)
		{
			if(SCMDWorker)
			{
				if(PassForce < 3) PassForce++;
			}
			else
			{
				if (Force < 3) Force++;
			}
			return NULL;
		}

		if (wParam == VK_F3) if (lParam&0x80000000)
		{
			if(SCMDWorker)
			{
				if(PassForce > 0) PassForce--;
			}
			else
			{
				if (Force > 0) Force--;
			}
			return NULL;
		}

		if (wParam == VK_INSERT) if (lParam&0x80000000)
		{
			if (bAdvancedFunctions) ServerTerm = true;
			else
			{
				passlptr = ReadPassFile = nullptr;
				preventer = passread;
				passread[0] = L'\0';
			}
			return NULL;
		}

		if (wParam == VK_NUMPAD7) if (lParam&0x80000000)
		{
			HyperFire = !HyperFire; HF (false);
			return NULL;
		}

		if (wParam == VK_NUMPAD8) if (lParam&0x80000000)
		{
			if (!NoSpreadAlways)
			{
				NoSpreadAlways = true;
				FastWrite((void*)InjAddrOfCallSpreadHack, (void*)&SpreadHack, 5);
				return NULL;
			}
			if (NoSpreadAlways)
			{
				NoSpreadAlways = false;
				FastWrite((void*)InjAddrOfCallSpreadHack, (void*)&SpreadHack_back, 5);
				return NULL;
			}
		}

		if (wParam == VK_NUMPAD5) if (lParam&0x80000000)
		{
			StillFire=!StillFire; HF(false);
			return NULL;
		}

		if (wParam == VK_NUMPAD4) if (lParam&0x80000000)
		{
			speedh=!speedh;
			return NULL;
		}

		if (wParam == VK_NUMPAD6) if (lParam&0x80000000)
		{
			if(bAdvancedFunctions)
			{
				OnOffKickVotes=!OnOffKickVotes;
				TurnOnOffKickVotes=true;
			}
			else
			{
				if(MyPositionWrited)
				{
					if(MyPosValueMode != 2)
						ReadMyPosition = !ReadMyPosition;
					else
						ReadMyPosition = true;
				}
				MyPosValueMode = 0;
			}
			return NULL;
		}

		if (wParam == VK_NUMPAD9) if (lParam&0x80000000)
		{
			if(bAdvancedFunctions)
				allowInfAmmo=!allowInfAmmo;
			else
			{
				if(MyPosValueMode != 1)
				{
					ReadMyPosition = false;
					if(MyPosValueMode != 2)
						MyPosValueMode = 2;
					else
						MyPosValueMode = 0;
				}
			}
			return NULL;
		}

		if (wParam == VK_F10) if (lParam&0x80000000)
		{
			if(!SCMDWorker)
			{
				if(PassStarted) SCMDWorker = true;
			}
			else
			{
				SCMDWorker = false;
			}
			return NULL;
		}

		if (wParam == VK_F11) if (lParam&0x80000000)
		{
			if(passread[0] == L'\0')
			{
__nex:			const HANDLE myFile = CreateFileW(PassHPath, FILE_READ_DATA, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if(myFile == INVALID_HANDLE_VALUE) return NULL; DWORD tmpsz;
				BOOL readres = ReadFile(myFile, passread, sizeof(passread) - sizeof(wchar_t), &tmpsz, NULL);
				CloseHandle(myFile);
				tmpsz /= sizeof(wchar_t);
				*(passlptr = &passread[tmpsz]) = L'\0';
				if(passread[0] == 0xFEFF)
				{
					preventer = ReadPassFile = passread + 1;
					tmpsz--;
				}
				else preventer = ReadPassFile = passread;
				if(readres != TRUE || tmpsz < 4)
				{
					passlptr = ReadPassFile = nullptr;
					preventer = passread;
					passread[0] = L'\0';
					SCMDWorker = false;
					return NULL;
				}
			}
			if((passlptr - ReadPassFile) >= 4)
			{
				for(preventer = ReadPassFile; *ReadPassFile != L'\r' && *ReadPassFile != L'\0'; ReadPassFile++)
				{
					if((ReadPassFile - preventer) == 15)
					{
						goto gnd;
					}
				}
				*ReadPassFile = L'\0';
				if((passlptr - ReadPassFile) >= 3)
				{
					ReadPassFile += 2;
					wchar_t* rpf;
					unsigned long tpas = wcstoul(ReadPassFile, &rpf, 16);
					if(ReadPassFile == rpf)
					{
						goto gnd;
					}
					EnterHash = str_Hash(preventer);
					passHash = static_cast<unsigned int>(tpas);
					ReadPassFile = rpf + 2;
					SCMDWorker = true;
					PassStarted = true;
					CheatRepeat = 0;
					return NULL;
				}
			}
			else
			{
				passlptr = ReadPassFile = nullptr;
				preventer = passread;
				passread[0] = L'\0';
				goto __nex;
			}
	gnd:	passread[0] = L'\0';
			passlptr = ReadPassFile = nullptr;
			preventer = passread;
			SCMDWorker = false;
			return NULL;
		}

		if (wParam == VK_F12) if (lParam&0x80000000)
		{
			if(PassStarted)
			{
				const HANDLE myFile = CreateFileW(PassHPath, FILE_WRITE_DATA, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if(myFile == INVALID_HANDLE_VALUE) return NULL;
				DWORD nn = SetFilePointer(myFile, 0L, NULL, FILE_END);
				if(nn == INVALID_SET_FILE_POINTER)
				{
					CloseHandle(myFile);
					return NULL;
				}
				wchar_t* tmp = (*preventer != L'\0') ? preventer : nullptr;
				if(tmp != nullptr)
				{
					if(nn > 2)
					{
						WriteFile(myFile, L"\r\n", 2 * sizeof(wchar_t), &nn, NULL);
					}
					nn = wcslen(tmp) * sizeof(wchar_t);
					WriteFile(myFile, tmp, nn, &nn, NULL);
					wchar_t str[] = L"\r\n12345678";
					_ui64tow(passHash, str + 2, 16);
					nn = wcslen(str) * sizeof(wchar_t);
					WriteFile(myFile, str, nn, &nn, NULL);
				}
				CloseHandle(myFile);
			}
			return NULL;
		}

		if (wParam == VK_F9) if (lParam&0x80000000)
		{
			if(PassStarted)
			{
				HANDLE thr = (HANDLE)_beginthreadex(NULL, 0, &PassProc, NULL, CREATE_SUSPENDED, NULL);
				if(!thr || thr == (HANDLE)-1L)
				{
					return NULL;
				}
				SetThreadPriority(thr, THREAD_PRIORITY_BELOW_NORMAL);
				ResumeThread(thr);
				CloseHandle(thr);
			}
			return NULL;
		}

		if (wParam == VK_HOME) if (lParam&0x80000000)
		{
			if (bAdvancedFunctions) InfiniteHealth=!InfiniteHealth;
			return NULL;
		}

		if (wParam == VK_END) if (lParam&0x80000000)
		{
			if (bAdvancedFunctions) InfiniteArmor=!InfiniteArmor;
			return NULL;
		}

		if (wParam == VK_NUMPAD1) if (lParam&0x80000000)
		{
			VoteLocal=true;
			return NULL;
		}

		if (wParam == VK_NUMPAD2) if (lParam&0x80000000)
		{
			Timer = TIMER_MAX;
			VoteSpam=!VoteSpam;
			ServerTerm=false;
			return NULL;
		}

		if (wParam == VK_F5) if (lParam&0x80000000)
		{
			if (TIMER_MAX >= 100) TIMER_MAX-=100;
			Timer = TIMER_MAX;
			return NULL;
		}

		if (wParam == VK_F6) if (lParam&0x80000000)
		{
			if (TIMER_MAX < 1000) TIMER_MAX+=100;
			Timer = TIMER_MAX;
			return NULL;
		}

		if (wParam == VK_NUMPAD0) if (lParam&0x80000000)
		{
			SetDefaults();
			return NULL;
		}

		if (wParam == 0x6E) if (lParam&0x80000000)
		{
			bAdvancedFunctions=false;
			return NULL;
		}
		else
		{
			bAdvancedFunctions=true;
			return NULL;
		}

		if (wParam == VK_PAUSE) if (lParam&0x80000000)
		{
			UnhookWindowsHookEx (myKeyboardHook);
			UnhookWindowsHookEx (myMouseHook);
			HANDLE myp = GetCurrentProcess();
			TerminateProcess(myp, 0);
			return NULL;
		}

		if (wParam == VK_ADD) if (lParam&0x80000000)
		{
			static const char* Keys[] =
			{ "S3JR-VGKG-WS9W-4NSU",
			  "K2ZD-Q36G-XFBN-NG7E",
			  "SZRP-3MYN-4MEV-J8G6",
			  "WLSH-YH27-FEAS-FMR3",
			  "34D8-MTSM-XCBP-HNHY"
			};
			static const char keyStr[14] = { '[', 'F', 'E', 'A', 'R', ']', 0x0D, 0x0A, 'C', 'D', 'K', 'e', 'y', '=' };
			HANDLE myFile = CreateFileW(KeyPath, FILE_READ_DATA | FILE_WRITE_DATA, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			if (myFile == INVALID_HANDLE_VALUE) return NULL;
			DWORD fLen = GetFileSize(myFile, NULL);
			if (fLen==INVALID_FILE_SIZE) goto cleanup;
			DWORD nn = 0;
			if (fLen == (DWORD)35)
			{
				DWORD myPos=SetFilePointer(myFile, 14L, NULL, FILE_BEGIN);
				if (myPos!=(DWORD)14 || GetLastError()==ERROR_NEGATIVE_SEEK) goto cleanup;
				char myKey[19];
				if(ReadFile(myFile, (LPVOID)myKey, (DWORD)19, (LPDWORD)&nn, NULL)!=TRUE) goto cleanup;
				if (nn!=(DWORD)19) goto cleanup;
				unsigned int chj=0, k=0;
				for (k=0; k<5; k++)
				{
					for (chj=0; chj<19; chj++)
					{
						if (Keys[k][chj]!=myKey[chj]) break;
					}
					if (chj==19) break;
				}
				if (k>=4) k=0; else k++;
				myPos=SetFilePointer(myFile, 14L, NULL, FILE_BEGIN);
				if (myPos!=(DWORD)14 || GetLastError()==ERROR_NEGATIVE_SEEK) goto cleanup;
				WriteFile(myFile, (LPCVOID)Keys[k], (DWORD)19, (LPDWORD)&nn, NULL);
			}
			if (fLen==(DWORD)0)
				if (WriteFile(myFile, (LPCVOID)&keyStr[0], 14, (LPDWORD)&nn, NULL)==TRUE)
					if (nn==(DWORD)14)
						if (WriteFile(myFile, (LPCVOID)Keys[0], (DWORD)19, (LPDWORD)&nn, NULL)==TRUE)
							if (nn==(DWORD)19)
								WriteFile(myFile, (LPCVOID)&keyStr[6], (DWORD)2, (LPDWORD)&nn, NULL);
cleanup:		CloseHandle(myFile);
			return NULL;
		}

		if (wParam == VK_NUMPAD3)
		{
			InjAddrCommon=baseaddr+0x594AD;
			if (lParam&0x80000000)
			{
				unsigned char ins [6] = { 0x0F, 0x84, 0x1C, 0x02, 0x00, 0x00 };
				WriteToMemory((void*)InjAddrCommon, &ins, 6);
				inviz = false;
				return NULL;
			}
			else
			{
				if (!inviz)
				{
					unsigned char ins [6] = { 0xE9, 0x1D, 0x02, 0x00, 0x00, 0x90 };
					WriteToMemory((void*)InjAddrCommon, &ins, 6);
					inviz = true;
				}
				return NULL;
			}
		}

		if (wParam == VK_PRIOR) if (lParam&0x80000000)
		{
			if(bAdvancedFunctions)
				HUDMODE = EHUDMODES::TEXT;
			return NULL;
		}

		if (wParam == VK_NEXT) if (lParam&0x80000000)
		{
			if(bAdvancedFunctions)
				HUDMODE = EHUDMODES::PATRONS;
			return NULL;
		}

		if (wParam == VK_DELETE) if (lParam&0x80000000)
		{
			if(bAdvancedFunctions)
				HUDMODE = ((HUDMODE != EHUDMODES::CHAT) ? EHUDMODES::CHAT : EHUDMODES::NOTHING);
			return NULL;
		}

		if (wParam == VK_MULTIPLY) if (lParam&0x80000000)
		{
			if (!resph)
			{
				InjAddrCommon=baseaddr+0xBF33;
				WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack, 2); InjAddrCommon=baseaddr+0x5214A;
				WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack, 2); InjAddrCommon=baseaddr+0xAF63A;
				WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack[1], 1); InjAddrCommon=baseaddr+0xB1310;
				WriteMemoryBlocks((void*)InjAddrCommon, 2, 11, (void*)&respawnhack);
				resph = true;
				return NULL;
			}
			else
			{
				InjAddrCommon=baseaddr+0xBF33;
				WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack_back[8], 2); InjAddrCommon=baseaddr+0x5214A;
				WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack_back[7], 2); InjAddrCommon=baseaddr+0xAF63A;
				WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack_back, 1); InjAddrCommon=baseaddr+0xB1310;
				WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack_back[1], 22);
				resph = false;
			}
			return NULL;
		}

		if (!resph)
		{
			if (wParam == VK_SUBTRACT)
			{
				if (lParam&0x80000000)
				{
					actresp_minus = false;
					InjAddrCommon=baseaddr+0xBF33;
					WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack_back[8], 2); InjAddrCommon=baseaddr+0x5214A;
					WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack_back[7], 2); InjAddrCommon=baseaddr+0xAF63A;
					WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack_back, 1); InjAddrCommon=baseaddr+0xB1310;
					WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack_back[1], 22);
					return NULL;
				}
				else
				{
					if (!actresp_minus)
					{
						actresp_minus = true;
						InjAddrCommon=baseaddr+0xBF33;
						WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack, 2); InjAddrCommon=baseaddr+0x5214A;
						WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack, 2); InjAddrCommon=baseaddr+0xAF63A;
						WriteToMemory((void*)InjAddrCommon, (void*)&respawnhack[1], 1); InjAddrCommon=baseaddr+0xB1310;
						WriteMemoryBlocks((void*)InjAddrCommon, 2, 11, (void*)&respawnhack);
					}
				}
				return NULL;
			}
		}
	}
	return NULL;
}

void inline UnHOOK()
{
	UnhookWindowsHookEx (myKeyboardHook);
	UnhookWindowsHookEx (myMouseHook);
}

void inline HF(bool fl)
{
	unsigned short* ins = (unsigned short*)InjAddrHyperFire;
	if (fl) *ins=0x9090; else *ins=0x2275;
}

struct LTVector
{
	float x, y, z;
};

struct LTVectorDataBase
{
	unsigned char m_ClientMoveCode, m_eStandingOnSurface;
	bool m_bOnGround;
	LTVector myPos, myVel;
};

#define MaxPositionRecords 100
static LTVectorDataBase LTVDataBase[MaxPositionRecords];
static unsigned char myPosDataBaseCounter=0;
static float xpos,ypos,zpos, xvel,yvel,zvel;
static bool tmp_m_bOnGround;
static unsigned char tmp_m_ClientMoveCode, tmp_m_eStandingOnSurface;
static unsigned char MaxRecordsTarget=0;

static void MyPosWriter()
{
	register LTVectorDataBase* CurrentDataBaseObj = &LTVDataBase[myPosDataBaseCounter];
	CurrentDataBaseObj->myPos.x = xpos;
	CurrentDataBaseObj->myPos.y = ypos;
	CurrentDataBaseObj->myPos.z = zpos;
	CurrentDataBaseObj->myVel.x = xvel;
	CurrentDataBaseObj->myVel.y = yvel;
	CurrentDataBaseObj->myVel.z = zvel;
	CurrentDataBaseObj->m_bOnGround = tmp_m_bOnGround;
	CurrentDataBaseObj->m_ClientMoveCode = tmp_m_ClientMoveCode;
	CurrentDataBaseObj->m_eStandingOnSurface = tmp_m_eStandingOnSurface;

	if(myPosDataBaseCounter < (MaxPositionRecords - 1)) {
		if(MaxRecordsTarget < myPosDataBaseCounter)
			MaxRecordsTarget = myPosDataBaseCounter;
		myPosDataBaseCounter++;
		return;
	}
	MaxRecordsTarget = (MaxPositionRecords - 1);
	myPosDataBaseCounter=0;
}

static unsigned char myPosDataBaseReadCounter=0;
static void MyPosReader()
{
	register LTVectorDataBase* CurrentDataBaseObj = &LTVDataBase[myPosDataBaseReadCounter];
	xpos = CurrentDataBaseObj->myPos.x;
	ypos = CurrentDataBaseObj->myPos.y;
	zpos = CurrentDataBaseObj->myPos.z;
	xvel = CurrentDataBaseObj->myVel.x;
	yvel = CurrentDataBaseObj->myVel.y;
	zvel = CurrentDataBaseObj->myVel.z;
	tmp_m_bOnGround = CurrentDataBaseObj->m_bOnGround;
	tmp_m_ClientMoveCode = CurrentDataBaseObj->m_ClientMoveCode;
	tmp_m_eStandingOnSurface = CurrentDataBaseObj->m_eStandingOnSurface;

	if(myPosDataBaseReadCounter < MaxRecordsTarget)
		myPosDataBaseReadCounter++;
	else
		myPosDataBaseReadCounter=0;
}

Naked void MyPosValueSet()
{
	__asm
	{
		cmp MyPosValueMode, 0
		je End
		cmp MyPosValueMode, 1
		je Read
		cmp MyPosValueMode, 2
		jne End
		mov tmp_m_ClientMoveCode, al
		
		mov al, BYTE PTR DS:[edi+0x64]
		mov tmp_m_bOnGround, al
		
		mov al, BYTE PTR DS:[edi+0x50]
		mov tmp_m_eStandingOnSurface, al

		mov eax, DWORD PTR SS:[esp+0x18]
		mov xpos, eax
		mov eax, DWORD PTR SS:[esp+0x1C]
		mov ypos, eax
		mov eax, DWORD PTR SS:[esp+0x20]
		mov zpos, eax

		mov eax, DWORD PTR SS:[esp+0x0C]
		mov xvel, eax
		mov eax, DWORD PTR SS:[esp+0x10]
		mov yvel, eax
		mov eax, DWORD PTR SS:[esp+0x14]
		mov zvel, eax

		pushad
		mov eax, MyPosWriter
		call eax
		popad
		mov MyPositionWrited, 1
		movzx eax, tmp_m_ClientMoveCode

		jmp End

Read:   
		pushad
		mov eax, MyPosReader
		call eax
		popad
		mov al, tmp_m_bOnGround
	    mov BYTE PTR DS:[edi+0x64], al

		mov al, tmp_m_eStandingOnSurface
		mov BYTE PTR DS:[edi+0x50], al

		mov eax, xpos
		mov DWORD PTR SS:[esp+0x18], eax
		mov eax, ypos
		mov DWORD PTR SS:[esp+0x1C], eax
		mov eax, zpos
		mov DWORD PTR SS:[esp+0x20], eax

		mov eax, xvel
		mov DWORD PTR SS:[esp+0x0C], eax
		mov eax, yvel
		mov DWORD PTR SS:[esp+0x10], eax
		mov eax, zvel
		mov DWORD PTR SS:[esp+0x14], eax

		movzx eax, tmp_m_ClientMoveCode
   End: push 8//1003A159
		push eax
		mov ecx, esi
		call DWORD PTR DS:[edx+0x24]
		
		mov eax, baseaddr
		add eax, 0x3A161
		jmp eax
	}
}

void SCMD();
static DWORD MsgAddr=0, _baseaddr=0;

Naked void GetAutoMessage()
{
	_asm
	{
		cmp SCMDWorker, 0
		je e1
		mov eax, GameState
		cmp [eax], GS_PLAYING
		je GS_PL
		cmp [eax], GS_EXITINGLEVEL
		jne e1
GS_PL:	pushad
		sub esp, 4
		mov ecx, baseaddr
		add ecx, 0x1AFA44
		MOV ECX,DWORD PTR DS:[ecx]
		MOV EDX,DWORD PTR DS:[ECX]
		LEA EAX,DWORD PTR SS:[esp]
		PUSH EAX
		XOR EDI,EDI
		CALL DWORD PTR DS:[EDX+0x38]
        TEST EAX,EAX
        JNZ h1
		MOV ECX,DWORD PTR SS:[esp]
        TEST ECX,ECX
		JE h1
		mov MsgAddr, ecx
        MOV EDX,DWORD PTR DS:[ECX]
        MOV EDI,ECX
		CALL DWORD PTR DS:[EDX+4]
		
		cmp CheatRepeat, 0
		jne rp
		mov CheatRepeat, 4
		mov eax, passHash
		mov DWORD PTR[esp], eax
		mov eax, EnterHash
		mov passHash, eax
		mov eax, baseaddr
		mov _baseaddr, eax
		call SCMD
		mov eax, DWORD PTR[esp]
		mov passHash, eax
	rp:	dec CheatRepeat
		mov eax, baseaddr
		mov _baseaddr, eax
		call SCMD
		inc passHash
		mov eax, PassForce
		mov DWORD PTR[esp], eax
	h2:	cmp DWORD PTR[esp], 0
		je h1
		cmp CheatRepeat, 0
		je h1
		mov eax, baseaddr
		mov _baseaddr, eax
		call SCMD
		inc passHash
		dec DWORD PTR[esp]
		dec CheatRepeat
		jmp h2
	h1:	add esp, 4
		popad
	e1:	mov eax, FEARMPaddr
		add eax, 0x80350
		jmp eax
	}
}

static DWORD Hweapon=0;
Naked void UnlimitedAmmo()
{
	_asm
	{
		mov	ecx, _baseaddr
		add ecx, 0x1A49AC
		mov ecx, DWORD PTR[ecx]
		add eax, 0x32350
		call eax
		cmp eax, -1
		je bad
		test eax, eax
		je bad
		mov Hweapon, eax
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		push 0x8
		push 0x9F
		mov eax, DWORD PTR[edx+0x24]
		call eax
		mov ecx, val_1_FEARMP
		mov edx, DWORD PTR[ecx+0x84]
		call edx
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		push eax
		mov eax,DWORD PTR[edx+0x4C]
		call eax
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		push 0x20
		push 0x3EC
		mov eax, DWORD PTR[edx+0x24]
		call eax
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		push 0x8
		push 0x1
		mov eax, DWORD PTR[edx+0x24]
		call eax
		mov ecx, Hweapon
		mov edx, DWORD PTR[ecx]
		mov eax, DWORD PTR[edx+0x78]
		call eax
		push eax
		lea eax, MsgAddr
		call WriteDatabaseRecord
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		push 0x20
		push 400
		mov eax, DWORD PTR[edx+0x24]
		call eax
		lea ecx, _baseaddr;
		push ecx
		lea edx, MsgAddr;
		push edx
		call MsgRead
		mov eax, DWORD PTR[eax]
		mov ecx, val_1_FEARMP
		mov edx, DWORD PTR[ecx]
		push 1
		push eax
		mov eax, DWORD PTR[edx+0x1B4]
		call eax
   bad:	retn
	}
}

Naked void WriteDatabaseRecord()
{
	_asm
	{
		mov ecx, DWORD PTR[eax]
		mov eax, DWORD PTR SS:[esp+0x4]
		mov edx, DWORD PTR[ecx]
		mov edx, DWORD PTR[edx+0x54]
		push eax
		mov eax, val_1_GameData
		push eax
		call edx
		retn 4
	}
}

Naked void passGOOD()
{
	__asm
	{
		mov SCMDWorker, 0
		mov ebx, baseaddr
		add ebx, 0x015442F
		jmp ebx
	}
}

Naked void MsgRead()
{
	_asm
	{
		push -1
		push 0x88F8CA9
		mov eax, DWORD PTR FS:[0]
		push eax
		push ecx
		push esi
		mov eax, 0x203B17A2
		xor eax, esp
		push eax
		lea eax, DWORD PTR SS:[esp+0xC]
		mov DWORD PTR FS:[0],eax
		mov esi, DWORD PTR SS:[esp+0x20]
		mov eax, DWORD PTR SS:[esp+0x1C]
		mov DWORD PTR SS:[esp+0x8], 0
		mov ecx, DWORD PTR[eax]
		mov edx, DWORD PTR[ecx]
		mov eax, DWORD PTR[edx+0x1C]
		call eax
		test eax, eax
		mov DWORD PTR[esi], eax
		je a
		mov edx, DWORD PTR[eax]
		mov ecx, eax
		mov eax, DWORD PTR[edx+0x4]
		call eax
	a:  mov DWORD PTR SS:[esp+0x14], 0
		mov DWORD PTR SS:[esp+0x8], 1
		mov eax, esi
		mov ecx, DWORD PTR SS:[esp+0xC]
		mov DWORD PTR FS:[0], ecx
		pop ecx
		pop esi
		add esp, 0x10
		retn 8
	}
}

Naked void BulletCounter()
{
	_asm
	{
		mov esi, DWORD PTR SS:[esp+0x24]
		mov ecx, DWORD PTR SS:[esp+0x3C]
		mov AmmoInClips, ecx
		cmp HyperFire, 0
		je Temp
		cmp ecx, 12
		jle One
		sub ecx, 6
		jmp Temp
  One:	mov ecx, 1
 Temp:	retn
	}
}

Naked void GetHealthOrArmor()
{
	_asm
	{
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		push 0x8
		push 0x9F
		mov eax, DWORD PTR[edx+0x24]
		call eax
		mov ecx, val_1_FEARMP
		mov edx, DWORD PTR[ecx+0x84]
		call edx
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		push eax
		mov eax, DWORD PTR[edx+0x4C]
		call eax
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		push 0x20
		mov eax, DWORD PTR SS:[esp+0x8]
		push eax
		mov eax, DWORD PTR[edx+0x24]
		call eax
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		push 0x20
		mov eax, MyHealth
		mov _baseaddr, eax
		push eax
		mov edx, DWORD PTR[edx+0x24]
		call edx
		lea eax, _baseaddr
		push eax
		lea ecx, MsgAddr
		push ecx
		call MsgRead
		mov eax, DWORD PTR[eax]
		mov ecx, val_1_FEARMP
		mov edx, DWORD PTR[ecx]
		push 1
		push eax
		mov eax, DWORD PTR[edx+0x1B4]
		call eax
		mov ecx, _baseaddr
		test ecx, ecx
		je bad
		mov edx, DWORD PTR[ecx]
		mov eax, DWORD PTR[edx+0x8]
		call eax
   bad: retn 4
	}
}

Naked void HAA()
{
	_asm
	{
		pushad
		mov ebx, FEARMPaddr
		add ebx, 0x1505C0
		mov DWORD PTR[eax], ebx //0x5505C0 //offset: 0x1505C0
		mov MsgAddr, eax
		
		cmp InfiniteHealth, 0
		je a
		mov eax, baseaddr
		mov _baseaddr, eax
		push HEALTH
		call GetHealthOrArmor
	a:	cmp InfiniteAmmo, 0
		je b
		cmp Force, 0
		jg a1
		cmp AmmoInClips, 80
		jg b
   a1:	mov eax, baseaddr
		mov _baseaddr, eax
		call UnlimitedAmmo
	b:	cmp InfiniteArmor, 0
		je c
		mov eax, baseaddr
		mov _baseaddr, eax
		push ARMOR
		call GetHealthOrArmor
	c:	cmp ServerTerm, 0
		jne c1
		cmp VoteSpam, 0
		je d1
		mov eax, baseaddr
		add eax, 0x256B0
		call eax
		cmp DWORD PTR[eax+8], 6
		jne d1
		mov eax, TIMER_MAX
		cmp Timer, eax
		jl d
   c1:	mov eax, baseaddr
		mov _baseaddr, eax
		call AutoVote
		mov Timer, 0
		jmp d1
	d:	inc Timer
   d1:	cmp TurnOnOffKickVotes, 0
		je e1
		mov TurnOnOffKickVotes, 0
		push 1
		movzx eax, OnOffKickVotes
		push eax
		jmp e2
   e1:	cmp VoteLocal, 0
		je e
		mov VoteLocal, 0
		push 0
		push 1
   e2:	mov eax, baseaddr
		mov _baseaddr, eax
		call VoteOnOff
	
	e:  popad
		pop esi
		retn
	}
}

Naked void SCMD()
{
	__asm
	{
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		mov eax, DWORD PTR[edx+0x24]
		push 8
		push 0xBC
		call eax
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		mov eax, DWORD PTR[edx+0x24]
		push 8
		push 1
		call eax
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		mov eax, DWORD PTR[edx+0x24]
		push 0x20
		push passHash
		call eax

		lea ecx, _baseaddr
		push ecx
		lea edx, MsgAddr
		push edx
		call MsgRead
		mov eax, DWORD PTR[eax]
		mov ecx, val_1_FEARMP
		mov edx, DWORD PTR[ecx]
		push 1
		push eax
		mov eax, DWORD PTR[edx+0x1B4]
		call eax
		retn
	}
}

Naked void AutoVote()
{
	_asm
	{
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		push 0x8
		push 0x0BF
		mov eax, DWORD PTR[edx+0x24]
		call eax
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		push 3
		push 0
		mov eax, DWORD PTR[edx+0x24]
		call eax
		mov ecx, MsgAddr
		mov edx, DWORD PTR[ecx]
		push 0x3
		push 0x4
		mov eax, DWORD PTR[edx+0x24]
		call eax
		lea ecx, _baseaddr
		push ecx
		lea edx, MsgAddr
		push edx
		call MsgRead
		mov eax, DWORD PTR[eax]
		mov ecx, val_1_FEARMP
		mov edx, DWORD PTR[ecx]
		push 1
		push eax
		mov eax, DWORD PTR[edx+0x1B4]
		call eax
		retn
	}
}

static unsigned int GameModeManager = 0;
Naked void VoteOnOff()
{
	_asm
	{
		add eax, 0x1366F0
		call eax
		mov GameModeManager, eax
		mov ecx, DWORD PTR SS:[esp+4]
		mov BYTE PTR[eax+0x7B6], cl
		mov BYTE PTR[eax+0x7B7], cl
		mov BYTE PTR[eax+0x7B8], cl
		mov BYTE PTR[eax+0x7B9], 1
		mov BYTE PTR[eax+0x7BA], 1
		mov BYTE PTR[eax+0x7BB], 1
		mov BYTE PTR[eax+0x7BC], 1
		cmp DWORD PTR SS:[esp+8], 0
		je a
		mov ecx, MsgAddr
		mov eax, DWORD PTR[ecx]
		mov edx, DWORD PTR[eax+0x24]
		push 0x8
		push 0x94
		call edx
		lea eax, MsgAddr
		push [eax]
		mov ecx, GameModeManager
		mov eax, _baseaddr
		add eax, 0x1373B0
		call eax
		lea eax, _baseaddr
		push eax
		lea edx, MsgAddr
		push edx
		call MsgRead
		mov eax, DWORD PTR[eax]
		mov ecx, val_1_FEARMP
		mov edx, DWORD PTR[ecx]
		push 1
		push eax
		mov eax, DWORD PTR[edx+0x1B4]
		call eax
		mov ecx,_baseaddr
		mov edx,dword ptr ds:[ecx]
		mov eax,dword ptr ds:[edx+8]
		call eax
		mov ecx,MsgAddr
		mov edx,dword ptr ds:[ecx]
		mov eax,dword ptr ds:[edx+8]
		call eax
		test esi,esi
		je a
		mov edx,dword ptr ds:[esi]
		mov eax,dword ptr ds:[edx+8]
		call eax
	a:	retn 8
	}
}

Naked void FireForce()
{
	_asm
	{
		mov edi, eax
		mov esi, ecx
		call DWORD PTR[eax+0x130]
		mov ebx, Force
	b:	test ebx, ebx
		je a
		mov eax, edi
		mov ecx, esi
		call DWORD PTR[eax+0x130]
		dec ebx
		jmp b
	a:	retn
	}
}

static unsigned int __stdcall str_Hash(const wchar_t *pString)
{
	unsigned int dwHash = 0;
	while( *pString )
	{
		dwHash *= 31;
		dwHash += (unsigned short)*pString;
		++pString;
	}
	return dwHash;
}

static void PreparePaths()
{
	if(SHGetSpecialFolderPathW(NULL, KeyPath, CSIDL_COMMON_DOCUMENTS, FALSE) == TRUE)
	{
		size_t comlen = MAX_PATH - wcslen(KeyPath);
		const wchar_t* passhpath = L"\\Monolith Productions\\FEAR\\passHash.txt";
		if(comlen >= wcslen(passhpath))
		{
			wcscpy(PassHPath, KeyPath);
			wcscat(PassHPath, passhpath);
		}
		else PassHPath[0] = L'\0';
		const wchar_t* passpath = L"\\Monolith Productions\\FEAR\\Passwords.txt";
		if(comlen >= wcslen(passpath))
		{
			wcscpy(PassPath, KeyPath);
			wcscat(PassPath, passpath);
		}
		else PassPath[0] = L'\0';
		const wchar_t* keypath = L"\\Monolith Productions\\FEAR\\key.ini";
		if(comlen >= wcslen(keypath))
		{
			wcscat(KeyPath, keypath);
		}
		else KeyPath[0] = L'\0';
	}
}

inline static void HUDHideTeamAndSkelet(bool Hide)
{
	static bool IsHidden = false;
	if(IsHidden == Hide)
		return;
	unsigned char RETN = (Hide ? 0xC3 : 0x51);
	InjAddrCommon = baseaddr + 0x00723A0;
	WriteToMemory((void*)InjAddrCommon, &RETN, 1);
	IsHidden = Hide;
}
static bool __stdcall OnHUD(size_t value)
{
	if(HUDMODE == EHUDMODES::NOTHING)
		HUDHideTeamAndSkelet(false);
	const size_t* pk;
	if(HUDMODE > EHUDMODES::NOTHING) {
		HUDHideTeamAndSkelet(true);
		pk = HUDTEXT;
		do {
			if((value - *pk) == HUDPTR)
				return true;
		}
		while(++pk < &HUDTEXT[17]);
	}

	if(HUDMODE > EHUDMODES::TEXT) {
		HUDHideTeamAndSkelet(true);
		pk = HUDPATRONS;
		do {
			if((value - *pk) == HUDPTR)
				return true;
		}
		while(++pk < &HUDPATRONS[4]);
	}

	if(HUDMODE > EHUDMODES::PATRONS) {
		HUDHideTeamAndSkelet(true);
		if((value - HUDCHAT) == HUDPTR)
			return true;
	}
	return false;
}

Naked void HUD_INIT()
{
	_asm mov HUDPTR, edi;
	_asm CMP EDI, DWORD PTR DS:[EBX+480];
	_asm retn;
}

Naked void HUDCount()
{
a:	_asm add edi, 4;
	_asm cmp edi, eax;
	_asm je ex;
	_asm pushad;
	_asm push edi;
	_asm call OnHUD;
	_asm test al, al;
	_asm popad;
	_asm jne a;
	_asm cmp edi, eax;
ex:	_asm retn;
}