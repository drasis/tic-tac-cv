#include "thresh.h"
#include "board.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "luaconf.h"


/**
 * @brief      Prints a board.
 * @param[in]  board represents the state of the board from 
 * left-to-right, top-to-bottom
 */
void printBoard(BoxState board[9]) {
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
}

/**
 * @brief
 * Here an instance of the tic-tac-toe game initialized:
 * The board is drawn, a homography matrix is found (from the video frame to
 * the paper bounds), and the pixel coordinates of the bounds of the 
 * tic-tac-toe board are found
 * @param[out] homography a 3x3 matrix that represents the transformation from 
 *      frame coordinates to paper coordinates
 * @param[out] boardBounds a rectangle that represents the bounding box of the 
 * tic-tac-toe board in paper coordinates
 * @param[in] cap the webcam feed
 */
void initGame(cv::Mat& homography, cv::Rect& boardBounds, cv::VideoCapture& cap) {
    std::cout << "********************************************" << std::endl;
    std::cout << "INITIALIZING. DO NOT SCREW WITH THE PLOTTER" << std::endl;
    std::cout << "********************************************" << std::endl;


    // draw tictactoe board
    selectPen(1);
    std::cout << "drawing board\n:";
    drawBoard();    

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
    cv::imshow("out of homography", frameWarped);
    cv::waitKey(0);

    // find bounds of tictactoe board
    findBoardBounds(cap, homography, boardBounds);
    cv::rectangle(frameWarped, boardBounds, cv::Scalar(0,0,255));
    cv::imshow("with rectangle", frameWarped);
    cv::waitKey(0);
    std::cout << "********************************************" << std::endl;
    std::cout << "              YOU MAY DRAW NOW" << std::endl;
    std::cout << "********************************************" << std::endl;
}

/**
 * @brief      takes the state of the board, passes it to the lua minimax
 * algorithm to get the game's response
 *
 * @param[in, out] board  The current state of the board
 * @param[out]     row    The row of the game's move
 * @param[out]     col    The column of the game's move
 * @param[in]      L      The global Luastate
 */
void getPlotterAI(BoxState board[9], int *row, int *col, lua_State *L) {
    int ret;
    int isnum;
    lua_getglobal(L, "getPlotterMove"); 
    for (int i = 0; i < 9; i++) {
        lua_pushinteger(L, (int)board[i]);
    }
    lua_pushinteger(L, 1);
    if (lua_pcall(L, 10, 1, 0) != 0) {
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

/**
 * @brief      determines whether the player or computer has won
 *
 * @param[in]      board  The board
 *
 * @return     whether the player or computer has won
 */
bool someoneHasWon(BoxState board[9]) {
    for (int i = 0; i < 3; ++i) {
        if ((board[i] == board[i + 3]) && (board[i] == board[i + 6]) && (board[i] != BOX_EMPTY)) {
            //printBoard(board);
            return true;
        }
        if ((board[i * 3] == board[i * 3 + 1]) && (board[i * 3] == board[i * 3 + 2]) 
            && (board[i * 3] != BOX_EMPTY)) {
            //printBoard(board);
            return true;
        }
    }
    if ((board[0] == board[4]) && (board[0] == board[8]) && (board[0] != BOX_EMPTY)) {
        //printBoard(board);
        return true;
    }
    if ((board[2] == board[4]) && (board[2] == board[6]) && (board[2] != BOX_EMPTY)) {
        return true;
    }
    return false;
}

/**
 * @brief      Takes the board, and draws a line over the winning 
 * row/column/diagonal.
 *
 * @param      board[in]  The state of a winning board
 */
void drawWinner(BoxState board[9]) {
    for (int i = 0; i < 3; ++i) {
        if ((board[i] == board[i + 3]) && (board[i] == board[i + 6]) && (board[i] != BOX_EMPTY)) {
            drawWin((WinLine)(i));
            //printBoard(board);
            return;
        }
        if ((board[i * 3] == board[i * 3 + 1]) && (board[i * 3] == board[i * 3 + 2]) 
            && (board[i * 3] != BOX_EMPTY)) {
            drawWin((WinLine)(i + 3));
            //printBoard(board);
            return;
        }
    }
    if ((board[0] == board[4]) && (board[0] == board[8]) && (board[0] != BOX_EMPTY)) {
        //printBoard(board);
        drawWin(WIN_DIAG_0);
        return;
    }
    if ((board[2] == board[4]) && (board[2] == board[6]) && (board[2] != BOX_EMPTY)) {
        //printBoard(board);
        drawWin(WIN_DIAG_1);
        return;
    }
}

/**
 * @brief      This is where the magic happens. The game board is initialized to
 * empty boxes. While there isn't a winner, we check if there is a hand in the frame,
 * and whether there is a new 'O' on the board.
 *
 * @param[in]  homography   The homography from camera coordinates to paper coordinates
 * @param[in]  boardBounds  The board bounds relative to paper coordiantes
 * @param      L            The global Luastate
 * @param      cap          The the webcam feed
 *
 * @return     whether the game is able to sucessfully open the webcam feed
 */

bool gameLoop(const cv::Mat& homography, const cv::Rect& boardBounds,
    lua_State *L, cv::VideoCapture& cap) {
    BoxState board[9] = {BOX_EMPTY, BOX_EMPTY, BOX_EMPTY,
     BOX_EMPTY, BOX_EMPTY, BOX_EMPTY,
     BOX_EMPTY, BOX_EMPTY, BOX_EMPTY};
    cv::Mat frame;
    cv::Mat baseline;
    if (!cap.isOpened()) {
        return false;
    }

    // initialize baseline (this is used to check
    // whether there is a hand in the frame)
    cap.read(frame);
    cv::warpPerspective(frame, baseline, homography, frame.size());
    while (true) {
        cap.read(frame);
        // crop frame to paper
        cv::warpPerspective(frame, frame, homography, frame.size());
        if (handInFrame(baseline, frame)) {
            std::cout << "hand found in frame\n";
            continue;
        } else if (checkForO(frame, boardBounds, board)) {
            if (someoneHasWon(board)) {
                break;
            }
            std::cout << "Found O in frame. Plotter's turn\n";
            // Plotter's turn.
            int row = -1;
            int col = -1;
            getPlotterAI(board, &row, &col, L);
            std::cout << "Plotter plays X at (" << row << ", " << col << ")\n"; 
            printBoard(board);
            drawX(row, col);
            if (someoneHasWon(board)) {
                break;
            }
            revealPage();
            baseline = frame.clone();
        }
        sleep(1.5);
    }
    selectPen(3);
    drawWinner(board);
    selectPen(0);
    revealPage();
    cap.release();
}


int main(void) {
    bool playAgain = true;
    cv::VideoCapture cap(2);
    if (!cap.isOpened()) {
        return -1;
    }
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    int status = luaL_dofile(L, "./minimax.lua");
    if (status) {
        std::cout << "couldn't open minimax.lua\n";
        std::cout << "Error: " << lua_tostring(L, -1) << std::endl;
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
    }
    cap.release();
    std::cout << "thanks 4 playing :)\n";
  
    return 0;
}
