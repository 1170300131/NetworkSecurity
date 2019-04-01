#ifndef SCAN_H
#define SCAN_H
#include <QThread>
#include <QMutex>

extern int global_finish_num;

class ScanThread : public QThread
{
    Q_OBJECT

public:
    ScanThread();
    ~ScanThread();
    void run();
    void stop();

signals:
    void send_text(const QString& str);
    void send_progress_bar(int num);
    void send_have_stopped();

public slots:
    void scan_port(const char *ip_in, int from_in, int to_in);

private:
    bool thread_stop;
    QMutex stop_mutex;
    bool start_flag;
    QMutex start_mutex;

    int from,to;
    char ip[32];
};

#endif // SCAN_H
