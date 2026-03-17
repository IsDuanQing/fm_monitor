#include "mainwindow.h"

#include <QChartView>
#include <QLineSeries>          //线条类
#include <QtMath>
#include <QTimer>               //时间类
#include <QDateTime>
#include <QDateTimeAxis>        //时间坐标轴类
#include <QValueAxis>           //普通坐标轴类
#include <QTextCodec>
#include <QMessageBox>
#include <string>
#include <QDebug>
#include <QRandomGenerator>

QT_CHARTS_USE_NAMESPACE

#include "ui_mainwindow.h"

bool MS = true;//模式选择按钮图标变换
bool flag_Sw=false;

//本机发送标志位
bool flag_Send=0;

//温湿度变量
float temp_data;
float humi_data;
//可燃气体变量
float mq9_data;
//面粉浓度变量
float pm_data;
//蜂鸣器&&led状态
float led_beep;
//噪音大小
float db_data;

QString str;//模式选择文字变换

//发送缓冲区
extern QString strbuf;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //创建表一
    creatChart();

    //模式选择文字初始化
    str = "主机";
    ui->sermode_l->setText(str);
    ui->sermode_l->setText(str.split("",QString::SkipEmptyParts).join("\n"));
    ui->sermode_l->setAlignment(Qt::AlignCenter);

    //创建定时器 表格更新
    timer=new QTimer(this);
    timer->start(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(ReData_Slot()));

    // 创建定时器  发送更新
    timer_send = new QTimer(this);
    timer_send->start(20);
    connect(timer_send,SIGNAL(timeout()),this,SLOT(timer_send_Slot()));



    //默认使用主机模式，等待新的连接
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(newConnection_Slot()));

    ui->ip_edi->setText("192.168.10.169");
    ui->port_edi->setText("10083");

    //初始化数据库
    dbui = new database;

    setWindowTitle("--Intelligent Terminal--");
}


MainWindow::~MainWindow()
{
    delete ui;
}

//发送标志位
void MainWindow::ttest()
{
    flag_Send = 1;
}

/*网络部分*/
//检测是否有新连接进来
void MainWindow::newConnection_Slot()
{

    tcpSocket=tcpServer->nextPendingConnection();//得到通信的套接字对象
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readyRead_Slot()));
    connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(disconnected_Slot()));
    ui->connect_l->setStyleSheet("border-image: url(:/img/connect.png)");
    qDebug() << "连接成功";
}

//收到的数据放入接受框   解析
void MainWindow::readyRead_Slot()
{

    QByteArray receiveDate;
    QTextCodec *tc = QTextCodec::codecForName("GBK");  //编码转换,必须转换编码，否则乱码

    while(!tcpSocket->atEnd()){
        receiveDate = tcpSocket->readAll();
    }

    if (!receiveDate.isEmpty())
    {
        QString strBuf=tc->toUnicode(receiveDate);         //编码转换,必须转换编码，否则乱码
        //传入界面二数据
        deb->DisplayData(strBuf);
        //解析数据
        BackDataParsing(strBuf);
    }
    receiveDate.clear();
}

//客户机连接
void MainWindow::connected_Slot()
{

    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readyRead_Slot()));
    ui->connect_l->setStyleSheet("border-image: url(:/img/connect.png)");
    connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(disconnected_Slot()));
}

//服务器或客户机连接状态
void MainWindow::disconnected_Slot()
{

    tcpSocket->close();
    ui->connect_l->setStyleSheet("border-image: url(:/img/discon.png)");
}

void MainWindow::timer_send_Slot()
{
    if(flag_Send==1){
        flag_Send=0;
      tcpSocket->write(strbuf.toLocal8Bit().data());
    }
}

//模式选择按钮
void MainWindow::on_sermode_clicked()
{
    if(!flag_Sw)
    {
       //全关闭
       tcpServer->close();
       tcpSocket->close();
       //客户机
       if(MS){
           MS=false;
           str = "客户机";
           ui->sermode_l->setText(str);
           ui->sermode_l->setText(str.split("",QString::SkipEmptyParts).join("\n"));
           ui->sermode_l->setAlignment(Qt::AlignCenter);
           ui->sermode->setStyleSheet("border-image: url(:/img/client.png);");
       }

       else//主机
       {
           MS=true;
           str = "主机";
           ui->sermode_l->setText(str);
           ui->sermode_l->setText(str.split("",QString::SkipEmptyParts).join("\n"));
           ui->sermode_l->setAlignment(Qt::AlignCenter);
           ui->sermode->setStyleSheet("border-image: url(:/img/server.png);");
       }
    }
    else
        QMessageBox::critical(this,"提示","请先关闭网络，再切换模式类型");
}


//开始建立连接
void MainWindow::on_open_wifi_triggered()
{
    flag_Sw=!flag_Sw;

    if(flag_Sw)
    {
        //打开
        ui->wifi_l->setStyleSheet("border-image: url(:/img/wifi_on.png);");
        ui->open_wifi->setIcon(QIcon(":/img/open.png"));

        //选择主机
        if(MS)
        {
            if( tcpServer->listen(QHostAddress::Any,ui->port_edi->text().toUInt()) )
                qDebug() << "正在监听";
        }
        else//客户机
        {
            tcpSocket->connectToHost(ui->ip_edi->text(),ui->port_edi->text().toUInt());
            connect(tcpSocket,SIGNAL(connected()),this,SLOT(connected_Slot()));
        }
    }
    else
    {
        //关闭
        ui->wifi_l->setStyleSheet("border-image: url(:/img/wifi_off.png);");
        ui->open_wifi->setIcon(QIcon(":/img/close.png"));
        ui->connect_l->setStyleSheet("border-image: url(:/img/discon.png)");

        tcpServer->close();
        tcpSocket->close();
    }
}

//调试窗口
void MainWindow::on_deb_triggered()
{
    deb->show();
}

//创建图表一
void MainWindow::creatChart()
{
    QChart *qchart = new QChart();
    //把chart放到容器里
    ui->graphicsView->setChart(qchart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing); //设置抗锯齿

    //创建一条线
    QLineSeries *series = new QLineSeries;

    //设置名字
    series->setName("噪音强度");

    //把线条放到chart里
    qchart->addSeries(series);

    //创建x 坐标
    QDateTimeAxis *axisX = new QDateTimeAxis;

    //格式
    axisX->setFormat("hh:mm:ss");
    //设置竖条数量
    axisX->setTickCount(5);

    //设置坐标名称
    axisX->setTitleText("time(sec)");

    qchart->setAxisX(axisX,series);

    //创建y坐标
    QValueAxis  *axisY = new QValueAxis;
    axisY->setRange(0,100);
    axisY->setTickCount(5);

    qchart->setAxisY(axisY,series);

    qchart->setDropShadowEnabled(true);

    //初始化坐标
    //设置最大值坐标值 系统时间当前时间
    qchart->axisX()->setMin(QDateTime::currentDateTime().addSecs(0));
    //设置最大值坐标值 系统时间后5*30秒
    qchart->axisX()->setMax(QDateTime::currentDateTime().addSecs(5*30));
}

//表一刷新
void MainWindow::DisplayChart1()
{
    //获取当前时间
    QDateTime currentTime = QDateTime::currentDateTime();

    //获取初始化的qchart
    QChart *qchart =(QChart *)ui->graphicsView->chart();
    //获取初始化的series;
    QLineSeries *series = (QLineSeries *)ui->graphicsView->chart()->series().at(0);

    //db_data = QRandomGenerator::global()->bounded(101);
    series->append(currentTime.toMSecsSinceEpoch(),db_data);

    qchart->axisX()->setMin(QDateTime::currentDateTime().addSecs(-5*30));
    qchart->axisX()->setMax(QDateTime::currentDateTime().addSecs(5*30));
}

//时间刷新
void MainWindow::ReData_Slot(){
    static int timer=0;
    DisplayChart1();
    //当前时间
    ui->time->setText(QTime::currentTime().toString("hh:mm:ss"));

}

//图表一控件
void MainWindow::on_charts1_big_clicked()
{
    ui->graphicsView->chart()->zoom(1.2);
}

void MainWindow::on_charts1_small_clicked()
{
    ui->graphicsView->chart()->zoom(0.8);
}

void MainWindow::on_charts1_rest_clicked()
{
    ui->graphicsView->chart()->zoomReset();
}
//清除图表一
void MainWindow::on_pushButton_clicked()
{
    QLineSeries *series0 = (QLineSeries *)ui->graphicsView->chart()->series().at(0);
    series0->clear();
}

void MainWindow::on_data_triggered()
{
    dbui->show();
}

//数据解析
void MainWindow::BackDataParsing(QString strBuf){

    //查找是否为参数;  -1表示没有该子串
   if(strBuf.startsWith("ESP8266")){

        //温度
        QString str1 = strBuf.mid(strBuf.indexOf("temp:")+((QString)"temp:").length(),strBuf.indexOf("humi:")-strBuf.indexOf("temp:")-((QString)"temp:").length()-1);
        //tcpSocket->write(str1.toUtf8());
        //tcpSocket->write("0");

        //湿度
        QString str2 = strBuf.mid(strBuf.indexOf("humi:")+((QString)"humi:").length(),strBuf.indexOf("mq9:")-strBuf.indexOf("humi:")-((QString)"humi:").length()-1);
        //tcpSocket->write(str2.toUtf8());
        //tcpSocket->write("0");

        //可燃气体
        QString str3 = strBuf.mid(strBuf.indexOf("mq9:")+((QString)"mq9:").length(),strBuf.indexOf("flour:")-strBuf.indexOf("mq9:")-((QString)"mq9:").length()-1);
        //tcpSocket->write(str3.toUtf8());
        //tcpSocket->write("0");

        //面粉浓度
        QString str4 = strBuf.mid(strBuf.indexOf("flour:")+((QString)"flour:").length(),strBuf.indexOf("led&&beep:")-strBuf.indexOf("flour:")-((QString)"flour:").length()-1);
        //tcpSocket->write(str4.toUtf8());
        //tcpSocket->write("0");

        //蜂鸣器&&led状态
        QString str5 = strBuf.mid(strBuf.indexOf("led&&beep:")+((QString)"led&&beep:").length(),strBuf.indexOf("db:")-strBuf.indexOf("led&&beep:")-((QString)"led&&beep:").length()-1);
        //tcpSocket->write(str5.toUtf8());
        //tcpSocket->write("0");

        //噪音大小
        QString str6 = strBuf.mid(strBuf.indexOf("db:")+((QString)"db:").length(),strBuf.indexOf("}")-strBuf.indexOf("db:")-((QString)"db:").length()-1);
        //tcpSocket->write(str5.toUtf8());
        //tcpSocket->write("0");

        temp_data = str1.toFloat();
        humi_data = str2.toFloat();
        mq9_data = str3.toFloat();
        pm_data = str4.toFloat();
        led_beep = str5.toFloat();
        db_data = str6.toFloat();


        for (int i = 0; i < 10; i++)
             db_data = QRandomGenerator::global()->bounded(10, 41); // 生成 [10, 40] 的随机数


         ui->temp_lcd->display(temp_data);
         ui->huim_lcd->display(humi_data);
         ui->progressBar_2->setValue(mq9_data);
         ui->progressBar->setValue(pm_data);

         qDebug() << "温度：" << temp_data;
         qDebug() << "湿度：" << humi_data;
         qDebug() << "可燃气体浓度：" << mq9_data;
         qDebug() << "面粉浓度：" << pm_data;
         qDebug() << "蜂鸣器&&led状态：" << led_beep;
         qDebug() << "噪音大小：" << db_data;

         if(led_beep == 1)
         {
              ui->pushButton_2->setIcon(QIcon(":/img/led_on.png"));
              ui->pushButton_4->setIcon(QIcon(":/img/buzzer_on.png"));
         } else {
              ui->pushButton_2->setIcon(QIcon(":/img/led_off.png"));
              ui->pushButton_4->setIcon(QIcon(":/img/buzzer_off.png"));
         }

         //送入数据库
          dbui->UpdataToDataBase(str1,str2,str3,str4,str5,str6);

   }
}

void MainWindow::on_pushButton_2_clicked()
{
    tcpSocket->write("0");
}

void MainWindow::on_pushButton_4_clicked()
{
    tcpSocket->write("0");
}
