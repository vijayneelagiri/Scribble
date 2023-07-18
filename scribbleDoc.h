
// scribbleDoc.h : interface of the CscribbleDoc class
//


#pragma once
/////////////////////////////////////////////////////
// class CStroke
// A stroke is a series of connected points in the Scribble drawing.
// A Scribble document may have multiple strokes.
class CStroke : public CObject
{
public:
	CStroke(UINT nPenWidth);

protected:
	
public:
	
protected:
	CStroke();
	DECLARE_SERIAL(CStroke)
	// Attributes
	
		
		
protected:
	UINT m_nPenWidth; // One width applies to entire stroke
public:
	CArray<CPoint, CPoint> m_pointArray; // Series of connected // points
	// Operations
protected:
	CRect m_rectBounding; // smallest rect that surrounds all
	// of the points in the stroke
public:
	CRect& GetBoundingRect() { return m_rectBounding; }
public:
	BOOL DrawStroke(CDC* pDC);
public:
	virtual void Serialize(CArchive& ar);
	void FinishStroke();
};



class CscribbleDoc : public CDocument
{
protected: // create from serialization only
	CscribbleDoc() noexcept;
	DECLARE_DYNCREATE(CscribbleDoc)
	BOOL m_bThickPen; // Thick currently selected or not
	UINT m_nThinWidth; // Current definition of thin
	UINT m_nThickWidth; // Current definition of thick 

// Attributes
public:

// Operations
public:
	CSize GetDocSize() { return m_sizeDoc; }

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	CTypedPtrList<CObList, CStroke*> m_strokeList;
	CPen* GetCurrentPen() { return &m_penCur; }
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CscribbleDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CSize m_sizeDoc;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
	unsigned int m_nPenWidth;
	

	CPen m_penCur;
public:
	CStroke* NewStroke();
protected:
	void InitDocument();
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	afx_msg void OnEditClearAll();
	afx_msg void OnPenThickOrThin();
protected:
	void ReplacePen();
public:
	afx_msg void OnUpdateEditClearAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePenThickOrThin(CCmdUI* pCmdUI);
	afx_msg void OnPenWidths();
};
