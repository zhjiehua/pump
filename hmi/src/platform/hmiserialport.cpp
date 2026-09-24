#include "platform/hmiserialport.h"

#include <QTimer>

#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
#include <QSerialPortInfo>
#else
#include <QDir>
#include <QFileInfo>
#endif

namespace {

#if QT_VERSION < QT_VERSION_CHECK(5, 1, 0)
BaudRateType baudToQext(int baud)
{
    switch (baud)
    {
    case 4800: return BAUD4800;
    case 9600: return BAUD9600;
    case 19200: return BAUD19200;
    case 38400: return BAUD38400;
    case 57600: return BAUD57600;
    case 115200: return BAUD115200;
    default: return BAUD9600;
    }
}
#endif

} // namespace

HmiSerialPort::HmiSerialPort(QObject *parent)
    : QObject(parent)
#if QT_VERSION < QT_VERSION_CHECK(5, 1, 0)
    , m_port(0)
    , m_poll(0)
#endif
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    connect(&m_port, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
#else
    m_poll = new QTimer(this);
    m_poll->setInterval(50);
    connect(m_poll, SIGNAL(timeout()), this, SLOT(onPoll()));
#endif
}

HmiSerialPort::~HmiSerialPort()
{
    close();
#if QT_VERSION < QT_VERSION_CHECK(5, 1, 0)
    delete m_port;
    m_port = 0;
#endif
}

bool HmiSerialPort::open(const QString &port, int baud)
{
    close();
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    m_port.setPortName(port);
    m_port.setBaudRate(baud);
    m_port.setDataBits(QSerialPort::Data8);
    m_port.setParity(QSerialPort::NoParity);
    m_port.setStopBits(QSerialPort::OneStop);
    m_port.setFlowControl(QSerialPort::NoFlowControl);
    return m_port.open(QIODevice::ReadWrite);
#else
    m_port = new Posix_QextSerialPort(port, QextSerialBase::Polling);
    if (!m_port->open(QIODevice::ReadWrite))
    {
        m_lastError = QString::fromLatin1("open failed: %1").arg(port);
        delete m_port;
        m_port = 0;
        return false;
    }
    m_port->setTimeout(10);
    m_port->setBaudRate(baudToQext(baud));
    m_port->setDataBits(DATA_8);
    m_port->setStopBits(STOP_1);
    m_port->setParity(PAR_NONE);
    m_port->setFlowControl(FLOW_OFF);
    m_poll->start();
    return true;
#endif
}

void HmiSerialPort::close()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    if (m_port.isOpen())
        m_port.close();
#else
    if (m_poll)
        m_poll->stop();
    if (m_port)
    {
        if (m_port->isOpen())
            m_port->close();
        delete m_port;
        m_port = 0;
    }
#endif
}

bool HmiSerialPort::isOpen() const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    return m_port.isOpen();
#else
    return m_port && m_port->isOpen();
#endif
}

qint64 HmiSerialPort::write(const QByteArray &data)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    return m_port.write(data);
#else
    if (!m_port || !m_port->isOpen())
        return -1;
    return m_port->write(data.constData(), data.size());
#endif
}

QByteArray HmiSerialPort::readAll()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    return m_port.readAll();
#else
    if (!m_port || !m_port->isOpen())
        return QByteArray();
    return m_port->readAll();
#endif
}

QString HmiSerialPort::errorString() const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    return m_port.errorString();
#else
    return m_lastError;
#endif
}

void HmiSerialPort::onPoll()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 1, 0)
    if (m_port && m_port->isOpen() && m_port->bytesAvailable() > 0)
        emit readyRead();
#endif
}

QStringList HmiSerialPortInfo::availablePortNames()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    QStringList names;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        names << info.portName();
    return names;
#else
    QStringList names;
    const QFileInfoList entries = QDir(QString::fromLatin1("/dev")).entryInfoList(
        QStringList() << QString::fromLatin1("ttySAC*")
                      << QString::fromLatin1("ttyUSB*")
                      << QString::fromLatin1("ttyS*"),
        QDir::System);
    foreach (const QFileInfo &fi, entries)
        names << QString::fromLatin1("/dev/") + fi.fileName();
    names.sort();
    return names;
#endif
}
