#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ctime>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    //在没有按下鼠标按键的时候依然追踪鼠标位置
    this->setMouseTracking(true);
    ui->centralWidget->setMouseTracking(true);
    ui->Main_label->setMouseTracking(true);
    
    //初始化
    mapInit();
    
    kernel.create(3,3,CV_8U);
    kernel = 1;
    kernel.at<uchar>(1,1)=0;
    
    //连接timer与updateFrame
    connect(timer,SIGNAL(timeout()),this,SLOT(updateFrame()));
    timer->setInterval(1000/ui->Speed_spinBox->value());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateFrame()
{
    //演化
    evolution();
    
    //绘制与显示
    drawMap();
    showMap();
}

void MainWindow::evolution()
{
    cv::filter2D(coreMat,coreMat,coreMat.depth(),kernel,cv::Point(-1,-1),0,cv::BORDER_ISOLATED);
    
    cv::Mat upperLimitMask,lowerLimitMask;
    cv::threshold(coreMat,upperLimitMask,ui->LifeUpperLimit_spinBox->value(),1,CV_THRESH_BINARY_INV);
    cv::threshold(coreMat,lowerLimitMask,ui->LifeLowerLimit_spinBox->value()-1,1,0);
    
    coreMat = upperLimitMask & lowerLimitMask;
}

void MainWindow::mapInit()
{
    //coreMat初始化
    coreMat.create(71,110,CV_8U);
    coreMat = 0;
    
    //map初始化
    map.create(710,1100,CV_8U);
    map = 0;
    
    //计算unitX和unitY
    unitX = map.cols/coreMat.cols;
    unitY = map.rows/coreMat.rows;
    
    //绘制
    drawMap();
    
    //显示
    showMap();
}

void MainWindow::drawMap()
{
    //画方块
    for(int i=0;i<coreMat.rows;i++)
        for(int j=0;j<coreMat.cols;j++)
        {
            if(coreMat.at<uchar>(i,j) == 1)
                cv::rectangle(map,cv::Rect(j*unitX,i*unitY,unitX,unitY),255,-1);
            else
                cv::rectangle(map,cv::Rect(j*unitX,i*unitY,unitX,unitY),0,-1);
        }
    
    //画线
    for(int i=0;i<coreMat.rows;i++)
        cv::line(map,cv::Point(0,i*unitY),cv::Point(map.cols-1,i*unitY),150);
    for(int j=0;j<coreMat.cols;j++)
        cv::line(map,cv::Point(j*unitX,0),cv::Point(j*unitX,map.rows-1),150);
}

void MainWindow::showMap()
{
    qimgMap = QImage((const uchar*)map.data,map.cols,map.rows,map.cols,QImage::Format_Grayscale8);
    ui->Main_label->setPixmap(QPixmap::fromImage(qimgMap));
}

void MainWindow::on_RunStop_pushButton_clicked()
{
    if(timer->isActive())
    {
        timer->stop();
        ui->RunStopState_label->setText("");
    }
    else
    {
        timer->start();
        ui->RunStopState_label->setText("Running...");
    }
}

void MainWindow::on_Exit_pushButton_clicked()
{
    exit(0);
}

void MainWindow::on_Clear_pushButton_clicked()
{
    mapInit();
}

void MainWindow::on_Random_pushButton_clicked()
{
    cv::RNG rng((unsigned)time(NULL));
    rng.fill(coreMat,cv::RNG::UNIFORM,0,2);
    
    drawMap();
    showMap();
}

void MainWindow::on_Speed_spinBox_valueChanged(int arg1)
{
    timer->setInterval(1000/arg1);
}

void MainWindow::on_Save_pushButton_clicked()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this);
    if(fileName.isEmpty()) return;
    
    cv::imwrite(fileName.toLocal8Bit().data(),map);
}

void MainWindow::on_LifeLowerLimit_spinBox_valueChanged(int arg1)
{
    if(arg1 > ui->LifeUpperLimit_spinBox->value())
        ui->LifeLowerLimit_spinBox->setValue(ui->LifeUpperLimit_spinBox->value());
}

void MainWindow::on_LifeUpperLimit_spinBox_valueChanged(int arg1)
{
    if(arg1 < ui->LifeLowerLimit_spinBox->value())
        ui->LifeUpperLimit_spinBox->setValue(ui->LifeLowerLimit_spinBox->value());
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    //如果鼠标按下的是左键且在Main_label范围内
    int x = event->pos().x() - ui->Main_label->pos().x();
    int y = event->pos().y() - ui->Main_label->pos().y();
    if(event->buttons()==Qt::LeftButton
       && x > 0 && y > 0
       && x < ui->Main_label->width() && y < ui->Main_label->height())
    {
        int realX = map.cols * x / ui->Main_label->width();
        int realY = map.rows * y / ui->Main_label->height();
        
        coreMat.at<uchar>(realY/unitY,realX/unitX) = 1;
        
        drawMap();
        showMap();
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //如果鼠标在Main_label范围内
    int x = event->pos().x() - ui->Main_label->pos().x();
    int y = event->pos().y() - ui->Main_label->pos().y();
    if(x > 0 && y > 0 && x < ui->Main_label->width() && y < ui->Main_label->height())
    {
        //如果鼠标没有按下
        if(event->buttons() == Qt::NoButton)
        {
            int realX = map.cols * x / ui->Main_label->width();
            int realY = map.rows * y / ui->Main_label->height();
            
            drawMap();
            cv::rectangle(map,cv::Rect(realX/unitX*unitX,realY/unitY*unitY,unitX,unitY),127,-1);
            showMap();
        }
        
        //如果鼠标按下左键
        if(event->buttons()==Qt::LeftButton)
        {
            int realX = map.cols * x / ui->Main_label->width();
            int realY = map.rows * y / ui->Main_label->height();
            
            coreMat.at<uchar>(realY/unitY,realX/unitX) = 1;
            
            drawMap();
            showMap();
        }
    }
}
