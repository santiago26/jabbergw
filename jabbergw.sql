CREATE TABLE `gateways` (
  `gw_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `jid` char(64) NOT NULL,
  `url` char(128) NOT NULL,
  PRIMARY KEY (`gw_id`)
);

INSERT INTO `gateways` VALUES (1,'8ball','http://jabbergw.com/8ball');
