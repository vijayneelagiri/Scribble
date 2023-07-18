
// scribbleView.h : interface of the CscribbleView class
//

#pragma once


class CscribbleView : public CScrollView
{
protected: // create from serialization only
	CscribbleView() noexcept;
	DECLARE_DYNCREATE(CscribbleView)

// Attributes
public:
	CscribbleDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	CStroke* m_pStrokeCur; // The stroke in progress
	CPoint m_ptPrev; 
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CscribbleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
//	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // debug version in scribbleView.cpp
inline CscribbleDoc* CscribbleView::GetDocument() const
   { return reinterpret_cast<CscribbleDoc*>(m_pDocument); }
#endif

