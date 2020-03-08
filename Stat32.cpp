// Stat32.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#include <commdlg.h>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>
#include <math.h>
#include <time.h>

#define mStr 128

/*======================================================================================*/
/* declaring additional variables */
struct INFO_FILE_STAT {
 double  TIME;/* Время выполнения программы*/
 double  DIST;/* */
 long int numbers_of_kadrs;/* Число кадров*/
 long int numbers_of_kadrs_line;/* Число кадров с линейными перемещениями*/
 long int numbers_of_kadrs_circle;/* Число кадров с круговыми перемещениями*/
 double min_feed,max_feed;
 double min_points[3],max_points[3];
 char info[mStr*3];
 short int fl_n33;
 int err;
 double  SUMXYZ[3];
} ;
struct INFO_FILE_STAT infofile ;
int  read_word(char *,char ,char *) ;
void replace_word(char *,char ,char *) ;
int delete_word(char *,char ) ;
double decodeFeed (double ) ;
long int program_format_H33(char *, int ) ;
double program_time_H33(char *) ;
int program_statistics_save(char *, struct INFO_FILE_STAT );
long int program_statistics(char *,double , struct INFO_FILE_STAT *) ;
/*======================================================================================*/


#define MAX_LOADSTRING 100

// Global Variables:
HWND hWnd;				// current HWND
HINSTANCE hInst;			// current instance
TCHAR szTitle[MAX_LOADSTRING];		// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];	// The title bar text

// Foward declarations of functions included in this code module:
ATOM			MyRegisterClass(HINSTANCE hInstance);
BOOL			InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

/*======================================================================================*/
int boolRunCmd; // запуск из командной строки
char FileName[255]; // имя файла
int openFileDlg(void); //открытие диалогового окна
int saveResultFileDlg(void); //открытие диалогового окна для записи результатов
int mainWin(char *);  //запуск статистики
int mainWinClear(void); //очистка окна
int mainCmdLine(LPSTR ); //запуск метода из командной строки
/*======================================================================================*/


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

/*======================================================================================*/
	int i;
	boolRunCmd=0;
	FileName[0]='\0';
	//MessageBox(NULL,lpCmdLine,"CmdLine",0);
	if (0!=strcmp(lpCmdLine,""))
	{
	  i=mainCmdLine(lpCmdLine);
	  if (i!=0) {
	     PostQuitMessage(0);
	     return FALSE;
	  }
	}
/*======================================================================================*/


	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_STAT32, szWindowClass, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_STAT32);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if ( (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) &&
		     (!IsDialogMessage(hWnd, &msg)) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



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

	wcex.style		= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon		= LoadIcon(hInstance, (LPCTSTR)IDI_STAT32);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW); // (COLOR_WINDOW+1)
	wcex.lpszMenuName	= (LPCSTR)IDC_STAT32;
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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   //CW_USEDEFAULT, 0,
   /*hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED|WS_SYSMENU|WS_DISABLED,
      CW_USEDEFAULT, 0, 450, 150, NULL, NULL, hInstance, NULL);*/

  FARPROC DlgStat = MakeProcInstance((FARPROC)WndProc,hInst);
  hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_INFO), NULL, (DLGPROC) DlgStat);

   if (!hWnd)
   {
      return FALSE;
   }

   if (boolRunCmd==1) {
   	mainWin(FileName);
   	//  SendMessage(hWnd,WM_SETTEXT,0,0);
        boolRunCmd=0;
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int Errval, i;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	char info[mStr*3];

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
				case IDM_EXIT: case IDOKEXIT: case IDCANCEL :
				   //EndDialog(hDlg, LOWORD(wParam));
				   DestroyWindow(hWnd);
				   PostQuitMessage(0);
				   break;
/*======================================================================================*/
     	 			case IDM_NOTE:
     	 		           strcpy(info,"notepad \"");strcat(info,FileName);strcat(info,"\"");
	                           i=WinExec(info,SW_SHOW);
	                           if (i < 32) MessageBox(GetFocus(), "WinExec failed", "Error", MB_ICONSTOP);
     	 			   break;
     	 			case IDM_SAVERESULT:
     	 			   saveResultFileDlg();
     	 			   break;
     	 			case IDM_CLEAR:
     	 			   mainWinClear();
     	 			   break;
     	 			case IDC_FILE:
     	 			    Errval=openFileDlg();
     	 			    if (Errval==0)
     	 			    {
				      if (!strcmp(FileName,"")) {
				      	 // MessageBox(GetFocus(),"Внимание:\n Имя: УП файла  - не задано . ","Предупреждение",MB_OK|MB_ICONSTOP);
				       } else {
				         mainWin(FileName);
			              }

     	 			    }
				break;
/*======================================================================================*/
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			DestroyWindow(hWnd);
			PostQuitMessage(0);
			break;
		default:
			//return DefWindowProc(hWnd, message, wParam, lParam);
			break;
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

//-------------------------------------------------------------------
int openFileDlg(void)
{
 OPENFILENAME ofnTemp;
 DWORD Errval;/* Error value*/
 char buf[255];	/* Error buffer*/
 char szTemp[] = "UP Files (*.up)\0*.up\0Data Files (*.dat)\0*.dat\0Alpha Files (*.upa)\0*.upa\0Ptp Files (*.ptp)\0*.ptp\0All Files (*.*)\0*.*\0";

  buf[0]='\0';
  Errval=0;
  FileName[0]='\0';

  ofnTemp.lStructSize = sizeof( OPENFILENAME );
  ofnTemp.hwndOwner = hWnd;
  ofnTemp.hInstance = 0;
  ofnTemp.lpstrFilter = (LPSTR)szTemp;
  ofnTemp.lpstrCustomFilter = NULL;
  ofnTemp.nMaxCustFilter = 0;
  ofnTemp.nFilterIndex = 1;
  ofnTemp.lpstrFile = (LPSTR)FileName;/* Stores the result in this variable*/
  ofnTemp.nMaxFile = sizeof( FileName );
  ofnTemp.lpstrFileTitle = NULL;
  ofnTemp.nMaxFileTitle = 0;
  ofnTemp.lpstrInitialDir = NULL;
  ofnTemp.lpstrTitle = "Выберите Файл";	/* Title for dialog*/
  ofnTemp.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
  ofnTemp.nFileOffset = 0;
  ofnTemp.nFileExtension = 0;
  ofnTemp.lpstrDefExt = "*";
  ofnTemp.lCustData = NULL;
  ofnTemp.lpfnHook = NULL;
  ofnTemp.lpTemplateName = NULL;
  if(GetOpenFileName( &ofnTemp ) != TRUE)
  { Errval=CommDlgExtendedError();
    if(Errval!=0) /* 0 value means user selected Cancel*/
    {   sprintf(buf,"GetOpenFileName returned Error # %ld",Errval);
  	MessageBox(GetFocus(),buf,"WARNING",MB_OK|MB_ICONSTOP);
     }
  }

  return Errval ;
}


//-------------------------------------------------------------------
int saveResultFileDlg(void)
{
 OPENFILENAME ofnTemp;
 DWORD Errval;/* Error value*/
 char buf[255];	/* Error buffer*/
 char szTemp[] = "Txt Files (*.txt)\0*.txt\0Log Files (*.log)\0*.log\0Data Files (*.dat)\0*.dat\0All Files (*.*)\0*.*\0";
 char szFileName[255];	/* FileName for save result*/

  buf[0]='\0';
  Errval=0;
  szFileName[0]='\0';

  ofnTemp.lStructSize = sizeof( OPENFILENAME );
  ofnTemp.hwndOwner = hWnd;
  ofnTemp.hInstance = 0;
  ofnTemp.lpstrFilter = (LPSTR)szTemp;
  ofnTemp.lpstrCustomFilter = NULL;
  ofnTemp.nMaxCustFilter = 0;
  ofnTemp.nFilterIndex = 1;
  ofnTemp.lpstrFile = (LPSTR)szFileName;/* Stores the result in this variable*/
  ofnTemp.nMaxFile = sizeof( szFileName );
  ofnTemp.lpstrFileTitle = NULL;
  ofnTemp.nMaxFileTitle = 0;
  ofnTemp.lpstrInitialDir = NULL;
  ofnTemp.lpstrTitle = "Выберите Файл для сохранения результатов..";	/* Title for dialog*/
  ofnTemp.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
  ofnTemp.nFileOffset = 0;
  ofnTemp.nFileExtension = 0;
  ofnTemp.lpstrDefExt = "*";
  ofnTemp.lCustData = NULL;
  ofnTemp.lpfnHook = NULL;
  ofnTemp.lpTemplateName = NULL;
  if(GetSaveFileName( &ofnTemp ) != TRUE)
  { Errval=CommDlgExtendedError();
    if(Errval!=0) /* 0 value means user selected Cancel*/
    {   sprintf(buf,"GetSaveFileName returned Error # %ld",Errval);
  	MessageBox(GetFocus(),buf,"WARNING",MB_OK|MB_ICONSTOP);
     }
   } else {

     Errval=program_statistics_save(szFileName, infofile);
     if (Errval<0) {
       MessageBeep(0);
       buf[0]='\0';
       sprintf(buf,"Невозможно открыть файл:\n %s \n на 'дополнение' \n",szFileName );
       MessageBox(NULL,(LPCTSTR)buf,"Сохранение результатов статистики -> ошибка...",MB_ICONSTOP|MB_OK);
     }

  }


  return Errval ;
}


//-------------------------------------------------------------------
int mainWinClear(void)
{
     FileName[0]='\0';
     //IDC_CAPTION
	SendDlgItemMessage(hWnd,IDC_CAPTION,WM_SETTEXT,0,long("............."));
     //IDC_TIME
	SendDlgItemMessage(hWnd,IDC_TIME,WM_SETTEXT,0,long(""));
     //IDC_DIST
	SendDlgItemMessage(hWnd,IDC_DIST,WM_SETTEXT,0,long(""));
     //IDC_NumKadrs
	SendDlgItemMessage(hWnd,IDC_NumKadrs,WM_SETTEXT,0,long(""));
     //IDC_NumLine  IDC_NumCircle
	SendDlgItemMessage(hWnd,IDC_NumLine,WM_SETTEXT,0,long(""));
	SendDlgItemMessage(hWnd,IDC_NumCircle,WM_SETTEXT,0,long(""));
     //IDC_Minfeed  IDC_Maxfeed
	SendDlgItemMessage(hWnd,IDC_Minfeed,WM_SETTEXT,0,long(""));
	SendDlgItemMessage(hWnd,IDC_Maxfeed,WM_SETTEXT,0,long(""));
     //IDC_TYPE
        SendDlgItemMessage(hWnd,IDC_TYPE,WM_SETTEXT,0,long(".."));
     //IDC_minX
	SendDlgItemMessage(hWnd,IDC_minX,WM_SETTEXT,0,long(""));
     //IDC_minY
	SendDlgItemMessage(hWnd,IDC_minY,WM_SETTEXT,0,long(""));
     //IDC_minZ
	SendDlgItemMessage(hWnd,IDC_minZ,WM_SETTEXT,0,long(""));
     //IDC_maxX
	SendDlgItemMessage(hWnd,IDC_maxX,WM_SETTEXT,0,long(""));
     //IDC_maxY
	SendDlgItemMessage(hWnd,IDC_maxY,WM_SETTEXT,0,long(""));
     //IDC_maxZ
	SendDlgItemMessage(hWnd,IDC_maxZ,WM_SETTEXT,0,long(""));
     //IDC_sumX
	SendDlgItemMessage(hWnd,IDC_sumX,WM_SETTEXT,0,long(""));
     //IDC_sumY
	SendDlgItemMessage(hWnd,IDC_sumY,WM_SETTEXT,0,long(""));
     //IDC_sumZ
	SendDlgItemMessage(hWnd,IDC_sumZ,WM_SETTEXT,0,long(""));
     //
  return(0);
}

//-------------------------------------------------------------------
int mainWin(char *lpFileName)
{
  int error;
  char info[mStr*3];

  error=program_statistics(lpFileName,200000.0,&infofile);

  if (error==-1) {
    MessageBeep(0);
    info[0]='\0';
    sprintf(info,"Невозможно открыть файл:\n %s \n на 'чтение' \n",lpFileName );
    MessageBox(NULL,(LPCTSTR)info,"Статистика УП - > ошибка ...",MB_ICONSTOP|MB_OK);
    return(-1);
  }

     //IDC_CAPTION
	SendDlgItemMessage(hWnd,IDC_CAPTION,WM_SETTEXT,0,long(infofile.info));
     //IDC_TIME
        info[0]='\0';  sprintf(info,"%.2lf",infofile.TIME);
	SendDlgItemMessage(hWnd,IDC_TIME,WM_SETTEXT,0,long(info));
     //IDC_DIST
        info[0]='\0';  sprintf(info,"%.3lf",infofile.DIST);
	SendDlgItemMessage(hWnd,IDC_DIST,WM_SETTEXT,0,long(info));
     //IDC_NumKadrs
        info[0]='\0';  sprintf(info,"%ld",infofile.numbers_of_kadrs);
	SendDlgItemMessage(hWnd,IDC_NumKadrs,WM_SETTEXT,0,long(info));
     //IDC_NumLine  IDC_NumCircle
        info[0]='\0';  sprintf(info,"%ld",infofile.numbers_of_kadrs_line);
	SendDlgItemMessage(hWnd,IDC_NumLine,WM_SETTEXT,0,long(info));
        info[0]='\0';  sprintf(info,"%ld",infofile.numbers_of_kadrs_circle);
	SendDlgItemMessage(hWnd,IDC_NumCircle,WM_SETTEXT,0,long(info));
     //IDC_Minfeed  IDC_Maxfeed
        info[0]='\0';  sprintf(info,"%.2lf",infofile.min_feed);
	SendDlgItemMessage(hWnd,IDC_Minfeed,WM_SETTEXT,0,long(info));
        info[0]='\0';  sprintf(info,"%.2lf",infofile.max_feed);
	SendDlgItemMessage(hWnd,IDC_Maxfeed,WM_SETTEXT,0,long(info));
     //IDC_TYPE
        if (infofile.fl_n33 != 0) {
           SendDlgItemMessage(hWnd,IDC_TYPE,WM_SETTEXT,0,long("H33(ATAKA),.."));
         } else {
           SendDlgItemMessage(hWnd,IDC_TYPE,WM_SETTEXT,0,long("Alpha,MTC200,.."));
        }

     //IDC_minX
        info[0]='\0';  sprintf(info,"%.3lf",infofile.min_points[0]);
	SendDlgItemMessage(hWnd,IDC_minX,WM_SETTEXT,0,long(info));
     //IDC_minY
        info[0]='\0';  sprintf(info,"%.3lf",infofile.min_points[1]);
	SendDlgItemMessage(hWnd,IDC_minY,WM_SETTEXT,0,long(info));
     //IDC_minZ
        info[0]='\0';  sprintf(info,"%.3lf",infofile.min_points[2]);
	SendDlgItemMessage(hWnd,IDC_minZ,WM_SETTEXT,0,long(info));
     //IDC_maxX
        info[0]='\0';  sprintf(info,"%.3lf",infofile.max_points[0]);
	SendDlgItemMessage(hWnd,IDC_maxX,WM_SETTEXT,0,long(info));
     //IDC_maxY
        info[0]='\0';  sprintf(info,"%.3lf",infofile.max_points[1]);
	SendDlgItemMessage(hWnd,IDC_maxY,WM_SETTEXT,0,long(info));
     //IDC_maxZ
        info[0]='\0';  sprintf(info,"%.3lf",infofile.max_points[2]);
	SendDlgItemMessage(hWnd,IDC_maxZ,WM_SETTEXT,0,long(info));
     //IDC_sumX
        info[0]='\0';  sprintf(info,"%.2lf",infofile.SUMXYZ[0]);
	SendDlgItemMessage(hWnd,IDC_sumX,WM_SETTEXT,0,long(info));
     //IDC_sumY
        info[0]='\0';  sprintf(info,"%.2lf",infofile.SUMXYZ[1]);
	SendDlgItemMessage(hWnd,IDC_sumY,WM_SETTEXT,0,long(info));
     //IDC_sumZ
        info[0]='\0';  sprintf(info,"%.2lf",infofile.SUMXYZ[2]);
	SendDlgItemMessage(hWnd,IDC_sumZ,WM_SETTEXT,0,long(info));
     //

 return (0);
}

//-------------------------------------------------------------------
int mainCmdLine(LPSTR lpCmdLineProc)
{
  int argc,loop,j,fl,error;
  char  filenameIn[255], filenameOut[255];
  char info[mStr*3];

 /* check for only one command-line parameter */
  error=0;
  filenameIn[0]='\0';filenameOut[0]='\0';argc=0;
  loop=0;j=0;fl=0;
  while(lpCmdLineProc[loop]!='\0')
  {
    if (lpCmdLineProc[loop]=='"') { if (fl==0) {fl=1;loop++;argc++;j=0;} else fl=0; }
    if (fl==1) {
      if (argc==1) { filenameIn[j]=lpCmdLineProc[loop]; j++; filenameIn[j]='\0'; }
      if (argc==2) { filenameOut[j]=lpCmdLineProc[loop]; j++; filenameOut[j]='\0'; }
     }
    loop++;
  }

 if (argc==2)
  {
     error=program_statistics(filenameIn,200000.0,&infofile);

     if (error<0) {
       MessageBeep(0);
       info[0]='\0';
       sprintf(info,"Невозможно открыть файл:\n %s \n на 'чтение' \n",filenameIn );
       MessageBox(NULL,(LPCTSTR)info,"Статистика УП - > ошибка ...",MB_ICONSTOP|MB_OK);
       return(-1);
     }

     error=program_statistics_save(filenameOut, infofile);
     if (error<0) {
       MessageBeep(0);
       info[0]='\0';
       sprintf(info,"Невозможно открыть файл:\n %s \n на 'дополнение' \n",filenameOut );
       MessageBox(NULL,(LPCTSTR)info,"Сохранение результатов статистики -> ошибка...",MB_ICONSTOP|MB_OK);
       return(-1);
     }

     return(1);
  }

  strcpy(FileName,filenameIn);
  boolRunCmd=1;
/*
  info[0]='\0';
  sprintf(info,"%s \n %s \n %d ",filenameIn,filenameOut,error);
  MessageBox(GetFocus(),(LPCTSTR)info,"Error",MB_ICONSTOP|MB_OK);
*/
 return (0);
}


/*======================================================================================*/

//---------------------------------------------------------------------------
/* This function read the numeric string after key letter
	in success it return 1, otherwice 0
*/
int read_word(char *s,char w,char *sw)
{
         char *word,sym;
	 int  n;
	 if(s==NULL) return(0);
	 if ((word=strchr(s,w))==NULL) return(0);
	 n=0;
	 do
	 {  sym=*(word+(n+1));
		 if (isdigit(sym)||sym==' '||sym=='+'||sym=='-'||sym=='.') n++;
		 else break;
	 } while (1);
	 if (n>0){ strncpy(sw,word+1,n); sw[n]='\0'; return(n); }
	 else                                        return(0);
}

/* This function replace the numeric string after key letter
	if there isn't a key letter in a string funktion adds it
	at the end of a string
*/
void  replace_word(char *s,char w,char *sw)
{
  int n;
  char old_sw[20],save[100],*p;

  if(s==NULL) return;

  if ((n=read_word(s,w,old_sw))!=0)
  {
	 /* replace word */
	 p=strchr(s,w);
	 *(p+1)='\0'; strcpy(save,p+n+1);
	 strcat(strcat(s,sw),save);
  }
  else
  {
	 /* add word at the end of string */
       n=read_word(s,'L',old_sw);
       if(n==0)
        {
			p=strchr(s,'<') ; /* string have <CR> or <LF> as text */
	 if (p==NULL)   p=strchr(s,'\r'); /* string have \r\n */
	 if (p==NULL)   p=strchr(s,'\n'); /* string have \n   */
	 if (p!=NULL)
	 { strcpy(save,p); *p=w; *(p+1)='\0'; strcat(strcat(s,sw),save); }
	 else
	 {
		save[0]=' ';save[1]=w; save[2]='\n'; save[3]='\0'; strcat(strcat(s,save),sw); }
        }
       else
        {
        p=strchr(s,'L'); *p=w;
	*(p+1)='\0'; strcpy(save,p+n+1);
	strcat(strcat(s,sw),save);
        strcat(strcat(s,"L"),strcat(old_sw,"\n"));
        }

  }
}

/*Удаление кодов*/
int delete_word(char *s,char w)
{
 int n;
 char old_sw[mStr],save[mStr],*p;

 if(s==NULL) return (0);

 n=0;
 if ((n=read_word(s,w,old_sw))!=0)
  {
	 /* replace word */
	 p=strchr(s,w);
	 *(p+0)='\0'; strcpy(save,p+n+1);
	 strcat(s,save);
  }

 return n;
}

//---------------------------------------------------------------------------

/*# Декодирует подачу Н33 в нормальное число*/
double decodeFeed (double ffeed )  {
/*# ffeed - общий случай , ГОСТ - обозначения*/
  int  fmode,intf ;
  double ffeed_tmp, kf ;
  if (ffeed<0.0) ffeed=0;

  fmode=0;
  if (ffeed>4000) { ffeed-=4000 ; fmode=4 ; }

  ffeed_tmp=ffeed/100.0 ;
  intf=(int)ffeed_tmp ;
  ffeed_tmp-=intf ;
  kf=intf - 3 ;

  ffeed_tmp*=pow(10,kf) ;

  return (ffeed_tmp);
}


/*################################################################################*/
/* Author: Gumen Olga, 7 nov 2003 */
/*
Проверяет - является ли УП - программой для Н33 (Атака)
Параметры :
    namefile - имя файла ,
    codefeed - 0 адреса F - проверять,
               1 адреса F - не проверять
Возвращаемое значение :
    0  -  УП является программой для Н33 (Атака)
    -1 -  Невозможно открыть файл 'чтение'
    n  -  число ошибок -> УП не является программой для Н33 (Атака)
*/
long int program_format_H33(char *namefile, int codefeed = 0)
{
/*локальные переменные*/
 FILE *rf; /*Указатель на файл*/
 long int numbers_of_kadrs;/* Число кадров*/
 long int error;
 char info[mStr*3];
 char curr[mStr], word[mStr];

 /*Открываем файл*/
 rf=fopen(namefile,"r");
 if (rf==NULL) {
   info[0]='\0';
   sprintf(info,"Невозможно открыть файл:\n %s \n на 'чтение' \n",namefile );
   /*Application->MessageBox(info,"Проверка программы на формат H33 - > ошибка ...", MB_OK);*/
   fprintf(stderr, info);
   return(-1);
  }

 /*Начальные условия*/
 error=0;
 /*if (codefeed!=1) codefeed=0;*/
 numbers_of_kadrs=0;

 curr[0]='\0'; fgets(curr,mStr,rf);
 while(!feof(rf))
 {

   /*Первичные признаки*/
   if (read_word(curr,'X',word)) {
      if (7!=strlen(word)) error++;
   }
   if (read_word(curr,'Y',word)) {
      if (7!=strlen(word)) error++;
   }
   if (read_word(curr,'Z',word)) {
      if (7!=strlen(word)) error++;
   }
   if (read_word(curr,'I',word)) {
      if (7!=strlen(word)) error++;
   }
   if (read_word(curr,'J',word)) {
      if (7!=strlen(word)) error++;
   }
   if (read_word(curr,'K',word)) {
      if (7!=strlen(word)) error++;
   }

   /*Вспомогательные признаки*/
   if (codefeed==0) {
      if (read_word(curr,'F',word)) {
         if (4!=strlen(word)) error++;
      }
   }

   /* if (read_word(curr,'M',word)) {
      if (3<=strlen(word)) error++;
   }  */
   if (read_word(curr,'L',word)) {
      if (3!=strlen(word)) error++;
   }
   /*if (read_word(curr,'N',word)) {
      if ((3!=strlen(word))||(5!=strlen(word))) error++;
   } */
   if (read_word(curr,'G',word)) {
      if (3<=strlen(word)) error++;
   }

   if (read_word(curr,'D',word)) error++;
   if (read_word(curr,'H',word)) error++;

   /*Вторичные признаки*/
   if (NULL!=strchr(curr,'.')) error++;
   if (NULL!=strchr(curr,';')) error++;
   if (NULL!=strchr(curr,'(')) error++;
   if (NULL!=strchr(curr,')')) error++;
   if (NULL!=strchr(curr,'"')) error++;
   if (NULL!=strchr(curr,'\'')) error++;

   curr[0]='\0'; fgets(curr,mStr,rf);
   numbers_of_kadrs++;
 } // end while

 fclose(rf);

 return(error);
}

/*################################################################################*/
/* Author: Gumen Olga, 8 dec 2003 */
/*
Вычисляет время выполнения УП (УЧПУ H33)
Замечание :
    Ошибка вычисления составляет ±5 мин (нет учета разгона-торможения)
    если в УП очень много движений по окружностям - ошибка возрастает
Параметры :
    namefile - имя файла ,
Возвращаемое значение :
    t  -  время выполнения УП - мин
    -1 -  Невозможно открыть файл 'чтение'
*/
double program_time_H33(char *namefile)
{
 double  TIME;/* Время выполнения программы*/
/*локальные переменные*/
 FILE *rf; /*Указатель на файл*/
 char info[mStr*3];
 char curr[mStr];
 char word[mStr];
 double curr_points[3];
 double curr_feed;
 long int i;
 double mv;
 short int fl_move;

 /*Открываем файл*/
 rf=fopen(namefile,"r");
 if (rf==NULL) {
   info[0]='\0';
   sprintf(info,"Невозможно открыть файл:\n %s \n на 'чтение' \n",namefile );
   /*Application->MessageBox(info,"Статистика программы - > ошибка ...", MB_OK);*/
   fprintf(stderr, info);
   return(-1);
  }

 /*Начальные условия*/
 TIME=0.0;
 curr_feed=20;

 curr[0]='\0'; fgets(curr,mStr,rf);
 while(!feof(rf))
 {

   for(i=0;i<3;i++) curr_points[i]=0.0;

   fl_move=0;/*флаг движения*/

   if (read_word(curr,'X',word)) { sscanf(word,"%lf",&curr_points[0]);fl_move++; }
   if (read_word(curr,'Y',word)) { sscanf(word,"%lf",&curr_points[1]);fl_move++; }
   if (read_word(curr,'Z',word)) { sscanf(word,"%lf",&curr_points[2]);fl_move++; }
   if (read_word(curr,'F',word)) {
      sscanf(word,"%lf",&curr_feed);
      curr_feed=decodeFeed(curr_feed);
   }

   mv=0.0; for(i=0;i<3;i++) { curr_points[i]/=100.0 ; mv+=pow(curr_points[i],2); }
   if (curr_feed>=0.1) TIME+=sqrt(mv)/curr_feed;

   curr[0]='\0'; fgets(curr,mStr,rf);
 } // end while

 fclose(rf);

 /* параметры вывода */

 return(TIME);
}

/*################################################################################*/
/* Author: Gumen Olga, 17 oct 2004 */
/*
Статистика УП (УЧПУ MTC200 & Alpha & УЧПУ H33)
Замечание :

Параметры :
   namefile - имя файла (исходник),
   rapid_speed - значение ускоренной подачи (мм/мин),
   struct INFO_FILE_STAT *infofile - статистика УП

Возвращаемое значение :

   struct INFO_FILE_STAT {
      double  TIME;
      double  DIST;
      long int numbers_of_kadrs;
      long int numbers_of_kadrs_line;
      long int numbers_of_kadrs_circle;
      double min_feed,max_feed;
      double min_points[3],max_points[3];
      char info[mStr*3];
      short int fl_n33;
      int err;
      double  SUMXYZ[3];
    } ;

    0  -  ошибок нет
    -1 -  Невозможно открыть файл namefile - 'чтение'
*/

long int program_statistics(char *namefile,double rapid_speed, struct INFO_FILE_STAT *infofile)
{
 FILE *rf; /*Указатель на файл*/
 double  TIME;/* Время выполнения программы*/
 double  DIST;/* */
 long int numbers_of_kadrs;/* Число кадров*/
 long int numbers_of_kadrs_line;/* Число кадров с линейными перемещениями*/
 long int numbers_of_kadrs_circle;/* Число кадров с круговыми перемещениями*/
 double min_feed,max_feed;
 double min_points[3],max_points[3];
 char info[mStr*3];
 short int fl_n33;
 int err;
 double  SUMXYZ[3];

//локальные переменные
 char curr[mStr],currtmp[mStr];
 char word[mStr];
 double curr_points[3],prev_points[3];
 double curr_feed,prev_feed,curr_feed_min;
 long int i,j,k ;
 double Gmode_coord, Gmotion ;
 double mv,tmp,gcode;
 char *jpd,*jpdg;
 short int fl_move;

 /*Открываем файл*/
 rf=fopen(namefile,"r");
 if (rf==NULL) {
   info[0]='\0';
   sprintf(info,"Невозможно открыть файл:\n %s \n на 'чтение' \n",namefile );
   /*Application->MessageBox(info,"Статистика программы - > ошибка ...", MB_OK);*/
   fprintf(stderr, info);
   return(-1);
  }

 /*Информация о файле*/
 /*1 - Builder C++ v5.0 get information about the file */
/*
 info[0]='\0';
 getftime(fileno(rf), &ft);
 fstat(fileno(rf), &statbuf);
 tmp=statbuf.st_size/1024.0;
 sprintf(info,"Файл: %s \n Дата изменения %u/%u/%u , %u:%u:%u\n Размер: %.2lf Kb ( %ld bytes )\n Время последнего открытия:\n\t\t %s ", \
                 namefile, \
                 ft.ft_month, ft.ft_day,ft.ft_year+1980, \
                 ft.ft_hour, ft.ft_min,ft.ft_tsec * 2, \
                 tmp, statbuf.st_size, \
                 ctime(&statbuf.st_ctime));
*/

 /*2 - Microsoft VC++ v6.0 get information about the file */
   struct stat buf;
   int fh, result;
   if( (fh = _open( namefile, _O_RDONLY )) ==  -1 ) return(-1);
   /* Get data associated with "fh": */
   result = fstat( fh, &buf );
   info[0]='\0';
   /* Check if statistics are valid: */
   if( result != 0 ) {
       sprintf(info,"ERROR! \n  Файл: %s \n\t Bad file handle\n ",namefile );
       return(-1);
     }
     else
     {
       tmp=buf.st_size/1024.0;
       sprintf(info,"Файл: %s \n Размер: %.2lf Kb ( %ld bytes )\n Время последнего открытия:\n\t\t %s ", \
                 namefile, \
                 tmp, buf.st_size, \
                 ctime( &buf.st_ctime ));
     }
   _close( fh );


/* Application->MessageBox(info,"Информация о файле", MB_OK); */

 /*Начальные условия*/
 TIME=0.0;
 DIST=0.0;
 numbers_of_kadrs=0;
 numbers_of_kadrs_line=0;
 numbers_of_kadrs_circle=0;
 min_feed=rapid_speed;
 max_feed=-rapid_speed;
 for(i=0;i<3;i++)
 {
   min_points[i]=999999.999;
   max_points[i]=-999999.999;
   SUMXYZ[i]=0;
   prev_points[i]=0;
   curr_points[i]=0;
 }
 curr_feed=-rapid_speed;
 curr_feed_min=rapid_speed;
 prev_feed=-rapid_speed;
 Gmode_coord=90;
 Gmotion=0;
 fl_move=0; /*флаг движения*/
 fl_n33=0;  /*признак формата Н33*/

 err=program_format_H33(namefile,0);
 if (err==0) {
      Gmode_coord=91;
      fl_n33=1;
 }


   /*Обнуляем структуру*/
   infofile->TIME=TIME;
   infofile->DIST=DIST;
   infofile->numbers_of_kadrs=numbers_of_kadrs;
   infofile->numbers_of_kadrs_line=numbers_of_kadrs_line;
   infofile->numbers_of_kadrs_circle=numbers_of_kadrs_circle;
   infofile->min_feed=min_feed;
   infofile->max_feed=max_feed;
   for(i=0;i<3;i++)
   {
      infofile->min_points[i]=min_points[i];
      infofile->max_points[i]=max_points[i];
      infofile->SUMXYZ[i]=SUMXYZ[i];
   }
   infofile->info[0]='\0';
   infofile->fl_n33=fl_n33;
   infofile->err=err;


 curr[0]='\0'; fgets(curr,mStr,rf);
 while(!feof(rf))
 {

   /*Кадры с G - взято из проекта Symmetry ( блок trans)*/
   if (read_word(curr,'G',word))  {
     currtmp[0]='\0';strcpy(currtmp,curr);
     jpd=strchr(currtmp,'G');
     if(jpd!=NULL) {
       while(strcmp(jpd,""))
       {
         if (read_word(jpd,'G',word))
         { sscanf(word,"%lf",&gcode);
           /*printf(" Gf==%f Gs=%s |" ,gcode,word);*/
	   if (gcode==0) {
	       curr_feed= rapid_speed ;
	       Gmotion=gcode;
	   }
	   if (gcode==1) {
	       Gmotion=gcode;
	   }
	   if ((gcode==2)||(gcode==3)) {
	       Gmotion=gcode;
	   }
	   if ((gcode==90)||(gcode==91)) {
	       Gmode_coord=gcode;
	   }
         }
         jpdg=strchr(jpd,'G');
         if(jpdg==NULL) strcpy(jpd,"");else strcpy(jpd,jpdg+1);
       }
     }

   }

   if (Gmode_coord==91) {
       for(i=0;i<3;i++) curr_points[i]=0.0;
   }

   fl_move=0;

   if (read_word(curr,'X',word)) { sscanf(word,"%lf",&curr_points[0]);fl_move++; }
   if (read_word(curr,'Y',word)) { sscanf(word,"%lf",&curr_points[1]);fl_move++; }
   if (read_word(curr,'Z',word)) { sscanf(word,"%lf",&curr_points[2]);fl_move++; }
   if (read_word(curr,'F',word)) {
      sscanf(word,"%lf",&curr_feed);
      if (fl_n33!=0) {
         curr_feed=decodeFeed(curr_feed) ;
      }
      if (curr_feed<0.01) { curr_feed= prev_feed ; }
      if (curr_feed<0.01) { curr_feed= rapid_speed ; }
   }

   if (fl_move!=0) {
   	if ((Gmotion==0)||(Gmotion==1)) { numbers_of_kadrs_line++ ; }
   	if ((Gmotion==2)||(Gmotion==3)) { numbers_of_kadrs_circle++ ; }
   }

   if (Gmode_coord==91) {
       for(i=0;i<3;i++) {
         SUMXYZ[i]+=curr_points[i];
         curr_points[i]+=prev_points[i];
       }
   }

   mv=0.0; for(i=0;i<3;i++) mv+=pow(curr_points[i]-prev_points[i],2);
   DIST+=sqrt(mv);
   if (curr_feed>=0.1) TIME+=sqrt(mv)/curr_feed;

   /*Анализ на экстремум 1*/
   for(i=0;i<3;i++) {
      if (curr_points[i]<=min_points[i]) { min_points[i]=curr_points[i]; }
      if (curr_points[i]>=max_points[i]) { max_points[i]=curr_points[i]; }
   }

   /*Анализ на экстремум 2*/
   curr_feed_min=-curr_feed;
   if (curr_feed_min>=min_feed) { min_feed=curr_feed_min ; }
   if (curr_feed<=min_feed) { min_feed=curr_feed ; }
   if (curr_feed>=max_feed) { max_feed=curr_feed ; }

   prev_feed=curr_feed;
   for(i=0;i<3;i++) prev_points[i]=curr_points[i];

   curr[0]='\0'; fgets(curr,mStr,rf);
   numbers_of_kadrs++;
 } // end while

 min_feed=fabs(min_feed);

 fclose(rf);

 /* параметры вывода */
 if (fl_n33!=0) {
    DIST/=100.0;
    for(i=0;i<3;i++) {
      min_points[i]/=100.0;
      max_points[i]/=100.0;
      SUMXYZ[i]/=100.0;
    }
    /* Время выполнения*/
    TIME=program_time_H33(namefile);

 }

   /*Заполняем структуру*/
   infofile->TIME=TIME;
   infofile->DIST=DIST;
   infofile->numbers_of_kadrs=numbers_of_kadrs;
   infofile->numbers_of_kadrs_line=numbers_of_kadrs_line;
   infofile->numbers_of_kadrs_circle=numbers_of_kadrs_circle;
   infofile->min_feed=min_feed;
   infofile->max_feed=max_feed;
   for(i=0;i<3;i++)
   {
      infofile->min_points[i]=min_points[i];
      infofile->max_points[i]=max_points[i];
      infofile->SUMXYZ[i]=SUMXYZ[i];
   }
   strcpy(infofile->info,info);
   infofile->fl_n33=fl_n33;
   infofile->err=err;

 return(0);
}


/*################################################################################*/
/* Author: Gumen Olga, 18 oct 2004 */
/*
Статистика УП сохраняется в файл
Замечание :

Параметры :
   namefile - имя файла,
   struct INFO_FILE_STAT *infofile - статистика УП

Возвращаемое значение :
    0  -  ошибок нет
    -1 -  Невозможно открыть файл namefile на - 'дополнение'
*/
int program_statistics_save(char *namefile, struct INFO_FILE_STAT infofile)
{
//локальные переменные
 FILE *rf; /*Указатель на файл*/
 int i;
 char info[mStr*3];

 /*Открываем файл*/
 rf=fopen(namefile,"a");
 if (rf==NULL) {
   info[0]='\0';
   sprintf(info,"Невозможно открыть файл:\n %s \n на 'дополнение' \n",namefile );
   /*Application->MessageBox(info,"Статистика программы - > ошибка ...", MB_OK);*/
   fprintf(stderr, info);
   return(-1);
  }

  /*Сохраняем структуру*/
  fprintf(rf,"\n==================================================\n");
  fprintf(rf,"%s",infofile.info);
  if (infofile.fl_n33==0)
     fprintf(rf,"Тип файла = (%ld) \n",infofile.fl_n33);
    else
     fprintf(rf,"Тип файла = (%ld) - Н33 (Ataka) \n",infofile.fl_n33);
  fprintf(rf,"Время выполнения (мин)= %.1lf \n",infofile.TIME);
  fprintf(rf,"Пройденный путь (мм)= %.2lf \n",infofile.DIST);
  fprintf(rf,"Число кадров= %ld \n",infofile.numbers_of_kadrs);
  fprintf(rf,"Число кадров с линейной интерполяцией= %ld \n",infofile.numbers_of_kadrs_line);
  fprintf(rf,"Число кадров с круговой интерполяцией= %ld \n",infofile.numbers_of_kadrs_circle);
  fprintf(rf,"Подачи: \n");
  fprintf(rf," min подача (мм/мин) = %.2lf \n",infofile.min_feed);
  fprintf(rf," max подача (мм/мин) = %.2lf \n",infofile.max_feed);
  fprintf(rf,"Предельные перемещения:\n");
  fprintf(rf," min: (мм) \n\t");
  for(i=0;i<3;i++)
  {
      fprintf(rf," %10.3lf ",infofile.min_points[i]);
   }
  fprintf(rf,"\n max: (мм)\n\t");
  for(i=0;i<3;i++)
  {
      fprintf(rf," %10.3lf ",infofile.max_points[i]);
   }
  fprintf(rf,"\nКонтрольные суммы:\n");
  fprintf(rf," SUM : (мм)\n\t");
  for(i=0;i<3;i++)
  {
      fprintf(rf," %10.2lf ",infofile.SUMXYZ[i]);
   }
  fprintf(rf,"\nЧисло ошибок = %ld \n",infofile.err);
  fprintf(rf,"==================================================\n");

  fclose(rf);

  return (0);
}

/*===========================================================================*/

/*===========================================================================*/

