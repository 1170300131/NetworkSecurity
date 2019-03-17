#include "mainwindow.h"
#include <unistd.h>

#include <iostream>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle(tr("scan ports"));
    this->resize(300,200);

    text_result->hide();
    progress_bar->hide();

    label[0] = new QLabel("ip",this, 0);
    label[0]->setGeometry(QRect(40,20,150,30));
    label[1] = new QLabel("port from",this,0);
    label[1]->setGeometry(QRect(40,65,150,30));
    label[2] = new QLabel("to",this,0);
    label[2]->setGeometry(QRect(180,65,30,30));
    label[3] = new QLabel("pthread",this, 0);
    label[3]->setGeometry(QRect(40,105,150,30));

    for(int i=0;i<4;i++)
    {
        edit[i] = new QLineEdit(this);
        edit[i]->setAlignment(Qt::AlignCenter);
    }
    edit[0]->setGeometry(QRect(120,25,130,20));
    edit[1]->setGeometry(QRect(120,70,50,20));
    edit[2]->setGeometry(QRect(200,70,50,20));
    edit[3]->setGeometry(QRect(120,110,50,20));
    edit[0]->setText("127.0.0.1");
    edit[1]->setText("8000");
    edit[2]->setText("8090");
    edit[3]->setText("1");

    button_scan->setText(tr("scan now"));
    button_scan->setGeometry(QRect(50,155,100,30));
    button_cancel->setText(tr("cancel"));
    button_cancel->setGeometry(QRect(160,155,100,30));

    connect(button_scan, SIGNAL(clicked()), this,SLOT(slot_scan()));
    connect(button_cancel, SIGNAL(clicked()), this, SLOT(slot_cancel()));
}

MainWindow::~MainWindow()
{
    for(int i=0;i<global_thread_num;i++){
        if(scan_thread[i].isRunning()){
            scan_thread[i].stop();
        }
    }
}

void MainWindow::slot_cancel()
{
    if(!is_scanning){
        QMessageBox::warning(this,"Warning", "There is no scan process!");
        return;
    }
    QMessageBox messageBox(QMessageBox::Question,"Notice","Sure to cancel the process?",QMessageBox::Yes|QMessageBox::No);
    if(messageBox.exec()==QMessageBox::Yes){
        for(int i=0;i<global_thread_num;i++){
            if(scan_thread[i].isRunning()){
                scan_thread[i].stop();
            }
        }
        text_result->append("-------------interrupt-------------");
        is_scanning = false;
        global_finish_num = 0;
    }
}

void MainWindow::slot_scan()
{
    const char* ip_text = edit[0]->text().toStdString().data();
    char ip_local[32];
    memcpy(ip_local, ip_text, 32);
    int from = edit[1]->text().toInt();
    int to = edit[2]->text().toInt();
    int thread_num = edit[3]->text().toInt();

    if(is_scanning){
        QMessageBox::warning(this, "Warning", "Scanning!Please click the cancel button if you would like to stop it.");
        return;
    }
    if(from < 0 || to > 65535 || from > to)
        QMessageBox::warning(this, "Warning", "Please correct the range of port numbers!");
    else if(thread_num <=0)
        QMessageBox::warning(this, "Warning", "Please make sure that the thread_num is positive!");
    else{
        text_result->show();
//        text_result->clear();
        text_result->setGeometry(QRect(270, 25, 245, 140));
        progress_bar->show();
        progress_bar->setRange(0,to-from+1);
        progress_bar->setValue(0);
        progress_bar->setGeometry(QRect(270, 140, 245, 80));
        this->resize(530, 200);
        scan(ip_local,from, to, thread_num);
        is_scanning = true;
    }
}

void MainWindow::scan(const char* ip, int from, int to, int thread_num)
{
    global_thread_num = thread_num;
    global_port_num = to - from +1;
    int each_num = global_port_num / thread_num;
    text_result->append(QString("each thread should scan %1 ports").arg(each_num,0,10));

    scan_thread = new ScanThread[thread_num];
    for(int i=0;i<thread_num;i++)
    {
        int port_start = from + each_num * i;
        int port_end;
        if(i==thread_num-1){
            port_end = to;
        }
        else{
            port_end = port_start + each_num - 1;
        }

//        connect(&scan_thread[i], &ScanThread::finished, &scan_thread[i], &QObject::deleteLater);
        connect(this, &MainWindow::start_scaner, &scan_thread[i], &ScanThread::scan_port);
        connect(&scan_thread[i], &ScanThread::send_text, this, &MainWindow::show_text);
        connect(&scan_thread[i], &ScanThread::send_progress_bar, this, &MainWindow::set_progress_bar);
        connect(&scan_thread[i], &ScanThread::send_have_stopped, this, &MainWindow::count_stopped_num);
        scan_thread[i].start();
        emit start_scaner(ip, port_start, port_end);
    }
}

void MainWindow::show_text(const QString& str)
{
    text_result->append(str);
}

void MainWindow::set_progress_bar(int num)
{
    progress_bar->setValue(num);
}

void MainWindow::count_stopped_num()
{
    count++;
    if(count == global_thread_num){
        text_result->append("-------------All done-------------");
        global_finish_num = 0;
        is_scanning = false;
        count = 0;
    }
}

void start_scaner(const char* ip, int from, int to)
{
}

