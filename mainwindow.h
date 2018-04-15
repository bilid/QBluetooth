#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QLatin1String>
#include <QtBluetooth/qbluetoothglobal.h>
#include <QtBluetooth/qbluetoothlocaldevice.h>  //获取设备的蓝牙状态信息
#include <QtBluetooth/qbluetoothsocket.h>  //进行链接蓝牙设备，读写信息
#include <QtBluetooth/qbluetoothservicediscoveryagent.h>  //扫描周围蓝牙设备
#include <QFile>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothLocalDevice *localDevice;
    QBluetoothSocket *socket;



private slots:
    void on_pushButton_scanblt_released();
    void bltDevDiscover(const QBluetoothDeviceInfo &info);
    void scanFinshed();
    void on_pushButton_OpenBlu_clicked();

    void on_pushButton_CloseBlu_clicked();

    void on_pushButton_scanblt_clicked();
    void itemActivated(QListWidgetItem *item);
    void  Connected();
    void   bltReadMsg();
    void   sentData(int a);

    void on_pushButton_Send_clicked();

private:
    Ui::MainWindow *ui;




};

#endif // MAINWINDOW_H
