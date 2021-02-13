#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <QThread>
inline QThread* getNewThread()
{
    return new QThread();
}

inline bool releaseThread(QThread* thread)
{
//    emit thread->finished();
    QObject::connect(thread,&QThread::finished,thread,&QThread::deleteLater);
    thread->quit();

    return true;
}
#endif // THREADPOOL_H
