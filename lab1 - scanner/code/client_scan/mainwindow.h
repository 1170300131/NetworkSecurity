#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QMessageBox>
#include <QDebug>
#include <QProgressBar>

#include "scan.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slot_scan();
    void slot_cancel();
    void show_text(const QString &str);
    void set_progress_bar(int finished_num);
    void count_stopped_num();

signals:
    void start_scaner(const char* ip, int from, int to);

public:
    QLabel* label[4];
    QLineEdit* edit[4];
    QPushButton* button_scan = new QPushButton(this);
    QPushButton* button_cancel = new QPushButton(this);
    QTextBrowser* text_result = new QTextBrowser(this);
    QProgressBar* progress_bar = new QProgressBar(this);


private:
    void scan(const char *ip, int from, int to, int thread_num);
    ScanThread* scan_thread;
    int global_thread_num = 0;
    int global_port_num = 0;
    int count = 0;
    bool is_scanning = false;
};

#endif // MAINWINDOW_H
