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
#include "fdstring.h"
#include "fdhostinfo.h"

const char* host_database="/host/host.db";

bool connectSocket(QTcpSocket* socket, QString server, quint16 port)
{
    socket->connectToHost(server, port);
    Window::write("Connecting to "+server+":"+QString::number(port)+"...");
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
        if (msg->getMessageCode()==Faithcore::ERROR)
        {
            FdString* str = static_cast<FdString*>(msg->getData());
            if (str) Window::showMessageBox("ERROR", str->value());
            else Window::showMessageBox("ERROR", "Recived ERROR message");
        }
        else Window::showMessageBox("ERROR", "Recived wrong message");
        delete msg;
        return 0;
    }
    else return msg;
}

int hostInfo(char* server_char, quint16 port)
{
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
            Window::Destroy();
            return -1;
        }
    }
    else
    {
        Window::showMessageBox("ERROR", "Can't open host database\n"+QString(host_database));
        Window::Destroy();
        return -1;
    }
    //-------------------------------FIRST-CONNECTION-------------------------------------------------
    QTcpSocket *socket = new QTcpSocket();
    if (!connectSocket(socket,QString(server_char), port))
    {
        Window::Destroy();
        return -1;
    }
    FaithMessage *msg = sendMessage(socket, FaithMessage::MsgGetLabListOrHostInfo(mac), {Faithcore::HOST_INFO, Faithcore::LAB_LIST});
    disconnectSocket(socket);

    if (!msg)
    {
        Window::Destroy();
        return -1;
    }

    HostConfig *cnf = 0;
    QStringList laboratories;

    if (msg->getMessageCode()==Faithcore::LAB_LIST)
    {
        FdStringList *list = static_cast<FdStringList*>(msg->getData());
        if (!list)
        {
            Window::showMessageBox("ERROR", "Can't extract StringList from message");
            Window::Destroy();
            return -1;
        }
        for (int i=0;i<list->count();i++)
        {
            laboratories.append(list->at(i));
        }
        delete msg;
        while (true)
        {
            int lab_index = Window::showComuterLabWindow(laboratories);
            if (!connectSocket(socket,QString(server_char), port))
            {
                Window::Destroy();
                return -1;
            }
            QString lab_name = laboratories.at(lab_index);
            QString ip, hostname;
            msg = sendMessage(socket,FaithMessage::MsgReserveIp(lab_name), {Faithcore::PROPOSED_IP});
            disconnectSocket(socket);
            if (!msg)
            {
                Window::Destroy();
                return -1;
            }

            FdProposedIp* proposal = static_cast<FdProposedIp*>(msg->getData());
            if (!proposal)
            {
                Window::showMessageBox("ERROR", "Can't extract ProposedIp from message");
                Window::Destroy();
                return -1;
            }
            ip = HostConfig::ipFromInt32(proposal->ip());
            hostname = proposal->hostname();
            delete msg;

            cnf = Window::showConfigForm(mac,lab_name,ip, hostname);

            if (!connectSocket(socket,QString(server_char), port))
            {
                Window::Destroy();
                return -1;
            }
            msg = sendMessage(socket, FaithMessage::MsgAcceptIp(cnf->lab(), cnf->hostname(), cnf->ip(), cnf->mac()), {Faithcore::OK, Faithcore::ERROR});
            disconnectSocket(socket);
            if (!msg)
            {
                Window::Destroy();
                return -1;
            }

            if (msg->getMessageCode() == Faithcore::ERROR)
            {
                QString error;
                FdString *str = static_cast<FdString*>(msg->getData());
                if (str) error = "\n"+str->value();
                Window::showMessageBox("ERROR", "Can't Accept selected configuration"+error);
            }
            else break;
        }
    }
    else
    {
        FdHostInfo* hinfo = static_cast<FdHostInfo*>(msg->getData());
        if (!hinfo)
        {
            Window::showMessageBox("ERROR", "Can't extract HostInfo from message");
            Window::Destroy();
            return -1;
        }
        cnf = new HostConfig(hinfo->ip(), hinfo->mac(), hinfo->lab(), hinfo->hostname());
        delete msg;
    }

    //------------------------------PREAPARE-DATABASE----------------------------------------------------
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
    querry.prepare("insert into general values ('lab',?)");
    querry.bindValue(0, cnf->lab());
    if (querry.exec())
    {
        Window::writeLn("Writen lab into database");
    }
    db.close();
    //------------------------------FINAL-CONNECTION------------------------------------------------------
    if (!connectSocket(socket,QString(server_char), port))
    {
        Window::Destroy();
        return -1;
    }
    Window::writeLn("Sending host configuration to server");
    msg = sendMessage(socket, FaithMessage::MsgSendFile(host_database), {Faithcore::OK, Faithcore::ERROR});
    disconnectSocket(socket);
    if (!msg) Window::showMessageBox("ERROR", "Unknown Error");
    else
    {
        if (msg->getMessageCode()==Faithcore::OK) Window::showMessageBox("SUCCES", "Host Configuration succesfull sended to server");
        else
        {
            FdString* str = static_cast<FdString*>(msg->getData());
            if (str) Window::showMessageBox("ERROR", str->value());
            else Window::showMessageBox("ERROR", "Recived Error\nCan't extract error message from message");
        }
    }
    Window::Destroy();
    return 0;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    char* server_char = getenv("SERVER");
    char* port_char = getenv("FAITH_PORT");

    Window::Initialize();
    Window::refresh();

    quint16 port = 0;
    if (!server_char)
    {
        Window::showMessageBox("ERROR", "Can't find variable 'SERVER' in environment list");
        Window::Destroy();
        return -1;
    }
    else
    {
        Window::writeLn("SERVER: "+QString(server_char));
    }
    if (!port_char)
    {
        Window::showMessageBox("ERROR", "Can't find variable 'FAITH_PORT' in environment list");
        Window::Destroy();
        return -1;
    }
    else
    {
        bool conversion_ok = false;
        port = QString(port_char).toUShort(&conversion_ok);
        if (!conversion_ok)
        {
            Window::showMessageBox("ERROR", "Can't convert variable 'FAITH_PORT' to unsigned short");
            Window::Destroy();
            return -1;
        }
    }

    if (argc>=2)
    {
        QString option = QString(argv[1]);
        if (option=="info")
        {
            return hostInfo(server_char, port);
        }
        else if (option=="completed")
        {            
            qDebug() << "connectiong to "+QString(server_char)+" on port "+QString(port_char);
            QTcpSocket *socket = new QTcpSocket();
            if (!connectSocket(socket,QString(server_char), port))
            {
                qDebug() << "can't connect to "+QString(server_char)+" on port "+QString(port_char);
                Window::Destroy();
                return -1;
            }
            FaithMessage *msg = sendMessage(socket, FaithMessage::MsgInstallComplete(), {Faithcore::OK});
            disconnectSocket(socket);
            if (msg)
            {
                Window::Destroy();
                return 0;
            }
            else
            {
                qDebug() << server_char << port;
                qDebug() << "message INSTALL_COMPLETE not delivered ";
                Window::Destroy();
                return -1;
            }
        }
        else if (option=="aborted")
        {
            QString message="";
            if (argc>=3) message = QString(argv[2]);
            QTcpSocket *socket = new QTcpSocket();
            qDebug() << "connectiong to "+QString(server_char)+" on port "+QString(port_char);
            if (!connectSocket(socket,QString(server_char), port))
            {
                qDebug() << "can't connect to "+QString(server_char)+" on port "+QString(port_char);
                Window::Destroy();
                return -1;
            }
            FaithMessage *msg = sendMessage(socket, FaithMessage::MsgInstallAborted(message), {Faithcore::OK});
            disconnectSocket(socket);
            if (msg)
            {
                Window::Destroy();
                return 0;
            }
            else
            {
                qDebug() << server_char << port;
                qDebug() << "message INSTALL_ABORT not delivered ";
                Window::Destroy();
                return -1;
            }
        }
    }
    Window::Destroy();
    qDebug() << "Usage: "+QString(argv[0]).split("/").last()+" (info|completed|aborted)";
    return 1;
}
