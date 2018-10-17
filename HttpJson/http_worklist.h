#ifndef HTTP_WORKLIST_H
#define HTTP_WORKLIST_H

#include <QObject>

#include "QtHttpServer.h"
#include "QtHttpRequest.h"
#include "QtHttpReply.h"
#include "QtHttpHeader.h"


class HTTP_WORKLIST : public QObject
{
    Q_OBJECT


public:

    explicit HTTP_WORKLIST (QObject * parent = NULL);
    virtual ~HTTP_WORKLIST (void);

    void setPort(int port = 8088);

public slots:

    void s_start(int m_port);
    void s_stop(void);


    void onServerStopped      (void);
    void onServerStarted      (quint16 port);
    void onServerError        (QString msg);
    void onClientConnected    (QString guid);
    void onClientDisconnected (QString guid);
    void onRequestNeedsReply  (QtHttpRequest * request, QtHttpReply * reply);






private:
    QtHttpServer * server;
    int m_port;

    QByteArray m_json;
    bool m_resultVaild;

    void out_json(QtHttpRequest * request, QtHttpReply * reply);
    void out_demo(QtHttpRequest * request, QtHttpReply * reply);
    void out_default(QtHttpRequest * request, QtHttpReply * reply);


public slots:
    void s_result();


};

#endif // HTTP_WORKLIST_H
