#include <QCoreApplication>
#include <QDebug>

#include "window.h"
#include "cstdio"
#include <QStringList>
#include <QTcpSocket>
#include "faithmessage.h"
#include "fdstringlist.h"
#include "fdproposedip.h"

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

FaithMessage* sendMessage(QTcpSocket* socket, FaithMessage& message, Faithcore::MessageCode expectedResponse)
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

    if (msg->getMessageCode()!=expectedResponse)
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

    QTcpSocket *socket = new QTcpSocket();
    //-------------------------------FIRST-CONNECTION-------------------------------------------------
    if (!connectSocket(socket,QString(server_char), port)) return -1;
    QStringList laboratories;
    FaithMessage *msg = sendMessage(socket, FaithMessage::MsgGetLabList(), Faithcore::LAB_LIST);
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
    msg = sendMessage(socket,FaithMessage::MsgReserveIp(lab_name), Faithcore::PROPOSED_IP);
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

    HostConfig *cnf = Window::showConfigForm("mac",lab_name,ip, hostname);
    //Window::getCh();
    //delete cnf;

    return a.exec();
}
