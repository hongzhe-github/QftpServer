#include "server.h"
#include "ui_server.h"

Server::Server(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
}

Server::~Server()
{
    delete ui;
}



// 接收文件函数
void Server::acceptConnection()
{
    // 获取已连接的套接字
    QTcpSocket* socket = server_obj->nextPendingConnection();

    // 读取文件名和文件大小
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_2);

    qint64 bytesReceived = 0;
    qint64 fileNameSize = 0;
    QString fileName;
    qint64 fileSize = 0;

    if(socket->waitForReadyRead())
    {
        in >> bytesReceived >> fileNameSize >> fileName >> fileSize;
    }

    // 打开本地文件用于接收数据
    QString saveFilePath = ui->lineEditSavePath->text() + "/" + fileName;

    QFile file(saveFilePath);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, "Error", "Could not open file");
        return;
    }

    // 接收文件
    QByteArray buffer;
    while(bytesReceived < fileSize)
    {
        if(socket->waitForReadyRead(30000))  // 等待数据30秒
        {
            buffer = socket->readAll();
            bytesReceived += buffer.size();
            file.write(buffer);
            // 计算接收进度并更新进度条
            double receiveProgress = (double)bytesReceived / (double)fileSize;
            ui->progressBarReceiveProgress->setValue(receiveProgress * 100);
        }
        else
        {
            QMessageBox::critical(this, "Error", "接收文件超时！");
            break;
        }
    }

    // 关闭文件和套接字
    file.close();
    socket->close();
    socket->deleteLater();
}

void Server::on_pushButtonStopListening_clicked()
{
    // 停止监听
    server_obj->close();

    // 更新界面状态
    ui->ListenButton->setEnabled(true);
    ui->ListenButton->setText("开始监听");
    ui->lineEditReceiveStatus->setText("未开启监听");
}


void Server::on_ListenButton_clicked()
{
    // 判断保存文件路径是否存在
    QFileInfo fileInfo(ui->lineEditSavePath->text());
    if(!fileInfo.exists())
    {
        QMessageBox::critical(this, "错误", "未正确选择保存路径！");
        return;
    }

    // 创建服务器并开始监听
    server_obj = new QTcpServer(this);
    if(!server_obj->listen(QHostAddress::Any, ui->spinBoxServerPort->value()))
    {
        QMessageBox::critical(this, "Error", "Could not start server");
        return;
    }

    // 连接新连接的信号槽
    connect(server_obj, &QTcpServer::newConnection, this, &Server::acceptConnection);

    // 获取服务器地址并将其显示在lineEditReceiverIP组件中
    QString IpAddress = getHostIpAddress();

    // 更新状态信息
    ui->lineEditReceiverIP->setText(IpAddress);

    // 更新界面状态
    ui->ListenButton->setEnabled(false);
    ui->lineEditReceiveStatus->setText("正在监听...");
}


void Server::on_pushButtonSelectSavePath_clicked()
{
    // 显示选择路径的line Edit控件(objectName:lineEditSavePath)
    QString saveDirPath = QFileDialog::getExistingDirectory(this, "选择保存文件的路径");
    ui->lineEditSavePath->setText(saveDirPath);
}

