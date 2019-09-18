#ifndef __BOARD_H__
#define __BOARD_H__

#include "plotter.h"

typedef enum _WinLine
{
    WIN_COL_0,
    WIN_COL_1,
    WIN_COL_2,

    WIN_ROW_0,
    WIN_ROW_1,
    WIN_ROW_2,

    WIN_DIAG_0,
    WIN_DIAG_1,
} WinLine;

bool drawBoard(void);
bool drawX(int row, int col);
bool drawWin(WinLine line);
bool revealPage(void);
bool selectPen(int pen);

#endif
