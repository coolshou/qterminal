#ifndef THREAD_H
#define THREAD_H

#include <QThread>

class scriptThread : public QThread
{
    // A thread that's safe to destruct, like it ought to be
    using QThread::run; // final
public:
    scriptThread();
    ~scriptThread();
};

#endif // THREAD_H
