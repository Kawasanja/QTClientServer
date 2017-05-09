#include "widget.h"

Widget::Widget(const QString& strHost, int nPort,
               QWidget* parent):QWidget(parent),
               m_nNextBlockSize(0)
{
    this->strHost = strHost;
    this->nPort = nPort;
    m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->connectToHost(this->strHost, this->nPort);

    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
           );

        m_ptxtInfo  = new QTextEdit;
        m_ptxtInfo->setMinimumSize(300,300);
        pcmd = new QPushButton("Send");
        con = new QPushButton("Reconnect");
        m_ptxtInfo->setReadOnly(true);

        connect(pcmd, SIGNAL(clicked()), SLOT(slotSendToServer()));
        connect(con, SIGNAL(clicked()), SLOT(reconnect()));


        //Layout setup
        QVBoxLayout* pvbxLayout = new QVBoxLayout;
        pvbxLayout->addWidget(new QLabel("<H1>Client</H1>"));
        pvbxLayout->addWidget(m_ptxtInfo);
        QHBoxLayout* phbxLayout = new QHBoxLayout;
        phbxLayout->addWidget(pcmd);
        phbxLayout->addWidget(con);
        pvbxLayout->addLayout(phbxLayout);
        setLayout(pvbxLayout);
}

void Widget::slotReadyRead()
{
    pcmd->setEnabled(true);
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_5_7); //обезопаситься, вообще, хорошая практика
    for (;;) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < (int)sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime   time;
        QString str;
        in >> time >> str;

        m_ptxtInfo->append(time.toString() + " " + str);
        m_nNextBlockSize = 0;
    }
}

void Widget::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                     "The host was not found." :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     "The remote host is closed." :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     "The connection was refused." :
                     QString(m_pTcpSocket->errorString())
                    );
    m_ptxtInfo->append(strError);
}


void Widget::slotSendToServer()
{
    pcmd->setEnabled(false);
    QString filename;
    filename = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Image Files (*.jpg)"));
    if(filename == "")
    {
        pcmd->setEnabled(true);
        return;
    }
    QPixmap * pix = new QPixmap(filename);
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);
    QImage * img = new QImage;
    *img = pix->toImage();
    out << quint32(0);
    out << (qint16)img->format();
    out << (quint64)img->byteCount();
    out << (qint16)img->width();
    out << (qint16)img->height();
    out.writeRawData((char*)img->bits(), img->byteCount());

    out.device()->seek(0);
    out << quint32(arrBlock.size() - sizeof(quint32));
    m_pTcpSocket->write(arrBlock);
}

void Widget::slotConnected()
{
    m_ptxtInfo->append("You Are Connected!");
}

void Widget::reconnect()
{
    m_pTcpSocket->connectToHost(this->strHost, this->nPort);
}


Widget::~Widget()
{

}

