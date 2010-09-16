#include "jabbergw.h"

Component *JabberGW::j;
MYSQL JabberGW::sql;
Capabilities *JabberGW::caps;

JabberGW::JabberGW() {}
JabberGW::~JabberGW() {}

void JabberGW::start() {
  mysql_init(&sql);
  my_bool reconnect=1;
  mysql_options(&sql,MYSQL_OPT_RECONNECT,&reconnect);
  mysql_options(&sql,MYSQL_SET_CHARSET_NAME,"utf8");
  if(!mysql_real_connect(&sql,"localhost","jabbergw","secret","jabbergw",0,NULL,0)) {
    printf("Can't connect to mysql.\n");
    return;
  }

  curl_global_init(CURL_GLOBAL_ALL);

  j=new Component(XMLNS_COMPONENT_ACCEPT,"127.0.0.1","jabbergw.com","secret",5347);
  j->registerConnectionListener(this);
  j->logInstance().registerLogHandler(LogLevelWarning,LogAreaAll,this);
  j->registerMessageHandler(this);
  j->registerPresenceHandler(this);
  j->registerSubscriptionHandler(this);
  j->registerIqHandler(this,ExtVCard);

  j->disco()->setIdentity("application","bot","JabberGW");
  j->disco()->setVersion("JabberGW","1.0","Linux");
  j->disco()->removeFeature("jabber:iq:version");
  j->disco()->removeFeature("urn:xmpp:ping");
  
  j->disco()->addFeature("http://jabber.org/protocol/caps");
  j->disco()->addFeature("http://jabber.org/protocol/disco#info");
  j->disco()->addFeature("http://jabber.org/protocol/disco#items");
//  j->disco()->addFeature("vcard-temp");

  caps=new Capabilities(j->disco());
  caps->setNode("http://jabbergw.com/caps");
  caps->ver();  // TAK NADO!!!

  j->connect();
  delete(caps);
  delete(j);
}

void JabberGW::onConnect() {
  printf("CONNECTED\n");
  j->setFrom("jabbergw@jabbergw.com/JabberGW");
  j->selectResource("JabberGW");
}
      
void JabberGW::onDisconnect(ConnectionError e) {
  printf("DISCONNECT\n");
}
	
bool JabberGW::onTLSConnect(const CertInfo &info) {
  printf("TLS CONNECT\n");
  return false;
}

void JabberGW::handleLog( LogLevel level, LogArea area, const std::string& message) {
  printf("log: level: %d, area: %d, %s\n", level, area, message.c_str() );
}

string JabberGW::encloseSQL(const string &ostr) {
  string::const_iterator s=ostr.begin(),smax=ostr.end();
  string str;
  str.reserve(smax-s);
  while(s!=smax) {
    switch(*s) { 
      case '\'': str+="\\'"; break;
      case '\"': str+="\\\""; break;
      case '\\': str+="\\\\"; break;
      default: str+=*s;
    }
    s++;
  }
  return str;
}

int JabberGW::getGWID(const string &jid) {
  string query="SELECT gw_id FROM gateways WHERE jid='";
  query+=encloseSQL(jid);
  query+="'";
  MYSQL_RES *res;
  MYSQL_ROW row;
  int gwid=0;
  if(!mysql_query(&sql,query.c_str()) && (res=mysql_use_result(&sql))) {
    if((row=mysql_fetch_row(res))) gwid=atoi(row[0]);
    mysql_free_result(res);
  }
  return gwid;
}

int main(int /*argc*/,char** /*argv*/) {
  printf("START\n");
  fflush(NULL);

  JabberGW *r=new JabberGW();
  r->start();
  delete(r);
  return 0;
}
