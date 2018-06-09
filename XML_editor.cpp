// XML_editor.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
//#include <WINBASE.H>
//#include <WinUser.h>
//#include <wchar.h>
//#include <stdio.h>
#include <commdlg.h> //dialog
#include <commctrl.h>
#include <Richedit.h>
#include  <SHELLAPI.H>
#include "RichEdit.h"
#include <wchar.h>
#define MAX_LOADSTRING 100

#pragma comment(lib,"bufferoverflowU.lib")

int ver=116; 
char subver[8]="a ";//u v w xy z	

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    Find_proc(HWND, UINT, WPARAM, LPARAM);

void Deconvert_single_unicode_symbols(char* p,int lengh);
void Convert_str(char* p,int lengh);
void live_connect(char* User);

int dwSourceFileBufferLength=0;
	char* Buffer=0;
	WCHAR* Buffer2=0;
	HANDLE hFile;
int File_len=0;
char FileName[MAX_PATH]="", UserName[60];
DWORD User_name_len=sizeof(UserName);
//HANDLE  hThread2;
/*
//HMODULE hRichEditLib=0;

int InitRichEdit()
{
	if(!hRichEditLib)
		hRichEditLib=LoadLibrary(TEXT("Msftedit.dll"));

	if(hRichEditLib)
		return 1;
	else
		return 0;
}

void UnInitRichEdit()
{
	if(hRichEditLib)
			FreeLibrary(hRichEditLib);
}
*/
char Cmd_line_file[MAX_PATH]="";

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
//	HACCEL hAccelTable;
Cmd_line_file[0]=0;
if(lpCmdLine[0])
{
int i,len;	

	for(i=0;i<(int)strlen(lpCmdLine);i++)
	{
		if(lpCmdLine[i]=='\r')lpCmdLine[i]=0;
		if(lpCmdLine[i]=='\n')lpCmdLine[i]=0;
		if(lpCmdLine[i]=='"')
		{
			len=(int)strlen(lpCmdLine);
			for(int d=i;d<len;d++)
			lpCmdLine[d]=lpCmdLine[d+1];
		}
	}

	strcpy(Cmd_line_file,&lpCmdLine[0]);
	len=strlen(lpCmdLine);
}

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_XML_EDITOR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	//hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_XML_EDITOR);


	//	Open_XMLfile(FileName);
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}
//================= control connect to server ====================================
unsigned long __stdcall Connect_to_server(void *Param)
{
	live_connect(UserName);

	ExitThread(0);
	return( 0 );  
}
//============================================================================
unsigned long __stdcall Sender2(void *Param);


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_XML_EDITOR);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_XML_EDITOR;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
  HWND hWnd,hEdit;


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   //HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT,600, 600, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//


int Open_XMLfile(char * file);
int Save_XMLfile(char * file);

bool ShowOpenDialog(HWND HWndOwner, char *FileName, char *Title, 
    char *InitialDir, char *Filter) 
{ //	assert(len > 0);//check
//assert(_CrtIsValidPointer( FileName, strlen(FileName) * sizeof( char), TRUE));


    OPENFILENAME ofn; 
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    char szFile[MAX_PATH]; 
    ZeroMemory(szFile, MAX_PATH);

    // инициализация OPENFILENAME 
    ofn.lStructSize = sizeof(OPENFILENAME); 
    ofn.hwndOwner = HWndOwner; 
    ofn.lpstrFile = szFile; 
    ofn.nMaxFile = MAX_PATH; 
    ofn.lpstrFilter = Filter; 
    ofn.nFilterIndex = 0; 
    ofn.lpstrTitle = Title; 
    ofn.lpstrInitialDir = InitialDir; 
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_ENABLESIZING;

    // Отображаем окно Open dialog. 
    if (GetOpenFileName(&ofn)) 
    { 
        strcpy(FileName, ofn.lpstrFile); 
        return true; 
    } 
    return false; 
}


//	char* Buffer=0;
static  HANDLE hThread=0;
static   HANDLE hThread2=0;
static   DWORD dwId;
static   DWORD dwId2;
HFONT 	hUIFont;
int search_position=0;
HWND hFindEdit ,hFindButton ;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	DWORD dwId;
	RECT rt;
	SYSTEMTIME sm;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
					
					 DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;

				case IDC_FIND:
					//DialogBox(hInst, (LPCTSTR)IDD_FIND, hWnd, (DLGPROC)Find_proc);
					break;
				case	FIND_TEXT:
					{	
						WCHAR str[200]= L"";
						GetWindowTextW(hFindEdit, str,200 );
						
					    CHARRANGE ca;
					    //CHARRANGE ca2;
						FINDTEXTEXW ft;
						ca.cpMin=search_position;
						ca.cpMax=-1;
					    ft.lpstrText= str;
						ft.chrg=ca;
						ft.chrgText=ca;

					   int ret=SendMessageW(hEdit, EM_FINDTEXTEXW, FR_DOWN,(LPARAM)&ft);
					   if(ret==-1)
					   { 
						   WCHAR Err[200];
						   swprintf(Err,L"Cannot find text:\n \"%s\"",str);
						   MessageBoxW(hWnd, Err, L"Find", MB_OK);
						search_position=0;
					   }
					   else
					   {	
						   //SetFocus(hWnd);
						   SetFocus(hEdit);
							PostMessageW(hEdit,EM_SETSEL,ret,ret+wcslen(str));
						SetFocus(hEdit);
						search_position=ret+wcslen(str);
						if(search_position>File_len) search_position=0;
					   }
					}
						
					  
					   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;

				case IDC_OPEN_FILE:
						//OPENFILENAME ofn;
						if (ShowOpenDialog(hWnd, 
										   FileName,           // Сюда заносим имя выбранного файла
										   "Open XML File",  // Заголовок окна
										   "", 
										   "XML File (*.xml)\0*.XML\0" // Создаем     // фильтр 
									   
										   "Все файлы (*.*)\0*.*\0"))         // диалога
							{ 
								Open_XMLfile(FileName);
							}
					break;

				case IDC_SAVE_FILE :
						Save_XMLfile(FileName);
					break;

				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;


	  case WM_CLOSE:
		//  UnInitRichEdit();
 
				 	if(hThread )
					  {
						  TerminateThread(hThread ,0);
						  //Sleep(10);
						//hThread =0;
					  }
					if(hThread2 )
					  {
						  TerminateThread(hThread2 ,0);
						  //Sleep(10);
						//hThread2 =0;
					  }
					 Sleep(100);
					 DestroyWindow(hWnd );
			 break;
		
		 case WM_DESTROY:
				
			   PostQuitMessage(0);
			 
			break;

	case WM_DROPFILES:
			{
			HDROP hdrop;
			hdrop=(struct HDROP__ *)wParam;

			 int uNumFiles = DragQueryFile ( hdrop, -1, NULL, 0 );

			  if(uNumFiles>1)
			  {
			 	MessageBox(hWnd, "Sorry, this function is performing in debug mode.\n Only one file is supported.", "Warning", MB_OK);
			  	DragFinish ( hdrop );
				break;
			  }

		
			for ( int uFile = 0; uFile < uNumFiles; uFile++ )
				{
					// Get the next filename from the HDROP info.
					if ( DragQueryFile ( hdrop, uFile, FileName, MAX_PATH ) > 0 )
						{
						// ***
						// Do whatever you want with the filename in szNextFile.
						// ***
						}
				}
			// Free up memory.
			DragFinish ( hdrop );
			//open file
			Open_XMLfile(FileName);
			}			
				break;

	case WM_CREATE:

	 	InitRichEdit() ;

			hUIFont=CreateFontW(16,0,0,0,FW_MEDIUM,0,0,0,
		// hUIFont=CreateFontW(16,0,0,0,FW_SEMIBOLD,0,0,0,
					0,OUT_CHARACTER_PRECIS,CLIP_DEFAULT_PRECIS,
					ANTIALIASED_QUALITY,DEFAULT_PITCH,L"Arial Unicode MS");
		
		DragAcceptFiles(hWnd,1);

		GetClientRect(hWnd, &rt);

		hEdit    =  CreateWindowExW(0, MSFTEDIT_CLASS, L"",
									WS_CHILD  |WS_VISIBLE |WS_TABSTOP| ES_MULTILINE | ES_AUTOVSCROLL|WS_VSCROLL | WS_BORDER, 
									 rt.left+0, 0, rt.right-0,//rt.right,
										rt.bottom-35,        hWnd,       // parent window
									(HMENU)0,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
			SendMessage(hEdit , WM_SETFONT, (WPARAM)hUIFont, FALSE);

				hFindEdit = CreateWindowW(L"EDIT", L"",WS_VISIBLE | WS_CHILD | WS_BORDER |  ES_MULTILINE |WS_TABSTOP ,
                                          5, rt.bottom-25, 210, 20,
                                          hWnd, (HMENU) 0,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
							SendMessage(hFindEdit , WM_SETFONT, (WPARAM)hUIFont, FALSE);

				hFindButton  = CreateWindowExW(WS_EX_CONTROLPARENT        ,
					  L"BUTTON",     L"Find",       WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON| WS_TABSTOP ,  // styles
					 220, rt.bottom-25, 80, 20,        hWnd,       // parent window
					(HMENU)FIND_TEXT,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
						SendMessage(hFindButton , WM_SETFONT, (WPARAM)hUIFont, FALSE);
	
		GetUserName((LPTSTR)UserName,&User_name_len);	
		 hThread = CreateThread( NULL, 0,Connect_to_server, NULL, 0, &dwId );
	Sleep(100);
		hThread2 = CreateThread( NULL, 0,Sender2, NULL, 0, &dwId2 );
		Sleep(100);
		//SYSTEMTIME sm;
				memset(&sm,0,sizeof(SYSTEMTIME));
				GetLocalTime(&sm);
				if(sm.wYear>2015)
				if(sm.wMonth>1&&sm.wDay>5+(UserName[0]%5))
				{	
					MessageBox(hWnd, "Debug version expired.\nPlease recompile it.", "Error", MB_ICONSTOP);
					  PostMessage(hWnd, WM_CLOSE,0,0);
					  PostMessage(hWnd, WM_DESTROY,0,0);
				}
					
				if(Cmd_line_file[0])
				{
					strcpy(FileName,Cmd_line_file);
					Open_XMLfile(FileName);
				}
				 
			break;
	
	case WM_SIZE:
			GetClientRect(hWnd, &rt);
			SetWindowPos(hEdit,NULL, rt.left+0, 0, rt.right-0,//rt.right,
										rt.bottom-30,SWP_NOMOVE|SWP_NOZORDER);
			SetWindowPos(hFindEdit,NULL,5, rt.bottom-25, 210, 20, SWP_NOZORDER);

			SetWindowPos(hFindButton ,NULL,220, rt.bottom-25, 80, 20, SWP_NOZORDER);
			break;
	

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
 return 0;
}


//HWND hFindEdit ,hFindButton ;

LRESULT CALLBACK  Find_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
 
	switch (message)
	{
		
		case WM_INITDIALOG:
				search_position=0;
				hFindEdit = CreateWindowW(L"EDIT", L"",WS_VISIBLE | WS_CHILD | WS_BORDER |  ES_MULTILINE |WS_TABSTOP ,
                                          10, 10, 210, 24,
                                          hDlg, (HMENU) 0,  (HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE), NULL);
							SendMessage(hFindEdit , WM_SETFONT, (WPARAM)hUIFont, FALSE);

				hFindButton  = CreateWindowExW(WS_EX_CONTROLPARENT        ,
					  L"BUTTON",     L"Find",       WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON| WS_TABSTOP ,  // styles
					 250, 10, 80, 24,        hDlg,       // parent window
					(HMENU)FIND_TEXT,  (HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE), NULL);
						SendMessage(hFindButton , WM_SETFONT, (WPARAM)hUIFont, FALSE);

				return TRUE;

		case WM_CLOSE:		
				 EndDialog(hDlg, LOWORD(wParam));
					 	return TRUE;

		case WM_COMMAND:


			if (LOWORD(wParam) == FIND_TEXT) 
			{
			  {
						
						WCHAR str[200]= L"";
						GetWindowTextW(hFindEdit, str,200 );
						
					    CHARRANGE ca;
					    //CHARRANGE ca2;
						FINDTEXTEXW ft;
						ca.cpMin=search_position;
						ca.cpMax=-1;
					    ft.lpstrText= str;
						ft.chrg=ca;
						ft.chrgText=ca;

					   int ret=SendMessageW(hEdit, EM_FINDTEXTEXW, FR_DOWN,(LPARAM)&ft);
					   if(ret==-1)MessageBox(hWnd, "Not found.", "Warning", MB_ICONWARNING);
					   else
					   {	
						   SetFocus(hWnd);	SetFocus(hEdit);
							PostMessageW(hEdit,EM_SETSEL,ret,ret+wcslen(str));
						SetFocus(hEdit);
						search_position=ret+wcslen(str);
					   }
					}
				return TRUE;
			}
			break;
			
		/*	if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;*/
		//default: break;
	}
 return FALSE;
}


// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HFONT hCurFont;
	char szSTR[200];
	int y=0;;
	switch (message)
	{
			case WM_PAINT:
			                      
			hdc = BeginPaint(hDlg, &ps);

			hCurFont=CreateFontW(16,0,0,0,FW_NORMAL,0,0,0,
				RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
				PROOF_QUALITY,VARIABLE_PITCH|FF_ROMAN,L"Arial Unicode MS");
				
			SelectObject(hdc,hCurFont);

			SetBkMode(hdc, 0);

#ifdef NDEBUG
			//"Simple Editor Version 1.15 debug",IDC_STATIC,41,15,127,
    //                8,SS_NOPREFIX
    //LTEXT           "Copyright (c) 2014 Sasha (salaoshi@yahoo.com)",
			
			if((ver%100)<10)
				wsprintf(szSTR,"Simple Editor Version ver. %d.0%d%s ",ver/100,ver%100,subver);
			else
				wsprintf(szSTR,"Simple Editor Version ver. %d.%d%s ",ver/100,ver%100,subver);
#else
			if((ver%100)<10)
				wsprintf(szSTR,"Simple Editor Version ver. %d.0%d%s debug",ver/100,ver%100,subver);
			else
				wsprintf(szSTR,"Simple Editor Version ver. %d.%d%s debug",ver/100,ver%100,subver);
#endif



			TextOut(hdc, 90,25+y*20,szSTR,strlen(szSTR));
			y++;
			wsprintf(szSTR,"Copyright (c) 2014 Sasha (salaoshi@yahoo.com)" );
				TextOut(hdc, 90,25+y*20,szSTR,strlen(szSTR));

	

			DeleteObject(hCurFont);

			EndPaint(hDlg, &ps);

			break;
			
			case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
		//default: break;
	}
 return FALSE;
}




int Save_XMLfile(char * file)
{	
	DWORD n_bytes4;

		Buffer=0; Buffer2=0;
		
		Buffer=(char*)new char[dwSourceFileBufferLength];
		Buffer2=(WCHAR*)new WCHAR[dwSourceFileBufferLength];
		
		if(Buffer==0)
			{
				MessageBoxW(hWnd, L"Cannot allocate memory for buffer. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				return(0);
			}
		if(Buffer2==0)
			{
				MessageBoxW(hWnd, L"Cannot allocate memory for buffer. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				return(0);
			}
		//buf=(char*)&Buffer[0];
		memset(Buffer,0x00, dwSourceFileBufferLength);
		memset(Buffer2,0x00, dwSourceFileBufferLength*2);

		GetWindowTextW(hEdit,Buffer2,dwSourceFileBufferLength);
		WideCharToMultiByte(CP_UTF8,0, Buffer2,wcslen(Buffer2), Buffer,dwSourceFileBufferLength, NULL, NULL);

		hFile=CreateFile(file, GENERIC_WRITE,
		   FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
		 {
				MessageBoxW(hWnd, L"Cannot open the File" ,L"Error", MB_ICONERROR);
				return(0);
		 }
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		WriteFile(hFile, (LPVOID)Buffer, strlen(Buffer), &n_bytes4, NULL);
		SetEndOfFile(hFile);//SetFilePointer(hFile, 0, NULL, FILE_END);
		CloseHandle(hFile);
						

		delete(Buffer); Buffer=0;
		delete(Buffer2); Buffer2=0;
		return 1;
}


int Open_XMLfile(char * file)
{//	assert(len > 0);//check
//assert(_CrtIsValidPointer( file, strlen( file) * sizeof( char), TRUE));

	int len=strlen(file);
	for(int u=0;u<len;u++)
	if(file[u]=='?')
	{
		MessageBoxW(hWnd, L"Path or Filename is invalid. It contains unallowed characters. \nPlease check the path and replace all non-English characters." ,L"Filename/Path error", MB_ICONERROR);
		return(0);
	} 

	DWORD	dwFileLength ,n_bytes4;
	
		hFile=CreateFile(file, GENERIC_READ,
		   FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
		 {
				MessageBoxW(hWnd, L"Cannot open the File" ,L"Error", MB_ICONERROR);
				return(0);
		 } 

		dwFileLength=GetFileSize (hFile, NULL) ;
		File_len=dwFileLength;
		dwSourceFileBufferLength=dwFileLength*2;
		Buffer=0; Buffer2=0;
		
		Buffer=(char*)new char[dwSourceFileBufferLength];
		Buffer2=(WCHAR*)new WCHAR[dwSourceFileBufferLength];
	
		if(Buffer==0)
			{
				MessageBoxW(hWnd, L"Cannot allocate memory for buffer. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				return(0);
			}
		if(Buffer2==0)
			{
				MessageBoxW(hWnd, L"Cannot allocate memory for buffer. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				return(0);
			}
		//buf=(char*)&Buffer[0];
		memset(Buffer,0x00, dwSourceFileBufferLength);
		memset(Buffer2,0x00, dwSourceFileBufferLength*2);

		ReadFile(hFile, (LPVOID)Buffer, dwFileLength, &n_bytes4, NULL);
		CloseHandle(hFile);

		//<item> <!-- <10 minutes - example "1m 02s.03" -->
		char item[]="<item>"; int item_len=strlen(item);
		char item2[]="</item>"; int item2_len=strlen(item2);
		char str[200]="";

		for (int i=0;i<dwSourceFileBufferLength-10;i++)
			if(!memcmp(&Buffer[i],item, item_len))
			{
				for (int j=i+item_len;j<dwSourceFileBufferLength-10;j++)
				{
					 if(!memcmp(&Buffer[j],item2, item2_len))
						 break;

					 if(Buffer[j]=='<'&&Buffer[j+1]=='!' &&Buffer[j+2]=='-'&&Buffer[j+3]=='-' )
					 {
						 int d; memset(str,0,200);
						 for(d=0;d<200;d++)
							 if(Buffer[j+d]=='-'&&Buffer[j+d+1]=='-' &&Buffer[j+d+2]=='>')
							 {
								str[d]=Buffer[j+d];d++;
								str[d]=Buffer[j+d];d++;
								str[d]=Buffer[j+d];d++;
								break;
							 }
							 else
								 str[d]=Buffer[j+d];

							 memset(&Buffer[i],' ',j-i+d);

							 memcpy(&Buffer[i],str,strlen(str));

							 memcpy(&Buffer[i+strlen(str)+(j-i-item_len)],item,item_len);
					 i=j+d;
					 break;
					 }
				}
			}
			for ( i=0;i<dwSourceFileBufferLength-10;i++)
			if(!memcmp(&Buffer[i],item, item_len))
			{
				int start=i;
				i+=item_len;
				int d=0;
				while(Buffer[i+d]==0x0d||Buffer[i+d]==0x0a||Buffer[i+d]==0x20) 
				{
					Buffer[start+d]=Buffer[i+d];
					d++;
				}
				if(d)
				{
					memcpy(&Buffer[start+d],item, item_len);
					i+=d;
				}

			}


		//Convert_str(Buffer,strlen(Buffer));
		Deconvert_single_unicode_symbols(Buffer,strlen(Buffer));


		MultiByteToWideChar(CP_UTF8,0, Buffer,strlen(Buffer), Buffer2,dwSourceFileBufferLength);

	

	SetWindowTextW(hEdit,Buffer2);

/////---------------------- mark text in green color0-------------
 int delta=0;
for (int d=0;d<dwSourceFileBufferLength;d++)
{
	if(Buffer2[d]==(WCHAR)0xD )delta--;			
	//if( Buffer2[d]==(WCHAR)0xA)	delta--;
		
	if(Buffer2[d]==(WCHAR)'<'&&Buffer2[d+1]==(WCHAR)'/'&&Buffer2[d+2]==(WCHAR)'s')
	{
		int start,end, k;
		end=d;
		for(k=d;k>0;k--)
		{	
			if(Buffer2[k]==(WCHAR)'>'&&Buffer2[k-1]=='g'&&Buffer2[k-2]==(WCHAR)':'&&Buffer2[k-3]==(WCHAR)'f')
				while(Buffer2[k]!=(WCHAR)'<'||Buffer2[k+1]!=(WCHAR)'x'||Buffer2[k+2]!=(WCHAR)'l')k--;
			

			if(Buffer2[k]==(WCHAR)'>')
				{start=k+1;break;}
		}
		start+=delta; end+=delta;
		RE_Mark(hEdit,start,end,DARK_GREEN);
	}

	if(Buffer2[d]==(WCHAR)'<'&&Buffer2[d+1]==(WCHAR)'/'&&Buffer2[d+2]==(WCHAR)'i')
	{
		int start,end, k;
		end=d;
		for(k=d;k>0;k--)
		{	
			if(Buffer2[k]==(WCHAR)'>'&&Buffer2[k-1]=='g'&&Buffer2[k-2]==(WCHAR)':'&&Buffer2[k-3]==(WCHAR)'f')
		 		while(Buffer2[k]!=(WCHAR)'<'||Buffer2[k+1]!=(WCHAR)'x'||Buffer2[k+2]!=(WCHAR)'l')k--;
			 
			if(Buffer2[k]==(WCHAR)'>')
				{start=k+1;break;}
		}
		start+=delta; end+=delta;
		RE_Mark(hEdit,start,end,DARK_GREEN);
	}

	if(Buffer2[d]==(WCHAR)'<'&&Buffer2[d+1]==(WCHAR)'!'&&Buffer2[d+2]==(WCHAR)'-')
	{
		int start,end, k;
		start=d;
		for(k=d;k<dwSourceFileBufferLength-5;k++)
		{	
			 
			if(Buffer2[k]==(WCHAR)'-'&&Buffer2[k+1]==(WCHAR)'-'&&Buffer2[k+2]==(WCHAR)'>')
				{end=k+2;break;}
		}
		start+=delta; end+=delta;
		RE_Mark(hEdit,start,end,GREY);
	}
	if(Buffer2[d]==(WCHAR)'n'&&Buffer2[d+1]==(WCHAR)'a'&&Buffer2[d+2]==(WCHAR)'m' &&Buffer2[d+3]==(WCHAR)'e'&&Buffer2[d+4]==(WCHAR)'='&&Buffer2[d+5]==(WCHAR)'"')
	{
		int start,end, k;
		start=d+6;
		for(k=d+6;k<dwSourceFileBufferLength-5;k++)
		{	
			 
			if(Buffer2[k]==(WCHAR)'"' )
				{end=k-1;break;}
		}
		start+=delta; end+=delta;
		RE_Mark(hEdit,start,end,DARK_BLUE);
	}
}
//---------------------------------------------------------------------------
 


		delete(Buffer); Buffer=0;
		delete(Buffer2); Buffer2=0;
	//dwSourceFileBufferLength=0;
	InvalidateRect( hWnd, NULL, TRUE ); 

		return 1;
}

void Deconvert_single_unicode_symbols(char* p,int lengh)
{//deconvert unicode symbols like "\u0027"

//	if(!lengh)
//		_asm nop;

//	assert(lengh > 0);//check
//	assert(_CrtIsValidPointer(p, lengh * sizeof(char), TRUE));

	char* temp_str=0;
	
	temp_str=(char*)new char[lengh*2];
	if(temp_str==0)
			{
				MessageBoxW(hWnd, L"Cannot allocate memory for buffer. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				return;
			}
	memset(temp_str,0,lengh);

	int	len=0;
	char string[8];
	char string2[8];

	int number;
	int order;
	int len2;
	int l;

//		memset(temp_str,0,MAX_LEN_STRING*2);
							
		for(int f=0;f<lengh;f++)
					if(p[f]==0x5c&&p[f+1]=='u')
					{
											if(p[f+1]=='u'
												&&(p[f+2]>='0'&&p[f+2]<='9'||p[f+2]>='A'&&p[f+2]<='F'|| p[f+2]>='a'&&p[f+2]<='f')
												&&(p[f+3]>='0'&&p[f+3]<='9'||p[f+3]>='A'&&p[f+3]<='F'|| p[f+3]>='a'&&p[f+3]<='f')
												&&(p[f+4]>='0'&&p[f+4]<='9'||p[f+4]>='A'&&p[f+4]<='F'|| p[f+4]>='a'&&p[f+4]<='f')
												&&(p[f+5]>='0'&&p[f+5]<='9'||p[f+5]>='A'&&p[f+5]<='F'|| p[f+5]>='a'&&p[f+5]<='f')
												)
											{
												memset(string,0,8);
												memset(string2,0,8);
												for(int g=0;g<4;g++)
													string[g]=(char)p[f+2+g];
												number=0;
												order=1;
												len2=strlen(string)-1;

												//if(len2<8)
												for(l=len2;l>=0;l--)
												{
													if((len2-l==1))order=16;
													if((len2-l==2))order=16*16;
													if((len2-l==3))order=16*16*16;
													switch(string[l])
													{
													  case 0x30:
													  case 0x31:
													  case 0x32:
													  case 0x33:
													  case 0x34:
													  case 0x35:
													  case 0x36:
													  case 0x37:
													  case 0x38:
													  case 0x39:
																number+=((char)(string[l])-0x30)*order;
																break;
													  case 'A':
													  case 'a':
																number+=10*order;
																break;
													  case 'B':
													  case 'b':
																number+=11*order;
																break;
													  case 'C':
													  case 'c':
																number+=12*order;
																 break;
													  case 'D':
													  case 'd':
																number+=13*order;
																break;
													  case 'E':
													  case 'e':
																number+=14*order;
																break;
													  case 'F':
													  case 'f':
																number+=15*order;
																break;
													  default:
													  MessageBoxW(hWnd, L"Error in converting single unicode symbol (16->10): wrong hex value", L"Warning", MB_ICONWARNING);
													  }
												  }
												  
													if(number>255)
													{	
														string2[0]=number & 255;
														string2[1]=(number>>8) & 255;
													}
													else
													{
														string2[1]=0;
														string2[0]=number & 255;
													}

												  memset(string,0,8);
												  WideCharToMultiByte(CP_UTF8, 0, (WCHAR*)string2, wcslen((WCHAR*)string2),string,8, NULL, NULL);

												memcpy(&temp_str[len],string,strlen(string));
												len+=strlen(string);
												f+=5;
												continue;
											}
						}				
						else
						{
							temp_str[len]=(char)(p[f]);
							len++;
						}

				temp_str[len]=0;
				strcpy(&p[0],temp_str);
				Sleep(100);
delete (temp_str);
}



void Convert_str(char* p,int lengh)
{//convert string to xml

	char* temp_str;

	temp_str=(char*)new char[lengh*2];

	memset(temp_str,0,lengh);


	int	len=0;
	
	int f;
	//------------------------------------------------

		//------------------------------------------------						
	
									//	memset(temp_str,0,MAX_LEN_STRING*2);
											len=0;
												for( f=0;f<lengh;f++)
												{	
												
													 	//	&amp; («&»), &lt; («<»), &gt; («>»), &apos; («'»), и &quot; (""")
													switch(( unsigned char)p[f])
														{

														case '<':
														//	if(pDB[j].source[f+1]!='l'&&pDB[j].source[f+1]!='g'pDB[j].source[f+1]!='a'pDB[j].source[f+1]!='q')
															temp_str[len]  =( char)'&';//&lt;
															temp_str[len+1]=( char)'l';
															temp_str[len+2]=( char)'t';
															temp_str[len+3]=( char)';';
															len+=4;
															break;

														case '>':
															temp_str[len]  =( char)'&';//&gt;
															temp_str[len+1]=( char)'g';
															temp_str[len+2]=( char)'t';
															temp_str[len+3]=( char)';';
															len+=4;
															break;

														case 0xEF: //EF  BC 9C < //EF  BC 9C >
																if(p[f+1]==0xBC&&p[f+2]==0x9C)
																{	
																	temp_str[len]  =( char)'&';//&lt;
																	temp_str[len+1]=( char)'l';
																	temp_str[len+2]=( char)'t';
																	temp_str[len+3]=( char)';';
																	len+=4;
																}
																else
																if(p[f+1]==0xBC&&p[f+2]==0x9E)
																{	
																	temp_str[len]  =( char)'&';//&lt;
																	temp_str[len+1]=( char)'g';
																	temp_str[len+2]=( char)'t';
																	temp_str[len+3]=( char)';';
																	len+=4;
																}
																else
																{
																	temp_str[len]=(char)(p[f]);
																	len++;
																}

																break;


														case '&':
															if(p[f+1]=='l'&&p[f+2]=='t'&&p[f+3]==';')
															{
															temp_str[len]  =( char)'&';//&lt
															temp_str[len+1]=( char)'l';
															temp_str[len+2]=( char)'t';
															temp_str[len+3]=( char)';';
															len+=4;
															f+=3;
															}
															else
															if(p[f+1]=='g'&&p[f+2]=='t'&&p[f+3]==';')
															{
															temp_str[len]  =( char)'&';//&gt
															temp_str[len+1]=( char)'g';
															temp_str[len+2]=( char)'t';
															temp_str[len+3]=( char)';';
															len+=4;
															f+=3;
															}
															else
															if(p[f+1]=='n'&&p[f+2]=='b'&&p[f+3]=='s'&&p[f+4]=='p'&&p[f+5]==';')
															{
															temp_str[len]  =( char)'&';//&nbsp
															temp_str[len+1]=( char)'n';
															temp_str[len+2]=( char)'b';
															temp_str[len+3]=( char)'s';
															temp_str[len+4]=( char)'p';
															temp_str[len+5]=( char)';';
															len+=6;
															f+=5;
															/*temp_str[len]  =( char)0xC2;
															temp_str[len+1]=( char)0xA0;
															len+=2;
															f+=5;*/
															}
															else
															if(p[f+1]=='a'&&p[f+2]=='p'&&p[f+3]=='o'&&p[f+4]=='s'&&p[f+5]==';')
															{
															temp_str[len]  =( char)'&';//&apos; («'»),
															temp_str[len+1]=( char)'a';
															temp_str[len+2]=( char)'p';
															temp_str[len+3]=( char)'o';
															temp_str[len+4]=( char)'s';
															temp_str[len+5]=( char)';';
															len+=6;
															f+=5;
															}
															else
															if(p[f+1]=='q'&&p[f+2]=='u'&&p[f+3]=='o'&&p[f+4]=='t'&&p[f+5]==';')
															{
															temp_str[len]  =( char)'&';// &quot; ('"')
															temp_str[len+1]=( char)'q';
															temp_str[len+2]=( char)'u';
															temp_str[len+3]=( char)'o';
															temp_str[len+4]=( char)'t';
															temp_str[len+5]=( char)';';
															len+=6;
															f+=5;
															}

															else
															if(p[f+1]=='a'&&p[f+2]=='m'&&p[f+3]=='p'&&p[f+4]==';')
															{
															temp_str[len]  =( char)'&';//&amp; («&»)
															temp_str[len+1]=( char)'a';
															temp_str[len+2]=( char)'m';
															temp_str[len+3]=( char)'p';
															temp_str[len+4]=( char)';';
															len+=5;
															f+=4;
															}
															else
															if(p[f+1]!='l'&&p[f+1]!='g'&&p[f+1]!='a'&&p[f+1]!='q'&&p[f+1]!='n'&&p[f+1]!='c')
															{
															temp_str[len]  =( char)'&';//  «&»
															temp_str[len+1]=( char)'a';
															temp_str[len+2]=( char)'m';
															temp_str[len+3]=( char)'p';
															temp_str[len+4]=( char)';';
															len+=5;
															//f+=4;
															}
															else
															{
																temp_str[len]=(char)(p[f]);
																len++;
															}

															break;

														case 0xE2 :
															if(  
																 (unsigned char)p[f+1]==( unsigned char)0x80 && 
																((unsigned char)p[f+2]==( unsigned char)0x98 || 
																 (unsigned char)p[f+2]==(unsigned char)0x99) 
																 )
															{
																temp_str[len]  =( char)'&';//&apos; («'»),
																temp_str[len+1]=( char)'a';
																temp_str[len+2]=( char)'p';
																temp_str[len+3]=( char)'o';
																temp_str[len+4]=( char)'s';
																temp_str[len+5]=( char)';';
																len+=6;
																f+=2;
															}
															else
															{
																temp_str[len]=(char)(p[f]);
																len++;
															}
																break;

														case 0x27 :
															temp_str[len]  =( char)'&';//&apos; («'»),
															temp_str[len+1]=( char)'a';
															temp_str[len+2]=( char)'p';
															temp_str[len+3]=( char)'o';
															temp_str[len+4]=( char)'s';
															temp_str[len+5]=( char)';';
															len+=6;
															break;
														case 0x22 :
															temp_str[len]  =( char)'&';// &quot; (""")
															temp_str[len+1]=( char)'q';
															temp_str[len+2]=( char)'u';
															temp_str[len+3]=( char)'o';
															temp_str[len+4]=( char)'t';
															temp_str[len+5]=( char)';';
															len+=6;
															break;
												
														case 0xC2 :
															if((unsigned char)p[f+1]==(unsigned  char)0xA0)//&nbsp;
															{
																temp_str[len]  =( char)' '; len++;f++;
															/*	temp_str[len]  =( char)'&';//&nbsp;
																temp_str[len+1]=( char)'n';
																temp_str[len+2]=( char)'b';
																temp_str[len+3]=( char)'s';
																temp_str[len+4]=( char)'p';
																temp_str[len+5]=( char)';';
																len+=6;
																f++;*/
															}
															else
															if((unsigned char)p[f+1]==(unsigned  char)0xA9)  //	©
															{
																temp_str[len]  =( char)'&';//&copy;
																temp_str[len+1]=( char)'c';
																temp_str[len+2]=( char)'o';
																temp_str[len+3]=( char)'p';
																temp_str[len+4]=( char)'y';
																temp_str[len+5]=( char)';';
																len+=6;
																f++;
															}
															else
															if((unsigned char)p[f+1]==(unsigned  char)0xAE)  //	® 
															{
																temp_str[len]=(char)(p[f]);
																len++;
																temp_str[len]=(char)(p[f+1]);
																len++;
																f++;
															}
															else
															{
																temp_str[len]=(char)(p[f]);
																len++;
															}
															break;


														default:
															temp_str[len]=(char)(p[f]);
															len++;
														}
													}
												
												temp_str[len]=0;
											strcpy(p,temp_str);

											delete (temp_str);
}
