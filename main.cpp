#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QQmlComponent>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QDataStream>
#include <QTimer>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>
#include <bluetooth_wrapper.hpp>
#include <QQmlProperty>
#include <QQuickItem>


#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

using namespace std;

class BackendStuff : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString receiveBuffer READ getBuffer NOTIFY bufferChanged)
    Q_PROPERTY(QString UserInput READ UserInput WRITE setUserInput NOTIFY UserInputChanged)
    Q_PROPERTY(QString BluetoothAdr READ BluetoothAdr WRITE setBluetoothAdr NOTIFY BluetoothAdrChanged)

public:
    explicit BackendStuff(QObject *parent = nullptr);
    QTcpSocket *tcpSocket;

    QString getBuffer(){
        //bytesAvailable(): Returns the number of incoming bytes that are waiting to be read.
        //'Sec-WebSocket-Accept' = 0x 5365 632d 5765 6253 6f63 6b65 742d 4163 6365 7074
        if (tcpSocket->bytesAvailable() > (quint16)sizeof(quint16))
            {
                 receive_dump.clear();
                 receive_dump.append(tcpSocket->readAll());

                 //checks whether websocket-connection is established
                 if(ws_status != true)
                 {
                     QRegExp cmp("\\b(Sec-WebSocket-Accept)\\b");
                     if(cmp.indexIn((QString)receive_dump) != -1)
                         ws_status = true;
                 }
                 else
                 {
                     int pos=0;
                     while(receive_dump[pos] != '[') pos++;     //find first match of '[' in ByteArray
                     receive_dump.remove(0,pos);    //removes unused preamble
                 }
        }

        m_receivebuffer = (QString)receive_dump;
        return m_receivebuffer;
    }

    void setUserInput(const QString &a)
    {
        if (a != NULL)
        {
            if (a != m_UserInput)
            {
                m_UserInput = a;
                emit UserInputChanged();
            }
            //qDebug() << "setUserInput: " << m_UserInput;

            QString path = QString("/tmp/motalkfile.txt");  //set File-Path
            QFile file(path);

            //write QString m_UserInput into file in overwrite mode
            if(!file.open(QIODevice::WriteOnly)){
                    file.close();
                } else {
                    QTextStream out(&file); out << m_UserInput;
                    file.close();
                }
        }
    }

    QString UserInput() const {
            qDebug() << "UserInput getter function";
            return m_UserInput;
    }

    void setBluetoothAdr(const QString &a)
    {
        if (a != NULL)
        {
            if (a != m_BluetoothAdr)
            {
                m_BluetoothAdr = a;
                emit BluetoothAdrChanged();
            }
            qDebug() << "(in function setBluetoothAdr) setBluetoothAdr: " << m_BluetoothAdr;

            //write Bluetooth-Adr in file to make it persistent
            QString path = QString("/tmp/motalk_bt-address.txt"); //set filepath
            QFile file(path);

            //write m_BluetoothAdr into file with overwrite-mode
            if(!file.open(QIODevice::WriteOnly)){
                file.close();
            } else{
                QTextStream out(&file); out << m_BluetoothAdr;
                file.close();
            }
        }
    }

    QString BluetoothAdr() const {
        //qDebug() << "BluetoothAdr getter function";
        QString parsed_btadr;
        QString path = QString("/tmp/motalk_bt-address.txt");
        QFile file(path);

        //read out of file
        if(!file.open(QIODevice::ReadOnly)){
            file.close();
        } else {
            QTextStream in(&file);
            while(!in.atEnd()){
                parsed_btadr = in.readLine();
            }

            file.close();
        }

        return parsed_btadr;
        //return m_BluetoothAdr;
    }

    //from QAfbWebSocketClient
    using closure_t = std::function<void(bool, const QJsonValue&)>;
    void call(const QString& api, const QString& verb, const QJsonValue& arg = QJsonValue(), closure_t closure = nullptr);

    //signals can trigger qml from c++
signals:
    void someError(QString err);
    void statusChanged2(bool);
    void hasReadSome(QString msg);
    void bufferChanged();
    void UserInputChanged();
    void BluetoothAdrChanged();
    void speechFinished();
    void speechStarted();
    void listenFinished();
    void listenStarted();

    //slots make c++ methods callable from qml
public slots:
    void gotError(QAbstractSocket::SocketError err);
    void sendClicked(QString msg);
    //überladene Funktionen von sendMessage mit mehreren möglichen Parametern
    void sendMessage(QString api, QString verb);
    void sendMessage(QString api, QString verb, QJsonValue arg);
    void connectClicked();
    void disconnectClicked();
    void closeConnection();
    void connect2host();
    //bluetooth-test
    void bluetooth_test();
    void capture_voice();
    void listen_msg();
    void teststart();
    void testfinish();
    void listenFinish();
    void listenStart();

private slots:
    void readyRead();
    void connected();
    void connectionTimeout();

private:
    //from QAfbWebSocketClient
    int m_nextCallId, port;
    bool tcp_status, ws_status = false;
    QMap<QString, closure_t> m_closures;
    QString host, m_receivebuffer, m_UserInput, m_BluetoothAdr;
    QTimer *timeoutTimer;
    QByteArray receive_dump;

public:
    bool voice_flag;
};

//constructor
BackendStuff::BackendStuff(QObject *parent) : QObject(parent)
{
    tcp_status = false;
    tcpSocket = new QTcpSocket();

    host = "127.0.0.1";
    port = 1234;

    timeoutTimer = new QTimer();
    timeoutTimer->setSingleShot(true);
    connect(timeoutTimer, &QTimer::timeout, this, &BackendStuff::connectionTimeout);

    //connect(client->tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(gotError(QAbstractSocket::SocketError)));
    connect(tcpSocket, &QTcpSocket::disconnected, this, &BackendStuff::closeConnection);
}

//call an api's verb with an argument
void BackendStuff::call(const QString &api, const QString &verb, const QJsonValue &arg, closure_t closure)
{

    QString callId = QString::number(m_nextCallId);
    m_closures[callId] = closure;

    static uint16_t upcount = 1;
    QJsonArray msg;
    msg.append(2);  //refers to Call
    //msg.append(callId);
    msg.append(upcount);
    upcount++;
    msg.append(api + "/" + verb);
    msg.append(arg);

    //m_nextCallId++;

    QJsonDocument value;
    value.setArray(msg);

    QByteArray AfbJson;
    AfbJson.append("\x81");             //Byte-Wörder für SET_OPCODE usw. und Array-Size
    AfbJson.append(value.toJson(QJsonDocument::Compact).size());
    AfbJson.append(value.toJson(QJsonDocument::Compact));
    tcpSocket->write(AfbJson);
}

void BackendStuff::gotError(QAbstractSocket::SocketError err)
{
    QString strError = "unknown";
    switch (err)
    {
        case 0:
            strError = "Connection was refused";
            break;
        case 1:
            strError = "Remote host closed the connection";
            break;
        case 2:
            strError = "Host address was not found";
            break;
        case 5:
            strError = "Connection timed out";
            break;
        default:
            strError = "Unknown error";
    }

    emit someError(strError);
}

void BackendStuff::connectClicked()
{
    connect2host();
}

void BackendStuff::sendClicked(QString msg)
{
    QByteArray arrBlock;
    /*
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    //out.setVersion(QDataStream::Qt_5_10);
    out << quint16(0) << msg;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    */
    arrBlock = msg.toUtf8();
    tcpSocket->write(arrBlock);
}

//überladene Funktion von sendMessage mit mehreren möglichen Parametern
void BackendStuff::sendMessage(QString api, QString verb, QJsonValue arg)
{
    call(api,verb,QJsonValue(arg),nullptr);
}

void BackendStuff::sendMessage(QString api, QString verb)
{
    QJsonObject obj
    {
        {"index", m_UserInput}
    };

    call(api,verb,(QJsonValue)obj,nullptr);
}

void BackendStuff::bluetooth_test()
{
    qDebug() << "Bluetooth-test-Bluetooth-test\n";

    Bluetooth_Wrapper bt_wrapper;
    int s, status;

    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);	//domain: bluetooth, type:data-stream, BT-Protocol: RFCOMM

    std::string bthadr = BluetoothAdr().toStdString();

    std::cout << "User getter bluetooth addr: " << bthadr << std::endl;

        //status = bt_wrapper.bt_connect(s, (struct sockaddr *)&addr2, sizeof(addr2));	//connect to server
        status = bt_wrapper.bt_connect(s,&bthadr);

        std::string output = m_UserInput.toStdString(); //convert QString to std::string

        // send  message
        if( status == 0 ) {
            //status = bt_wrapper.bt_write(s, "hello!", 6);
            bt_wrapper.bt_write(s,&output[0],output.length()); //write-function requires void-pointer, hence the address of first element
            //bt_wrapper.bt_write(s,output,output.length());
        }

        if( status < 0 ){
            std::cout << "status: " << status << std::endl;
            std::perror("uh oh - bt connecting and sending not possible");
    }
        close(s);
}

void BackendStuff::teststart()
{
    speechStarted();
}

void BackendStuff::testfinish()
{
    speechFinished();
}

void BackendStuff::listenFinish()
{
    listenFinished();
}

void BackendStuff::listenStart()
{
    listenStarted();
}

void BackendStuff::capture_voice()
{
     //qDebug() << "Aufruf von capture_voice()!\n";
     voice_flag = false;
     int rc, openfd, size, dir;
      long loops;
      snd_pcm_t *handle;
      snd_pcm_hw_params_t *params;
      unsigned int val;
      snd_pcm_uframes_t frames;
      char *buffer;

      if( fopen("/tmp/jabra_capture.wav", "w") == NULL) printf("Error opening/creating audio sample file!\n");
      openfd = open("/tmp/jabra_capture.wav", O_WRONLY);
      if(openfd < 0) printf("Error opening audio sample file!\n");

      rc = snd_pcm_open(&handle, "hw:1,0", SND_PCM_STREAM_CAPTURE, 0);
        if (rc < 0) {
          fprintf(stderr,"unable to open pcm device: %s\n",snd_strerror(rc));
          exit(1);
        }

        /* Allocate a hardware parameters object. */
        snd_pcm_hw_params_alloca(&params);

        /* Fill it in with default values. */
        snd_pcm_hw_params_any(handle, params);

        /* Set the desired hardware parameters. */
        /* Interleaved mode */
        snd_pcm_hw_params_set_access(handle, params,SND_PCM_ACCESS_RW_INTERLEAVED);

        /* Signed 16-bit little-endian format */
        snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

        /* Two channels (stereo) */
        //snd_pcm_hw_params_set_channels(handle, params, 2);
        //test mono, 1 channel:
        snd_pcm_hw_params_set_channels(handle, params, 1);

        /* 44100 bits/second sampling rate (CD quality) */
        val = 44100;
        snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

        /* Set period size to 32 frames. */
        //frames = 32;
        frames = 4410; // = 1 * period-size
        snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

        /* Write the parameters to the driver */
        rc = snd_pcm_hw_params(handle, params);
        if (rc < 0) {
           fprintf(stderr,"unable to set hw parameters: %s\n",
                   snd_strerror(rc));
           exit(1);
        }

        /* Use a buffer large enough to hold one period */
        snd_pcm_hw_params_get_period_size(params, &frames, &dir);
        //size = frames * 4; /* 2 bytes/sample, 2 channels */
        size = frames * 2;
        buffer = (char *) malloc(size);

        /* We want to loop for 5 seconds */
        snd_pcm_hw_params_get_period_time(params, &val, &dir);
        loops = 5000000 / val;

        while (loops > 0  && voice_flag == false) {
          loops--;
          rc = snd_pcm_readi(handle, buffer, frames);
          if (rc == -EPIPE) {
            /* EPIPE means overrun */
            fprintf(stderr, "overrun occurred\n");
            snd_pcm_prepare(handle);
          } else if (rc < 0) {
            fprintf(stderr,"error from read: %s\n",snd_strerror(rc));
          } else if (rc != (int)frames) {
            fprintf(stderr, "short read, read %d frames\n", rc);
          }
          //rc = write(1, buffer, size);
          rc = write(openfd, buffer, size);
          if (rc != size)
            fprintf(stderr,"short write: wrote %d bytes\n", rc);
        }

        snd_pcm_drain(handle);
        snd_pcm_close(handle);
        free(buffer);
        //qDebug() << "Ende von capture_voice()!\n";
        close(openfd);
}

void BackendStuff::listen_msg()
{
    //qDebug() << "Aufruf von listen_msg()!\n";
    long loops;
    int rc, openfd, size, dir;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int val;
    snd_pcm_uframes_t frames;
    char *buffer;

    //read in file-descriptor of audiofile
    if( fopen("/tmp/jabra_capture.wav", "r") == NULL) {
        printf("Error opening/creating audio sample file!\n");
        exit(1);
    }
    openfd = open("/tmp/jabra_capture.wav", O_RDONLY);
    if(openfd < 0) printf("Error opening audio sample file!\n");

    //opens the default PCM device and set the access mode to PLAYBACK.
    //This function returns a handle in the first function argument that is used in subsequent calls to manipulate the PCM stream
    /* Open PCM device for playback. */
    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
      fprintf(stderr,"unable to open pcm device: %s\n",snd_strerror(rc));
      exit(1);
    }

    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&params);

    /* Fill it in with default values. */
    snd_pcm_hw_params_any(handle, params);

    /* Set the desired hardware parameters. */
    /* Interleaved mode = left and right channel information is stored alternately within a frame; this is called interleaved mode */
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

    /* Signed 16-bit little-endian format */
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

    /* One channel (mono) */
    snd_pcm_hw_params_set_channels(handle, params, 1);

    /* 44100 bits/second sampling rate (CD quality)
        note: sound hardware is not always able to support every sampling rate exactly.
        We use the function snd_pcm_hw_params_set_rate_near to request the nearest supported sampling rate. */
    val = 44100;
    snd_pcm_hw_params_set_rate_near(handle, params,&val, &dir);

    //The buffer can be quite large, and transferring it at once could cause latency. Hence ALSA splits the buffer up into a series of periods and transfers the data in units of a period.
    //A period stores frames, each of which contains the samples captured at one point in time. For a stereo device, the frame would contain samples for two channels.
    /* Set period size to 4410 frames. */
    frames = 4410; // = 1 period-size

    /* Params not active until we call the function snd_pcm_hw_params!! Write the parameters to the driver! */
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
      fprintf(stderr,"unable to set hw parameters: %s\n", snd_strerror(rc));
      exit(1);
    }

    /* Use a buffer large enough to hold one period */
    snd_pcm_hw_params_get_period_size(params, &frames, &dir);
    //size = frames * 2 --> 2 bytes/sample, 1 channel
    size = frames * 2;
    buffer = (char *) malloc(size);

    /* We want to loop for 5 seconds */
    snd_pcm_hw_params_get_period_time(params, &val, &dir);
    /* 5 seconds in microseconds divided by period time */
    loops = 5000000 / val;

    while (loops > 0) {
      loops--;
      //rc = read(0, buffer, size);	//hier fd=0 ändern um Fie direkt im Programm zu lesen anstatt per terminal einzulesen, fd=0 bedeutet "read from keyboard"
      rc = read(openfd, buffer, size);
      if (rc == 0) {
        fprintf(stderr, "end of file on input\n");
        break;
      } else if (rc != size) {
        fprintf(stderr, "short read: read %d bytes\n", rc);
      }
      rc = snd_pcm_writei(handle, buffer, frames);
      if (rc == -EPIPE) {
        /* EPIPE means underrun */
        fprintf(stderr, "underrun occurred\n");
        snd_pcm_prepare(handle);
      } else if (rc < 0) {
        fprintf(stderr,"error from writei: %s\n", snd_strerror(rc));
      }  else if (rc != (int)frames) {
        fprintf(stderr, "short write, write %d frames\n", rc);
      }
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(buffer);
    close(openfd);
    if( remove( "/tmp/jabra_capture.wav" ) != 0 )
        perror( "Error deleting file" );
      else
        puts( "File successfully deleted");

    //qDebug() << "Ende von listen_msg()!\n";
}

void BackendStuff::disconnectClicked()
{
    closeConnection();
}

//here the connection is executed via QAbstractSocket
void BackendStuff::connect2host()
{
    timeoutTimer->start(3000);
    tcpSocket->connectToHost(host, port);
    connect(tcpSocket, &QTcpSocket::connected, this, &BackendStuff::connected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &BackendStuff::readyRead);
}

void BackendStuff::connectionTimeout()
{
    //qDebug() << tcpSocket->state();
    if(tcpSocket->state() == QAbstractSocket::ConnectingState)
    {
        tcpSocket->abort();
        emit tcpSocket->error(QAbstractSocket::SocketTimeoutError);
    }
}

void BackendStuff::connected()
{
    tcp_status = true;
    emit statusChanged2(tcp_status);
}

void BackendStuff::readyRead()
{
   if(getBuffer() != 0) emit bufferChanged();
}

void BackendStuff::closeConnection()
{
    timeoutTimer->stop();

    disconnect(tcpSocket, &QTcpSocket::connected, 0, 0);
    disconnect(tcpSocket, &QTcpSocket::readyRead, 0, 0);

    bool shouldEmit = false;
    switch (tcpSocket->state())
    {
        case 0:
            tcpSocket->disconnectFromHost();
            shouldEmit = true;
            break;
        case 2:
            tcpSocket->abort();
            shouldEmit = true;
            break;
        default:
            tcpSocket->abort();
    }

    if (shouldEmit)
    {
        tcp_status = false;
        ws_status = false;
        emit statusChanged2(tcp_status);
    }
}


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<BackendStuff>("io.qt.BackendStuff", 1, 0, "BackendStuff");

    QQmlApplicationEngine engine;

    //engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/startingpage.qml")));
    if (engine.rootObjects().isEmpty()) { return -1; }

    return app.exec();
}

#include <main.moc>
