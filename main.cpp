#include "thresh.h"
#include "board.h"


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

bool gameLoop(const cv::Mat& homography, const cv::Rect& boardBounds) {
    BoxState board[9];
    cv::VideoCapture cap(2);
    cv::Mat frame;
    if (!cap.isOpened()) {
        return false;
    }
    while (!gameOver) {
        cap.read(frame);
        // crop frame to paper
        if (handInFrame(frame)) {
            continue;
        } else if (checkForO(frame, boardBounds, board)) {
            // Plotter's turn.
            int row = -1
            int col = -1;
            getPlotterAI(board, row, col);
            drawX(row, col);
            gameOver = someoneHasWon(board);
            revealPage();
        }
    }
    drawWinner(board);
    cap.close();
}

int main(void) {
    bool playAgain = true;
    while (playAgain) {
        cv::Mat homography;
        cv::Rect boardBounds;
        initGame(homography, boardBounds);
        gameLoop(homography, boardBounds);
        // get input from player
    }
  
    return 0;
}
