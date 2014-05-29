#ifndef HOSTCONFIG_H
#define HOSTCONFIG_H

#include <QString>

class HostConfig
{
    unsigned int _ip;
    QString _mac, _hostname, _lab;
public:
    HostConfig();
    HostConfig(int ip, QString mac, QString lab, QString hostname);

    unsigned int ip() const;
    void setIp(unsigned int ip);
    QString mac() const;
    void setMac(const QString &mac);
    QString hostname() const;
    void setHostname(const QString &hostname);
    QString lab() const;
    void setLab(const QString &lab);

    static unsigned int ipFromString(const QString& str);
    static QString ipFromInt32 (const unsigned int &int32);
};

#endif // HOSTCONFIG_H
