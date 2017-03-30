#ifndef GRAPHICTHREAD_H
#define GRAPHICTHREAD_H

#include <QThread>

#define GRAPHIC_THREAD_INTERVAL_MS  20  // msec

class GraphicThread : public QThread
{
    Q_OBJECT

public:
    GraphicThread(QObject *parent = 0);
    ~GraphicThread();

protected:
    /**
     * @brief Override of run method
     */
    void run();

private:
    /**
     * @brief Default is true, when false, exit the run method
     */
    bool gt_ExitThread;

    /**
     * @brief initGraphics prepare visualization objects
     * @return status (OK or NO)
     */
    int initGraphics();
};



#endif // GRAPHICTHREAD_H
