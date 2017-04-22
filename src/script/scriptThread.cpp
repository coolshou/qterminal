#include "scriptThread.h"

scriptThread::scriptThread()
{

}
scriptThread::~scriptThread()
{
    quit(); wait();
}
