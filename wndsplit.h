// WndSplit.h: interface for the CWndSplit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDSPLIT_H__0145C759_317F_49C8_9016_A07CA5FFAC9D__INCLUDED_)
#define AFX_WNDSPLIT_H__0145C759_317F_49C8_9016_A07CA5FFAC9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wnd.h"
#include "closure.h"

class CWndSplit : public CWnd{
public:
	enum{
		EVENT_POS_CHANGED = 1,
	};
public:
    HBRUSH m_hbrDithered; // Brush for the splitter bar
    HBITMAP m_hbm;      // Used to create m_hbrDithered
    int m_nWidth;       // Width of the splitter bar
    int m_nBarPos;      // X or Y position of the splitter bar
    int m_nOldBarPos;   // Previous bar position, updated during move
    int m_nBarpreMove;  // Previous bar position, not updated during move
    BOOL m_bCapture;    // Mouse capture flag
    BOOL m_bVertical;   // Vertical or horizontal spliter bar
    HWND pane0;
    HWND pane1;
    RECT border_rect;
	CClosure m_Callback;
public:
	CClosure* Callback();
    int GetBarWidth();
    int GetPos();
    int SetBarPos(int pos);
    int RecalcLayout(RECT r);
    int DrawBar(int Pos);
    int Registe();
    CWndSplit();
    virtual ~CWndSplit();
    int Init(int vert);
    int Destroy();
    int InitBasic();
    int OnLButtonDown(WPARAM wparam,LPARAM lparam);
    int OnLButtonUp(WPARAM wparam,LPARAM lparam);
    int OnMouseMove(WPARAM wparam,LPARAM lparam);
    int OnPaint(WPARAM wparam,LPARAM lparam);
};

#endif // !defined(AFX_WNDSPLIT_H__0145C759_317F_49C8_9016_A07CA5FFAC9D__INCLUDED_)
