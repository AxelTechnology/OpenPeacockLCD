#include "common.h"
#include "ui_common.h"
#include "graphicthread.h"

// this thread takes values from data structures and paint them on display

GraphicThread::GraphicThread(QObject *parent) :
                               QThread(parent),
                               gt_ExitThread(false)
{

    int status; // tells if init is ok

    // setup connection
    status = initGraphics();

    // if init was ok, then start the thread
    if (!status) this->start();
}

GraphicThread::~GraphicThread()
{
    // set the exit flag to true
    gt_ExitThread = true;

    // wait for the run method to be completed
    this->wait();
}

void GraphicThread::run()
{

    forever {

        if (gt_ExitThread == true)
        {
            // in this case exit the run method
            break;
        }

        // read source data block and

        // sleep
        msleep(GRAPHIC_THREAD_INTERVAL_MS);
     }
}

int GraphicThread::initGraphics()
{

    return 0;
}
