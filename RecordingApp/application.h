#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QAudioRecorder>
#include <QAudioProbe>
#include <QUrl>
#include <memory>
#include <QCoreApplication>
#include <QtMultimedia>

#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QAudioBuffer>

struct WavHeader {
    char riff[4];      // "RIFF"
    uint32_t fileSize; // Total file size (excluding header)
    char wave[4];      // "WAVE"
    char fmt[4];       // "fmt "
    uint32_t chunkSize; // Size of the "fmt " chunk
    uint16_t audioFormat; // 1 for PCM (uncompressed)
    uint16_t numChannels; // Number of channels
    uint32_t sampleRate; // Samples per second
    uint32_t avgBytesPerSec; // Bytes transferred per second
    uint16_t blockAlign;  // Size of a single frame (bytes)
    uint16_t bitsPerSample; // Bits per sample
    char data[4];       // "data"
    uint32_t dataSize;  // Size of the "data" chunk (audio data size)
};

class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString analyzedMessage READ analyzedMessage WRITE setAnalyzedMessage NOTIFY analyzedMessageChanged FINAL)
public:
    explicit Application(QObject *parent = nullptr);
    ~Application();

    void configureAudioRecorder();
    void listAvailableRecordingDevices();

    Q_INVOKABLE void startAudioRecording();
    Q_INVOKABLE void stopAudioRecording();
    Q_INVOKABLE void pauseAudioRecording();

    // Audio sender
    void startSendingViaSockets();
    void sendNextChunk();

    QString analyzedMessage();
    void setAnalyzedMessage(QString message);


    void readTextSocketData();
    void textSocketDisconnected();

signals:
    void analyzedMessageChanged();

private slots:
    void onStateChanged(QMediaRecorder::State state);

    // Audio sender
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();

    void textServerNewConnection();

    void processBuffer(const QAudioBuffer& buffer);

    void createWavHeader();
    void saveAndCloseWaveFile();

    void handleIncomingAudioSegment();

private:
    std::shared_ptr<QAudioRecorder> m_AudioRecorder;
    std::shared_ptr<QAudioProbe> m_AudioProbe;

    QString m_DriectoryLocation;
    QString m_AudioFileName;

    // Audio sender
    QString audioFilePath;
    QString m_TrimFilePath;
    QNetworkReply *reply;

    QTcpSocket* audioSocket;
    QTcpServer* textServer;

    WavHeader m_wavHeader;

    QFile file;
    QFile m_TrimFile;
    int chunkSize;
    size_t totalBytesWritten;
    QUrl audio_url;

    QString m_AnalyzedMessage;
    size_t m_waveDataSize;

    QTimer m_CutoffTimer;
    int m_CutOffInterval;
};

#endif // APPLICATION_H
