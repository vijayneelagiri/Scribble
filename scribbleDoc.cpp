
// scribbleDoc.cpp : implementation of the CscribbleDoc class
//

#include "pch.h"
#include "framework.h"
#include "CPenWidthsDlg.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "scribble.h"
#endif

#include "scribbleDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CscribbleDoc

IMPLEMENT_DYNCREATE(CscribbleDoc, CDocument)

BEGIN_MESSAGE_MAP(CscribbleDoc, CDocument)
	ON_COMMAND(ID_EDIT_CLEAR_ALL, &CscribbleDoc::OnEditClearAll)
	ON_COMMAND(ID_PEN_THICK_OR_THIN, &CscribbleDoc::OnPenThickOrThin)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_ALL, &CscribbleDoc::OnUpdateEditClearAll)
	ON_UPDATE_COMMAND_UI(ID_PEN_THICK_OR_THIN, &CscribbleDoc::OnUpdatePenThickOrThin)
	ON_COMMAND(ID_PEN_WIDTHS, &CscribbleDoc::OnPenWidths)
END_MESSAGE_MAP()


// CscribbleDoc construction/destruction

CscribbleDoc::CscribbleDoc() noexcept
{
	// TODO: add one-time construction code here

}

CscribbleDoc::~CscribbleDoc()
{
}

BOOL CscribbleDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	
	InitDocument();

	return TRUE;
}




// CscribbleDoc serialization

void CscribbleDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_sizeDoc;

		
		// TODO: add storing code here
	}
	else
	{
		ar >> m_sizeDoc;
		// TODO: add loading code here
	}
	m_strokeList.Serialize(ar);
}
BOOL CStroke::DrawStroke(CDC* pDC)
{
	CPen penStroke;
	if (!penStroke.CreatePen(PS_SOLID, m_nPenWidth, RGB(0, 0, 0)))
		return FALSE;
	CPen* pOldPen = pDC->SelectObject(&penStroke);
	pDC->MoveTo(m_pointArray[0]);
	for (int i = 1; i < m_pointArray.GetSize(); i++)
	{
		pDC->LineTo(m_pointArray[i]);
	}
	pDC->SelectObject(pOldPen);
	return TRUE;
}


#ifdef SHARED_HANDLERS

// Support for thumbnails
void CscribbleDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CscribbleDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CscribbleDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CscribbleDoc diagnostics

#ifdef _DEBUG
void CscribbleDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CscribbleDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CscribbleDoc commands


CStroke* CscribbleDoc::NewStroke()
{
	CStroke* pStrokeItem = new CStroke(m_nPenWidth);
	m_strokeList.AddTail(pStrokeItem);
	SetModifiedFlag(); // Mark document as modified
	// to confirm File Close.
	return pStrokeItem;

	// TODO: Add your implementation code here.
	//return nullptr;
}



void CscribbleDoc::InitDocument()
{
	m_bThickPen = FALSE;
	m_nThinWidth = 2; // Default thin pen is 2 pixels wide
	m_nThickWidth = 5; // Default thick pen is 5 pixels wide
	ReplacePen();
	// Default document size is 800 x 900 screen pixels.
	m_sizeDoc = CSize(800, 900);
}


BOOL CscribbleDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	InitDocument();

	// TODO:  Add your specialized creation code here

	return TRUE;
}


void CscribbleDoc::DeleteContents()
{
	while (!m_strokeList.IsEmpty())
	{
		delete m_strokeList.RemoveHead();
	}


	CDocument::DeleteContents();
}
IMPLEMENT_SERIAL(CStroke, CObject, 2)
CStroke::CStroke()
{
	// This empty constructor should be used by
	// the application framework for serialization only
}
CStroke::CStroke(UINT nPenWidth)
{
	m_nPenWidth = nPenWidth;
	m_rectBounding.SetRectEmpty();
}
void CStroke::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_rectBounding;
		ar << (WORD)m_nPenWidth;
		m_pointArray.Serialize(ar);
	}
	else
	{
		ar >> m_rectBounding;
		WORD w;
		ar >> w;
		m_nPenWidth = w;
		m_pointArray.Serialize(ar);
	}

	//ar >> m_rectBounding;



}



void CscribbleDoc::OnEditClearAll()
{
	DeleteContents();
	SetModifiedFlag();
	UpdateAllViews(NULL);
	// TODO: Add your command handler code here
}


void CscribbleDoc::OnPenThickOrThin()
{
	// TODO: Add your command handler code here
	// Toggle the state of the pen between thin and thick.
	m_bThickPen = !m_bThickPen;
	// Change the current pen to reflect the new width.
	ReplacePen();
}


void CscribbleDoc::ReplacePen()
{

	m_nPenWidth = m_bThickPen ? m_nThickWidth : m_nThinWidth;
	// Change the current pen to reflect the new width.
	m_penCur.DeleteObject();
	m_penCur.CreatePen(PS_SOLID, m_nPenWidth, RGB(0, 0, 0));
	// TODO: Add your implementation code here.
}


void CscribbleDoc::OnUpdateEditClearAll(CCmdUI* pCmdUI)
{
	// Enable the user-interface object (menu item or tool-
// bar button) if the document is non-empty, i.e., has
// at least one stroke.
	pCmdUI->Enable(!m_strokeList.IsEmpty());
	// TODO: Add your command update UI handler code here
}


void CscribbleDoc::OnUpdatePenThickOrThin(CCmdUI* pCmdUI)
{
	// Add check mark to Pen Thick Line menu item if the current
// pen width is "thick."
	pCmdUI->SetCheck(m_bThickPen);
	// TODO: Add your command update UI handler code here
}


void CscribbleDoc::OnPenWidths()
{
	CPenWidthsDlg dlg;
	// Initialize dialog data
	dlg.m_nThinWidth = m_nThinWidth;
	dlg.m_nThickWidth = m_nThickWidth;
	// Invoke the dialog box
	if (dlg.DoModal() == IDOK)
	{
		// retrieve the dialog data
		m_nThinWidth = dlg.m_nThinWidth;
		m_nThickWidth = dlg.m_nThickWidth;
		// Update the pen used by views when drawing new strokes
		// to reflect the new pen widths for "thick" and "thin".
		ReplacePen();
	}

	// TODO: Add your command handler code here
}


void CStroke::FinishStroke()
{
	if (m_pointArray.GetSize() == 0)
	{
		m_rectBounding.SetRectEmpty();
		return;
	}
	CPoint pt = m_pointArray[0];
	m_rectBounding = CRect(pt.x, pt.y, pt.x, pt.y);
	for (int i = 1; i < m_pointArray.GetSize(); i++)
	{
		// If the point lies outside of the accumulated bounding
		// rectangle, then inflate the bounding rect to include it.
		pt = m_pointArray[i];
		m_rectBounding.left = min(m_rectBounding.left, pt.x);
		m_rectBounding.right = max(m_rectBounding.right, pt.x);
		m_rectBounding.top = min(m_rectBounding.top, pt.y);
		m_rectBounding.bottom = max(m_rectBounding.bottom, pt.y);
	}
	// Add the pen width to the bounding rectangle. This is needed
	// to account for the width of the stroke when invalidating
	// the screen.
	m_rectBounding.InflateRect(CSize(m_nPenWidth, m_nPenWidth));
	return;
	// TODO: Add your implementation code here.
}
