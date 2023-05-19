#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QDialog>
#include <QTcpServer>
#include <QMessageBox>
#include <QTcpSocket>
#include <QtNetwork>
#include <QFileDialog>

namespace Ui
{
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(QWidget* parent = nullptr);
    ~Server();

private slots:
    void on_pushButtonStopListening_clicked();

    void on_ListenButton_clicked();

    void on_pushButtonSelectSavePath_clicked();

private:
    Ui::Server* ui;
    QTcpServer* server_obj;
    void acceptConnection();
};

static QString getHostIpAddress()
{
    QString strIpAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // 获取第一个本主机的IPv4地址
    for(auto ipAddress : ipAddressesList)
    {
        if(ipAddress.protocol() == QAbstractSocket::IPv4Protocol &&
                !ipAddress.isLoopback())
        {
            strIpAddress = ipAddress.toString();
            break;
        }
    }
    // 如果没有找到，则以本地IP地址为IP
    if(strIpAddress.isEmpty())
    {
        strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }
    return strIpAddress;
}
#endif // SERVER_H
