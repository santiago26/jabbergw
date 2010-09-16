#include <gloox/component.h>
#include <gloox/connectionlistener.h>
#include <gloox/loghandler.h>
#include <gloox/message.h>
#include <gloox/presence.h>
#include <gloox/capabilities.h>
#include <gloox/subscription.h>
#include <gloox/iq.h>
#include <gloox/presencehandler.h>
#include <gloox/subscriptionhandler.h>
#include <gloox/messagehandler.h>
#include <gloox/iqhandler.h>
#include <gloox/vcard.h>
#include <gloox/disco.h>

#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <boost/format.hpp>
#include <curl/curl.h>

using std::string;
using namespace gloox;
using boost::format;

class JabberGW : public LogHandler,ConnectionListener,PresenceHandler,SubscriptionHandler,IqHandler,MessageHandler {
  public:
    JabberGW();
    virtual ~JabberGW();

    void start();

    virtual void handleLog(LogLevel level,LogArea area,const std::string& message);

    virtual void onConnect();
    virtual void onDisconnect(ConnectionError);
    virtual bool onTLSConnect(const CertInfo &);

    virtual void handlePresence(const Presence &);
    virtual void handleSubscription(const Subscription &);
    virtual bool handleIq(const IQ &);
    virtual void handleIqID(const IQ &,int);
    virtual void handleMessage(const Message &,MessageSession *);

    static int getGWID(const string &);
    static string encloseSQL(const string &);
  private:
    void sendVCardReply(const JID &,const JID &,const string &);
    void sendPresence(const JID &,const JID &);

    static Component *j;
    static MYSQL sql;
    static Capabilities *caps;
};

struct HTTPQuery {
  string url;
  string from;
  string to;
  string body;
  Component *j;
};

int main(int /*argc*/,char** /*argv*/);
