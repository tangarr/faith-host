#include "hostconfig.h"
#include "faithcore.h"

unsigned int HostConfig::ip() const
{
    return _ip;
}

void HostConfig::setIp(unsigned int ip)
{
    _ip = ip;
}

QString HostConfig::mac() const
{
    return _mac;
}

void HostConfig::setMac(const QString &mac)
{
    _mac = mac;
}

QString HostConfig::hostname() const
{
    return _hostname;
}

void HostConfig::setHostname(const QString &hostname)
{
    _hostname = hostname;
}

QString HostConfig::lab() const
{
    return _lab;
}

void HostConfig::setLab(const QString &lab)
{
    _lab = lab;
}

unsigned int HostConfig::ipFromString(const QString &str)
{
    return Faithcore::ipFromString(str);
}

QString HostConfig::ipFromInt32(const unsigned int &int32)
{
    return Faithcore::ipFromInt(int32);
}
HostConfig::HostConfig()
{
}

HostConfig::HostConfig(int ip, QString mac, QString lab, QString hostname)
{
    _ip=ip;
    _mac=mac;
    _hostname=hostname;
    _lab=lab;
}
