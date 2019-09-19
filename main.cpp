#include "thresh.h"
#include "board.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

void initGame(cv::Mat& homography, cv::Rect& boardBounds,cv::VideoCapture& cap) {
    // draw tictactoe board
    selectPen(1);
    drawBoard();

    selectPen(2);
    // send pen to 0,0
    revealPage();
    cv::Mat frame;
    while(!frame.empty()) {
        cap.read(frame);
    }
    // calculate homography matrix
    findHomography(frame, homography);
    // find bounds of tictactoe board
    findBoardBounds(cap, homography, boardBounds);
}

// wrapper for lua calls
void getPlotterAI(BoxState board[9], int *row, int *col, lua_State *L) {
    int ret;
    int isnum;
    lua_getglobal(L, "getPlotterMove"); 
    for (int i = 0; i < 9; i++) {
        lua_pushinteger(L, board[i]);
    }
    if (lua_pcall(L, 9, 1, 0) != LUA_OK) {
        throw std::runtime_error("Lua is NOT ok");
    }

    ret = lua_tointegerx(L, -1, &isnum);
    if (!isnum || ret < 0 || ret > 8) {
        throw std::runtime_error("Lua is NOT OK");
    }
    lua_pop(L, 1);
    board[ret] = BOX_X;
    *row = ret / 3;
    *col = ret % 3;
}

bool someoneHasWon(BoxState board[9]) {
    for (int i = 0; i < 3; ++i) {
        if (board[i] == board[i + 3] == board[i + 6] && board[i] != BOX_EMPTY) {
            return true;
        }
        if (board[i * 3] == board[i * 3 + 1] == board[i * 3 + 2] 
            && board[i * 3] != BOX_EMPTY) {
            return true;
        }
    }
    return (board[0] == board[4] == board[8] && board[0] != BOX_EMPTY)
            || (board[2] == board[4] == board[6] && board[2] != BOX_EMPTY);
}
void drawWinner(BoxState board[9]) {
    // WinLine[8] = {  }
    for (int i = 0; i < 3; ++i) {
        if ((board[i] == board[i + 3] == board[i + 6] && board[i] != BOX_EMPTY)) {
            drawWin((WinLine)(i));
            return;
        }
        if (board[i * 3] == board[i * 3 + 1] == board[i * 3 + 2] 
            && board[i * 3] != BOX_EMPTY) {
            drawWin((WinLine)(i + 3));
            return;
        }
    }
    if (board[0] == board[4] == board[8] && board[0] != BOX_EMPTY) {
        drawWin(WIN_DIAG_0);
        return;
    }
    if (board[2] == board[4] == board[6] && board[2] != BOX_EMPTY) {
        drawWin(WIN_DIAG_1);
        return;
    }
}

bool gameLoop(const cv::Mat& homography, const cv::Rect& boardBounds,
    lua_State *L, cv::VideoCapture& cap) {
    bool gameOver = false;
    BoxState board[9];
    cv::Mat frame;
    cv::Mat baseline;
    if (!cap.isOpened()) {
        return false;
    }

    // initialize baseline
    cap.read(frame);
    cv::warpPerspective(frame, baseline, homography, frame.size());
    while (!gameOver) {
        cap.read(frame);
        // crop frame to paper
        cv::warpPerspective(frame, frame, homography, frame.size());
        if (handInFrame(baseline, frame)) {
            continue;
        } else if (checkForO(frame, boardBounds, board)) {
            // Plotter's turn.
            int row = -1;
            int col = -1;
            getPlotterAI(board, &row, &col, L);
            drawX(row, col);
            gameOver = someoneHasWon(board);
            revealPage();
            baseline = frame.clone();
        }
    }
    drawWinner(board);// THIS NEEDS 2 B IMPLEMENTED LMAO
    // cap.release();
}

int main(void) {
    bool playAgain = true;
    cv::VideoCapture cap(2);
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    while (playAgain) {
        cv::Mat homography;
        cv::Rect boardBounds;
        initGame(homography, boardBounds, cap);
        gameLoop(homography, boardBounds, L, cap);
        // get input from player
    }
    cap.release();
    std::cout << "thanks 4 playing :)\n";
  
    return 0;
}
