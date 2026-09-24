#include "core/appsettings.h"

#include "dualbackup/jsondualbackup.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include "utils/configpaths.h"
#include "utils/qjsonshim.h"

namespace {

double defaultWordFactorForPump(int pumpType)
{
    static const double kFactor[] = {
        6.0, 1.5, 0.245, 0.11, 0.11, 0.05, 0.05, 0.025, 0.025, 0.008333, 0.008333
    };
    const int i = (pumpType < 0 || pumpType > 10) ? 0 : pumpType;
    return kFactor[i] * (4294967296.0) / 125000000.0 / 10.0;
}

QJsonArray serializeFlowTable(const QVector<RatePoint> &table)
{
    QJsonArray arr;
    for (const auto &p : table)
    {
        QJsonObject o;
        o.insert(QStringLiteral("rpm"), p.rpm);
        o.insert(QStringLiteral("rate"), p.rate);
        arr.append(o);
    }
    return arr;
}

QJsonArray serializePressTable(const QVector<PressPoint> &table)
{
    QJsonArray arr;
    for (const auto &p : table)
    {
        QJsonObject o;
        o.insert(QStringLiteral("adc"), p.adc);
        o.insert(QStringLiteral("pressure"), p.pressure);
        arr.append(o);
    }
    return arr;
}

QJsonArray serializePulseTable(const QVector<PulsePoint> &table)
{
    QJsonArray arr;
    for (const auto &p : table)
    {
        QJsonObject o;
        o.insert(QStringLiteral("position"), p.position);
        o.insert(QStringLiteral("factor"), p.factor);
        arr.append(o);
    }
    return arr;
}

QVector<RatePoint> parseFlowTable(const QJsonArray &arr)
{
    QVector<RatePoint> table;
    for (const QJsonValue &v : arr)
    {
        const QJsonObject o = v.toObject();
        RatePoint p;
        p.rpm = o.value(QStringLiteral("rpm")).toDouble();
        p.rate = o.value(QStringLiteral("rate")).toDouble();
        table.append(p);
    }
    return table;
}

QVector<PressPoint> parsePressTable(const QJsonArray &arr)
{
    QVector<PressPoint> table;
    for (const QJsonValue &v : arr)
    {
        const QJsonObject o = v.toObject();
        PressPoint p;
        p.adc = o.value(QStringLiteral("adc")).toDouble();
        p.pressure = o.value(QStringLiteral("pressure")).toDouble();
        table.append(p);
    }
    return table;
}

QVector<PulsePoint> parsePulseTable(const QJsonArray &arr)
{
    QVector<PulsePoint> table;
    for (const QJsonValue &v : arr)
    {
        const QJsonObject o = v.toObject();
        PulsePoint p;
        p.position = o.value(QStringLiteral("position")).toDouble();
        p.factor = o.value(QStringLiteral("factor")).toDouble(1.0);
        table.append(p);
    }
    return table;
}

} // namespace

AppSettings::AppSettings(QObject *parent)
    : QObject(parent)
{
    mcuWordFactor = defaultWordFactorForPump(0);
    ensureGradients();
}

QString AppSettings::resolveConfigDir() const
{
    const QString appDir = QCoreApplication::applicationDirPath();
    if (!appDir.isEmpty() && QDir(appDir).exists())
        return appDir;
    return ConfigPaths::writableAppConfigDir();
}

QString AppSettings::configPath() const
{
    return QDir(resolveConfigDir()).filePath(QStringLiteral("system.json"));
}

QString AppSettings::backupPath() const
{
    return QDir(resolveConfigDir()).filePath(QStringLiteral("system.json.bak"));
}

QString AppSettings::deviceInfoPath() const
{
    return QDir(resolveConfigDir()).filePath(QStringLiteral("deviceinfo.json"));
}

QString AppSettings::deviceInfoBackupPath() const
{
    return QDir(resolveConfigDir()).filePath(QStringLiteral("deviceinfo.json.bak"));
}

QString AppSettings::dataPath() const
{
    return QDir(resolveConfigDir()).filePath(QStringLiteral("data.json"));
}

QString AppSettings::dataBackupPath() const
{
    return QDir(resolveConfigDir()).filePath(QStringLiteral("data.json.bak"));
}

void AppSettings::applyPumpTypeFactor()
{
    mcuWordFactor = defaultWordFactorForPump(pumpType);
}

double AppSettings::defaultMaxPressForPump() const
{
    static const double kMax[] = {42, 25, 20, 20, 20, 15, 15, 10, 10, 10, 10};
    const int i = (pumpType < 0 || pumpType > 10) ? 0 : pumpType;
    return kMax[i];
}

void AppSettings::ensureGradients()
{
    if (gradients.size() < 10)
        gradients.resize(10);
    for (int i = 0; i < 10; ++i)
    {
        if (gradients[i].isEmpty())
        {
            const GradientPoint p0(0.0, 1.0);
            const GradientPoint p1(10.0, 1.0);
            gradients[i] = {p0, p1};
        }
    }
}

QVector<GradientPoint> &AppSettings::gradientTable(int which)
{
    ensureGradients();
    return gradients[qBound(0, which, 9)];
}

const QVector<GradientPoint> &AppSettings::gradientTable(int which) const
{
    return const_cast<AppSettings *>(this)->gradientTable(which);
}

bool AppSettings::parseDeviceInfoJson(const QByteArray &raw)
{
    const QJsonDocument doc = QJsonDocument::fromJson(raw);
    if (!doc.isObject())
        return false;
    const QJsonObject o = doc.object();

    license = o.value(QStringLiteral("license")).toString(license);
    serial = o.value(QStringLiteral("serial")).toString(serial);
    serialId = quint32(o.value(QStringLiteral("serialId")).toDouble(0));
    bActive = o.value(QStringLiteral("bActive")).toBool(false);
    tryDay = o.value(QStringLiteral("tryDay")).toInt(14);
    bugleCnt = quint32(o.value(QStringLiteral("bugleCnt")).toDouble(0));
    sysUsedSec = quint32(o.value(QStringLiteral("sysUsedSec")).toDouble(0));
    pumpUsedSec = quint32(o.value(QStringLiteral("pumpUsedSec")).toDouble(0));
    manufYear = o.value(QStringLiteral("manufYear")).toString(manufYear);
    manufMonth = o.value(QStringLiteral("manufMonth")).toString(manufMonth);
    manufDay = o.value(QStringLiteral("manufDay")).toString(manufDay);
    instYear = o.value(QStringLiteral("instYear")).toString(instYear);
    instMonth = o.value(QStringLiteral("instMonth")).toString(instMonth);
    instDay = o.value(QStringLiteral("instDay")).toString(instDay);
    repairYear = o.value(QStringLiteral("repairYear")).toString(repairYear);
    repairMonth = o.value(QStringLiteral("repairMonth")).toString(repairMonth);
    repairDay = o.value(QStringLiteral("repairDay")).toString(repairDay);
    return true;
}

bool AppSettings::parseSystemJson(const QByteArray &raw)
{
    const QJsonDocument doc = QJsonDocument::fromJson(raw);
    if (!doc.isObject())
        return false;
    const QJsonObject o = doc.object();
    const QJsonObject mcu = o.value(QStringLiteral("mcu")).toObject();
    const QJsonObject pc = o.value(QStringLiteral("pc")).toObject();
    const QJsonObject ui = o.value(QStringLiteral("ui")).toObject();
    const QJsonObject sec = o.value(QStringLiteral("security")).toObject();
    const QJsonObject net = o.value(QStringLiteral("net")).toObject();
    const QJsonObject calib = o.value(QStringLiteral("calib")).toObject();

    mcuProtocol = McuProtocol(mcu.value(QStringLiteral("protocol")).toInt(int(Legacy)));
    mcuPort = mcu.value(QStringLiteral("port")).toString();
    mcuBaud = mcu.value(QStringLiteral("baud")).toInt(mcuProtocol == QinFine ? 115200 : 9600);
    mcuAddress = quint8(mcu.value(QStringLiteral("address")).toInt(1));
    pumpType = mcu.value(QStringLiteral("pumpType")).toInt(0);
    if (mcu.contains(QStringLiteral("wordFactor")))
        mcuWordFactor = mcu.value(QStringLiteral("wordFactor")).toDouble();
    else
        applyPumpTypeFactor();
    pressRawV0 = quint32(mcu.value(QStringLiteral("pressRawV0")).toDouble(0));
    pressRawScale = mcu.value(QStringLiteral("pressRawScale")).toDouble(0.0128);

    pcProtocol = PcProtocol(pc.value(QStringLiteral("protocol")).toInt(int(Clarity)));
    pcPort = PcPort(pc.value(QStringLiteral("portType")).toInt(int(Udp)));
    pcSerialPort = pc.value(QStringLiteral("serial")).toString();
    pcSerialBaud = pc.value(QStringLiteral("serialBaud")).toInt(9600);
    localUdpPort = quint16(pc.value(QStringLiteral("localUdp")).toInt(8080));
    remoteIp = pc.value(QStringLiteral("remoteIp")).toString(QStringLiteral("127.0.0.1"));
    remotePort = quint16(pc.value(QStringLiteral("remotePort")).toInt(8081));
    machineCode = quint8(pc.value(QStringLiteral("machineCode")).toInt(0x12));

    if (net.contains(QStringLiteral("localIp")))
        localIp = net.value(QStringLiteral("localIp")).toString(localIp);
    else if (pc.contains(QStringLiteral("localIp")))
        localIp = pc.value(QStringLiteral("localIp")).toString(localIp);

    scale = qBound(1, ui.value(QStringLiteral("scale")).toInt(1), 3);
    autoConnect = ui.value(QStringLiteral("autoConnect")).toBool(true);
    language = Language(ui.value(QStringLiteral("language")).toInt(int(English)));

    adminPwd = sec.value(QStringLiteral("adminPwd")).toString(adminPwd);
    userPwd = sec.value(QStringLiteral("userPwd")).toString(userPwd);

    gradients.clear();
    const QJsonArray garr = o.value(QStringLiteral("gradients")).toArray();
    for (const QJsonValue &tv : garr)
    {
        QVector<GradientPoint> table;
        for (const QJsonValue &pv : tv.toArray())
        {
            const QJsonObject po = pv.toObject();
            GradientPoint gp;
            gp.timeMin = po.value(QStringLiteral("t")).toDouble();
            gp.flow = po.value(QStringLiteral("f")).toDouble();
            table.append(gp);
        }
        gradients.append(table);
    }
    ensureGradients();

    flowTable = parseFlowTable(calib.value(QStringLiteral("flowTable")).toArray());
    pressTable = parsePressTable(calib.value(QStringLiteral("pressTable")).toArray());
    pulseTable = parsePulseTable(calib.value(QStringLiteral("pulseTable")).toArray());
    pressCompen = quint8(calib.value(QStringLiteral("pressCompen")).toInt(0));
    loadRate = calib.value(QStringLiteral("loadRate")).toDouble(0);
    loadReal = calib.value(QStringLiteral("loadReal")).toDouble(0);
    loadPress = calib.value(QStringLiteral("loadPress")).toDouble(0);
    return true;
}

bool AppSettings::parseDataJson(const QByteArray &raw)
{
    const QJsonDocument doc = QJsonDocument::fromJson(raw);
    if (!doc.isObject())
        return false;
    const QJsonObject o = doc.object();

    flowSet = o.value(QStringLiteral("flow")).toDouble(1.0);
    percent = o.value(QStringLiteral("percent")).toDouble(100.0);
    coefficient = o.value(QStringLiteral("coefficient")).toDouble(100.0);
    pressMin = o.value(QStringLiteral("pmin")).toDouble(0.0);
    pressMax = o.value(QStringLiteral("pmax")).toDouble(42.0);
    pmaxLimit = o.value(QStringLiteral("pmaxLimit")).toDouble(0.0);
    purgeFlow = o.value(QStringLiteral("purgeFlow")).toDouble(5.0);
    gradientIndex = o.value(QStringLiteral("gradient")).toInt(0);
    currentGradient = o.value(QStringLiteral("currentGradient")).toInt(0);
    return true;
}

bool AppSettings::parseLegacyMonolithicJson(const QByteArray &raw)
{
    const QJsonDocument doc = QJsonDocument::fromJson(raw);
    if (!doc.isObject())
        return false;
    const QJsonObject o = doc.object();
    if (!o.contains(QStringLiteral("glp")) && !o.contains(QStringLiteral("run")))
        return false;

    const QJsonObject mcu = o.value(QStringLiteral("mcu")).toObject();
    const QJsonObject pc = o.value(QStringLiteral("pc")).toObject();
    const QJsonObject ui = o.value(QStringLiteral("ui")).toObject();
    const QJsonObject run = o.value(QStringLiteral("run")).toObject();
    const QJsonObject glp = o.value(QStringLiteral("glp")).toObject();
    const QJsonObject sec = o.value(QStringLiteral("security")).toObject();
    const QJsonObject net = o.value(QStringLiteral("net")).toObject();

    mcuProtocol = McuProtocol(mcu.value(QStringLiteral("protocol")).toInt(int(Legacy)));
    mcuPort = mcu.value(QStringLiteral("port")).toString();
    mcuBaud = mcu.value(QStringLiteral("baud")).toInt(mcuProtocol == QinFine ? 115200 : 9600);
    mcuAddress = quint8(mcu.value(QStringLiteral("address")).toInt(1));
    pumpType = mcu.value(QStringLiteral("pumpType")).toInt(0);
    if (mcu.contains(QStringLiteral("wordFactor")))
        mcuWordFactor = mcu.value(QStringLiteral("wordFactor")).toDouble();
    else
        applyPumpTypeFactor();
    pressRawV0 = quint32(mcu.value(QStringLiteral("pressRawV0")).toDouble(0));
    pressRawScale = mcu.value(QStringLiteral("pressRawScale")).toDouble(0.0128);

    pcProtocol = PcProtocol(pc.value(QStringLiteral("protocol")).toInt(int(Clarity)));
    pcPort = PcPort(pc.value(QStringLiteral("portType")).toInt(int(Udp)));
    pcSerialPort = pc.value(QStringLiteral("serial")).toString();
    pcSerialBaud = pc.value(QStringLiteral("serialBaud")).toInt(9600);
    localUdpPort = quint16(pc.value(QStringLiteral("localUdp")).toInt(8080));
    remoteIp = pc.value(QStringLiteral("remoteIp")).toString(QStringLiteral("127.0.0.1"));
    remotePort = quint16(pc.value(QStringLiteral("remotePort")).toInt(8081));
    machineCode = quint8(pc.value(QStringLiteral("machineCode")).toInt(0x12));

    if (net.contains(QStringLiteral("localIp")))
        localIp = net.value(QStringLiteral("localIp")).toString(localIp);
    else if (pc.contains(QStringLiteral("localIp")))
        localIp = pc.value(QStringLiteral("localIp")).toString(localIp);

    scale = qBound(1, ui.value(QStringLiteral("scale")).toInt(1), 3);
    autoConnect = ui.value(QStringLiteral("autoConnect")).toBool(true);
    language = Language(ui.value(QStringLiteral("language")).toInt(int(English)));

    flowSet = run.value(QStringLiteral("flow")).toDouble(1.0);
    percent = run.value(QStringLiteral("percent")).toDouble(100.0);
    coefficient = run.value(QStringLiteral("coefficient")).toDouble(100.0);
    pressMin = run.value(QStringLiteral("pmin")).toDouble(0.0);
    pressMax = run.value(QStringLiteral("pmax")).toDouble(42.0);
    pmaxLimit = run.value(QStringLiteral("pmaxLimit")).toDouble(0.0);
    purgeFlow = run.value(QStringLiteral("purgeFlow")).toDouble(5.0);
    gradientIndex = run.value(QStringLiteral("gradient")).toInt(0);
    currentGradient = run.value(QStringLiteral("currentGradient")).toInt(0);

    license = glp.value(QStringLiteral("license")).toString(license);
    serial = glp.value(QStringLiteral("serial")).toString(serial);
    bActive = glp.value(QStringLiteral("bActive")).toBool(false);
    tryDay = glp.value(QStringLiteral("tryDay")).toInt(14);
    bugleCnt = quint32(glp.value(QStringLiteral("bugleCnt")).toDouble(0));
    sysUsedSec = quint32(glp.value(QStringLiteral("sysUsedSec")).toDouble(0));
    pumpUsedSec = quint32(glp.value(QStringLiteral("pumpUsedSec")).toDouble(0));
    manufYear = glp.value(QStringLiteral("manufYear")).toString(manufYear);
    manufMonth = glp.value(QStringLiteral("manufMonth")).toString(manufMonth);
    manufDay = glp.value(QStringLiteral("manufDay")).toString(manufDay);
    instYear = glp.value(QStringLiteral("instYear")).toString(instYear);
    instMonth = glp.value(QStringLiteral("instMonth")).toString(instMonth);
    instDay = glp.value(QStringLiteral("instDay")).toString(instDay);
    repairYear = glp.value(QStringLiteral("repairYear")).toString(repairYear);
    repairMonth = glp.value(QStringLiteral("repairMonth")).toString(repairMonth);
    repairDay = glp.value(QStringLiteral("repairDay")).toString(repairDay);

    adminPwd = sec.value(QStringLiteral("adminPwd")).toString(adminPwd);
    userPwd = sec.value(QStringLiteral("userPwd")).toString(userPwd);

    gradients.clear();
    const QJsonArray garr = o.value(QStringLiteral("gradients")).toArray();
    for (const QJsonValue &tv : garr)
    {
        QVector<GradientPoint> table;
        for (const QJsonValue &pv : tv.toArray())
        {
            const QJsonObject po = pv.toObject();
            GradientPoint gp;
            gp.timeMin = po.value(QStringLiteral("t")).toDouble();
            gp.flow = po.value(QStringLiteral("f")).toDouble();
            table.append(gp);
        }
        gradients.append(table);
    }
    ensureGradients();

    const QJsonObject calib = o.value(QStringLiteral("calib")).toObject();
    flowTable = parseFlowTable(calib.value(QStringLiteral("flowTable")).toArray());
    pressTable = parsePressTable(calib.value(QStringLiteral("pressTable")).toArray());
    pulseTable = parsePulseTable(calib.value(QStringLiteral("pulseTable")).toArray());
    pressCompen = quint8(calib.value(QStringLiteral("pressCompen")).toInt(0));
    loadRate = calib.value(QStringLiteral("loadRate")).toDouble(0);
    loadReal = calib.value(QStringLiteral("loadReal")).toDouble(0);
    loadPress = calib.value(QStringLiteral("loadPress")).toDouble(0);
    return true;
}

bool AppSettings::load()
{
    const auto deviceResult = JsonDualBackup::load(deviceInfoPath(), deviceInfoBackupPath());
    const auto systemResult = JsonDualBackup::load(configPath(), backupPath());
    const auto dataResult = JsonDualBackup::load(dataPath(), dataBackupPath());

    if (deviceResult.source == JsonDualBackup::Source::Backup)
        qWarning() << "deviceinfo primary invalid; loaded backup" << deviceInfoBackupPath();
    if (systemResult.source == JsonDualBackup::Source::Backup)
        qWarning() << "system primary invalid; loaded backup" << backupPath();
    if (dataResult.source == JsonDualBackup::Source::Backup)
        qWarning() << "data primary invalid; loaded backup" << dataBackupPath();

    bool deviceOk = false;
    bool systemOk = false;
    bool dataOk = false;

    if (deviceResult.source != JsonDualBackup::Source::None)
        deviceOk = parseDeviceInfoJson(deviceResult.data);
    if (systemResult.source != JsonDualBackup::Source::None)
        systemOk = parseSystemJson(systemResult.data);
    if (dataResult.source != JsonDualBackup::Source::None)
        dataOk = parseDataJson(dataResult.data);

    if (deviceOk && systemOk && dataOk)
        return true;

    if (systemResult.source != JsonDualBackup::Source::None
        && parseLegacyMonolithicJson(systemResult.data))
    {
        qInfo() << "Migrating legacy monolithic system.json to split config files";
        save();
        return true;
    }

    if (!deviceOk && !systemOk && !dataOk)
    {
        applyPumpTypeFactor();
        ensureGradients();
        return false;
    }

    if (!systemOk)
        applyPumpTypeFactor();
    ensureGradients();
    return deviceOk || systemOk || dataOk;
}

bool AppSettings::importFromJson(const QString &path)
{
    QByteArray raw;
    if (!JsonDualBackup::readChecked(path, raw))
        return false;

    const QJsonDocument doc = QJsonDocument::fromJson(raw);
    if (!doc.isObject())
        return false;
    const QJsonObject o = doc.object();

    bool ok = false;
    if (o.contains(QStringLiteral("glp")) || o.contains(QStringLiteral("run")))
        ok = parseLegacyMonolithicJson(raw);
    else if (o.contains(QStringLiteral("serial")) || o.contains(QStringLiteral("manufYear")))
        ok = parseDeviceInfoJson(raw);
    else if (o.contains(QStringLiteral("mcu")) || o.contains(QStringLiteral("gradients")))
        ok = parseSystemJson(raw);
    else if (o.contains(QStringLiteral("flow")) || o.contains(QStringLiteral("pmin")))
        ok = parseDataJson(raw);
    else
        return false;

    if (!ok)
        return false;
    emit changed();
    return save();
}

QByteArray AppSettings::serializeDeviceInfoJson() const
{
    QJsonObject o;
    o.insert(QStringLiteral("version"), 1);
    o.insert(QStringLiteral("license"), license);
    o.insert(QStringLiteral("serial"), serial);
    o.insert(QStringLiteral("serialId"), double(serialId));
    o.insert(QStringLiteral("bActive"), bActive);
    o.insert(QStringLiteral("tryDay"), tryDay);
    o.insert(QStringLiteral("bugleCnt"), double(bugleCnt));
    o.insert(QStringLiteral("sysUsedSec"), double(sysUsedSec));
    o.insert(QStringLiteral("pumpUsedSec"), double(pumpUsedSec));
    o.insert(QStringLiteral("manufYear"), manufYear);
    o.insert(QStringLiteral("manufMonth"), manufMonth);
    o.insert(QStringLiteral("manufDay"), manufDay);
    o.insert(QStringLiteral("instYear"), instYear);
    o.insert(QStringLiteral("instMonth"), instMonth);
    o.insert(QStringLiteral("instDay"), instDay);
    o.insert(QStringLiteral("repairYear"), repairYear);
    o.insert(QStringLiteral("repairMonth"), repairMonth);
    o.insert(QStringLiteral("repairDay"), repairDay);
    return QJsonDocument(o).toJson(QJsonDocument::Indented);
}

QByteArray AppSettings::serializeSystemJson() const
{
    QJsonObject mcu;
    mcu.insert(QStringLiteral("protocol"), int(mcuProtocol));
    mcu.insert(QStringLiteral("port"), mcuPort);
    mcu.insert(QStringLiteral("baud"), mcuBaud);
    mcu.insert(QStringLiteral("address"), int(mcuAddress));
    mcu.insert(QStringLiteral("pumpType"), pumpType);
    mcu.insert(QStringLiteral("wordFactor"), mcuWordFactor);
    mcu.insert(QStringLiteral("pressRawV0"), double(pressRawV0));
    mcu.insert(QStringLiteral("pressRawScale"), pressRawScale);

    QJsonObject pc;
    pc.insert(QStringLiteral("protocol"), int(pcProtocol));
    pc.insert(QStringLiteral("portType"), int(pcPort));
    pc.insert(QStringLiteral("serial"), pcSerialPort);
    pc.insert(QStringLiteral("serialBaud"), pcSerialBaud);
    pc.insert(QStringLiteral("localUdp"), int(localUdpPort));
    pc.insert(QStringLiteral("remoteIp"), remoteIp);
    pc.insert(QStringLiteral("remotePort"), int(remotePort));
    pc.insert(QStringLiteral("machineCode"), int(machineCode));

    QJsonObject net;
    net.insert(QStringLiteral("localIp"), localIp);

    QJsonObject ui;
    ui.insert(QStringLiteral("scale"), scale);
    ui.insert(QStringLiteral("autoConnect"), autoConnect);
    ui.insert(QStringLiteral("language"), int(language));

    QJsonObject sec;
    sec.insert(QStringLiteral("adminPwd"), adminPwd);
    sec.insert(QStringLiteral("userPwd"), userPwd);

    QJsonObject calib;
    calib.insert(QStringLiteral("flowTable"), serializeFlowTable(flowTable));
    calib.insert(QStringLiteral("pressTable"), serializePressTable(pressTable));
    calib.insert(QStringLiteral("pulseTable"), serializePulseTable(pulseTable));
    calib.insert(QStringLiteral("pressCompen"), int(pressCompen));
    calib.insert(QStringLiteral("loadRate"), loadRate);
    calib.insert(QStringLiteral("loadReal"), loadReal);
    calib.insert(QStringLiteral("loadPress"), loadPress);

    QJsonArray garr;
    for (const auto &table : gradients)
    {
        QJsonArray ta;
        for (const auto &gp : table)
        {
            QJsonObject po;
            po.insert(QStringLiteral("t"), gp.timeMin);
            po.insert(QStringLiteral("f"), gp.flow);
            ta.append(po);
        }
        garr.append(ta);
    }

    QJsonObject root;
    root.insert(QStringLiteral("version"), 3);
    root.insert(QStringLiteral("mcu"), mcu);
    root.insert(QStringLiteral("pc"), pc);
    root.insert(QStringLiteral("net"), net);
    root.insert(QStringLiteral("ui"), ui);
    root.insert(QStringLiteral("security"), sec);
    root.insert(QStringLiteral("gradients"), garr);
    root.insert(QStringLiteral("calib"), calib);

    return QJsonDocument(root).toJson(QJsonDocument::Indented);
}

QByteArray AppSettings::serializeDataJson() const
{
    QJsonObject o;
    o.insert(QStringLiteral("version"), 1);
    o.insert(QStringLiteral("flow"), flowSet);
    o.insert(QStringLiteral("percent"), percent);
    o.insert(QStringLiteral("coefficient"), coefficient);
    o.insert(QStringLiteral("pmin"), pressMin);
    o.insert(QStringLiteral("pmax"), pressMax);
    o.insert(QStringLiteral("pmaxLimit"), pmaxLimit);
    o.insert(QStringLiteral("purgeFlow"), purgeFlow);
    o.insert(QStringLiteral("gradient"), gradientIndex);
    o.insert(QStringLiteral("currentGradient"), currentGradient);
    return QJsonDocument(o).toJson(QJsonDocument::Indented);
}

bool AppSettings::save() const
{
    const bool deviceOk = JsonDualBackup::save(
        deviceInfoPath(), deviceInfoBackupPath(), serializeDeviceInfoJson());
    const bool systemOk = JsonDualBackup::save(
        configPath(), backupPath(), serializeSystemJson());
    const bool dataOk = JsonDualBackup::save(
        dataPath(), dataBackupPath(), serializeDataJson());
    return deviceOk && systemOk && dataOk;
}
