/*
Navicat MySQL Data Transfer

Source Server         : 127.0.0.1
Source Server Version : 50505
Source Host           : localhost:3306
Source Database       : ios

Target Server Type    : MYSQL
Target Server Version : 50505
File Encoding         : 65001

Date: 2019-05-23 16:56:42
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
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

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
  group_id int(11) DEFAULT '0' COMMENT '分组id',
  PRIMARY KEY (id)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for client_token
-- ----------------------------
DROP TABLE IF EXISTS client_token;
CREATE TABLE client_token (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL,
  token varchar(255) DEFAULT NULL,
  PRIMARY KEY (id)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

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
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for grouplist
-- ----------------------------
DROP TABLE IF EXISTS grouplist;
CREATE TABLE grouplist (
  id int(11) NOT NULL AUTO_INCREMENT,
  group_name varchar(255) DEFAULT NULL,
  userid int(11) DEFAULT NULL,
  PRIMARY KEY (id)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

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
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

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
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

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
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

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
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
