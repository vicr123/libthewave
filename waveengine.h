#ifndef WAVEENGINE_H
#define WAVEENGINE_H

#include "libthewave_global.h"
#include <QObject>
#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QEventLoop>

struct LIBTHEWAVESHARED_EXPORT PaneProperties
{
    QString title;
    QString field1;
    QString field2;
    QString field3;

    QString buttonTitle;
    QString buttonSpeechAction;
    QString buttonIcon;

    bool canOk = false;
};

class LIBTHEWAVESHARED_EXPORT WaveEngine : public QObject
{
    Q_OBJECT
public:
    WaveEngine(QObject* parent = 0);

    enum Pane {
        None,
        Email,
        Calculator,
        Reminder,
        Help
    };

    Pane getCurrentPane();

public slots:
    void processString(QString input);
    void cancelCurrentPane();
    void okCurrentPane();

private slots:
    void processEmailString(QString input);
    void processCalculationString(QString input);
    void processReminderString(QString input);
    void processWhatTimeString(QString input);

signals:
    void outputString(QString output);
    void outputSpeech(QString output, bool startMic = false);
    void outputInlinePane(PaneProperties properties);
    void changePane(WaveEngine::Pane pane);
    void changePaneProperties(PaneProperties properties);

private:
    Pane currentPane = None;
    int paneStage = 0;
    PaneProperties currentPaneProperties;
};

#endif // WAVEENGINE_H
