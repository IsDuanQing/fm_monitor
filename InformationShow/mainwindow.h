#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
/* 光添加头文件QChartView还不够，还需要引入QChart的命名空间 */
QT_CHARTS_USE_NAMESPACE

#include "deb.h"
#include "database.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void ttest();

    QTcpServer *tcpServer = new QTcpServer();
    QTcpSocket *tcpSocket = new QTcpSocket(); //接收

private slots:
    void ReData_Slot();
    void on_sermode_clicked();
    void newConnection_Slot();
    void connected_Slot();
    void disconnected_Slot();
    void on_open_wifi_triggered();
    void readyRead_Slot();
    void on_deb_triggered();
    void timer_send_Slot();
    void on_charts1_big_clicked();
    void on_charts1_small_clicked();
    void on_charts1_rest_clicked();
    void on_pushButton_clicked();
    void on_data_triggered();
    void BackDataParsing(QString strBuf);

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    void creatChart();
    void DisplayChart1();
    //图表对象
    QChart* m_chart;
    //横纵坐标轴对象
    QValueAxis *m_axisX, *m_axisY;
    //曲线图对象
    QLineSeries* m_lineSeries;
    //横纵坐标最大显示范围
    const int AXIS_MAX_X = 10, AXIS_MAX_Y = 10;
    //用来记录数据点数
    int pointCount = 0;

    //定时器1
    QTimer *timer;
    //定时器2
     QTimer *timer_send;
    //调试窗口
    Deb *deb = new Deb;
    //数据库窗口
    database *dbui;

};

#endif // MAINWINDOW_H
