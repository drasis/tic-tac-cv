#ifndef __BOARD_H__
#define __BOARD_H__

#include "plotter.h"

typedef enum _WinLine
{
    WIN_COL_0 = 0,
    WIN_COL_1,
    WIN_COL_2,

    WIN_ROW_0,
    WIN_ROW_1,
    WIN_ROW_2,

    WIN_DIAG_0,
    WIN_DIAG_1,
} WinLine;

/**
 * @brief      Instructs the plotter to draw the tic-tac-toe board.
 *
 * @return     Whether the commands were sucessfully flushed.
 */
bool drawBoard(void);
/**
 * @brief      Instructs the plotter to draw an 'X' on the specified row/column.
 *
 * @param[in]  row   The row
 * @param[in]  col   The column
 *
 * @return     Whether the plotter's buffer has sucessfully been flushed 
 * and the input row and column are valid
 */
bool drawX(int row, int col);
/**
 * @brief      Instructs the plotter to draw a line across the winning combination
 *
 * @param[in]  line  The winning line
 *
 * @return     Whether the plotter's buffer has sucessfully been flushed
 */
bool drawWin(WinLine line);
/**
 * @brief      Instructs the plotter to reveal the page by lifting the pen up, 
 * and moving it to co-ordinate (0, 0). 
 *
 * @return     Whether the plotter's buffer has been flushed (it always has, 
 * if the function is returning something)
 */
bool revealPage(void);
/**
 * @brief      Instructs the plotter to select a pen
 *
 * @param[in]  pen   The ID of the pen (1 - 8)
 *
 * @return     Whether the plotter's buffer has sucessfully been flushed
 */
bool selectPen(int pen);

#endif
