// dibview.cpp : implementation of the CDibView class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "diblook.h"



#include "dibdoc.h"
#include "dibview.h"
#include "dibapi.h"
#include "mainfrm.h"

#include "HRTimer.h"
#include "ErrorSelect.h"
#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define BEGIN_PROCESSING() INCEPUT_PRELUCRARI()

#define END_PROCESSING(Title) SFARSIT_PRELUCRARI(Title)

#define INCEPUT_PRELUCRARI() \
	CDibDoc* pDocSrc=GetDocument();										\
	CDocTemplate* pDocTemplate=pDocSrc->GetDocTemplate();				\
	CDibDoc* pDocDest=(CDibDoc*) pDocTemplate->CreateNewDocument();		\
	BeginWaitCursor();													\
	HDIB hBmpSrc=pDocSrc->GetHDIB();									\
	HDIB hBmpDest = (HDIB)::CopyHandle((HGLOBAL)hBmpSrc);				\
	if ( hBmpDest==0 ) {												\
		pDocTemplate->RemoveDocument(pDocDest);							\
		return;															\
	}																	\
	BYTE* lpD = (BYTE*)::GlobalLock((HGLOBAL)hBmpDest);					\
	BYTE* lpS = (BYTE*)::GlobalLock((HGLOBAL)hBmpSrc);					\
	int iColors = DIBNumColors((char *)&(((LPBITMAPINFO)lpD)->bmiHeader)); \
	RGBQUAD *bmiColorsDst = ((LPBITMAPINFO)lpD)->bmiColors;	\
	RGBQUAD *bmiColorsSrc = ((LPBITMAPINFO)lpS)->bmiColors;	\
	BYTE * lpDst = (BYTE*)::FindDIBBits((LPSTR)lpD);	\
	BYTE * lpSrc = (BYTE*)::FindDIBBits((LPSTR)lpS);	\
	int dwWidth  = ::DIBWidth((LPSTR)lpS);\
	int dwHeight = ::DIBHeight((LPSTR)lpS);\
	int w=WIDTHBYTES(dwWidth*((LPBITMAPINFOHEADER)lpS)->biBitCount);	\
	HRTimer my_timer;	\
	my_timer.StartTimer();	\

#define BEGIN_SOURCE_PROCESSING \
	CDibDoc* pDocSrc=GetDocument();										\
	BeginWaitCursor();													\
	HDIB hBmpSrc=pDocSrc->GetHDIB();									\
	BYTE* lpS = (BYTE*)::GlobalLock((HGLOBAL)hBmpSrc);					\
	int iColors = DIBNumColors((char *)&(((LPBITMAPINFO)lpS)->bmiHeader)); \
	RGBQUAD *bmiColorsSrc = ((LPBITMAPINFO)lpS)->bmiColors;	\
	BYTE * lpSrc = (BYTE*)::FindDIBBits((LPSTR)lpS);	\
	int dwWidth  = ::DIBWidth((LPSTR)lpS);\
	int dwHeight = ::DIBHeight((LPSTR)lpS);\
	int w=WIDTHBYTES(dwWidth*((LPBITMAPINFOHEADER)lpS)->biBitCount);	\
	


#define END_SOURCE_PROCESSING	\
	::GlobalUnlock((HGLOBAL)hBmpSrc);								\
    EndWaitCursor();												\
/////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------
#define SFARSIT_PRELUCRARI(Titlu)	\
	double elapsed_time_ms = my_timer.StopTimer();	\
	CString Title;	\
	Title.Format("%s - Proc. time = %.2f ms", Titlu, elapsed_time_ms);	\
	::GlobalUnlock((HGLOBAL)hBmpDest);								\
	::GlobalUnlock((HGLOBAL)hBmpSrc);								\
    EndWaitCursor();												\
	pDocDest->SetHDIB(hBmpDest);									\
	pDocDest->InitDIBData();										\
	pDocDest->SetTitle((LPCSTR)Title);									\
	CFrameWnd* pFrame=pDocTemplate->CreateNewFrame(pDocDest,NULL);	\
	pDocTemplate->InitialUpdateFrame(pFrame,pDocDest);	\

/////////////////////////////////////////////////////////////////////////////
// CDibView

IMPLEMENT_DYNCREATE(CDibView, CScrollView)

BEGIN_MESSAGE_MAP(CDibView, CScrollView)
	//{{AFX_MSG_MAP(CDibView)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_MESSAGE(WM_DOREALIZE, OnDoRealize)
	ON_COMMAND(ID_PROCESSING_PARCURGERESIMPLA, OnProcessingParcurgereSimpla)
	//}}AFX_MSG_MAP

	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_COMMAND(ID_PROJECT_OBJECTCONTOUR, &CDibView::OnProjectObjectcontour)
	ON_COMMAND(ID_PROJECT_OBJECTCONTOUR32776, &CDibView::OnProjectObjectcontour32776)
	ON_COMMAND(ID_PROJECT_POLYGONALAPPROXIMATION, &CDibView::OnProjectPolygonalapproximation)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDibView construction/destruction

CDibView::CDibView()
{
}

CDibView::~CDibView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDibView drawing

void CDibView::OnDraw(CDC* pDC)
{
	CDibDoc* pDoc = GetDocument();

	HDIB hDIB = pDoc->GetHDIB();
	if (hDIB != NULL)
	{
		LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);
		int cxDIB = (int) ::DIBWidth(lpDIB);         // Size of DIB - x
		int cyDIB = (int) ::DIBHeight(lpDIB);        // Size of DIB - y
		::GlobalUnlock((HGLOBAL) hDIB);
		CRect rcDIB;
		rcDIB.top = rcDIB.left = 0;
		rcDIB.right = cxDIB;
		rcDIB.bottom = cyDIB;
		CRect rcDest;
		if (pDC->IsPrinting())   // printer DC
		{
			// get size of printer page (in pixels)
			int cxPage = pDC->GetDeviceCaps(HORZRES);
			int cyPage = pDC->GetDeviceCaps(VERTRES);
			// get printer pixels per inch
			int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
			int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);

			//
			// Best Fit case -- create a rectangle which preserves
			// the DIB's aspect ratio, and fills the page horizontally.
			//
			// The formula in the "->bottom" field below calculates the Y
			// position of the printed bitmap, based on the size of the
			// bitmap, the width of the page, and the relative size of
			// a printed pixel (cyInch / cxInch).
			//
			rcDest.top = rcDest.left = 0;
			rcDest.bottom = (int)(((double)cyDIB * cxPage * cyInch)
					/ ((double)cxDIB * cxInch));
			rcDest.right = cxPage;
		}
		else   // not printer DC
		{
			rcDest = rcDIB;
		}
		::PaintDIB(pDC->m_hDC, &rcDest, pDoc->GetHDIB(),
			&rcDIB, pDoc->GetDocPalette());
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDibView printing

BOOL CDibView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CDibView commands


LRESULT CDibView::OnDoRealize(WPARAM wParam, LPARAM)
{
	ASSERT(wParam != NULL);
	CDibDoc* pDoc = GetDocument();
	if (pDoc->GetHDIB() == NULL)
		return 0L;  // must be a new document

	CPalette* pPal = pDoc->GetDocPalette();
	if (pPal != NULL)
	{
		CMainFrame* pAppFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		ASSERT_KINDOF(CMainFrame, pAppFrame);

		CClientDC appDC(pAppFrame);
		// All views but one should be a background palette.
		// wParam contains a handle to the active view, so the SelectPalette
		// bForceBackground flag is FALSE only if wParam == m_hWnd (this view)
		CPalette* oldPalette = appDC.SelectPalette(pPal, ((HWND)wParam) != m_hWnd);

		if (oldPalette != NULL)
		{
			UINT nColorsChanged = appDC.RealizePalette();
			if (nColorsChanged > 0)
				pDoc->UpdateAllViews(NULL);
			appDC.SelectPalette(oldPalette, TRUE);
		}
		else
		{
			TRACE0("\tSelectPalette failed in CDibView::OnPaletteChanged\n");
		}
	}

	return 0L;
}

void CDibView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	ASSERT(GetDocument() != NULL);

	SetScrollSizes(MM_TEXT, GetDocument()->GetDocSize());
}


void CDibView::OnActivateView(BOOL bActivate, CView* pActivateView,
					CView* pDeactiveView)
{
	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	if (bActivate)
	{
		ASSERT(pActivateView == this);
		OnDoRealize((WPARAM)m_hWnd, 0);   // same as SendMessage(WM_DOREALIZE);
	}
}

void CDibView::OnEditCopy()
{
	CDibDoc* pDoc = GetDocument();
	// Clean clipboard of contents, and copy the DIB.

	if (OpenClipboard())
	{
		BeginWaitCursor();
		EmptyClipboard();
		SetClipboardData (CF_DIB, CopyHandle((HANDLE) pDoc->GetHDIB()) );
		CloseClipboard();
		EndWaitCursor();
	}
}



void CDibView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->GetHDIB() != NULL);
}


void CDibView::OnEditPaste()
{
	HDIB hNewDIB = NULL;

	if (OpenClipboard())
	{
		BeginWaitCursor();

		hNewDIB = (HDIB) CopyHandle(::GetClipboardData(CF_DIB));

		CloseClipboard();

		if (hNewDIB != NULL)
		{
			CDibDoc* pDoc = GetDocument();
			pDoc->ReplaceHDIB(hNewDIB); // and free the old DIB
			pDoc->InitDIBData();    // set up new size & palette
			pDoc->SetModifiedFlag(TRUE);

			SetScrollSizes(MM_TEXT, pDoc->GetDocSize());
			OnDoRealize((WPARAM)m_hWnd,0);  // realize the new palette
			pDoc->UpdateAllViews(NULL);
		}
		EndWaitCursor();
	}
}


void CDibView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(::IsClipboardFormatAvailable(CF_DIB));
}

void CDibView::OnProcessingParcurgereSimpla() 
{
	// TODO: Add your command handler code here
	BEGIN_PROCESSING();

	// Makes a grayscale image by equalizing the R, G, B components from the LUT
	for (int k=0;  k < iColors ; k++)
		bmiColorsDst[k].rgbRed=bmiColorsDst[k].rgbGreen=bmiColorsDst[k].rgbBlue=k;

	
	//  Goes through the bitmap pixels and performs their negative	
	for (int i=0;i<dwHeight;i++)
		for (int j=0;j<dwWidth;j++)
		  {	
			lpDst[i*w+j]= 255 - lpSrc[i*w+j]; //makes image negative
	  }

	END_PROCESSING("Negativ imagine");
}

struct Point{
			int x;
			int y;
			};
Point contPoints[1000];
bool equalPoints(Point x,Point y)
{
	return ((x.x == y.x ) && (x.y ==y.y));
}



void CDibView::OnProjectObjectcontour()
{
}

int pointNo=0;
void CDibView::OnProjectObjectcontour32776()
{
		BEGIN_PROCESSING();

	int background=255;
	int objectPixel = 0;
	int offsetX[8]= {1,1,0,-1,-1,-1,0,1};
	int offsetY[8]= {0,1,1,1,0,-1,-1,-1};
	int chainCodes[10000];
	memset(chainCodes,0,10000*sizeof(int));
	int red= 100;
	
	bmiColorsDst[red].rgbRed= 255; bmiColorsDst[red].rgbGreen=0;bmiColorsDst[red].rgbBlue= 0;
			for (int i = dwHeight -1; i >= 0; i--)
			{
				for (int j = 1; j < dwWidth - 1; j++)
				{
					if(lpDst[i*w+j]==objectPixel)
					{
						Point p0,p1;
						p0.x=j;
						p0.y=i;
						p1.x=0;
						p1.y=0;

						Point curPoint, prevPoint;
						curPoint.x=j;
						curPoint.y=i;
						prevPoint.x=0;
						prevPoint.y=0;

						 pointNo = 1;
						
						int direction =7;
						do{
							if(direction%2 ==0)//EVEN
							{
								direction = (direction+7)%8;
							}
							else				//ODD
							{
								direction = (direction+6)%8;
							}

							Point nextPoint;
								nextPoint.x= curPoint.x +offsetX[direction];
								nextPoint.y= curPoint.y +offsetY[direction];

								while(lpDst[nextPoint.y*w+nextPoint.x]==background)
							{
								direction = (direction+1)%8;
								nextPoint.x= curPoint.x +offsetX[direction];
								nextPoint.y= curPoint.y +offsetY[direction];
							}
						
							prevPoint = curPoint;

							contPoints[pointNo] = curPoint;
							pointNo++;

							curPoint = nextPoint;
							chainCodes[pointNo]=direction;
							lpDst[nextPoint.y*w+nextPoint.x]=red;

							

							if(p1.x==0 && p1.y==0)
							{
								p1.x=p0.x+ offsetX[direction];
								p1.y =p0.y+offsetY[direction];
							}
					
						}while(pointNo<=2|| equalPoints(p0,prevPoint)==false || equalPoints(p1,curPoint)==false);
						i= -1;
						j=dwWidth ;
					}
				}

			}

	END_PROCESSING("Border  detection");
}

int findFurthest(Point point)
{
	int max = 0;
	int index = -1;
	for(int i = 1; i < pointNo; i++)
	{
		double t1 = pow((double)(point.x - contPoints[ i ].x), 2);
		double t2 = pow((double)(point.y - contPoints[ i ].y), 2);
		int dist = sqrt(t1 + t2);
		if (dist > max)
		{
			max = dist;
			index = i;
		}
	}
	return index;
}

int FarMostPointIndex(int k, int l)
{
	Point pk = contPoints[ k ];
	Point pl = contPoints[ l ];
	int maxDistance = 0;
	int index = 0;

	int a = pl.y - pk.y;
	int	b = pk.x - pl.x;
	int	c = pl.x * pk.y - pk.x * pl.y;
	int index2=0;
	if(k>l)
	{
		for (int i = k; i < pointNo; i++)
		{
			int dist = abs(a * contPoints[ i ].x + b * contPoints[ i ].y + c) / sqrt((double)(a * a + b * b));
			if (dist > maxDistance)
			{
				maxDistance = dist;
				index = i;
			}
		}
	

	}
	
	else
	{
		for (int i = k; i < l; i++)
		{
			int dist = abs(a * contPoints[ i ].x + b * contPoints[ i ].y + c) / sqrt((double)(a * a + b * b));
			if (dist > maxDistance)
			{
				maxDistance = dist;
				index = i;
			}
		}
	}
	return index;
}

int Distance(int k, int l, int index)
{
	Point pk = contPoints[ k ];
	Point pl = contPoints[ l ];

	int a = pl.y - pk.y;
	int	b = pk.x - pl.x;
	int	c = pl.x * pk.y - pk.x * pl.y;
	
	int dist = abs(a * contPoints[ index ].x + b * contPoints[ index ].y + c) / sqrt((double)(a * a + b * b));
		
	return dist;
}




int isOnLine(Point pk,Point pl,int i,int j)
{
	int a = pl.y - pk.y;
	int	b = pk.x - pl.x;
	int	c = pl.x * pk.y - pk.x * pl.y;

	if(a*j + b*i + c > -70 && a*j + b*i + c < 70  )
	{
		return 1;
	}
	else
	{
		return 0;
	}


}


void CDibView::OnProjectPolygonalapproximation()
{
	BEGIN_PROCESSING();

	ErrorSelect es;
	es.DoModal();

	int error = 20;
	switch(es.errorValue){
	    case ErrorSelect::one:
             error = 20;
	    break;
		case ErrorSelect::two:
             error = 30;
	    break;
		case ErrorSelect::three:
             error = 40;
	    break;
		case ErrorSelect::four:
			CString c = es.m_sliderValues;
			error=atoi((char*)(LPCTSTR)c);
	    break;
	}
	int red = 105;
	int blue=104;
	bmiColorsDst[red].rgbRed= 0; bmiColorsDst[red].rgbGreen=255;bmiColorsDst[red].rgbBlue= 0;


	long *a,  *b;
	a=(long*)malloc(sizeof(long) * w*dwHeight);
	b=(long*)malloc(sizeof(long) * w*dwHeight);
	/*long *a = new long[w*dwHeight];
	long *b = new long[w*dwHeight];*/
	long lengthA = 0;
	long lengthB = 0;
		
	a[ 0 ] = findFurthest( contPoints[ 1 ] );
	lengthA++;
	a[ 1 ] = 1;

	b[ 0 ]= a[ 0 ];
		
	Point pk, pl;
		

	
	int nr=0;
	
	while (lengthA >= 0)
	{
		if(nr>500) break;
		long k = a[ lengthA ];
		long l = b[ lengthB ];

		pk = contPoints[k ];
		pl = contPoints[ l ];

		long m = FarMostPointIndex( k, l );
		double d = Distance(k ,l , m); 
		if ( d> error && m!=0)
		{
			lengthA++;
			a[ lengthA ] = m;
		}
		else
		{
			
			lengthB++;
			b[ lengthB ] = m;
			lengthB++;
			b[ lengthB ] = a[lengthA];
			lengthA--;
		}	
	}
	int no = 0;
	Point  curPoint,nextPoint;	
	memset(lpDst,255,w*dwHeight);
    for(int z=0; z < lengthB-1 ;z++)
	{	
		curPoint.x = contPoints[b[z]].x;
		curPoint.y = contPoints[b[z]].y;
		if(z==lengthB-2)
		{
			nextPoint.x = contPoints[b[0]].x;
			nextPoint.y = contPoints[b[0]].y;
		}
		else
		{
			nextPoint.x = contPoints[b[z+1]].x;
			nextPoint.y = contPoints[b[z+1]].y;
		}
		

		
		//coloreaza punctele
		lpDst[curPoint.y*w+curPoint.x]=105;	
		curPoint.y++;curPoint.x++;
		lpDst[curPoint.y*w+curPoint.x]=105;
		curPoint.y++;curPoint.x++;
		lpDst[curPoint.y*w+curPoint.x]=105;	
		
		
		lpDst[nextPoint.y*w+nextPoint.x]=105;
		nextPoint.y++;nextPoint.x++;
		lpDst[nextPoint.y*w+nextPoint.x]=105;
		nextPoint.y++;nextPoint.x++;
		lpDst[nextPoint.y*w+nextPoint.x]=105;
		
				
		

	    int background=0;
		for (int i = dwHeight; i >= 0; i--)
		  {
				for (int j = 0; j < dwWidth ; j++)
				{
							if(lpSrc[i*w+j] == background && isOnLine(curPoint,nextPoint,i,j))
					{
						lpDst[i*w+j] = blue;
						
					}
				}
		  }


	}
	bmiColorsDst[blue].rgbRed= 0; bmiColorsDst[blue].rgbGreen=0;bmiColorsDst[blue].rgbBlue= 255;

	END_PROCESSING("Pol Approx");
}
