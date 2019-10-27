#include "plotter.h"
#include <cstdio>


Plotter::Plotter(const std::string file, int speed) {
    this->serialDev = open(file.c_str(), O_RDWR); 
    if (this->serialDev < 0) {
        throw PlotterException("Couldn't open serial device");
    }

    struct termios tty;
    memset(&tty, 0, sizeof(struct termios));

    // read in current settings
    int err = tcgetattr(this->serialDev, &tty);
    if (err != 0) {
        throw PlotterException("Couldn't read in port settings");
    }

    // set serial port to 9600 baud, 8 bits, no stop, no parity, no flow control
    tty.c_cflag &= ~(PARENB | CSTOPB | CRTSCTS);
    tty.c_cflag |= (CS8 | 1);

    // blocks for any data for up to 100 deciseconds
    tty.c_cc[VTIME] = 100;
    tty.c_cc[VMIN] = 0;

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    err = tcsetattr(this->serialDev, TCSANOW, &tty);
    if (err < 0) {
        throw PlotterException("Failed to set serial port settings");
    }

    // send IN command
    this->addCmd(std::string("IN;"));
    this->setPenSpeed(speed);
}

Plotter::~Plotter() {

}

bool Plotter::drawText(std::string text) {
    std::string cmd = "LB" + text + ";"; 
    return this->addCmd(cmd);
}

bool Plotter::setPenSpeed(int v) {
    if (v < 1 || v > 20) {
        return false;
    }
    this->penSpeed = v;
    std::string cmd = "VS" + std::to_string(v) + ";";
    return this->addCmd(cmd);
}

bool Plotter::drawLine(int startX, int startY, int endX, int endY) {
    if (!this->penUp()) {
        return false;
    }
    if (!this->setPenPos(startX, startY)) {
        return false;
    }
    if (!this->penDown()) {
        return false;
    }
    if (!this->setPenPos(endX, endY)) {
        return false;
    }
    return true;
}

// TODO assert that x and y are in correct range
bool Plotter::setPenPos(int x, int y) {
    if (x < 0 || y < 0) {
        return false;
    }
    std::vector<std::string> cmds;
    std::string pa_cmd = "PA" + std::to_string(x) + "," + std::to_string(y) + ";";
    return this->addCmd(pa_cmd);
}

bool Plotter::selectPen(int pen) {
    if (pen < 0 || pen > 8) {
        return false;
    }
    std::string pa_cmd = "SP" + std::to_string(pen) + ";";
    return this->addCmd(pa_cmd);
}


bool Plotter::penUp(void) {
    return this->addCmd(std::string("PU;"));
}

bool Plotter::penDown(void) {
    return this->addCmd(std::string("PD;"));
}

bool Plotter::returnPen(void) {
    if (!this->penUp()) {
        return false;
    }
    if (!this->selectPen(0)) {
        return false;
    }
    return true;
}

bool Plotter::sendBytes(std::string bytes) {
    if (bytes.length() == 0 || bytes.length() > 57) {
        return false;
    }
    bytes.append("OA;");
    std::cout << "Writing command: " << bytes << " || length is " << bytes.length() << std::endl;
    write(serialDev, bytes.c_str(), bytes.length());
    char ret = 0;
    while (ret != 13) {
        read(serialDev, &ret, 1);
    }
    std::cout << "flushed\n";
    return true;
}

bool Plotter::addCmd(std::string cmd) {
    if (this->currCmd.length() + cmd.length() < 57) {
        this->currCmd.append(cmd);
    } else {
        // otherwise, send bytes and clear
        this->flush();
        this->currCmd.append(cmd);
    }
    return true;
}

// flush will send the bytes and clear the internal command buffer
bool Plotter::flush(void) {
    if (this->currCmd.length() > 0) {
        bool ret = this->sendBytes(this->currCmd);
        this->currCmd.clear();
        return ret;
    }
    return true;
}

#if 0
int main(void) {
    Plotter p;
    p.selectPen(1);
    p.drawText(std::string("Hello world"));
    p.flush();
    
    return 0;
}
#endif
