#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>
#include <QTcpSocket>

class Widget : public QWidget
{
    Q_OBJECT

    QTcpSocket* m_pTcpSocket;
    QTextEdit*  m_ptxtInfo;
    quint16     m_nNextBlockSize;
    QPushButton* pcmd;
    QPushButton* con;
    QString strHost;
    int nPort;

public:
    Widget(const QString& strHost, int nPort, QWidget* parent = 0) ;

    ~Widget();
private slots:
    void slotReadyRead   ();
    void slotError       (QAbstractSocket::SocketError);
    void slotSendToServer();
    void slotConnected   ();
    void reconnect       ();
};

#endif // WIDGET_H
