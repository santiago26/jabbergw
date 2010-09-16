#include "jabbergw.h"

void JabberGW::handleSubscription(const Subscription &subscription) {
  int gwid=getGWID(subscription.to().username());
  if(gwid==0) return;

  Subscription::S10nType subtype=subscription.subtype();
  if(subtype==Subscription::Subscribe) {
    Subscription *s=new Subscription(Subscription::Subscribed,subscription.from().bare());
    s->setFrom(subscription.to().bare());
    j->send(*s);
    delete(s);

    sendPresence(subscription.to(),subscription.from());
    
    s=new Subscription(Subscription::Subscribe,subscription.from().bare());
    s->setFrom(subscription.to().bare());
    j->send(*s);
    delete(s);
  } else if(subtype==Subscription::Unsubscribe || subtype==Subscription::Unsubscribed) {
    Subscription *s=new Subscription(Subscription::Unsubscribed,subscription.from().bare());
    s->setFrom(subscription.to().bare());
    j->send(*s);
    delete(s);
  }
}
