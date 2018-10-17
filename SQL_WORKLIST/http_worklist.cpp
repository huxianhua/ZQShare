#include "http_worklist.h"
#include <QStringBuilder>
#include <QUrlQuery>
#include <QDebug>
#include <QList>
#include <QPair>

#include "mainwindow.h"

HTTP_WORKLIST::HTTP_WORKLIST (QObject * parent)
    : QObject (parent)
{
    server = new QtHttpServer (this);

    connect (server, &QtHttpServer::started,            this, &HTTP_WORKLIST::onServerStarted);
    connect (server, &QtHttpServer::stopped,            this, &HTTP_WORKLIST::onServerStopped);
    connect (server, &QtHttpServer::error,              this, &HTTP_WORKLIST::onServerError);
    connect (server, &QtHttpServer::clientConnected,    this, &HTTP_WORKLIST::onClientConnected);
    connect (server, &QtHttpServer::clientDisconnected, this, &HTTP_WORKLIST::onClientDisconnected);
    connect (server, &QtHttpServer::requestNeedsReply,  this, &HTTP_WORKLIST::onRequestNeedsReply);

    server->setServerName (QStringLiteral ("My Test Qt HTTP Server"));

    m_port = 8088;

    //server->start (8088);
}

HTTP_WORKLIST::~HTTP_WORKLIST () {
    server->stop ();
}


void HTTP_WORKLIST::s_start(int m_port)
{
    server->start (m_port);
}
void HTTP_WORKLIST::s_stop(void)
{
    server->stop ();
}
void HTTP_WORKLIST::setPort(int port)
{
    if(port > 79)
    {
        m_port = port;
    }

}


void HTTP_WORKLIST::onServerStarted (quint16 port) {
    qDebug () << "QtHttpServer started on port" << port << server->getServerName ();
}

void HTTP_WORKLIST::onServerStopped () {
    qDebug () << "QtHttpServer stopped" << server->getServerName ();
}

void HTTP_WORKLIST::onServerError (QString msg) {
    qDebug () << "QtHttpServer error :" << msg;
}

void HTTP_WORKLIST::onClientConnected (QString guid) {
    qDebug () << "QtHttpServer client connected :" << guid;
}

void HTTP_WORKLIST::onClientDisconnected (QString guid) {
    qDebug () << "QtHttpServer client disconnected :" << guid;
}

void HTTP_WORKLIST::onRequestNeedsReply (QtHttpRequest * request, QtHttpReply * reply) {
    //default_out(request, reply);
    //out_json(request, reply);



    QUrl url = request->getUrl();

    QString path = url.path();

    qDebug("[%s -- %d] path: (%s)",__FUNCTION__,__LINE__,path.toUtf8().data());

    if(0 == path.compare("/demo",Qt::CaseInsensitive))
    {
        out_demo(request, reply);
    }else
    if(0 == path.compare("/worklist",Qt::CaseInsensitive))
    {
        out_json(request, reply);
    }else
    {
        out_default(request, reply);
    }
}

void HTTP_WORKLIST::out_json(QtHttpRequest * request, QtHttpReply * reply)
{
    qDebug () << "QtHttpServer client request :" << request;

    reply->addHeader("Content-type","text/html;charset=utf-8");
    QString json = MainWindow::instance()->get_select_json();

    reply->appendRawData(json.toUtf8());



}
void HTTP_WORKLIST::out_demo(QtHttpRequest * request, QtHttpReply * reply)
{
    qDebug () << "QtHttpServer client request :" << request;

    reply->addHeader("Content-type","text/html;charset=utf-8");
    QString json = MainWindow::instance()->get_json_demo();
    reply->appendRawData(json.toUtf8());
}






void HTTP_WORKLIST::out_default(QtHttpRequest * request, QtHttpReply * reply)
{
    qDebug () << "QtHttpServer client request :" << request;

    reply->appendRawData (QByteArrayLiteral ("<html>"));
    reply->appendRawData (QByteArrayLiteral ("<head> "));
    reply->appendRawData (QByteArrayLiteral ("<title>Testing Qt5 HTTP Server</title>"));

    //<meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
    reply->appendRawData (QByteArrayLiteral ("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\">"));

    reply->appendRawData (QByteArrayLiteral ("<style>pre { border: 1px solid darkgray; background: lightgray; padding: 10px; } td,th { border: 1px solid gray; text-align: center; }</style>"));
    reply->appendRawData (QByteArrayLiteral ("</head>"));
    reply->appendRawData (QByteArrayLiteral ("<body style='margin: 50px'>"));
    reply->appendRawData (QByteArrayLiteral ("<h1>It Works !</h1>\r\n"));

    //request->getHeader (QtHttpHeader::Host)

    reply->appendRawData (QString("<a target=\"_blank\" href=\"worklist\" > WorkList </a> </br> ").toUtf8());

    reply->appendRawData (QString("<a target=\"_blank\" href=\"demo\" > Demo </a> ").toUtf8());
    //<a target="_blank" href="/tiy/t.asp?f=html_headers">亲自试一试</a>
    //<a target="_blank" href="/tiy/t.asp?f=html_headers">亲自试一试</a>



    reply->appendRawData (QByteArrayLiteral ("<hr />\r\n"));
    reply->appendRawData (QByteArrayLiteral ("<h2>HTTP command: ")       % request->getCommand ().toUtf8 ()             % QByteArrayLiteral ("</h2>\r\n"));
    reply->appendRawData (QByteArrayLiteral ("<h3>Requested URL: ")      % request->getUrl ().toString ().toUtf8 ()     % QByteArrayLiteral ("</h3>\r\n"));
    reply->appendRawData (QByteArrayLiteral ("<table style='border: 1px solid gray'>"));
    reply->appendRawData (QByteArrayLiteral ("<tr>"));
    reply->appendRawData (QByteArrayLiteral ("<th colspan='2'>URL parameters</th>"));
    reply->appendRawData (QByteArrayLiteral ("</tr>"));
    reply->appendRawData (QByteArrayLiteral ("<tr>"));
    reply->appendRawData (QByteArrayLiteral ("<th>Key</th>"));
    reply->appendRawData (QByteArrayLiteral ("<th>Value</th>"));
    reply->appendRawData (QByteArrayLiteral ("</tr>"));
    QUrlQuery params (request->getUrl ());




    QList<QPair<QString, QString> > pairsList = params.queryItems ();
    for (int idx = 0; idx < pairsList.size (); idx++) {
        QPair<QString, QString> pair = pairsList.at (idx);
        reply->appendRawData (QByteArrayLiteral ("<tr>"));
        reply->appendRawData (QByteArrayLiteral ("<td>") % pair.first.toUtf8 ()  % QByteArrayLiteral ("</td>"));
        reply->appendRawData (QByteArrayLiteral ("<td>") % pair.second.toUtf8 () % QByteArrayLiteral ("</td>"));
        reply->appendRawData (QByteArrayLiteral ("</tr>"));


    }
    reply->appendRawData (QByteArrayLiteral ("</table>"));
    reply->appendRawData (QByteArrayLiteral ("<h4>Virtual host: ")       % request->getHeader (QtHttpHeader::Host)      % QByteArrayLiteral ("</h4>\r\n"));
    reply->appendRawData (QByteArrayLiteral ("<h5>Your User-Agent: ")    % request->getHeader (QtHttpHeader::UserAgent) % QByteArrayLiteral ("</h5>\r\n"));
    reply->appendRawData (QByteArrayLiteral ("<h6>Request raw data:\r\n</h6>"));
    reply->appendRawData (QByteArrayLiteral ("<pre>")                    % request->getRawData ()                       % QByteArrayLiteral ("</pre>\r\n"));
    reply->appendRawData (QByteArrayLiteral ("<br />\r\n"));
    reply->appendRawData (QByteArrayLiteral ("<h2>Server name : ")       % reply->getHeader (QtHttpHeader::Server)      % QByteArrayLiteral ("</h2>\r\n"));
    reply->appendRawData (QByteArrayLiteral ("<h3>Reply creation : ")    % reply->getHeader (QtHttpHeader::Date)        % QByteArrayLiteral ("</h3>\r\n"));
    reply->appendRawData (QByteArrayLiteral ("<pre>")                    % QByteArrayLiteral ("OK.")                    % QByteArrayLiteral ("</pre>\r\n"));
    reply->appendRawData (QByteArrayLiteral ("<br />\r\n"));

   // reply->appendRawData(QString("<h4> %1 </h4>").arg(MainWindow::instance()->get_select_json()).toUtf8());


    reply->appendRawData (QByteArrayLiteral ("</body>"));
    reply->appendRawData (QByteArrayLiteral ("</html>"));
}
