#include "jabbergw.h"

size_t CURLcallback(void *ptr,size_t size,size_t nmemb,string *reply) {
  reply->append((char *)ptr,size*nmemb);
  return size*nmemb;
}

void *doHttpQuery(void *qptr) {
  HTTPQuery *httpq=(HTTPQuery *)qptr;

  CURL *curl=curl_easy_init();
  if(curl) {
    string post="body=";
    char *mbody=curl_escape(httpq->body.c_str(),0);
    post+=mbody;
    curl_free(mbody);
    post+="&from=";
    char *mfrom=curl_escape(httpq->from.c_str(),0);
    post+=mfrom;
    curl_free(mfrom);

    fwrite(post.c_str(),1,post.size(),stdout);
    printf("\n");
    fflush(NULL);

    string *reply=new string();

    curl_easy_setopt(curl,CURLOPT_URL,httpq->url.c_str());
    curl_easy_setopt(curl,CURLOPT_POST,1);
    curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post.c_str());
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,reply);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,&CURLcallback);
    CURLcode ret=curl_easy_perform(curl);
    if(ret==CURLE_OK) {
      long httpcode;
      CURLcode ccode=curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&httpcode);

      if(ccode==CURLE_OK && httpcode==200) {

        JID *to=new JID(httpq->from);
        Message *msg=new Message(Message::Chat,*to,*reply);
        JID *from=new JID(httpq->to);
        msg->setFrom(*from);
        httpq->j->send(*msg);
        delete(msg);
        delete(from);
        delete(to);
      
      }
    } else printf(curl_easy_strerror(ret));
    fflush(NULL);
    
    delete(reply);
    
    curl_easy_cleanup(curl);
  }
  
  delete(httpq);
}

void JabberGW::handleMessage(const Message &msg,MessageSession *session) {
  if(msg.subtype()!=Message::Chat && msg.subtype()!=Message::Normal) return;
  if(msg.body().empty()) return;

  string query="SELECT url FROM gateways WHERE jid='";
  query+=encloseSQL(msg.to().username());
  query+="'";
  MYSQL_RES *res;
  MYSQL_ROW row;
  string url;
  if(!mysql_query(&sql,query.c_str()) && (res=mysql_use_result(&sql))) {
    if((row=mysql_fetch_row(res))) url=row[0];
    mysql_free_result(res);
  }

  if(!url.empty()) {
    HTTPQuery *httpq=new HTTPQuery();
    httpq->url=url;
    httpq->from=msg.from().full();
    httpq->to=msg.to().full();
    httpq->body=msg.body();
    httpq->j=j;

    pthread_t thr;
    pthread_create(&thr,NULL,doHttpQuery,(void *)httpq);
  }
}
