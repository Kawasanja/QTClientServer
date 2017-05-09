#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>
#include <QtNetwork>

class Widget : public QWidget
{
    Q_OBJECT

    QTcpServer* m_ptcpServer;
    quint32     m_nNextBlockSize;
    QPixmap * pix;
    QTextEdit * txt;
    qint16 format = -1;
    quint64 bytesCount = 0;
    qint16 width = 0;
    qint16 height = 0;
    uchar* bytes;
    QImage * img;
    quint32 counter = 0;
    QByteArray  arrBlock;
    QPushButton * btn;

private:
    void sendToClient(QTcpSocket* pSocket, const QString& str);

public:
    Widget(int nPort, QWidget* parent = 0);
    ~Widget();
public slots:
    virtual void slotNewConnection();
            void slotReadClient   ();
            void slotSaveImage    ();
};

#endif // WIDGET_H
