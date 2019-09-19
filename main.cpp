#include "thresh.h"
#include "board.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

void initGame(cv::Mat& homography, cv::Rect& boardBounds) {
    // draw tictactoe board
    selectPen(1);
    drawBoard();

    selectPen(2);
    // send pen to 0,0
    revealPage();

    // initialize homography matrix
    // calculate homography matrix
    // find bounds of tictactoe board
    // findBoardBounds()
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
    if (!isnum) {
        throw std::runtime_error("Lua is NOT OK");
    }
    lua_pop(L, 1);

    *row = ret / 3;
    *col = ret % 3;
}

bool gameLoop(const cv::Mat& homography, const cv::Rect& boardBounds, lua_State *L) {
    BoxState board[9];
    cv::VideoCapture cap(2);
    cv::Mat frame;
    cv::Mat baseline;
    if (!cap.isOpened()) {
        return false;
    }

    // initialize baseline
    cap.read(frame);
    cv::warpPerspective(frame, baseline, homography);
    while (!gameOver) {
        cap.read(frame);
        // crop frame to paper
        cv::warpPerspective(frame, frame, homography);
        if (handInFrame(baseline, frame)) {
            continue;
        } else if (checkForO(baseline, frame, boardBounds, board)) {
            // Plotter's turn.
            int row = -1
            int col = -1;
            getPlotterAI(board, &row, &col, L);
            drawX(row, col);
            gameOver = someoneHasWon(board);
            revealPage();
            baseline = frame.clone();
        }
    }
    drawWinner(board);
    cap.close();
}

int main(void) {
    bool playAgain = true;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    while (playAgain) {
        cv::Mat homography;
        cv::Rect boardBounds;
        initGame(homography, boardBounds);
        gameLoop(homography, boardBounds, L);
        // get input from player
    }
  
    return 0;
}
