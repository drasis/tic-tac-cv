#include "thresh.h"
#include "board.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "luaconf.h"

void initGame(cv::Mat& homography, cv::Rect& boardBounds,cv::VideoCapture& cap) {
    // draw tictactoe board
    //selectPen(1);
    std::cout << "drawing board\n:";
    //drawBoard();

    selectPen(2);
    // send pen to 0,0
    revealPage();

    cv::Mat frame;
    while(frame.empty()) {
        cap.read(frame);
    }
    // calculate homography matrix
    findHomography(frame, homography);
    cv::Mat frameWarped;
    cv::warpPerspective(frame, frameWarped, homography, frame.size());
    std::cout << "out of homography:\n" << homography << std::endl;
    //cv::imshow("out of homography", frameWarped);
    //cv::waitKey(0);

    // find bounds of tictactoe board
    findBoardBounds(cap, homography, boardBounds);
    cv::rectangle(frameWarped, boardBounds, cv::Scalar(0,0,255));
    //cv::imshow("with rectangle", frameWarped);
    //cv::waitKey(0);
    std::cout << "init done" << std::endl;
}

// wrapper for lua calls
void getPlotterAI(BoxState board[9], int *row, int *col, lua_State *L) {
    int ret;
    int isnum;
    lua_getglobal(L, "getPlotterMove"); 
    for (int i = 0; i < 9; i++) {
        lua_pushinteger(L, (int)board[i]);
    }
    if (lua_pcall(L, 9, 1, 0) != 0) {
        std::cout << lua_tostring(L, -1) << std::endl;
        throw std::runtime_error("Lua pcall NOT ok");
    }

    ret = lua_tointeger(L, -1);
    std::cout << "got " << ret << " from lua\n";
    if (ret < 0 || ret > 8) {
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
    BoxState board[9] = {BOX_EMPTY, BOX_EMPTY, BOX_EMPTY, BOX_EMPTY, BOX_EMPTY, BOX_EMPTY, BOX_EMPTY, BOX_EMPTY, BOX_EMPTY};
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
            std::cout << "hand found in frame\n";
            continue;
        } else if (checkForO(frame, boardBounds, board)) {
            std::cout << "Found O in frame. Plotter's turn\n";
            // Plotter's turn.
            int row = -1;
            int col = -1;
            getPlotterAI(board, &row, &col, L);
            std::cout << "Plotter plays X at (" << row << ", " << col << ")\n"; 
            std::cout << "---------Board State ----------------\n";
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int curr = (i*3)+j;
                    switch(board[curr]) {
                        case BOX_EMPTY:
                            std::cout << ". ";
                            break;
                        case BOX_O:
                            std::cout << "O ";
                            break;
                        case BOX_X:
                            std::cout << "X ";
                            break;
                    }
                }
                std::cout << std::endl;
            }
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
    if (!cap.isOpened()) {
        return -1;
    }
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    int status = luaL_dofile(L, "../minimax.lua");
    if (status) {
        std::cout << "couldn't open minimax.lua\n";
        return -1;
    }
    std::cout << L << std::endl;
    std::cout << "opened lua file\n";
    while (playAgain) {
        cv::Mat homography;
        cv::Rect boardBounds;
        initGame(homography, boardBounds, cap);
        std::cout << "init game done\n";
        gameLoop(homography, boardBounds, L, cap);
        std::cout << "game done\n";
        playAgain = false;
        // get input from player
    }
    cap.release();
    std::cout << "thanks 4 playing :)\n";
  
    return 0;
}
