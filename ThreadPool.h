#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <QThread>
inline QThread* getNewThread()
{
    return new QThread();
}

inline bool releaseThread(QThread* thread)
{
    thread->deleteLater();
}
#endif // THREADPOOL_H
