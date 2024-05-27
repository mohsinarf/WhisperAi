#include "application.h"
#include <QVideoEncoderSettings>
#include <QMultimedia>

Application::Application(QObject *parent) : QObject(parent)
{
    m_AudioRecorder = std::make_shared<QAudioRecorder>();
    m_AudioProbe = std::make_shared<QAudioProbe>();

    m_TrimFilePath = "TrimedFile.wav";
    m_waveDataSize = 0;

    m_CutOffInterval = 1500;
    m_CutoffTimer.setSingleShot(true);
    m_CutoffTimer.setInterval(m_CutOffInterval);
    connect(&m_CutoffTimer, &QTimer::timeout, this, &Application::handleIncomingAudioSegment);
    configureAudioRecorder();
    createWavHeader();

    textServer  = new QTcpServer(this);

    // Specify the file path of the audio file to be sent
    audioFilePath = "AudioRecorded.wav";

    chunkSize = 1024* 10;
    audio_url = ("http://192.168.0.33:5000");

    connect(textServer, &QTcpServer::newConnection, this, &Application::textServerNewConnection);
    if (!textServer->listen(QHostAddress::Any, 8080))
    {
        qDebug() << "Server could not start";
    }
    else
    {
        qDebug() << "Server started!";
    }

    m_AnalyzedMessage = "";
}

Application::~Application()
{
    if(audioSocket != nullptr)
        delete audioSocket;

    if(textServer != nullptr)
        delete textServer;
}

void Application::configureAudioRecorder()
{
    connect(m_AudioRecorder.get(), &QAudioRecorder::stateChanged, this, &Application::onStateChanged);

    QObject::connect(m_AudioProbe.get(), &QAudioProbe::audioBufferProbed,
                     this, &Application::processBuffer);

    QAudioEncoderSettings Settings;
    Settings.setCodec("audio/pcm"); // Use WAV format "audio/pcm"
    Settings.setSampleRate(44100); // Set sample rate to CD quality (44100 kHz)
    Settings.setBitRate(320000); // Set a higher bit rate for better quality
    Settings.setChannelCount(-1); // Use stereo recording for better spatial separation
    Settings.setQuality(QMultimedia::EncodingQuality::NormalQuality); // Set high-quality encoding

    m_DriectoryLocation = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)[1];
    m_AudioFileName = "AudioRecorded";
    m_AudioRecorder->setContainerFormat("wav");

    QDir dir(m_DriectoryLocation);
    if (!dir.exists())
        dir.mkpath(".");

    QUrl url = QUrl::fromLocalFile(m_DriectoryLocation + "/" + m_AudioFileName);
    qDebug()<<"url: "<<url;

    QString trimPath = m_DriectoryLocation + "/"+ m_TrimFilePath;
    qDebug()<<"TrimFilePath: "<<trimPath;
    m_TrimFile.setFileName(trimPath);

    m_AudioRecorder->setEncodingSettings(Settings);

    m_AudioRecorder->setOutputLocation(url);

    m_AudioProbe->setSource(m_AudioRecorder.get());
    listAvailableRecordingDevices();
}

void Application::listAvailableRecordingDevices()
{
        qDebug() << "Available recording devices:";
        QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
        for (const QAudioDeviceInfo &deviceInfo : devices) {
            qDebug() << "Device name:" << deviceInfo.deviceName();
            qDebug() << "    Supported codecs:" << deviceInfo.supportedCodecs();
            qDebug() << "    Supported sample rates:" << deviceInfo.supportedSampleRates();
            qDebug() << "    Supported channel counts:" << deviceInfo.supportedChannelCounts();
        }
}

void Application::onStateChanged(QMediaRecorder::State state)
{
    // Handle state changes
    switch (state) {
    case QAudioRecorder::RecordingState:
        qDebug() << "Recording...";
        break;
    case QAudioRecorder::StoppedState:
        qDebug() << "Recording stopped.";
        break;
    case QAudioRecorder::PausedState:
        qDebug() << "Recording paused.";
        break;
    default:
        break;
    }
}

void Application::startAudioRecording()
{
    qDebug()<<"startAudioRecording";
    m_AudioRecorder->record();
}

void Application::stopAudioRecording()
{
    qDebug()<<"stopAudioRecording";
    m_AudioRecorder->stop();
}

void Application::pauseAudioRecording()
{
    qDebug()<<"pauseAudioRecording";
    m_AudioRecorder->pause();
}

void Application::startSendingViaSockets()
{
    qDebug()<<"startSendingViaSockets";

    audioSocket = new QTcpSocket(this);
    connect(audioSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(audioSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(audioSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    connect(audioSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    audioSocket->connectToHost(audio_url.host(), 5000);

    if(!audioSocket->waitForConnected(5000))
    {
        qDebug() << "Error: " << audioSocket->errorString();
    }
}

void Application::sendNextChunk()
{
    QByteArray bytes = file.read(chunkSize);
    if (bytes.isEmpty()) {
        file.close();
        audioSocket->disconnectFromHost();
        return;
    }

    audioSocket->write(bytes);
    totalBytesWritten += bytes.size();

    //    qDebug() << "Sending sound bytes. Total bytes sent: " << totalBytesWritten;

    // Wait for the bytes to be actually written before sending the next chunk
    audioSocket->waitForBytesWritten();

    // Call sendNextChunk() recursively to send the next chunk
    sendNextChunk();
}

QString Application::analyzedMessage()
{
    return m_AnalyzedMessage;
}

void Application::setAnalyzedMessage(QString message)
{
    m_AnalyzedMessage = message;
    emit analyzedMessageChanged();
}

void Application::connected()
{
    qDebug() << "Connected";
    file.setFileName(m_DriectoryLocation+ "/" + m_TrimFilePath);
//    file.setFileName(m_DriectoryLocation+ "/" +audioFilePath);
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Sound file could not be opened";
        audioSocket->disconnectFromHost();
        return;
    }

    // Initialize the total bytes written
    totalBytesWritten = 0;

    sendNextChunk();
}

void Application::disconnected()
{
    qDebug() << "Disconnected";
    audioSocket->disconnect();
    audioSocket->close();
    audioSocket->deleteLater();
}

void Application::bytesWritten(qint64 bytes)
{
//    qDebug() << "Bytes written: " << bytes;
}

void Application::readyRead()
{
    qDebug() << "Ready Read";
    QByteArray responseData = audioSocket->readAll();
    if (responseData.endsWith('\n')) {
        qDebug() << "Received data from server:";
        qDebug() << responseData;
        responseData.clear();
    }
}

void Application::textServerNewConnection()
{
    QTcpSocket *socket = textServer->nextPendingConnection();

    //    connect(socket, &QTcpSocket::readyRead, this, [=]() {
    //        qDebug() << "Reading: " << socket->readAll();
    //    });

    //    connect(socket, &QTcpSocket::disconnected, this, [=]() {
    //        socket->deleteLater();
    //        qDebug() << "Client disconnected";
    //    });

    connect(socket, &QTcpSocket::readyRead, this, &Application::readTextSocketData);
    connect(socket, &QTcpSocket::disconnected, this, &Application::textSocketDisconnected);

    qDebug() << "Connected to client";
}

void Application::readTextSocketData()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());

    if (socket) {
        QString replyMessage = QString(socket->readAll());
        qDebug()<<"replyMessage: "<<replyMessage;
        setAnalyzedMessage(replyMessage);
    }
}

void Application::textSocketDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        socket->deleteLater();
        qDebug() << "Client disconnected";
    }
}

void Application::processBuffer(const QAudioBuffer& buffer)
{
    const qint64 frameCount = buffer.frameCount();
    const int channelCount = buffer.format().channelCount();
    const int sampleSize = buffer.format().sampleSize();

//    qDebug() << "Frame Count:" << frameCount << "Channel Count:" << channelCount <<
//                "Sample Rate:" << sampleRate << "Sample Size:" << sampleSize <<
//                "Sample Count:" << sampleCount;

    double energy = 0.0;

    // Calculate energy by summing squared samples
    for (qint64 i = 0; i < frameCount; ++i) {
        for (int j = 0; j < channelCount; ++j) {
            if (sampleSize == 8) {
                const qint8 *ptr = buffer.constData<qint8>() + i * channelCount + j;
                energy += pow(*ptr / 128.0, 2); // Normalizing to [-1, 1]
            } else if (sampleSize == 16) {
                const qint16 *ptr = buffer.constData<qint16>() + i * channelCount + j;
                energy += pow(*ptr / 32768.0, 2); // Normalizing to [-1, 1]
            } else if (sampleSize == 32) {
                const qint32 *ptr = buffer.constData<qint32>() + i * channelCount + j;
                energy += pow(*ptr / 2147483648.0, 2); // Normalizing to [-1, 1]
            }
        }
    }

    if(energy > 0.5)
        m_CutoffTimer.start();

    if (m_CutoffTimer.isActive()) {
        // Update the total size of the audio data
        qint64 dataSize = frameCount * channelCount * (sampleSize / 8);
        m_waveDataSize += dataSize;

        // Access audio data from buffer
        const qint16* data = buffer.constData<qint16>();

        if (m_TrimFile.isOpen()) {
//            qDebug() << "File open at position: " << m_TrimFile.pos();
            m_TrimFile.write(reinterpret_cast<const char*>(data), dataSize);
        } else {
            qDebug() << "File is not open: " << m_TrimFile.fileName();
        }

        // Reset the timer if energy is greater than 0.5
        if (energy > 0.5) {
//            qDebug() << "Reset audio timer.";
            m_CutoffTimer.start(m_CutOffInterval);
        }
    } else {
//        qDebug() << "Skipping current audio segment.";
    }
}

void Application::createWavHeader()
{
    qDebug()<<" Create new wave header";

    int sampleRate = 44100;
    int channelCount = 1;
    int sampleSize = 16;

    // Calculate header values
    int blockAlign = channelCount * (sampleSize / 8);

    // Build the WAV header
    memcpy(m_wavHeader.riff, "RIFF", 4);

    m_wavHeader.fileSize = sizeof(WavHeader) - 8 + m_waveDataSize;

    memcpy(m_wavHeader.wave, "WAVE", 4);
    memcpy(m_wavHeader.fmt, "fmt ", 4);
    m_wavHeader.chunkSize = 16;
    m_wavHeader.audioFormat = 1; // PCM
    m_wavHeader.numChannels = channelCount;
    m_wavHeader.sampleRate = sampleRate;
    m_wavHeader.avgBytesPerSec = sampleRate * blockAlign;
    m_wavHeader.blockAlign = 2;    // numChannels * (bitsPerSample / 8)
    m_wavHeader.bitsPerSample = 16; // 8, 16, 24, etc.
    memcpy(m_wavHeader.data, "data", 4);
    m_wavHeader.dataSize = m_waveDataSize;

    if (m_TrimFile.open(QIODevice::WriteOnly)) {
        qDebug()<<"Position: " << m_TrimFile.pos();
        m_TrimFile.write(reinterpret_cast<const char*>(&m_wavHeader), sizeof(m_wavHeader));
    }
    else
        qDebug()<<" Could not open file to data: " << m_TrimFile.fileName();
}

void Application::saveAndCloseWaveFile()
{
    // update wave file header
    m_wavHeader.fileSize = sizeof(WavHeader) - 8 + m_waveDataSize;
    m_wavHeader.dataSize = m_waveDataSize;

    if (m_TrimFile.isOpen()) {
        qDebug()<<"File position before updating header: " << m_TrimFile.pos();
        if(m_TrimFile.seek(0)){
            qDebug()<<"Position: " << m_TrimFile.pos();
            m_TrimFile.write(reinterpret_cast<const char*>(&m_wavHeader), sizeof(m_wavHeader));
            m_TrimFile.close();
        }

        else{
            qDebug()<<"Could n't update at specified position";
        }
    }
    else
        qDebug()<<" Could not open file to update header data: " << m_TrimFile.fileName();
}

void Application::handleIncomingAudioSegment()
{
    qDebug()<<"handleIncomingAudioSegment.";

    // Save wave file
    saveAndCloseWaveFile();

    // send for transcribing
    startSendingViaSockets();

    createWavHeader();
}


