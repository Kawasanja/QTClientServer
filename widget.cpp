#include "widget.h"

Widget::Widget(int nPort, QWidget* parent) : QWidget(parent)
                                            , m_nNextBlockSize(0)
{
    txt = new QTextEdit(this);
    txt->setMinimumSize(300,300);
    txt->setReadOnly(true);
    btn = new QPushButton("Save");
    btn->setEnabled(false);
    m_ptcpServer = new QTcpServer(this);
        if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
            QMessageBox::critical(0,
                                  "Server Error",
                                  "Unable to start the server:"
                                  + m_ptcpServer->errorString());
            m_ptcpServer->close();
            return;
        }
        connect(m_ptcpServer, SIGNAL(newConnection()),
                        SLOT(slotNewConnection()));
        connect(btn, SIGNAL(clicked()), SLOT(slotSaveImage()));




        //Layout setup
        QVBoxLayout* pvbxLayout = new QVBoxLayout;
        pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
        pvbxLayout->addWidget(txt);
        pvbxLayout->addWidget(btn);
        setLayout(pvbxLayout);
}

void Widget::slotNewConnection()
{
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()),
                        SLOT(slotReadClient()));

    sendToClient(pClientSocket, "From Server: Connected!");
}

void Widget::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_7);
    if (!m_nNextBlockSize) {
        if (pClientSocket->bytesAvailable() < (int)sizeof(char)) {
            return;
        }
        btn->setEnabled(false);
        in >> m_nNextBlockSize;
        in >> format;
        in >> bytesCount;
        in >> width;
        in >> height;
        bytes = new uchar[bytesCount];
    }

    while(!in.atEnd())
    {
        in >> bytes[counter];
        counter++;
    }

    if(counter == bytesCount){
        btn->setEnabled(true);
        img = new QImage(bytes, width, height, (QImage::Format)format);
        pix = new QPixmap;
        pix->convertFromImage(*img);


       // m_ptxtInfo->append(time.toString() + " " + str);

        txt->setText(QTime::currentTime().toString() + " Incoming image");
        m_nNextBlockSize = 0;
        sendToClient(pClientSocket, "Server Response: Image recieved");
        counter = 0;
    }
}

void Widget::slotSaveImage()
{
    pix->save(QFileDialog::getSaveFileName(this, tr("Save File"),"",tr("Image Files (*.jpg)")));
}

void Widget::sendToClient(QTcpSocket* pSocket, const QString& str)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);
    out << quint16(0) << QTime::currentTime() << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}

Widget::~Widget()
{

}
