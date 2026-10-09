#ifndef ADAPTER_MCUPORTAGENT_H
#define ADAPTER_MCUPORTAGENT_H

#include <QObject>
#include <QString>

class QTimer;
class QinFineClient;
class CxthMcuClient;

/** Lives on the IO thread. Owns the active MCU serial client (one at a time). */
class McuPortAgent : public QObject
{
    Q_OBJECT
public:
    explicit McuPortAgent(QObject *parent = nullptr);

public slots:
    void init();
    void shutdown();

    bool openQinFine(const QString &port, int baud, int addr);
    bool openCxth(const QString &port, int baud);
    void closeMcu();

    void qfSetFlow(double mlMin);
    void qfSetPercent(int percent);
    void qfSetPressMin(double mpa);
    void qfSetPressMax(double mpa);
    void qfSetStartStop(bool run);
    void qfSetPause(bool on);
    void qfSetPurge(bool on);
    void qfPressZero();
    void qfSetPressCompen(int on);
    void qfSetWorkMode(int mode, int flag);
    void qfSetTableCmd(int sub, int cmd);
    void qfSetTablePoint(int sub, double a, double b);
    void qfGetTable(int sub);
    void qfSetLoadFloat(int sub, double v);
    void qfGetLoadFloat(int sub);

    void cxthSetFlowWord(uint word);
    void cxthStopMotor();

signals:
    void connectedChanged(bool on);
    void qfPressure(float mpa);
    void cxthPressureRaw(uint raw);
    void extPoint(int sub, float a, float b);
    void extFloat(int sub, float v);
    void extU8(int sub, int v);
    void errorText(const QString &text);
    void pollTick();

private slots:
    void onPoll();
    void onQfExtPoint(quint8 sub, float a, float b);
    void onQfExtFloat(quint8 sub, float v);
    void onQfExtU8(quint8 sub, quint8 v);
    void onCxthRaw(quint32 raw);

private:
    void bindQinFine();
    void bindCxth();
    void destroyClients();

    QinFineClient *m_qf = nullptr;
    CxthMcuClient *m_cxth = nullptr;
    QTimer *m_poll = nullptr;
    bool m_qfBound = false;
    bool m_cxthBound = false;
};

#endif
