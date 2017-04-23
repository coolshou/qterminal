#ifndef THREAD_H
#define THREAD_H

#include <QThread>

class scriptThread : public QThread
{
    // A thread that's safe to destruct, like it ought to be
    //using QThread::run; // final
public:
    scriptThread();
    ~scriptThread();
    void stop();
    void reset();

protected:
    void run();

private:
    bool stopped;
};

#endif // THREAD_H
