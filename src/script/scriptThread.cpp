#include "scriptThread.h"

scriptThread::scriptThread()
{
    this->stopped = false;
}
scriptThread::~scriptThread()
{
    quit();
    wait();
}
void scriptThread::run()
{
    while (!stopped) {
          // cerr << qPrintable(msg) << endl;
          QThread::sleep(1);
    }
}

void scriptThread::stop()
{
    stopped = true;
}
void scriptThread::reset()
{
    stopped = false;
}
