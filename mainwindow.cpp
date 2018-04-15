#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
static const QLatin1String serviceUuid("00001101-0000-1000-8000-00805F9B34FB");
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFile FileQss(":/qss.qrc");
    FileQss.open(QFile::ReadOnly);
    if(FileQss.isOpen())
    {
        QString qss = QLatin1String(FileQss.readAll());
        qApp->setStyleSheet(qss);
        FileQss.close();
    }
    ui->verticalSlider->setMinimum(0);
    ui->verticalSlider->setMaximum(100);

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    localDevice = new QBluetoothLocalDevice();
    socket  =new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);  //UnknownProtocol   L2capProtocol   RfcommProtocol 协议

    connect(discoveryAgent,SIGNAL(deviceDiscovered( QBluetoothDeviceInfo)),this,SLOT(bltDevDiscover(QBluetoothDeviceInfo)));
    connect(discoveryAgent,SIGNAL(finished()),this,SLOT(scanFinshed()));

    if(localDevice->hostMode()==QBluetoothLocalDevice::HostPoweredOff )
    {
        ui->pushButton_OpenBlu->setEnabled(true);
        ui->pushButton_CloseBlu->setEnabled(false);
        ui->textBrowser->clear();
        ui->textBrowser->append("你的蓝牙处于关闭状态，请打开您的蓝牙设备");
    }
    else
    {
        ui->pushButton_OpenBlu->setEnabled(false);
        ui->pushButton_CloseBlu->setEnabled(true);
        ui->textBrowser->clear();
        ui->textBrowser->append("您的蓝牙设备已经打开");
    }
    connect(ui->listWidget,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(itemActivated(QListWidgetItem*)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_scanblt_released()//扫描蓝牙前先检测蓝牙是否已经打开  如果没有打开蓝牙就打开蓝牙

{

    if(localDevice->hostMode()==QBluetoothLocalDevice::HostPoweredOff)
    {

        QMessageBox::warning(this,"错误","您还没有打开蓝牙");
    }
    else
    {
        ui->textBrowser->clear();
        ui->textBrowser->append("正在扫描中，请稍等！！！！！！");
        discoveryAgent->setInquiryType(QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);//设置蓝牙扫描周围蓝牙的方式，访问周围所有可以用到的蓝牙设备
        discoveryAgent->start();

    }

}

void MainWindow::bltDevDiscover(const QBluetoothDeviceInfo &info)//这一段程序是将扫描到的蓝牙设备在框框中显示出来
{
    QString label =QString("%1 %2 %3").arg(info.address().toString()).arg("       ").arg(info.name());//扫描到周围可以用到的蓝牙地址加名称
    QList <QListWidgetItem *> items=ui->listWidget->findItems(label,Qt::MatchExactly);
    if (items.empty())
    {
        QListWidgetItem *item = new QListWidgetItem(label);
        QBluetoothLocalDevice::Pairing pairingStatus =localDevice->pairingStatus(info.address());
        if(pairingStatus==QBluetoothLocalDevice::Paired||pairingStatus==QBluetoothLocalDevice::AuthorizedPaired)
        {
            item->setTextColor(QColor(Qt::green));//已经配对链接成功的，显示为绿色
        }
        else
        {
            item->setTextColor(QColor(Qt::black));//没有配对成功的显示为黑色
        }
        ui->listWidget->addItem(item);

    }
}
void MainWindow::scanFinshed()
{
    ui->textBrowser->clear();
    QMessageBox::warning(this,"恭喜","设备扫描完成!!");
}

void MainWindow::on_pushButton_OpenBlu_clicked()
{
    localDevice->powerOn();
    localDevice->setHostMode(QBluetoothLocalDevice::HostConnectable);
    if(localDevice->hostMode()==QBluetoothLocalDevice::HostConnectable)
    {
        ui->textBrowser->clear();
        ui->textBrowser->append("您的蓝牙设备已经打开");
        ui->pushButton_OpenBlu->setEnabled(false);
        ui->pushButton_CloseBlu->setEnabled(true);
    }

}

void MainWindow::on_pushButton_CloseBlu_clicked()
{
    localDevice->setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    if(localDevice->hostMode()==QBluetoothLocalDevice::HostPoweredOff)
    {
        ui->textBrowser->clear();
        ui->textBrowser->append("您的蓝牙设备已经关闭");
        ui->pushButton_OpenBlu->setEnabled(true);
        ui->pushButton_CloseBlu->setEnabled(false);
    }
}

void MainWindow::on_pushButton_scanblt_clicked()
{

}

void MainWindow::itemActivated(QListWidgetItem *item)
{
    QString text = item->text();//把所点击的选项卡的字符串都传给text
    int index =text.indexOf(' ');
    if(index==-1){
        return;
    }
    QBluetoothAddress Address(text.left(index));
    QString name(text.mid(index + 1));
    QString temp = QString("%1").arg(Address.toString());
    ui->textBrowser->append(QStringLiteral("你链接的蓝牙地址是：")+temp);
    ui->textBrowser->append(QStringLiteral("你链接的蓝牙名字：")+name);
    socket->connectToService(Address,QBluetoothUuid(serviceUuid),QIODevice::ReadWrite);
    connect(socket,SIGNAL(connected()),this,SLOT(Connected()));
}

void MainWindow::Connected()
{
    QMessageBox::information(this,"信息","成功连接");
    connect(socket,SIGNAL(readyRead()),this,SLOT(bltReadMsg()));
    connect(ui->verticalSlider,SIGNAL(valueChanged(int)),this,SLOT(sentData(int)));
}

void MainWindow::bltReadMsg()
{
    QByteArray buf =  socket->readAll();

    if(!buf.isEmpty())
    {
        QString str = ui->resTexEdit->toPlainText();
        str+=tr(buf);
        ui->resTexEdit->clear();
        ui->resTexEdit->append(str);
    }
    buf.clear();

}
void MainWindow::sentData(int a)
{

    QByteArray arrayData;
    QString s=QString::number(a);
    s=s+'z';
    arrayData = s.toUtf8();
    socket->write(arrayData);
}
void MainWindow::on_pushButton_Send_clicked()
{
    QByteArray arrayData;
    QString s=ui->textEdit->toPlainText();
    arrayData = s.toUtf8();
    socket->write(arrayData);
}
