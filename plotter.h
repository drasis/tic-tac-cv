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
// Path to serial port on Macs
const std::string port_file = "/dev/tty.usbserial"; 
#elif __linux__
// Path to serial port on Linux
const std::string port_file = "/dev/ttyUSB0";
#else
#error "Unsupported compiler"
#endif

/**
 * @brief      Exception for signaling plotter errors.
 */
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

/**
 * @brief      This class implements an interface to the HP7440A Plotter over serial.
 */
class Plotter
{
public:
    /**
     * @brief      Constructs a new instance of the plotter interface. 
     *             Opens and configures the serial port to match the settings of the HP7440A Plotter.
     *             Throws a PlotterException if it can't open the serial port or configure the settings.
     *             
     * @param[in]  file   The path to the serial port device.
     * @param[in]  speed  The pen speed
     */
    Plotter(const std::string file=port_file, int speed=5);
    /**
     * @brief      Destroys the plotter interface.
     */
    ~Plotter();
    /**
     * @brief      Draws a line in plotter co-ordinates.
     *
     * @param[in]  startX  The start x
     * @param[in]  startY  The start y
     * @param[in]  endX    The end x
     * @param[in]  endY    The end y
     *
     * @return     Successfully flushed command buffer
     */
    bool drawLine(int startX, int startY, int endX, int endY);
    /**
     * @brief      Sets the pen position in plotter co-ordinates.
     *
     * @param[in]  x     x position of pen
     * @param[in]  y     y position of pen
     *
     * @return     Successfully flushed command buffer
     */
    bool setPenPos(int x, int y);
    /**
     * @brief      Selects a pen for the plotter to use.
     *
     * @param[in]  pen   The pen ID (from 1-8)
     *
     * @return     Successfully flushed command buffer
     */
    bool selectPen(int pen);
    /**
     * @brief      Instructs the plotter to lift the pen off the paper.
     *
     * @return     Successfully flushed command buffer
     */
    bool penUp(void);
    /**
     * @brief      Instructs the plotter to place the pen down on the paper
     *
     * @return     Successfully flushed command buffer
     */
    bool penDown(void);
    /**
     * @brief      Instructs the plotter to return the currently held pen to the holder.
     *
     * @return     Successfully flushed command buffer
     */
    bool returnPen(void);
    /**
     * @brief      Flushes the plotter's command buffer.
     *
     * @return     Successfully flushed command buffer
     */
    bool flush(void);
    /**
     * @brief      Sets the pen speed.
     *
     * @param[in]  v     The new pen speed (between 1 and 40)
     *
     * @return     Successfully flushed command buffer
     */
    bool setPenSpeed(int v);
    /**
     * @brief      Instructs the plotter to draw text using the 'label' command.
     *
     * @param[in]  text  The string to be rendered
     *
     * @return     Successfully flushed command buffer
     */
    bool drawText(std::string text);

private:
    bool sendBytes(std::string bytes);
    bool addCmd(std::string cmd);

    std::string currCmd;
    int serialDev;
    int penSpeed;
};

#endif // __PLOTTER_H__
