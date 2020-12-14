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
    return true;
}
#endif // THREADPOOL_H
