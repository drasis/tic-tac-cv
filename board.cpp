#include "board.h"
#include <unistd.h>

// size of the edge of the board in HPGL co-ordinates
#define BOARD_BOX_LEN (1950)

/**
 * @brief container for the x,y paper co-ordiantes
 */
typedef struct _PaperCoord
{
    int x;
    int y;
} PaperCoord;


/**
 * @brief translation table from board coords (ie, (0,0) - (2,2))
 * to paper coordinates the paper coordinate is the top left of that box
 */
static const PaperCoord boardToPaper[3][3] = {
    { {.x = 6750,  .y = 900}, {.x = 4800,  .y = 900}, {.x = 2850,  .y = 900}, }, 
    { {.x = 6750, .y = 2850}, {.x = 4800, .y = 2850}, {.x = 2850, .y = 2850}, },
    { {.x = 6750, .y = 4800}, {.x = 4800, .y = 4800}, {.x = 2850, .y = 4800}, },
};

static Plotter plt;

bool drawBoard(void) {
    plt.selectPen(1);
    plt.drawLine(900, 2850, 6750, 2850);
    plt.drawLine(900, 4800, 6750, 4800);
    plt.drawLine(2850, 6750, 2850, 900);
    plt.drawLine(4800, 900, 4800, 6750);
    return plt.flush();
}

bool revealPage(void) {
    plt.setPenSpeed(20);
    plt.penUp();
    plt.setPenPos(0, 0);
    plt.setPenSpeed(5);
    plt.flush();
    sleep(2);
    return true;
}

bool selectPen(int pen) {
    plt.selectPen(pen);
    return plt.flush();
}

bool drawX(int row, int col) {
    if (row < 0 || row > 2 || col < 0 || col > 2) {
        return false;
    }
    static const int margin = 100;
    PaperCoord topLeft = boardToPaper[row][col];
    PaperCoord bottomRight = {.x = topLeft.x - (BOARD_BOX_LEN - margin), .y = topLeft.y + BOARD_BOX_LEN - margin};

    topLeft.x -= margin;
    topLeft.y += margin;

    // draw top left to bottom right diagonal
    plt.drawLine(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
    
    // draw top right to bottom left diagonal
    plt.drawLine(bottomRight.x, topLeft.y, topLeft.x, bottomRight.y);
    return plt.flush();
}

bool drawWin(WinLine line) {
    PaperCoord top;
    switch (line) {
        case WIN_COL_0:
            top = boardToPaper[0][0];
            top.x -= (BOARD_BOX_LEN / 2);
            top.y += (BOARD_BOX_LEN / 2);
            plt.drawLine(top.x, top.y, top.x, top.y + (2 * BOARD_BOX_LEN));
            break;
        case WIN_COL_1:
            top = boardToPaper[0][1];
            top.x -= (BOARD_BOX_LEN / 2);
            top.y += (BOARD_BOX_LEN / 2);
            plt.drawLine(top.x, top.y, top.x, top.y + (2 * BOARD_BOX_LEN));
            break;
        case WIN_COL_2:
            top = boardToPaper[0][2];
            top.x -= (BOARD_BOX_LEN / 2);
            top.y += (BOARD_BOX_LEN / 2);
            plt.drawLine(top.x, top.y, top.x, top.y + (2 * BOARD_BOX_LEN));
            break;
        case WIN_ROW_0:
            top = boardToPaper[0][0];
            top.x -= (BOARD_BOX_LEN / 2);
            top.y += (BOARD_BOX_LEN / 2);
            plt.drawLine(top.x, top.y, top.x - (2 * BOARD_BOX_LEN), top.y);
            break;
        case WIN_ROW_1:
            top = boardToPaper[1][0];
            top.x -= (BOARD_BOX_LEN / 2);
            top.y += (BOARD_BOX_LEN / 2);
            plt.drawLine(top.x, top.y, top.x - (2 * BOARD_BOX_LEN), top.y);
            break;
        case WIN_ROW_2:
            top = boardToPaper[2][0];
            top.x -= (BOARD_BOX_LEN / 2);
            top.y += (BOARD_BOX_LEN / 2);
            plt.drawLine(top.x, top.y, top.x - (2 * BOARD_BOX_LEN), top.y);
            break;
        case WIN_DIAG_0:
            top = boardToPaper[0][0];
            top.x -= (BOARD_BOX_LEN / 2);
            top.y += (BOARD_BOX_LEN / 2);
            plt.drawLine(top.x, top.y, top.x - (2 * BOARD_BOX_LEN), top.y + (2 * BOARD_BOX_LEN));
            break;
        case WIN_DIAG_1:
            top = boardToPaper[0][2];
            top.x -= (BOARD_BOX_LEN / 2);
            top.y += (BOARD_BOX_LEN / 2);
            plt.drawLine(top.x, top.y, top.x + (2 * BOARD_BOX_LEN), top.y + (2 * BOARD_BOX_LEN));
            break;
        default:
            return false;
    }
    return plt.flush();
}

#if 0
int main(void) {
    selectPen(1);
    drawBoard();
    selectPen(2);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            drawX(i, j);
        }
    }
    selectPen(3);
    drawWin(WIN_DIAG_0);
    drawWin(WIN_DIAG_1);
    drawWin(WIN_ROW_0);
    drawWin(WIN_ROW_1);
    drawWin(WIN_ROW_2);
    drawWin(WIN_COL_0);
    drawWin(WIN_COL_1);
    drawWin(WIN_COL_2);
    revealPage();
    plt.flush();
    return 0;
}
#endif
