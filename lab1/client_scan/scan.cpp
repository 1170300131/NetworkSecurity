#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "warp.h"
#include "scan.h"
#include "mainwindow.h"

using namespace std;

int global_finish_num = 0;

ScanThread::ScanThread()
    :QThread() ,thread_stop(true),start_flag(false)
{
}

ScanThread::~ScanThread()
{
    qDebug() << "thread ~thread";
}

void ScanThread::run()
{
    thread_stop = false;
    while(1){
        QMutexLocker locker(&start_mutex);
        if(start_flag){
            struct sockaddr_in server_addr;
            int sockfd;
            for(int port = from; port <= to; port++){
                sockfd = Socket(AF_INET, SOCK_STREAM, 0);

                bzero(&server_addr, sizeof(server_addr));
                server_addr.sin_family = AF_INET;
                inet_pton(AF_INET, ip, &server_addr.sin_addr);
                server_addr.sin_port = htons(port);

                int ret = ::connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
                if(ret == 0){
                    emit send_text(QString("port %1 open!").arg(port, 0, 10));
                }
                Close(sockfd);
                send_progress_bar(++global_finish_num);

                QMutexLocker locker(&stop_mutex);
                if(thread_stop){
                    send_progress_bar(0);
                    return;
                }
            }
            send_have_stopped();
            QMutexLocker locker(&start_mutex);
            start_flag = false;
            break;
        }
    }
}

void ScanThread::scan_port(const char* ip_in, int from_in, int to_in)
{
    if(!start_flag)
    {
        memcpy(ip,ip_in,32);
        from = from_in;
        to = to_in;
        QMutexLocker locker(&start_mutex);
        start_flag = true;
    }
}

void ScanThread::stop()
{
    QMutexLocker locker(&stop_mutex);
    thread_stop = true;
}

//void ScanObject::send_text(const QString& str)
//{
//}
