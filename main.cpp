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
//#include <bluetooth_recv.h>
#include <QQmlProperty>
#include <QQuickItem>
#include <backendstuff.h>

//extern std::thread btrcv2;
//extern bool bt_bound2;
extern bool bt_portnum;
bool bt_bound2 = false;
QString btaddr;

void set_btstate(bool x)
{
    bt_bound2 = x;
}

bool get_btstate()
{
    return bt_bound2;
}

QString translate_clr()
{
    qDebug() << "hallo from translate_clr\n";
    if (bt_bound2)
    {
        return "green";
    }
    else
    {
       return "red";
    }
}



int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<BackendStuff>("io.qt.BackendStuff", 1, 0, "BackendStuff");

    QQmlApplicationEngine engine;

    //BackendStuff x;
    //btrcv2 = std::thread{&BackendStuff::bt_server,&x};
    //btrcv2.detach();

    //engine.rootContext()->setContextProperty(translate_clr(), "bt_state_color");
    //QQuickView view;
    //view.rootContext()->setContextProperty("bt_state_color",translate_clr());

    //engine.rootContext()->setContextProperty("bt_state_color", translate_clr());
    //engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/startingpage.qml")));
    if (engine.rootObjects().isEmpty()) { return -1; }

    return app.exec();
}

//#include <main.moc>
