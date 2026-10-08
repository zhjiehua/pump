#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

struct GradientPoint {
    double timeMin;
    double flow;
    GradientPoint() : timeMin(0), flow(0) {}
    GradientPoint(double t, double f) : timeMin(t), flow(f) {}
};

struct RatePoint {
    double rpm;
    double rate;
    RatePoint() : rpm(0), rate(0) {}
    RatePoint(double r, double ra) : rpm(r), rate(ra) {}
};
struct PressPoint {
    double adc;
    double pressure;
    PressPoint() : adc(0), pressure(0) {}
    PressPoint(double a, double p) : adc(a), pressure(p) {}
};
struct PulsePoint {
    double position;
    double factor;
    PulsePoint() : position(0), factor(1) {}
    PulsePoint(double pos, double f) : position(pos), factor(f) {}
};

class AppSettings : public QObject
{
    Q_OBJECT
public:
    enum McuProtocol {
        Cxth = 0,     ///< CXTH MCU, legacy 0x80 UART
        QinFine = 1,  ///< QinFine MCU (reserved)
        Legacy = Cxth ///< historical name
    };
    enum PcProtocol { LegacyPc = 0, Clarity = 1, QinFinePc = 2 };
    enum PcPort { Serial = 0, Udp = 1, TcpServer = 2 };
    enum Language { English = 0, Chinese = 1 };

    explicit AppSettings(QObject *parent = nullptr);

    bool load();
    bool save() const;
    /** Load settings from an external JSON file and persist to config paths. */
    bool importFromJson(const QString &path);
    QByteArray exportBundleJson() const;
    bool importBundleJson(const QByteArray &raw);

    QString configPath() const;
    QString backupPath() const;
    QString deviceInfoPath() const;
    QString deviceInfoBackupPath() const;
    QString dataPath() const;
    QString dataBackupPath() const;
    QString deviceInfoFactoryPath() const;
    QString deviceInfoFactoryBackupPath() const;
    QString configFactoryPath() const;
    QString configFactoryBackupPath() const;
    QString dataFactoryPath() const;
    QString dataFactoryBackupPath() const;

    /** Snapshot current JSON set to *.factory.json (debugged “golden” copy). */
    bool saveFactorySnapshot() const;
    /** Restore runtime JSON from factory snapshot; returns false if snapshot missing. */
    bool restoreFactorySnapshot();
    bool hasFactorySnapshot() const;
    /** Reset all fields to compile-time defaults (first power-on). */
    void resetToBuiltInDefaults();
    /** Flow/press tables from weiduodianzi SQLite defaults (factor 1 → identity). */
    void restoreDefaultCalibrationTables();

    int pumpType = 0;
    double pmaxLimit = 0.0; // Admin override; 0 = use pump-type default
    double mcuWordFactor = 6.0 * (4294967296.0) / 125000000.0 / 10.0;
    quint32 pressRawV0 = 0;
    double pressRawScale = 0.0128;

    McuProtocol mcuProtocol = Cxth;
    QString mcuPort;
    int mcuBaud = 9600;
    quint8 mcuAddress = 0x01;

    PcProtocol pcProtocol = Clarity;
    PcPort pcPort = Udp;
    QString pcSerialPort;
    int pcSerialBaud = 9600;
    quint16 localPort = 8080;
    QString remoteIp = QStringLiteral("127.0.0.1");
    quint16 remotePort = 8081;

    bool dhcp = false;
    QString localIp = QStringLiteral("192.168.1.100");
    QString subnet = QStringLiteral("255.255.255.0");
    QString gateway = QStringLiteral("192.168.1.1");

    int scale = 1; // 1 = native 320×240 (weiduodianzi)
    bool autoConnect = true;
    Language language = English;
    quint8 machineCode = 0x12;

    double flowSet = 1.0;
    double percent = 100.0;
    double coefficient = 100.0;
    double pressMin = 0.0;
    double pressMax = 42.0;
    double purgeFlow = 5.0;
    int gradientIndex = 0; // 0 = local table; >=10 = PC/FG control
    int currentGradient = 0;
    int gradientMode = 0; // 0=high, 1=low

    QString license = QStringLiteral("1111111111");
    QString serial = QStringLiteral("0000000000");
    QString adminPwd = QStringLiteral("173895");
    QString userPwd = QStringLiteral("111111");
    bool bActive = false;
    int tryDay = 14;
    quint32 serialId = 0;
    quint32 bugleCnt = 0;
    quint32 sysUsedSec = 0;
    quint32 pumpUsedSec = 0;

    QString manufYear = QStringLiteral("2015");
    QString manufMonth = QStringLiteral("10");
    QString manufDay = QStringLiteral("1");
    QString instYear = QStringLiteral("2015");
    QString instMonth = QStringLiteral("10");
    QString instDay = QStringLiteral("1");
    QString repairYear = QStringLiteral("0000");
    QString repairMonth = QStringLiteral("00");
    QString repairDay = QStringLiteral("0");

    /** One local gradient table (weiduodianzi GRADIENTTABLE0). */
    QVector<QVector<GradientPoint>> gradients;

    QVector<RatePoint> flowTable;
    QVector<PressPoint> pressTable;
    QVector<PulsePoint> pulseTable;
    quint8 pressCompen = 0;
    double loadRate = 0;
    double loadReal = 0;
    double loadPress = 0;

    void applyPumpTypeFactor();
    double defaultMaxFlowForPump() const;
    double defaultMaxPressForPump() const;
    void ensureGradients();
    QVector<GradientPoint> &gradientTable();
    const QVector<GradientPoint> &gradientTable() const;

signals:
    void changed();

private:
    bool parseDeviceInfoJson(const QByteArray &raw);
    bool parseSystemJson(const QByteArray &raw);
    bool parseDataJson(const QByteArray &raw);
    QByteArray serializeDeviceInfoJson() const;
    QByteArray serializeSystemJson() const;
    QByteArray serializeDataJson() const;
    QString resolveConfigDir() const;
};

#endif
