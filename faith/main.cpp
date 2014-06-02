#include <QCoreApplication>
#include <QDebug>

#include "window.h"
#include "cstdio"
#include <QStringList>
#include <QTcpSocket>
#include "faithmessage.h"
#include "fdstringlist.h"
#include "fdproposedip.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

const char* host_database="/host/host.db";

bool connectSocket(QTcpSocket* socket, QString server, quint16 port)
{
    socket->connectToHost(server, port);
    Window::write("Connecting to "+server+"... ");
    socket->waitForConnected();
    if (socket->state()==QTcpSocket::ConnectedState)
    {
        Window::writeLn("OK");
    }
    else
    {
        Window::showMessageBox("ERROR", socket->errorString());
        return false;
    }
    return true;
}
void disconnectSocket(QTcpSocket* socket)
{
    socket->disconnectFromHost();
    if (socket->state() == QAbstractSocket::UnconnectedState || socket->waitForDisconnected())
        Window::writeLn("disconected from server");
}

FaithMessage* sendMessage(QTcpSocket* socket, FaithMessage& message, QList<Faithcore::MessageCode> expectedResponses)
{
    Window::writeLn("Sending message: "+Faithcore::MessageCodeToString(message.getMessageCode()));
    message.send(socket);
    if (!socket->waitForReadyRead())
    {
        Window::showMessageBox("ERROR", socket->errorString());
        return 0;
    }
    FaithMessage* msg = new FaithMessage();
    msg->recive(socket);
    Window::writeLn("Recived message: "+Faithcore::MessageCodeToString(msg->getMessageCode()));

    bool wrong_message = true;
    foreach (Faithcore::MessageCode msg_code , expectedResponses) {
        if (msg->getMessageCode()==msg_code)
        {
            wrong_message = false;
            break;
        }
    }
    if (wrong_message)
    {
        Window::showMessageBox("ERROR", "Recived wrong message");
        delete msg;
        return 0;
    }
    else return msg;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Window::Initialize();
    Window::refresh();

    /*
    char* server_char = getenv("SERVER");
    char* port_char = getenv("FAITH_PORT");
    /*/
    char* server_char = (char*)"127.0.0.1";
    char* port_char = (char*)"8100";
    //*/
    quint16 port = 0;
    if (!server_char)
    {
        Window::showMessageBox("ERROR", "Can't find variable 'SERVER' in environment list");
        return -1;
    }
    if (!port_char)
    {
        Window::showMessageBox("ERROR", "Can't find variable 'FAITH_PORT' in environment list");
        return -1;
    }
    else
    {
        bool conversion_ok = false;
        port = QString(port_char).toUShort(&conversion_ok);
        if (!conversion_ok)
        {
            Window::showMessageBox("ERROR", "Can't convert variable 'FAITH_PORT' to unsigned short");
            return -1;
        }
    }

    //-------------------------------RETRIVING HW ADDRESS FROM DATABASE-------------------------------
    QString mac;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(host_database);
    if (db.open())
    {
        QSqlQuery q("select value from general where key='mac'");
        if (q.next())
        {
            mac = q.value(0).toString();
        }
        else
        {
            Window::showMessageBox("ERROR", "Database "+QString(host_database)+" doesn't contain mac addres value\n"+q.lastError().text());
            return -1;
        }
    }
    else
    {
        Window::showMessageBox("ERROR", "Can't open host database\n"+QString(host_database));
        return -1;
    }
    //-------------------------------FIRST-CONNECTION-------------------------------------------------
    QTcpSocket *socket = new QTcpSocket();
    if (!connectSocket(socket,QString(server_char), port)) return -1;
    QStringList laboratories;    
    //socket->
    FaithMessage *msg = sendMessage(socket, FaithMessage::MsgGetLabListOrHostInfo(mac), {Faithcore::HOST_INFO, Faithcore::LAB_LIST});
    if (!msg) return -1;
    else
    {
        FdStringList *list = static_cast<FdStringList*>(msg->getData());
        if (!list)
        {
            Window::showMessageBox("ERROR", "Can't extract StringList from message");
            return -1;
        }
        for (int i=0;i<list->count();i++)
        {
            laboratories.append(list->at(i));
        }
        delete msg;
    }
    disconnectSocket(socket);
    //-------------------------------SECOND-CONNECTION-------------------------------------------------
    int lab_index = Window::showComuterLabWindow(laboratories);
    if (!connectSocket(socket,QString(server_char), port)) return -1;
    QString lab_name = laboratories.at(lab_index);
    QString ip, hostname;
    msg = sendMessage(socket,FaithMessage::MsgReserveIp(lab_name), {Faithcore::PROPOSED_IP});
    if (msg)
    {
        FdProposedIp* proposal = static_cast<FdProposedIp*>(msg->getData());
        if (!proposal)
        {
            Window::showMessageBox("ERROR", "Can't extract ProposedIp from message");
            return -1;
        }
        ip = HostConfig::ipFromInt32(proposal->ip());
        hostname = proposal->hostname();
    }
    disconnectSocket(socket);
    //------------------------------THIRD-CONNECTION----------------------------------------------------

    HostConfig *cnf = Window::showConfigForm(mac,lab_name,ip, hostname);
    QSqlQuery querry;
    querry.prepare("insert into general values ('ip',?)");
    querry.bindValue(0, HostConfig::ipFromInt32(cnf->ip()));
    if (querry.exec())
    {
        Window::writeLn("Writen ip into database");
    }
    querry.prepare("insert into general values ('hostname',?)");
    querry.bindValue(0, cnf->hostname());
    if (querry.exec())
    {
        Window::writeLn("Writen hostname into database");
    }

    //Window::getCh();
    //delete cnf;

    return a.exec();
}
