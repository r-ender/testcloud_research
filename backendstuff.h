#ifndef BACKENDSTUFF_H
#define BACKENDSTUFF_H

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
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <bluetooth_wrapper.hpp>
#include <bluetooth_recv.h>
#include <QQmlProperty>
#include <QQuickItem>


#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

using namespace std;

class bluetooth_recv;

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
    void bt_voice_send();
    void capture_voice();
    void listen_msg();
    void teststart();
    void testfinish();
    void listenFinish();
    void listenStart();
    void bt_recv_onoff(bool);
    //void bt_server(void);

private slots:
    void readyRead();
    void connected();
    void connectionTimeout();

private:    //note: private variablen könne nicht vererbt werden
    //from QAfbWebSocketClient
    int m_nextCallId, port;
    bool tcp_status, bt_bound = false, ws_status = false;

    QMap<QString, closure_t> m_closures;
    QString host, m_receivebuffer, m_UserInput, m_BluetoothAdr;
    QTimer *timeoutTimer;
    QByteArray receive_dump;

public:
    bool voice_flag;
    bluetooth_recv *bt_rec;
    int server_sock, client_sock;
};

//class bluetooth_recv : public QThread, public BackendStuff
class bluetooth_recv : public BackendStuff, public QThread
{
    Q_OBJECT    //tell the pre-compiler that this class needs to be run through the Meta-Object Compiler, which adds extra functions to the class
public:
    //explicit bluetooth_recv(QObject *parent = 0);
    void run();
    //bluetooth_recv();
};

#endif // BACKENDSTUFF_H
