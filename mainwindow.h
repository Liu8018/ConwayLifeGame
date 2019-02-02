#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    
    QTimer *timer = new QTimer(this);
    
    cv::Mat coreMat;
    cv::Mat map;
    QImage qimgMap;
    
    int unitX;
    int unitY;
    
    void mapInit();
    void drawMap();
    void showMap();
    
    cv::Mat kernel;
    void evolution();
    
private slots:
    void updateFrame();
    void on_RunStop_pushButton_clicked();
    void on_Exit_pushButton_clicked();
    void on_Clear_pushButton_clicked();
    void on_Random_pushButton_clicked();
    void on_Speed_spinBox_valueChanged(int arg1);
    void on_Save_pushButton_clicked();
    void on_LifeLowerLimit_spinBox_valueChanged(int arg1);
    void on_LifeUpperLimit_spinBox_valueChanged(int arg1);
};

#endif // MAINWINDOW_H
