#ifndef __PLOTTER_H__
#define __PLOTTER_H__

#include <string.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <vector>

#ifdef __APPLE__
const std::string port_file = "/dev/tty.usbserial"; 
#elif __linux__
const std::string port_file = "/dev/ttyUSB0";
#else
#error "Unsupported compiler"
#endif

class PlotterException : public std::exception
{
public:
    PlotterException(std::string msg) {
        this->msg = msg;
    };

    const char *what() const throw() {
        return msg.c_str();
    };

private:
    std::string msg;
};

class Plotter
{
public:
    Plotter(const std::string file=port_file, int speed=5);
    ~Plotter();
    bool drawLine(int startX, int startY, int endX, int endY);
    bool setPenPos(int x, int y);
    bool selectPen(int pen);
    bool movePen(int x, int y);
    bool penUp(void);
    bool penDown(void);
    bool returnPen(void);
    bool flush(void);
    bool setPenSpeed(int v);

private:
    bool sendBytes(std::string bytes);
    bool addCmd(std::string cmd);

    std::string currCmd;
    int serialDev;
    int penSpeed;
};

#endif // __PLOTTER_H__
