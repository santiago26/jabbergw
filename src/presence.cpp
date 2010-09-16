#include "jabbergw.h"

void JabberGW::sendPresence(const JID &from,const JID &to) {
  Presence *p=new Presence(Presence::Available,to,EmptyString,10);
  p->setFrom(from);
  p->addExtension(caps->clone());
  j->send(*p);
  delete(p);
}

void JabberGW::handlePresence(const Presence &presence) {
  if(presence.subtype()==Presence::Probe) {
    if(getGWID(presence.to().username())>0) sendPresence(presence.to(),presence.from());
    return;
  }
}
