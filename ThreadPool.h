#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <QThread>
inline QThread* getNewThread()
{
    auto p=new QThread();
    QObject::connect(p,&QThread::finished,p,&QThread::deleteLater);
    return p;
}

inline bool releaseThread(QThread* thread)
{

    return true;
}
#endif // THREADPOOL_H
