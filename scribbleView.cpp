
// scribbleView.cpp : implementation of the CscribbleView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "scribble.h"
#endif

#include "scribbleDoc.h"
#include "scribbleView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CscribbleView

IMPLEMENT_DYNCREATE(CscribbleView, CScrollView)

BEGIN_MESSAGE_MAP(CscribbleView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CscribbleView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
//	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_XBUTTONUP()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CscribbleView construction/destruction

CscribbleView::CscribbleView() noexcept
{
	// TODO: add construction code here

}

CscribbleView::~CscribbleView()
{
}

BOOL CscribbleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CscribbleView drawing

void CscribbleView::OnDraw(CDC* pDC)
{
	
	CscribbleDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// Get the invalidated rectangle of the view, or in the case
// of printing, the clipping region of the printer DC.
	CRect rectClip;
	CRect rectStroke;
	pDC->GetClipBox(&rectClip);
	//Note: CScrollView::OnPaint() will have already adjusted the
	//viewpoint origin before calling OnDraw(), to reflect the
	//currently scrolled position.

	// The view delegates the drawing of individual strokes to
// CStroke::DrawStroke( ).
	CTypedPtrList<CObList, CStroke*>& strokeList =
		pDoc->m_strokeList;
	POSITION pos = strokeList.GetHeadPosition();
	while (pos != NULL)
	{
		CStroke* pStroke = strokeList.GetNext(pos);
		rectStroke = pStroke->GetBoundingRect();
		if (!rectStroke.IntersectRect(&rectStroke, &rectClip))
			continue;
		pStroke->DrawStroke(pDC);

	}


	if (!pDoc)
	
		return ;

	// TODO: add draw code for native data here
}


// CscribbleView printing


void CscribbleView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CscribbleView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CscribbleView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CscribbleView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

//void CscribbleView::OnRButtonUp(UINT /* nFlags */, CPoint point)
//{
//	ClientToScreen(&point);
//	OnContextMenu(this, point);
//}

void CscribbleView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CscribbleView diagnostics

#ifdef _DEBUG
void CscribbleView::AssertValid() const
{
	CView::AssertValid();
}

void CscribbleView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CscribbleDoc* CscribbleView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CscribbleDoc)));
	return (CscribbleDoc*)m_pDocument;
}
#endif //_DEBUG


// CscribbleView message handlers


void CscribbleView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// CScrollView changes the viewport origin and mapping mode.
// It's necessary to convert the point from device coordinates
// to logical coordinates, such as are stored in the document.
	CClientDC dc(this);
	OnPrepareDC(&dc); // set up mapping mode and viewport origin
	dc.DPtoLP(&point);


	// Pressing the mouse button in the view window
// starts a new stroke.
	m_pStrokeCur = GetDocument()->NewStroke();
	// Add first point to the new stroke
	m_pStrokeCur->m_pointArray.Add(point);
	SetCapture(); // Capture the mouse until button up
	m_ptPrev = point; // Serves as the MoveTo( ) anchor point
	// for the LineTo() the next point, as
	// the user drags the mouse
	return;
	// TODO: Add your message handler code here and/or call default

	CView::OnLButtonDown(nFlags, point);
}


void CscribbleView::OnMouseMove(UINT nFlags, CPoint point)
{
	// Mouse movement is interesting in the Scribble application
// only if the user is currently drawing a new stroke by
// dragging the captured mouse.
	if (GetCapture() != this)
		return; // If this window (view) didn't capture the
	// mouse, the user isn't drawing in this window.
	CClientDC dc(this);

	// CScrollView changes the viewport origin and mapping mode.
// It's necessary to convert the point from device coordinates
// to logical coordinates, such as are stored in the document.
	OnPrepareDC(&dc); // set up mapping mode and viewport origin
	dc.DPtoLP(&point);

	m_pStrokeCur->m_pointArray.Add(point);
	// Draw a line from the previous detected point in the mouse
	// drag to the current point.
	CPen* pOldPen =
		dc.SelectObject(GetDocument()->GetCurrentPen());
	dc.MoveTo(m_ptPrev);
	dc.LineTo(point);
	dc.SelectObject(pOldPen);
	m_ptPrev = point;
	return;
	// TODO: Add your message handler code here and/or call default

	CView::OnMouseMove(nFlags, point);
}





void CscribbleView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// Mouse button up is interesting in the Scribble
// application only if the user is currently drawing a new
// stroke by dragging the captured mouse.
	if (GetCapture() != this)
		return; // If this window (view) didn't capture the
	// mouse, the user isn't drawing in this window.
	CscribbleDoc* pDoc = GetDocument();
	CClientDC dc(this);
	// CScrollView changes the viewport origin and mapping mode.
// It's necessary to convert the point from device coordinates
// to logical coordinates, such as are stored in the document.
	OnPrepareDC(&dc); // set up mapping mode and viewport origin
	dc.DPtoLP(&point);
	CPen* pOldPen = dc.SelectObject(pDoc->GetCurrentPen());
	dc.MoveTo(m_ptPrev);
	dc.LineTo(point);
	dc.SelectObject(pOldPen);
	m_pStrokeCur->m_pointArray.Add(point);
	// Tell the stroke item that we're done adding points to it.
// This is so it can finish computing its bounding rectangle.
	m_pStrokeCur->FinishStroke();
	// Tell the other views that this stroke has been added
	// so that they can invalidate this stroke's area in their
// client area.
	pDoc->UpdateAllViews(this, 0L, m_pStrokeCur);
	ReleaseCapture(); // Release the mouse capture established
	// at the beginning of the mouse drag.
	return;
	// TODO: Add your message handler code here and/or call default

	//CView::OnLButtonUp(nFlags, point);
}


void CscribbleView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* pHint)
{

	// The document has informed this view that some data has changed.
	if (pHint != NULL)
	{
		if (pHint->IsKindOf(RUNTIME_CLASS(CStroke)))
		{
			// The hint is that a stroke has been added (or changed).
			// So, invalidate its rectangle.
			CStroke* pStroke = (CStroke*)pHint;
			CClientDC dc(this);
			OnPrepareDC(&dc);
			CRect rectInvalid = pStroke->GetBoundingRect();
			dc.LPtoDP(&rectInvalid);
			InvalidateRect(&rectInvalid);
			return;
		}
	}
	// We can't interpret the hint, so assume that anything might
	// have been updated.
	Invalidate();
	return;
	// TODO: Add your specialized code here and/or call the base class
}


void CscribbleView::OnInitialUpdate()
{
	SetScrollSizes(MM_TEXT, GetDocument()->GetDocSize());
	CScrollView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
}
