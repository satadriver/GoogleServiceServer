/*
Navicat MySQL Data Transfer

Source Server         : localhost
Source Server Version : 50505
Source Host           : localhost:3306
Source Database       : iso

Target Server Type    : MYSQL
Target Server Version : 50505
File Encoding         : 65001

Date: 2019-05-18 10:55:46
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for audio
-- ----------------------------
DROP TABLE IF EXISTS audio;
CREATE TABLE audio (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL,
  size varchar(255) DEFAULT NULL,
  path varchar(255) DEFAULT NULL,
  time varchar(255) DEFAULT NULL,
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of audio
-- ----------------------------
INSERT INTO audio VALUES ('1', '1', '97.7B', 'H:\\PHP_Project\\iosserver\\test20190405\\1557389469.wav', '2019-05-08 19:06:38');
INSERT INTO audio VALUES ('2', '1', '97.7B', 'H:\\PHP_Project\\iosserver\\test20190405\\1557409258.wav', '2019-05-08 19:06:37');

-- ----------------------------
-- Table structure for clients
-- ----------------------------
DROP TABLE IF EXISTS clients;
CREATE TABLE clients (
  id int(11) NOT NULL AUTO_INCREMENT,
  userid int(11) DEFAULT NULL COMMENT '用户id',
  unique_id varchar(255) DEFAULT NULL COMMENT '唯一标识',
  name varchar(255) DEFAULT NULL COMMENT '自定义名称',
  infos text COMMENT '存储deviceinfojson',
  ts varchar(255) DEFAULT NULL,
  state int(10) DEFAULT '0' COMMENT '0不在线1在线',
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of clients
-- ----------------------------

replace INTO clients (id,userid,unique_id,name,infos,ts,state) VALUES (2, '2', 'e10adc3949ba59abbe56e057f20f883e', '', '{\n  \"wifimac\" : \"b0:95:8e:50:9b:eb\",\n  \"language\" : \"英文\",\n  \"release\" : \"18.2.0\",\n  \"devicename\" : \"“Administrator”的 iPhone\",\n  \"AvailableDiskSize\" : 2811162624,\n  \"version\" : \"Darwin Kernel Version 18.2.0: Mon Nov 12 20:32:02 PST 2018; root:xnu-4903.232.2~1\\/RELEASE_ARM64_S8000\",\n  \"localip\" : \"fe80::c370:7c5c:49ee:8797\",\n  \"os\" : \"iOS\",\n  \"isDeviceJailbreak\" : false,\n  \"identifier\" : \"0931247F-4927-43E9-8A39-E783EB15C85E\",\n  \"BatteryQuanitity\" : 85,\n  \"NetworkState\" : \"WIFI\",\n  \"token\" : \"\",\n  \"model\" : \"iPhone\",\n  \"wifiname\" : \"guangxin_wifi(public)\",\n  \"TotalDiskSize\" : 15989485568,\n  \"SystemVersion\" : \"12.1.2\",\n  \"resolutionY\" : 375,\n  \"resolutionX\" : 667,\n  \"machine\" : \"iPhone8,1\",\n  \"DeviceType\" : \"iPhone 6s\",\n  \"idfa\" : \"54003FB3-E4C3-4947-8C73-50EB33742BC1\"\n}', ' 2019-5-14-50:14:43', '1');

INSERT INTO clients VALUES (2, '2', 'e10adc3949ba59abbe56e057f20f883e', '', '{\n  \"wifimac\" : \"b0:95:8e:50:9b:eb\",\n  \"language\" : \"英文\",\n  \"release\" : \"18.2.0\",\n  \"devicename\" : \"“Administrator”的 iPhone\",\n  \"AvailableDiskSize\" : 2811162624,\n  \"version\" : \"Darwin Kernel Version 18.2.0: Mon Nov 12 20:32:02 PST 2018; root:xnu-4903.232.2~1\\/RELEASE_ARM64_S8000\",\n  \"localip\" : \"fe80::c370:7c5c:49ee:8797\",\n  \"os\" : \"iOS\",\n  \"isDeviceJailbreak\" : false,\n  \"identifier\" : \"0931247F-4927-43E9-8A39-E783EB15C85E\",\n  \"BatteryQuanitity\" : 85,\n  \"NetworkState\" : \"WIFI\",\n  \"token\" : \"\",\n  \"model\" : \"iPhone\",\n  \"wifiname\" : \"guangxin_wifi(public)\",\n  \"TotalDiskSize\" : 15989485568,\n  \"SystemVersion\" : \"12.1.2\",\n  \"resolutionY\" : 375,\n  \"resolutionX\" : 667,\n  \"machine\" : \"iPhone8,1\",\n  \"DeviceType\" : \"iPhone 6s\",\n  \"idfa\" : \"54003FB3-E4C3-4947-8C73-50EB33742BC1\"\n}', ' 2019-5-14-50:14:43', '1');

INSERT INTO clients VALUES ('1', '2', '8DB77D41E56342D3905C06DB62DD23D5', '测试', '{\n  \"wifimac\" : \"b0:95:8e:50:9b:eb\",\n  \"language\" : \"中文\",\n  \"release\" : \"17.7.0\",\n  \"devicename\" : \"“chenliqiang”的 iPhone\",\n  \"AvailableDiskSize\" : 21933158400,\n  \"version\" : \"Darwin Kernel Version 17.7.0: Mon Jun 11 19:06:26 PDT 2018; root:xnu-4570.70.24~3\\/RELEASE_ARM64_T8010\",\n  \"localip\" : \"fe80::9fcb:709d:3a1:9c19\",\n  \"os\" : \"iOS\",\n  \"isDeviceJailbreak\" : true,\n  \"identifier\" : \"A6F89B52-1928-4D3C-B6A8-8F456E0E2033\",\n  \"BatteryQuanitity\" : 100,\n  \"NetworkState\" : \"WIFI\",\n  \"token\" : \"\",\n  \"model\" : \"iPhone\",\n  \"wifiname\" : \"guangxin_wifi(public)\",\n  \"TotalDiskSize\" : 31989469184,\n  \"SystemVersion\" : \"11.4.1\",\n  \"resolutionY\" : 375,\n  \"resolutionX\" : 667,\n  \"machine\" : \"iPhone9,1\",\n  \"DeviceType\" : \"iPhone 7\",\n  \"idfa\" : \"8DB77D41-E563-42D3-905C-06DB62DD23D5\"\n}', ' 2019-5-14-50:14:43', '0');
INSERT INTO clients VALUES ('2', '2', '54003FB3E4C349478C7350EB33742BC1', '', '{\n  \"wifimac\" : \"b0:95:8e:50:9b:eb\",\n  \"language\" : \"英文\",\n  \"release\" : \"18.2.0\",\n  \"devicename\" : \"“Administrator”的 iPhone\",\n  \"AvailableDiskSize\" : 2811162624,\n  \"version\" : \"Darwin Kernel Version 18.2.0: Mon Nov 12 20:32:02 PST 2018; root:xnu-4903.232.2~1\\/RELEASE_ARM64_S8000\",\n  \"localip\" : \"fe80::c370:7c5c:49ee:8797\",\n  \"os\" : \"iOS\",\n  \"isDeviceJailbreak\" : false,\n  \"identifier\" : \"0931247F-4927-43E9-8A39-E783EB15C85E\",\n  \"BatteryQuanitity\" : 85,\n  \"NetworkState\" : \"WIFI\",\n  \"token\" : \"\",\n  \"model\" : \"iPhone\",\n  \"wifiname\" : \"guangxin_wifi(public)\",\n  \"TotalDiskSize\" : 15989485568,\n  \"SystemVersion\" : \"12.1.2\",\n  \"resolutionY\" : 375,\n  \"resolutionX\" : 667,\n  \"machine\" : \"iPhone8,1\",\n  \"DeviceType\" : \"iPhone 6s\",\n  \"idfa\" : \"54003FB3-E4C3-4947-8C73-50EB33742BC1\"\n}', ' 2019-5-14-50:14:43', '1');
INSERT INTO clients VALUES ('3', '2', '213213', null, '{\n  \"wifimac\" : \"b0:95:8e:50:9b:eb\",\n  \"language\" : \"英文\",\n  \"release\" : \"18.2.0\",\n  \"devicename\" : \"“Administrator”的 iPhone\",\n  \"AvailableDiskSize\" : 2811162624,\n  \"version\" : \"Darwin Kernel Version 18.2.0: Mon Nov 12 20:32:02 PST 2018; root:xnu-4903.232.2~1\\/RELEASE_ARM64_S8000\",\n  \"localip\" : \"fe80::c370:7c5c:49ee:8797\",\n  \"os\" : \"iOS\",\n  \"isDeviceJailbreak\" : false,\n  \"identifier\" : \"0931247F-4927-43E9-8A39-E783EB15C85E\",\n  \"BatteryQuanitity\" : 85,\n  \"NetworkState\" : \"WIFI\",\n  \"token\" : \"\",\n  \"model\" : \"iPhone\",\n  \"wifiname\" : \"guangxin_wifi(public)\",\n  \"TotalDiskSize\" : 15989485568,\n  \"SystemVersion\" : \"12.1.2\",\n  \"resolutionY\" : 375,\n  \"resolutionX\" : 667,\n  \"machine\" : \"iPhone8,1\",\n  \"DeviceType\" : \"iPhone 6s\",\n  \"idfa\" : \"54003FB3-E4C3-4947-8C73-50EB33742BC1\"\n}', ' 2019-5-14-50:14:43', '0');

-- ----------------------------
-- Table structure for contacts
-- ----------------------------
DROP TABLE IF EXISTS contacts;
CREATE TABLE contacts (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL,
  name varchar(255) DEFAULT NULL,
  phone varchar(255) DEFAULT NULL,
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=150 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of contacts
-- ----------------------------
INSERT INTO contacts VALUES ('2', '1', '孔德顺', '13256703063');
INSERT INTO contacts VALUES ('4', '1', '华为客服', '4008308300');
INSERT INTO contacts VALUES ('5', '1', '彭记钱', '18954130998');
INSERT INTO contacts VALUES ('6', '1', '韩轩', '18678120534');
INSERT INTO contacts VALUES ('7', '1', '杨老师小灵通', '05338996339');
INSERT INTO contacts VALUES ('8', '1', '王再学', '18615522075');
INSERT INTO contacts VALUES ('9', '1', '警察学生', '18353123198');
INSERT INTO contacts VALUES ('10', '1', '小叔', '13409055818');
INSERT INTO contacts VALUES ('11', '1', '长城24小时', '05332984676');
INSERT INTO contacts VALUES ('12', '1', '田怡轩', '15689099596');
INSERT INTO contacts VALUES ('13', '1', '钧豪干妈', '13561633881');
INSERT INTO contacts VALUES ('14', '1', '保养车', '13964498502');
INSERT INTO contacts VALUES ('15', '1', '广电客服', '6210000');
INSERT INTO contacts VALUES ('16', '1', '帆哥', '15092338515');
INSERT INTO contacts VALUES ('17', '1', '赵凯长途', '15689690221');
INSERT INTO contacts VALUES ('18', '1', '赵凯长途', '8615689690221');
INSERT INTO contacts VALUES ('19', '1', '王小军', '18615152725');
INSERT INTO contacts VALUES ('20', '1', '苏总', '18610562309');
INSERT INTO contacts VALUES ('21', '1', '纸厂老黑', '15949737570');
INSERT INTO contacts VALUES ('22', '1', '庆', '13561506195');
INSERT INTO contacts VALUES ('23', '1', '狗', '13656447195');
INSERT INTO contacts VALUES ('24', '1', '仙', '1795113806431184');
INSERT INTO contacts VALUES ('25', '1', '崔龙济南', '13854163057');
INSERT INTO contacts VALUES ('26', '1', '赵凯', '18605335510');
INSERT INTO contacts VALUES ('27', '1', '韦魏', '13964448822');
INSERT INTO contacts VALUES ('28', '1', '纸厂小崔', '15898767618');
INSERT INTO contacts VALUES ('29', '1', '吴坤', '18605331731');
INSERT INTO contacts VALUES ('30', '1', '张升远', '18560130536');
INSERT INTO contacts VALUES ('31', '1', '猪头', '1795115949744493');
INSERT INTO contacts VALUES ('32', '1', '刘帆', '15865477819');
INSERT INTO contacts VALUES ('33', '1', '李东旭', '13561606156');
INSERT INTO contacts VALUES ('34', '1', '刘茂明', '1795115698021526');
INSERT INTO contacts VALUES ('35', '1', '齐云妃', '18354156695');
INSERT INTO contacts VALUES ('36', '1', '李昊辰', '15153325925');
INSERT INTO contacts VALUES ('37', '1', '赵霞', '13616430902');
INSERT INTO contacts VALUES ('38', '1', '杨青', '8996339');
INSERT INTO contacts VALUES ('39', '1', '杨青', '05338996339');
INSERT INTO contacts VALUES ('40', '1', '杨青', '15898704275');
INSERT INTO contacts VALUES ('41', '1', '崔龙', '15064331363');
INSERT INTO contacts VALUES ('42', '1', '其武', '13969375188');
INSERT INTO contacts VALUES ('43', '1', '王克凡', '15254420970');
INSERT INTO contacts VALUES ('44', '1', '苏总老婆', '18653388986');
INSERT INTO contacts VALUES ('45', '1', '小姑', '13792157916');
INSERT INTO contacts VALUES ('46', '1', '刘倩', '13561606713');
INSERT INTO contacts VALUES ('47', '1', '刘倩', '18553376675');
INSERT INTO contacts VALUES ('48', '1', '嫂子', '13853393208');
INSERT INTO contacts VALUES ('49', '1', '广电', '13589576712');
INSERT INTO contacts VALUES ('50', '1', '粮油小叔', '7668579');
INSERT INTO contacts VALUES ('51', '1', '刘云', '13409089934');
INSERT INTO contacts VALUES ('52', '1', '老爸', '15065899634');
INSERT INTO contacts VALUES ('53', '1', '张兰真', '13153299192');
INSERT INTO contacts VALUES ('54', '1', '小韦', '18661669110');
INSERT INTO contacts VALUES ('55', '1', '大姐', '13573368826');
INSERT INTO contacts VALUES ('56', '1', '小姑夫', '15206685188');
INSERT INTO contacts VALUES ('57', '1', '周健老师', '15169207351');
INSERT INTO contacts VALUES ('58', '1', '妈妈', '13668837968');
INSERT INTO contacts VALUES ('59', '1', '杨振', '18364352079');
INSERT INTO contacts VALUES ('60', '1', '房产中介', '13054888392');
INSERT INTO contacts VALUES ('61', '1', '房产中介1', '13002718638');
INSERT INTO contacts VALUES ('62', '1', '房地产中介', '13573368896');
INSERT INTO contacts VALUES ('63', '1', '老婆', '15966985858');
INSERT INTO contacts VALUES ('64', '1', '二姐', '18605336998');
INSERT INTO contacts VALUES ('65', '1', '王传胜杭州', '13969379633');
INSERT INTO contacts VALUES ('66', '1', '于敏', '05337693567');
INSERT INTO contacts VALUES ('67', '1', '董京伟', '17705411718');
INSERT INTO contacts VALUES ('68', '1', '', '10655059113144');
INSERT INTO contacts VALUES ('69', '1', '', '106590256203144');
INSERT INTO contacts VALUES ('70', '1', '', '106575258192144');
INSERT INTO contacts VALUES ('71', '1', '', '106906199604916');
INSERT INTO contacts VALUES ('72', '1', '', '106904068039604916');
INSERT INTO contacts VALUES ('73', '1', '', '1065752551629604916');
INSERT INTO contacts VALUES ('74', '1', '测试电话', '17131605740');
INSERT INTO contacts VALUES ('75', '1', '陈总', '15553149815');
INSERT INTO contacts VALUES ('76', '1', '', '053168961330');
INSERT INTO contacts VALUES ('77', '1', '', '01053912850');
INSERT INTO contacts VALUES ('78', '1', '', '057189896576');
INSERT INTO contacts VALUES ('79', '1', '', '057126883145');
INSERT INTO contacts VALUES ('80', '1', '', '051482043244');
INSERT INTO contacts VALUES ('81', '1', '', '02131772034');
INSERT INTO contacts VALUES ('82', '1', '', '01053912898');
INSERT INTO contacts VALUES ('83', '1', '', '051482043258');
INSERT INTO contacts VALUES ('84', '1', '', '02131772099');
INSERT INTO contacts VALUES ('85', '1', '', '057126883138');
INSERT INTO contacts VALUES ('86', '1', '', '02131772083');
INSERT INTO contacts VALUES ('87', '1', '', '057156264863');
INSERT INTO contacts VALUES ('88', '1', '', '057126883397');
INSERT INTO contacts VALUES ('89', '1', '', '057189896579');
INSERT INTO contacts VALUES ('90', '1', '', '02131772182');
INSERT INTO contacts VALUES ('91', '1', '张萌', '18505331169');
INSERT INTO contacts VALUES ('92', '1', '吕建逊', '13864359631');
INSERT INTO contacts VALUES ('93', '1', '于老师工行贷款办理', '7317669');
INSERT INTO contacts VALUES ('94', '1', '李', '18453388875');
INSERT INTO contacts VALUES ('95', '1', '老刘搭车', '18201072229');
INSERT INTO contacts VALUES ('96', '1', '水泥', '13053315537');
INSERT INTO contacts VALUES ('97', '1', '黄零建', '13081426854');
INSERT INTO contacts VALUES ('98', '1', '李玉磊', '15610126361');
INSERT INTO contacts VALUES ('99', '1', '王传胜', '18954400983');
INSERT INTO contacts VALUES ('100', '1', '孩子老爷', '15269319983');
INSERT INTO contacts VALUES ('101', '1', '萌', '13366417102');
INSERT INTO contacts VALUES ('102', '1', '王浩燃', '18668975678');
INSERT INTO contacts VALUES ('103', '1', '王浩然', '18615173535');
INSERT INTO contacts VALUES ('104', '1', '于超盟', '18764447057');
INSERT INTO contacts VALUES ('105', '1', '证书指定电话', '13220581111');
INSERT INTO contacts VALUES ('106', '1', '广州证书', '17620826656');
INSERT INTO contacts VALUES ('107', '1', '济南技术', '18616331758');
INSERT INTO contacts VALUES ('108', '1', '济南技术交流', '15711208992');
INSERT INTO contacts VALUES ('109', '1', '', '057156215888');
INSERT INTO contacts VALUES ('110', '1', '吴金华', '15688868825');
INSERT INTO contacts VALUES ('111', '1', '南方灯具', '18253335776');
INSERT INTO contacts VALUES ('112', '1', '房租', '13280013940');
INSERT INTO contacts VALUES ('113', '1', '电脑维修', '05332724870');
INSERT INTO contacts VALUES ('114', '1', '页面设计', '15205416565');
INSERT INTO contacts VALUES ('115', '1', '管玉玺', '15905316520');
INSERT INTO contacts VALUES ('116', '1', '班主任李娟', '13969352926');
INSERT INTO contacts VALUES ('117', '1', '小亮', '15615937146');
INSERT INTO contacts VALUES ('118', '1', '李博士', '13811949068');
INSERT INTO contacts VALUES ('119', '1', '李延彪', '15215315353');
INSERT INTO contacts VALUES ('120', '1', '王东', '15169258028');
INSERT INTO contacts VALUES ('121', '1', '张永英', '18660189588');
INSERT INTO contacts VALUES ('122', '1', '陈松', '18660156473');
INSERT INTO contacts VALUES ('123', '1', '陈立强', '13396585715');
INSERT INTO contacts VALUES ('124', '1', '高洪波', '13475547252');
INSERT INTO contacts VALUES ('125', '1', '李建华', '13869389783');
INSERT INTO contacts VALUES ('126', '1', '李恒', '13465916366');
INSERT INTO contacts VALUES ('127', '1', '卢娟', '13634171759');
INSERT INTO contacts VALUES ('128', '1', '王传胜', '15924179119');
INSERT INTO contacts VALUES ('129', '1', '', '057128093514');
INSERT INTO contacts VALUES ('130', '1', '李红杰', '13082715712');
INSERT INTO contacts VALUES ('131', '1', '高国平', '15064384483');
INSERT INTO contacts VALUES ('132', '2', '移动网络客服', '15762812230');
INSERT INTO contacts VALUES ('133', '1', '浩然同事', '18905411355');
INSERT INTO contacts VALUES ('134', '2', '尚鹏', '13805773284');
INSERT INTO contacts VALUES ('135', '1', '顾明海', '18705336162');
INSERT INTO contacts VALUES ('136', '2', '肇事者老婆', '18560712129');
INSERT INTO contacts VALUES ('137', '1', '王医生', '13853393691');
INSERT INTO contacts VALUES ('138', '1', '', '05338795088');
INSERT INTO contacts VALUES ('139', '1', '测试手机号', '19905818706');
INSERT INTO contacts VALUES ('140', '1', '', '057128121818');
INSERT INTO contacts VALUES ('141', '1', '孔德顺', '13256703063');
INSERT INTO contacts VALUES ('142', '1', '卞', '15111155995');
INSERT INTO contacts VALUES ('143', '2', '王建', '13636539330');
INSERT INTO contacts VALUES ('144', '1', '王一峰', '16605338533');
INSERT INTO contacts VALUES ('145', '1', '创客公寓', '13336071953');
INSERT INTO contacts VALUES ('146', '1', '测试手机号', '19905818706');
INSERT INTO contacts VALUES ('147', '1', '小张', '15867410083');
INSERT INTO contacts VALUES ('148', '1', '待等', '13671836626');
INSERT INTO contacts VALUES ('149', '1', '刘帆', '13371585681');

-- ----------------------------
-- Table structure for location
-- ----------------------------
DROP TABLE IF EXISTS location;
CREATE TABLE location (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL,
  info varchar(255) DEFAULT NULL,
  latitude varchar(255) DEFAULT NULL,
  longitude varchar(255) DEFAULT NULL,
  time varchar(255) DEFAULT NULL,
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of location
-- ----------------------------
INSERT INTO location VALUES ('1', '1', '中国浙江省杭州市滨江区斯毕士大厦', '120.16580501873221', '30.182585972057488', '2019-05-08 17:41:09');
INSERT INTO location VALUES ('2', '1', '中国浙江省杭州市滨江区诚业路49号', '120.16580501873221', '30.182585972057488', '2019-05-08 17:41:09');
INSERT INTO location VALUES ('3', '1', '中国浙江省杭州市滨江区滨康路296号', '120.19462585449214', '30.181777954101552', '2019-05-08 19:06:38');

-- ----------------------------
-- Table structure for logs
-- ----------------------------
DROP TABLE IF EXISTS logs;
CREATE TABLE logs (
  id int(11) NOT NULL AUTO_INCREMENT,
  userid int(11) DEFAULT NULL,
  info varchar(255) DEFAULT NULL,
  time varchar(255) DEFAULT NULL,
  ip varchar(255) DEFAULT NULL,
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=18 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of logs
-- ----------------------------
INSERT INTO logs VALUES ('1', '1', '登陆系统', '2019-05-15 10:01:23', '127.0.0.1');
INSERT INTO logs VALUES ('2', '1', '登陆系统', '2019-05-15 10:01:23', '127.0.0.1');
INSERT INTO logs VALUES ('3', '1', '登陆系统', '2019-05-15 10:01:23', '127.0.0.1');
INSERT INTO logs VALUES ('4', '1', '登陆系统', '2019-05-15 10:01:23', '127.0.0.1');
INSERT INTO logs VALUES ('5', '1', '登陆系统', '2019-05-15 10:08:31', '127.0.0.1');
INSERT INTO logs VALUES ('6', '1', '登陆系统', '2019-05-15 10:28:59', '127.0.0.1');
INSERT INTO logs VALUES ('7', '2', '登陆系统', '2019-05-15 10:29:09', '127.0.0.1');
INSERT INTO logs VALUES ('8', '1', '登陆系统', '2019-05-15 10:29:27', '127.0.0.1');
INSERT INTO logs VALUES ('9', '2', '登陆系统', '2019-05-15 10:50:56', '127.0.0.1');
INSERT INTO logs VALUES ('10', '1', '登陆系统', '2019-05-15 10:51:14', '127.0.0.1');
INSERT INTO logs VALUES ('11', '2', '登陆系统', '2019-05-15 13:59:38', '127.0.0.1');
INSERT INTO logs VALUES ('12', '2', '登陆系统', '2019-05-15 14:07:40', '192.168.20.128');
INSERT INTO logs VALUES ('13', '1', '登陆系统', '2019-05-15 14:08:30', '192.168.20.128');
INSERT INTO logs VALUES ('14', '1', '登陆系统', '2019-05-15 14:09:29', '192.168.20.128');
INSERT INTO logs VALUES ('15', '2', '登陆系统', '2019-05-15 20:37:47', '127.0.0.1');
INSERT INTO logs VALUES ('16', '1', '登陆系统', '2019-05-15 20:52:05', '127.0.0.1');
INSERT INTO logs VALUES ('17', '2', '登陆系统', '2019-05-15 20:52:46', '127.0.0.1');

-- ----------------------------
-- Table structure for photo
-- ----------------------------
DROP TABLE IF EXISTS photo;
CREATE TABLE photo (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL,
  type int(11) DEFAULT NULL COMMENT '1相册2摄像头',
  path varchar(255) DEFAULT NULL COMMENT '图片的路径',
  time varchar(255) DEFAULT NULL,
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=13 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of photo
-- ----------------------------
INSERT INTO photo VALUES ('1', '1', '1', 'H:\\PHP_Project\\iosserver\\test20190405\\DCIM\\1557307924.png', '2019-05-08 19:06:38');
INSERT INTO photo VALUES ('2', '1', '1', 'H:\\PHP_Project\\iosserver\\test20190405\\DCIM\\1557308619.png', '2019-05-09 19:06:38');
INSERT INTO photo VALUES ('3', '1', '1', 'H:\\PHP_Project\\iosserver\\test20190405\\DCIM\\1557308619.png', '2019-05-09 19:06:38');
INSERT INTO photo VALUES ('4', '1', '1', 'H:\\PHP_Project\\iosserver\\test20190405\\DCIM\\1557308619.png', '2019-05-09 19:06:38');
INSERT INTO photo VALUES ('5', '1', '1', 'H:\\PHP_Project\\iosserver\\test20190405\\DCIM\\1557308619.png', '2019-05-09 19:06:38');
INSERT INTO photo VALUES ('6', '1', '1', 'H:\\PHP_Project\\iosserver\\test20190405\\DCIM\\1557309086.png', '2019-05-09 19:06:38');
INSERT INTO photo VALUES ('7', '1', '1', 'H:\\PHP_Project\\iosserver\\test20190405\\DCIM\\1557308619.png', '2019-05-09 19:06:38');
INSERT INTO photo VALUES ('8', '1', '1', 'H:\\PHP_Project\\iosserver\\test20190405\\DCIM\\1557308619.png', '2019-05-09 19:06:38');
INSERT INTO photo VALUES ('9', '1', '1', 'H:\\PHP_Project\\iosserver\\test20190405\\DCIM\\1557308619.png', '2019-05-09 19:06:38');
INSERT INTO photo VALUES ('10', '1', '1', 'H:\\PHP_Project\\iosserver\\test20190405\\DCIM\\1557308619.png', '2019-05-09 19:06:38');
INSERT INTO photo VALUES ('11', '1', '1', 'H:\\PHP_Project\\iosserver\\test20190405\\DCIM\\1557308619.png', '2019-05-09 19:06:38');
INSERT INTO photo VALUES ('12', '1', '1', 'H:\\PHP_Project\\iosserver\\test20190405\\DCIM\\1557308619.png', '2019-05-09 19:06:38');

-- ----------------------------
-- Table structure for user
-- ----------------------------
DROP TABLE IF EXISTS user;
CREATE TABLE user (
  id int(11) NOT NULL AUTO_INCREMENT,
  user varchar(255) DEFAULT NULL,
  password varchar(255) DEFAULT NULL,
  state int(2) DEFAULT '1' COMMENT '0停用1正常',
  PRIMARY KEY (id)
) ENGINE=MyISAM AUTO_INCREMENT=34 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of user
-- ----------------------------

UPDATE user SET user = 'Zhongshan 24', password = 'Nanjing1' WHERE user = 'test20190427';

UPDATE user SET user = 'Zhongshan 23', password = 'Nanjing' WHERE user = 'test20190425'

replace INTO user (user,password,state) VALUES ( 'test20190425', 'e10adc3949ba59abbe56e057f20f883e0', '1');
INSERT INTO user (user,password,state) VALUES ( 'test20190425', 'e10adc3949ba59abbe56e057f20f883e', '1');

INSERT INTO user VALUES ('2', 'test20190425', 'e10adc3949ba59abbe56e057f20f883e', '1');
INSERT INTO user VALUES ('1', 'admin', 'e10adc3949ba59abbe56e057f20f883e', '1');
