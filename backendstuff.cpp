#include <backendstuff.h>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QQmlComponent>
#include <QQmlProperty>
#include <QQuickItem>
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
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <bluetooth_wrapper.hpp>
//#include <bluetooth_recv.h>
//#include <time.h>
//#include <QThread>

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

    emit BluetoothClrChanged();
    BluetoothClr();

    //qDebug() << "Bluetooth msg recv true/false: " << bth_msg_recvd << "\n";

    emit Bth_msg_recvChanged();
    Bth_msg_recv();

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

void BackendStuff::bth_usage()
{
    //qDebug() << "Bluetooth-test-Bluetooth-test\n";

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

void BackendStuff::bt_voice_send()
{
    //qDebug() << "Bluetooth voice send\n";
    QByteArray ba;

    if( fopen("/tmp/jabra_capture.wav", "r") == NULL)
    {
        qDebug() << "Error parsing audio file!\n";

        //zum Testen:
        bth_msg_recvd = true;
        return;
    }

    Bluetooth_Wrapper bt_wrapper;
    int s, status;
    //QString input;

    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);	//domain: bluetooth, type:data-stream, BT-Protocol: RFCOMM

    //std::string bthadr = BluetoothAdr().toStdString();
    std::string bthadr = btaddr.toStdString();


    std::cout << "User getter bluetooth addr: " << bthadr << std::endl;

    //status = bt_wrapper.bt_connect(s, (struct sockaddr *)&addr2, sizeof(addr2));	//connect to server
    status = bt_wrapper.bt_connect(s,&bthadr);

    QFile f("/tmp/jabra_capture.wav");
    if(f.open(QIODevice::ReadOnly))
    {
        ba = f.readAll();
        //qDebug() << "read " << ba.size() << "bytes";
        //qDebug() << "print ByteArray:\n" << ba << "\n";
        f.close();
    }
    else
    {
        qDebug() << "Parsing jabra_capture failed!\n";
    }

    // send  message
    if( status == 0 ) {
        //status = bt_wrapper.bt_write(s, "hello!", 6);
        //bt_wrapper.bt_write(s,&output[0],output.length()); //write-function requires void-pointer, hence the address of first element
        bt_wrapper.bt_write(s,ba,ba.length());
    }

    if( status < 0 ){
        std::cout << "status: " << status << std::endl;
        std::perror("uh oh - bt connecting and sending not possible");
    }
    close(s);
    //qDebug() << "End of bt_voice_send\n";
}


void BackendStuff::bt_server(void)
{
        //qDebug() << "This is bt_server!\n";
        //std::cout << "Thread-id: " << std::this_thread::get_id() << std::endl;

        struct sockaddr_rc loc_addr;	/* local bluetooth adapter's info */
        struct sockaddr_rc client_addr;	/* filled in with remote (client) bluetooth device's info */
        char buf[1024] = { 0 };
        char bt_buf[80] = {0};

        int bytes_read,server_sock, client_sock;
        socklen_t opt = sizeof(client_addr);

        struct pollfd btpoll;

        //Handy-test
        //client_addr.rc_bdaddr = (sockaddr_rc).rc_bdaddr "A4:6C:F1:08:96:B7";
        //client_addr.rc_bdaddr = (bdaddr_t){0xB7, 0x96, 0x08, 0xF1, 0x6C, 0xA4};

        /* allocate socket */
        server_sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

        loc_addr.rc_family = AF_BLUETOOTH;		/* Addressing family, always AF_BLUETOOTH */
        //bacpy(&loc_addr.rc_bdaddr, BDADDR_ANY);		/* Bluetooth address of local bluetooth adapter */
        loc_addr.rc_bdaddr = *BDADDR_ANY;
        //loc_addr.rc_channel = RFCOMM_SERVER_PORT_NUM;	/* port number of local bluetooth adapter */
        //loc_addr.rc_channel = (uint8_t) 1;

        rfcomm_bth_portnr++;
        if (rfcomm_bth_portnr > 5) rfcomm_bth_portnr = 1;
        qDebug() << "Rfcomm-Portnumber: " << rfcomm_bth_portnr << "\n";
        loc_addr.rc_channel = rfcomm_bth_portnr;

        if(bind(server_sock, (struct sockaddr *)&loc_addr, sizeof(loc_addr)) < 0) //accept incoming connections and reserve OS resources
        {
            perror("failed to bind");
            //exit(1);
        } else
        {
            bt_bound = true;
        }

        /* put socket into listening mode */
        listen(server_sock, 1);		/* backlog = 1, max length to which the queue pending connections for sockfd may grow */

        /* accept one connection */
        fcntl(server_sock, F_SETFL, O_NONBLOCK); //set socket listen and non-blocking
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &opt);	/* return new socket for connection with a client */

        ba2str( &client_addr.rc_bdaddr, buf );

        btpoll.fd = server_sock;
        btpoll.events = POLLIN;		//tell me when ready to read
        int num_events = poll(&btpoll, 1, -1); // wait forever, what means block?!


        if (num_events == 0) {
            printf("Poll timed out!\n");
             }
        else {
             int pollin_happened = btpoll.revents & POLLIN;

             if (pollin_happened) {
                //printf("File descriptor %d is ready to read\n", btpoll.fd);

                client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &opt);	/* return new socket for connection with a client */

                ba2str( &client_addr.rc_bdaddr, buf );

                /* read data from the client */
                memset(buf, 0, sizeof(buf));
                //bytes_read = recv(client_sock, buf, sizeof(buf), 0);

                //if( fopen("/tmp/msg_recvd.txt", "w") == NULL) printf("Error opening/creating audio sample file!\n");
                //int openfd = open("/tmp/msg_recvd.txt", O_WRONLY);
                if( fopen("/tmp/msg_recvd.wav", "w") == NULL) printf("Error opening/creating audio sample file!\n");
                int openfd = open("/tmp/msg_recvd.wav", O_WRONLY);
                if(openfd < 0) printf("Error opening audio sample file!\n");

                //while(!(bytes_read = recv(client_sock, bt_buf, sizeof(bt_buf), 0)))
                while( (bytes_read = read(client_sock, bt_buf,sizeof(bt_buf))) > 0 )
                {
                  write(openfd, bt_buf, sizeof(bt_buf));
                }
                //bytes_read = recv(client_sock, bt_buf, sizeof(bt_buf), 0);
                if( bytes_read == 0 ) {
                    puts("no bytes received\n");
                }

                close(openfd);
                bth_msg_recvd = true;

             }
        else {
                printf("Unexpected event occurred: %d\n", btpoll.revents);
             }
        }

}

void BackendStuff::bt_recv_onoff(bool bt_on)
{
    //std::cout << "This is bt_recv_on_off" << std::endl;
    //std::cout << "variable states: bt_on = " << bt_on << " , bt_bound2 = " << get_btstate() << std::endl;

    switch (bt_on)
    {
        case true:
            if (get_btstate())
            {
                std::cout << "Bluetooth already bound - turn off and on again" << std::endl;
                //std::cout << "new variable states: bt_on = " << bt_on << " , bt_bound2 = " << get_btstate() << std::endl;
            }
            else
            {
                std::cout << "bluetooth recv thread started" << std::endl;
                set_btstate(true);

                btrcv = std::thread{&BackendStuff::bt_server, this};
                //std::cout << "After create, get btrcv id? --> " << btrcv.get_id() << std::endl;
                btrcv.detach();
                //std::cout << " new variable states: bt_on = " << bt_on << " , bt_bound2 = " << get_btstate() << std::endl;
            }
            break;

        case false:
            if (get_btstate())
            {
                std::cout << "Close bluetooth-socket" << std::endl;

                //totally destory socket
                int enable = 1;
                setsockopt(server_sock,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int));
                setsockopt(client_sock,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int));

                /* close connection */
                close(client_sock);
                close(server_sock);

                set_btstate(false);
                //std::cout << " new variable states: bt_on = " << bt_on << " , bt_bound2 = " << get_btstate() << std::endl;
            }
            else
            {
                std::cout << "bt is already off" << std::endl;
                //std::cout << " new variable states: bt_on = " << bt_on << " , bt_bound2 = " << get_btstate() << std::endl;
            }
            break;
        }

}

void BackendStuff::bluetoothStart()
{
    bluetoothStarted();
}

void BackendStuff::speechstart()
{
    speechStarted();
}

void BackendStuff::speechfinish()
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

      //rc = snd_pcm_open(&handle, "hw:1,0", SND_PCM_STREAM_CAPTURE, 0);
      //für CloudLab andere Headset-Schnittstelle
      rc = snd_pcm_open(&handle, "hw:2,0", SND_PCM_STREAM_CAPTURE, 0);

      if (rc < 0) {
        fprintf(stderr,"unable to open pcm device: %s\n",snd_strerror(rc));
        //exit(1);
        return;
      }

      if( fopen("/tmp/jabra_capture.wav", "w") == NULL) printf("Error opening/creating audio sample file!\n");
      openfd = open("/tmp/jabra_capture.wav", O_WRONLY);
      if(openfd < 0) printf("Error opening audio sample file!\n");

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
        //qDebug() << "buffer-size: " << size << "\n";        //8820

        /* We want to loop for 5 seconds */
        snd_pcm_hw_params_get_period_time(params, &val, &dir);
        loops = 5000000 / val; //5.000.000 / 44100 = ~120 loops

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

    //QFile file("/tmp/jabra_capture.wav");
    //QString hinput = input.toString();
    //hinput.toLatin1().toHex();

    //if( fopen("/tmp/msg_recvd.txt", "w") == NULL) printf("Error opening/creating audio sample file!\n");
    //int openfd = open("/tmp/msg_recvd.txt", O_WRONLY);

    QFile file("/tmp/msg_recvd.wav");
    QString content;
    if(file.open(QIODevice::ReadOnly)) {
         QTextStream in(&file);
         while(!in.atEnd())
             content = in.readAll();
    }
    file.close();

    //bool ok;
    //QString hexString = "0x03";
    //qDebug() << "BINARY 1: " << QString::number(hexString.toLongLong(&ok, 16),2);
    //qDebug() << "BINARY 2: " << QString("%1").arg(content.toULongLong(&ok, 16), 5, 2, QChar('0'));

    //read in file-descriptor of audiofile
    //if( fopen("/tmp/jabra_capture.wav", "r") == NULL) {
    if( fopen("/tmp/msg_recvd.wav", "r") == NULL) {
        qDebug() << "Error opening/creating audio sample file!\n";

        //zum Testen:
        bth_msg_recvd = false;
        //exit(1);
        return;
    }
    //openfd = open("/tmp/jabra_capture.wav", O_RDONLY);
    openfd = open("/tmp/msg_recvd.wav", O_RDONLY);
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
      //qDebug() << "listen-buffer: " << buffer << "\n";
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
        fprintf(stderr,"error from write: %s\n", snd_strerror(rc));
      }  else if (rc != (int)frames) {
        fprintf(stderr, "short write, write %d frames\n", rc);
      }
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(buffer);
    close(openfd);

    bth_msg_recvd = false;
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
