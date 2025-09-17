/*
 Navicat Premium Data Transfer

 Source Server         : 海牙rds
 Source Server Type    : MySQL
 Source Server Version : 80034
 Source Host           : rm-bp15fg96gd03xcpu8wo.mysql.rds.aliyuncs.com:3306
 Source Schema         : gamedb_base_becopy

 Target Server Type    : MySQL
 Target Server Version : 80034
 File Encoding         : 65001

 Date: 14/08/2024 10:43:47
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for account_login_info
-- ----------------------------
DROP TABLE IF EXISTS `account_login_info`;
CREATE TABLE `account_login_info` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `SERVERID` int unsigned NOT NULL DEFAULT '0',
  `ACCOUNT` varchar(50) NOT NULL DEFAULT '',
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `LOGINTIME` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`),
  UNIQUE KEY `ACCOUNT` (`ACCOUNT`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for activity_config
-- ----------------------------
DROP TABLE IF EXISTS `activity_config`;
CREATE TABLE `activity_config` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `ACTIVITY_ID` int unsigned NOT NULL DEFAULT '0',
  `CONFIG` varchar(4096) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for activity_log
-- ----------------------------
DROP TABLE IF EXISTS `activity_log`;
CREATE TABLE `activity_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `ACTIVITY_ID` int unsigned NOT NULL DEFAULT '0',
  `ACTIVITY_NAME` varchar(50) NOT NULL DEFAULT '',
  `STEP_ID` int unsigned NOT NULL DEFAULT '0',
  `STEP_DESC` varchar(50) NOT NULL DEFAULT '',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for arena_got_reward_charid
-- ----------------------------
DROP TABLE IF EXISTS `arena_got_reward_charid`;
CREATE TABLE `arena_got_reward_charid` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `REWARD_TIMESTAMP` int unsigned NOT NULL DEFAULT '0',
  `BINARYDATA` blob,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for arena_user_info
-- ----------------------------
DROP TABLE IF EXISTS `arena_user_info`;
CREATE TABLE `arena_user_info` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `CHALLENGENUM` int unsigned NOT NULL DEFAULT '0',
  `LASTPKTIME` int unsigned NOT NULL DEFAULT '0',
  `RESETNUM` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for arenasortlist
-- ----------------------------
DROP TABLE IF EXISTS `arenasortlist`;
CREATE TABLE `arenasortlist` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `RANK` int unsigned NOT NULL DEFAULT '0',
  `BATTLEPOWER` int unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for capturetresure
-- ----------------------------
DROP TABLE IF EXISTS `capturetresure`;
CREATE TABLE `capturetresure` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `PEACEENDTIME` int unsigned NOT NULL DEFAULT '0',
  `DATA` varchar(600) NOT NULL DEFAULT '',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for closebetareward
-- ----------------------------
DROP TABLE IF EXISTS `closebetareward`;
CREATE TABLE `closebetareward` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `ACCOUNT` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for compensation
-- ----------------------------
DROP TABLE IF EXISTS `compensation`;
CREATE TABLE `compensation` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `COMPENSATION_TYPE` tinyint unsigned NOT NULL DEFAULT '0',
  `TITLE` varchar(25) NOT NULL DEFAULT '',
  `CONTENT` varchar(150) NOT NULL DEFAULT '',
  `REWARD` varchar(150) NOT NULL DEFAULT '',
  `CREATETIME` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for copymap_log
-- ----------------------------
DROP TABLE IF EXISTS `copymap_log`;
CREATE TABLE `copymap_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `COPYMAPID` int unsigned NOT NULL DEFAULT '0',
  `WIN` tinyint unsigned NOT NULL DEFAULT '0',
  `ACTION` varchar(20) NOT NULL DEFAULT '',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for create_role_log
-- ----------------------------
DROP TABLE IF EXISTS `create_role_log`;
CREATE TABLE `create_role_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `OP_ID` int unsigned NOT NULL DEFAULT '0',
  `DEVICE_ID` varchar(125) NOT NULL DEFAULT '',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for dailytask_log
-- ----------------------------
DROP TABLE IF EXISTS `dailytask_log`;
CREATE TABLE `dailytask_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `TASKID` int unsigned NOT NULL DEFAULT '0',
  `TASKNAME` varchar(33) NOT NULL DEFAULT '',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for devilcall
-- ----------------------------
DROP TABLE IF EXISTS `devilcall`;
CREATE TABLE `devilcall` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `DATA` blob,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for discover_info
-- ----------------------------
DROP TABLE IF EXISTS `discover_info`;
CREATE TABLE `discover_info` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `MAPID` tinyint unsigned NOT NULL DEFAULT '0',
  `PLACE` tinyint unsigned NOT NULL DEFAULT '0',
  `TASKID` tinyint unsigned NOT NULL DEFAULT '0',
  `STATE` tinyint unsigned NOT NULL DEFAULT '0',
  `BATTLENUM` tinyint unsigned NOT NULL DEFAULT '0',
  `INTEGRAL` int unsigned NOT NULL DEFAULT '0',
  `REFRESHNUM` int unsigned NOT NULL DEFAULT '0',
  `PLAYDICENUM` int unsigned NOT NULL DEFAULT '0',
  `CANUSENUM` int unsigned NOT NULL DEFAULT '0',
  `HASRECEIVEDID` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for drawhero_activity
-- ----------------------------
DROP TABLE IF EXISTS `drawhero_activity`;
CREATE TABLE `drawhero_activity` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `ACTIVITY_ID` int unsigned NOT NULL DEFAULT '0',
  `IS_START` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for drawhero_rank
-- ----------------------------
DROP TABLE IF EXISTS `drawhero_rank`;
CREATE TABLE `drawhero_rank` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `SCORE` int unsigned NOT NULL DEFAULT '0',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for drawhero_score
-- ----------------------------
DROP TABLE IF EXISTS `drawhero_score`;
CREATE TABLE `drawhero_score` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `SCORE` int unsigned NOT NULL DEFAULT '0',
  `REWARDS` varchar(33) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for emerald_dream_open_time
-- ----------------------------
DROP TABLE IF EXISTS `emerald_dream_open_time`;
CREATE TABLE `emerald_dream_open_time` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `ACTIVITY_ID` int unsigned NOT NULL DEFAULT '0',
  `BEGIN_TIME` int unsigned NOT NULL DEFAULT '0',
  `END_TIME` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for emerald_dream_rank
-- ----------------------------
DROP TABLE IF EXISTS `emerald_dream_rank`;
CREATE TABLE `emerald_dream_rank` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `DAMAGE` int unsigned NOT NULL DEFAULT '0',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `LEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for enter_select_page
-- ----------------------------
DROP TABLE IF EXISTS `enter_select_page`;
CREATE TABLE `enter_select_page` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `FIRST_ENTER_TIME` datetime NOT NULL,
  `FIRST_CREATE_ROLE_TIME` datetime NOT NULL,
  `LAST_ENTER_TIME` datetime NOT NULL,
  `OP_ID` int unsigned NOT NULL DEFAULT '0',
  `DEVICE_ID` varchar(125) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for enter_select_page_log
-- ----------------------------
DROP TABLE IF EXISTS `enter_select_page_log`;
CREATE TABLE `enter_select_page_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `FIRST_ENTER_TIME` datetime NOT NULL,
  `OP_ID` int unsigned NOT NULL DEFAULT '0',
  `DEVICE_ID` varchar(125) NOT NULL DEFAULT '',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for equip_chg_log
-- ----------------------------
DROP TABLE IF EXISTS `equip_chg_log`;
CREATE TABLE `equip_chg_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `ITEMID` int unsigned NOT NULL DEFAULT '0',
  `OLDRAISELV` smallint unsigned NOT NULL DEFAULT '0',
  `NEWRAISELV` smallint unsigned NOT NULL DEFAULT '0',
  `OLDREFINELV` smallint unsigned NOT NULL DEFAULT '0',
  `NEWREFINELV` smallint unsigned NOT NULL DEFAULT '0',
  `ACTION` varchar(20) NOT NULL DEFAULT '',
  `DOTIME` datetime NOT NULL,
  `DECGOLDCOIN` int unsigned NOT NULL DEFAULT '0',
  `DECYUNTIE` int unsigned NOT NULL DEFAULT '0',
  `DECITEM` int unsigned NOT NULL DEFAULT '0',
  `ADDGOLDCOIN` int unsigned NOT NULL DEFAULT '0',
  `ADDYUNTIE` int unsigned NOT NULL DEFAULT '0',
  `ADDITEM` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for exchange_list
-- ----------------------------
DROP TABLE IF EXISTS `exchange_list`;
CREATE TABLE `exchange_list` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `FLAG` int unsigned NOT NULL DEFAULT '0',
  `NUM` int unsigned NOT NULL DEFAULT '0',
  `MODULE_NAME` varchar(33) NOT NULL DEFAULT '',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for expedition
-- ----------------------------
DROP TABLE IF EXISTS `expedition`;
CREATE TABLE `expedition` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `DATA` blob,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for first_kill
-- ----------------------------
DROP TABLE IF EXISTS `first_kill`;
CREATE TABLE `first_kill` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `COPYMAPID` int unsigned NOT NULL DEFAULT '0',
  `CHARID1` int unsigned NOT NULL DEFAULT '0',
  `JOB1` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX1` tinyint unsigned NOT NULL DEFAULT '0',
  `NAME1` varchar(33) NOT NULL DEFAULT '',
  `LEVEL1` smallint unsigned NOT NULL DEFAULT '0',
  `BATTLEPOWER1` int unsigned NOT NULL DEFAULT '0',
  `CHARID2` int unsigned NOT NULL DEFAULT '0',
  `JOB2` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX2` tinyint unsigned NOT NULL DEFAULT '0',
  `NAME2` varchar(33) NOT NULL DEFAULT '',
  `LEVEL2` smallint unsigned NOT NULL DEFAULT '0',
  `BATTLEPOWER2` int unsigned NOT NULL DEFAULT '0',
  `CHARID3` int unsigned NOT NULL DEFAULT '0',
  `JOB3` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX3` tinyint unsigned NOT NULL DEFAULT '0',
  `NAME3` varchar(33) NOT NULL DEFAULT '',
  `LEVEL3` smallint unsigned NOT NULL DEFAULT '0',
  `BATTLEPOWER3` int unsigned NOT NULL DEFAULT '0',
  `KILLTIME` int unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID1` smallint unsigned NOT NULL DEFAULT '1',
  `USERPHOTOID2` smallint unsigned NOT NULL DEFAULT '1',
  `USERPHOTOID3` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for friend
-- ----------------------------
DROP TABLE IF EXISTS `friend`;
CREATE TABLE `friend` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `OTHERID` int unsigned NOT NULL DEFAULT '0',
  `OTHERNAME` varchar(33) NOT NULL DEFAULT '',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for gift_bag
-- ----------------------------
DROP TABLE IF EXISTS `gift_bag`;
CREATE TABLE `gift_bag` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `GIFT_BAG_ID` int unsigned NOT NULL DEFAULT '0',
  `GIFT_CONTENT` varchar(250) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for global_var
-- ----------------------------
DROP TABLE IF EXISTS `global_var`;
CREATE TABLE `global_var` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `KEY` int unsigned NOT NULL DEFAULT '0',
  `VALUE` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for gloryfashion_activity
-- ----------------------------
DROP TABLE IF EXISTS `gloryfashion_activity`;
CREATE TABLE `gloryfashion_activity` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `ACTIVITY_ID` int unsigned NOT NULL DEFAULT '0',
  `IS_START` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for gloryfashion_rank
-- ----------------------------
DROP TABLE IF EXISTS `gloryfashion_rank`;
CREATE TABLE `gloryfashion_rank` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `BUYTIME` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for guard_info
-- ----------------------------
DROP TABLE IF EXISTS `guard_info`;
CREATE TABLE `guard_info` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `QUALITY` tinyint unsigned NOT NULL DEFAULT '0',
  `ENDTIME` int unsigned NOT NULL DEFAULT '0',
  `HELPFRIEND` int unsigned NOT NULL DEFAULT '0',
  `BEINTERCEPTNUM` smallint unsigned NOT NULL DEFAULT '0',
  `REFRESHNUM` smallint unsigned NOT NULL DEFAULT '0',
  `DECRESNUM1` int unsigned NOT NULL DEFAULT '0',
  `DECRESNUM2` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for guard_user_info
-- ----------------------------
DROP TABLE IF EXISTS `guard_user_info`;
CREATE TABLE `guard_user_info` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `GUARDNUM` smallint unsigned NOT NULL DEFAULT '0',
  `INTERCEPTNUM` smallint unsigned NOT NULL DEFAULT '0',
  `HELPNUM` smallint unsigned NOT NULL DEFAULT '0',
  `REFRESHFLAG` tinyint unsigned NOT NULL DEFAULT '0',
  `BUFF` smallint unsigned NOT NULL DEFAULT '0',
  `RESETNUM` smallint unsigned NOT NULL DEFAULT '0',
  `LASTINTERCEPTTIME` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for guild
-- ----------------------------
DROP TABLE IF EXISTS `guild`;
CREATE TABLE `guild` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `LEADERID` int unsigned NOT NULL DEFAULT '0',
  `CONTRIBUTE` int unsigned NOT NULL DEFAULT '0',
  `HALLLV` tinyint unsigned NOT NULL DEFAULT '0',
  `PRAYBUILDINGLV` tinyint unsigned NOT NULL DEFAULT '0',
  `SHOPLV` tinyint unsigned NOT NULL DEFAULT '0',
  `FIRETIME` int unsigned NOT NULL DEFAULT '0',
  `SIGN` varchar(150) NOT NULL DEFAULT '',
  `NOTICE` varchar(150) NOT NULL DEFAULT '',
  `BADGE` smallint unsigned NOT NULL DEFAULT '0',
  `ACTIVITY_DEGREE` int unsigned NOT NULL DEFAULT '0',
  `BLACKMARKETLV` tinyint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for guild_copymap
-- ----------------------------
DROP TABLE IF EXISTS `guild_copymap`;
CREATE TABLE `guild_copymap` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `COPYMAP_ID` int unsigned NOT NULL DEFAULT '0',
  `OPENTIME` int unsigned NOT NULL DEFAULT '0',
  `BOSSHP` int unsigned NOT NULL DEFAULT '0',
  `STEPID` smallint unsigned NOT NULL DEFAULT '0',
  `GUILD_ID` int unsigned NOT NULL DEFAULT '0',
  `KILL_CHARID` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for guild_copymap_score_rank
-- ----------------------------
DROP TABLE IF EXISTS `guild_copymap_score_rank`;
CREATE TABLE `guild_copymap_score_rank` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `COPYMAP_ID` int unsigned NOT NULL DEFAULT '0',
  `SCORE` int unsigned NOT NULL DEFAULT '0',
  `GUILD_ID` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for guild_log
-- ----------------------------
DROP TABLE IF EXISTS `guild_log`;
CREATE TABLE `guild_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `OP_ID` int unsigned NOT NULL DEFAULT '0',
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `LEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `VIPLV` smallint unsigned NOT NULL DEFAULT '0',
  `GUILDID` int unsigned NOT NULL DEFAULT '0',
  `ACTION` varchar(20) NOT NULL DEFAULT '',
  `OP_OBJ` varchar(20) NOT NULL DEFAULT '',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for guild_member
-- ----------------------------
DROP TABLE IF EXISTS `guild_member`;
CREATE TABLE `guild_member` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `TITLE` tinyint unsigned NOT NULL DEFAULT '0',
  `CONTRIBUTE` int unsigned NOT NULL DEFAULT '0',
  `GUILDID` int unsigned NOT NULL DEFAULT '0',
  `JOININTIME` int unsigned NOT NULL DEFAULT '0',
  `LASTLOGINTIME` int unsigned NOT NULL DEFAULT '0',
  `ACTIVITY_DEGREE` int unsigned NOT NULL DEFAULT '0',
  `CHALLENGE_NUM` smallint unsigned NOT NULL DEFAULT '0',
  `ASSIST_NUM` smallint unsigned NOT NULL DEFAULT '0',
  `DAMAGE1` int unsigned NOT NULL DEFAULT '0',
  `DAMAGE2` int unsigned NOT NULL DEFAULT '0',
  `DAMAGE3` int unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for guild_res_log
-- ----------------------------
DROP TABLE IF EXISTS `guild_res_log`;
CREATE TABLE `guild_res_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `GUILDID` int unsigned NOT NULL DEFAULT '0',
  `RES` varchar(20) NOT NULL DEFAULT '',
  `OLDVAL` int unsigned NOT NULL DEFAULT '0',
  `NEWVAL` int unsigned NOT NULL DEFAULT '0',
  `REASON` varchar(20) NOT NULL DEFAULT '',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for guild_trace
-- ----------------------------
DROP TABLE IF EXISTS `guild_trace`;
CREATE TABLE `guild_trace` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `TRACEID` int unsigned NOT NULL DEFAULT '0',
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `DOTIME` int unsigned NOT NULL DEFAULT '0',
  `GUILDID` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for haveinfo_statics
-- ----------------------------
DROP TABLE IF EXISTS `haveinfo_statics`;
CREATE TABLE `haveinfo_statics` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `NEWUSERS` varchar(250) NOT NULL DEFAULT '',
  `DAY_2` varchar(250) NOT NULL DEFAULT '',
  `DAY_3` varchar(250) NOT NULL DEFAULT '',
  `DAY_4` varchar(250) NOT NULL DEFAULT '',
  `DAY_5` varchar(250) NOT NULL DEFAULT '',
  `DAY_6` varchar(250) NOT NULL DEFAULT '',
  `DAY_7` varchar(250) NOT NULL DEFAULT '',
  `DAY_14` varchar(250) NOT NULL DEFAULT '',
  `DAY_30` varchar(250) NOT NULL DEFAULT '',
  `TIME` date NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for hero_league_champion
-- ----------------------------
DROP TABLE IF EXISTS `hero_league_champion`;
CREATE TABLE `hero_league_champion` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `AREACODE` int unsigned NOT NULL DEFAULT '0',
  `LEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `BATTLEPOWER` int unsigned NOT NULL DEFAULT '0',
  `HEADID` int unsigned NOT NULL DEFAULT '0',
  `BODYID` int unsigned NOT NULL DEFAULT '0',
  `WEAPONID` int unsigned NOT NULL DEFAULT '0',
  `CLOACKID` int unsigned NOT NULL DEFAULT '0',
  `WINGID` int unsigned NOT NULL DEFAULT '0',
  `SHOEID` int unsigned NOT NULL DEFAULT '0',
  `FASHIONHAIR` int unsigned NOT NULL DEFAULT '0',
  `FASHIONFACE` int unsigned NOT NULL DEFAULT '0',
  `FASHIONBODY` int unsigned NOT NULL DEFAULT '0',
  `FASHIONWEAPON` int unsigned NOT NULL DEFAULT '0',
  `FASHIONSHOE` int unsigned NOT NULL DEFAULT '0',
  `FASHIONDECORATION` int unsigned NOT NULL DEFAULT '0',
  `EFFECTID` int unsigned NOT NULL DEFAULT '0',
  `WINGBREAKLV` int unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for hero_league_eight
-- ----------------------------
DROP TABLE IF EXISTS `hero_league_eight`;
CREATE TABLE `hero_league_eight` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `MATCH` int unsigned NOT NULL DEFAULT '0',
  `WINORLOSS` tinyint unsigned NOT NULL DEFAULT '0',
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `PUBCHARID` int unsigned NOT NULL DEFAULT '0',
  `AREACODE` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `LEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `VIPLV` smallint unsigned NOT NULL DEFAULT '0',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `BATTLEPOWER` int unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for hero_league_four
-- ----------------------------
DROP TABLE IF EXISTS `hero_league_four`;
CREATE TABLE `hero_league_four` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `MATCH` int unsigned NOT NULL DEFAULT '0',
  `WINORLOSS` tinyint unsigned NOT NULL DEFAULT '0',
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `PUBCHARID` int unsigned NOT NULL DEFAULT '0',
  `AREACODE` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `LEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `VIPLV` smallint unsigned NOT NULL DEFAULT '0',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `BATTLEPOWER` int unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for hero_league_info
-- ----------------------------
DROP TABLE IF EXISTS `hero_league_info`;
CREATE TABLE `hero_league_info` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `PUBCHARID` int unsigned NOT NULL DEFAULT '0',
  `AREACODE` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `LEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `VIPLV` smallint unsigned NOT NULL DEFAULT '0',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `BATTLEPOWER` int unsigned NOT NULL DEFAULT '0',
  `PUBSERVERINDEX` int unsigned NOT NULL DEFAULT '0',
  `SCORE` int unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for hero_league_sixteen
-- ----------------------------
DROP TABLE IF EXISTS `hero_league_sixteen`;
CREATE TABLE `hero_league_sixteen` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `MATCH` int unsigned NOT NULL DEFAULT '0',
  `WINORLOSS` tinyint unsigned NOT NULL DEFAULT '0',
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `PUBCHARID` int unsigned NOT NULL DEFAULT '0',
  `AREACODE` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `LEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `VIPLV` smallint unsigned NOT NULL DEFAULT '0',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `BATTLEPOWER` int unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for hero_league_two
-- ----------------------------
DROP TABLE IF EXISTS `hero_league_two`;
CREATE TABLE `hero_league_two` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `MATCH` int unsigned NOT NULL DEFAULT '0',
  `WINORLOSS` tinyint unsigned NOT NULL DEFAULT '0',
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `PUBCHARID` int unsigned NOT NULL DEFAULT '0',
  `AREACODE` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `LEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `VIPLV` smallint unsigned NOT NULL DEFAULT '0',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `BATTLEPOWER` int unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for hero_prop_chg_log
-- ----------------------------
DROP TABLE IF EXISTS `hero_prop_chg_log`;
CREATE TABLE `hero_prop_chg_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `LEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `VIPLEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `HEROID` int unsigned NOT NULL DEFAULT '0',
  `ACTION` varchar(10) NOT NULL DEFAULT '',
  `OLDLV` smallint unsigned NOT NULL DEFAULT '0',
  `NEWLV` smallint unsigned NOT NULL DEFAULT '0',
  `OLDEVOLV` smallint unsigned NOT NULL DEFAULT '0',
  `NEWEVOLV` smallint unsigned NOT NULL DEFAULT '0',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for hero_rank_list
-- ----------------------------
DROP TABLE IF EXISTS `hero_rank_list`;
CREATE TABLE `hero_rank_list` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `SESSION` int unsigned NOT NULL DEFAULT '0',
  `RANK` tinyint unsigned NOT NULL DEFAULT '0',
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `PUBCHARID` int unsigned NOT NULL DEFAULT '0',
  `AREACODE` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `LEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `VIPLV` smallint unsigned NOT NULL DEFAULT '0',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `BATTLEPOWER` int unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for history_arena_sortlist
-- ----------------------------
DROP TABLE IF EXISTS `history_arena_sortlist`;
CREATE TABLE `history_arena_sortlist` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `REWARD_TIMESTAMP` int unsigned NOT NULL DEFAULT '0',
  `BINARYDATA` blob,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for history_one2onepvp_sortlist
-- ----------------------------
DROP TABLE IF EXISTS `history_one2onepvp_sortlist`;
CREATE TABLE `history_one2onepvp_sortlist` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `RANK_TYPE` tinyint unsigned NOT NULL DEFAULT '0',
  `REWARD_TIMESTAMP` int unsigned NOT NULL DEFAULT '0',
  `BINARYDATA` blob,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for honour_chg_log
-- ----------------------------
DROP TABLE IF EXISTS `honour_chg_log`;
CREATE TABLE `honour_chg_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `OLDHONOUR` int unsigned NOT NULL DEFAULT '0',
  `NEWHONOUR` int unsigned NOT NULL DEFAULT '0',
  `REASON` varchar(50) NOT NULL DEFAULT '',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for inner_update_info
-- ----------------------------
DROP TABLE IF EXISTS `inner_update_info`;
CREATE TABLE `inner_update_info` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `GAME_ID` int unsigned NOT NULL DEFAULT '0',
  `OP_ID` int unsigned NOT NULL DEFAULT '0',
  `GAME_OP_ID` int unsigned NOT NULL DEFAULT '0',
  `VERSION` varchar(25) NOT NULL DEFAULT '',
  `UPGRADE_URL` varchar(250) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for invite_info
-- ----------------------------
DROP TABLE IF EXISTS `invite_info`;
CREATE TABLE `invite_info` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `INVITE_CODE` varchar(50) NOT NULL DEFAULT '',
  `INVITE_COUNT` int unsigned NOT NULL DEFAULT '0',
  `INVITE_REWARD` varchar(100) NOT NULL DEFAULT '',
  `BE_INVITED` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for item_chg_log
-- ----------------------------
DROP TABLE IF EXISTS `item_chg_log`;
CREATE TABLE `item_chg_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `ITEMNAME` varchar(33) NOT NULL DEFAULT '',
  `NUM` int unsigned NOT NULL DEFAULT '0',
  `REASON` varchar(50) NOT NULL DEFAULT '',
  `LEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `VIPLEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `ISADD` tinyint unsigned NOT NULL DEFAULT '0',
  `ITEM_TYPE` smallint unsigned NOT NULL DEFAULT '0',
  `ITEM_QUALITY` smallint unsigned NOT NULL DEFAULT '0',
  `ITEM_RAISELV` smallint unsigned NOT NULL DEFAULT '0',
  `ITEM_REFINELV` smallint unsigned NOT NULL DEFAULT '0',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for leitai_rank
-- ----------------------------
DROP TABLE IF EXISTS `leitai_rank`;
CREATE TABLE `leitai_rank` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `USETIME` int unsigned NOT NULL DEFAULT '0',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for limit_pet_activity
-- ----------------------------
DROP TABLE IF EXISTS `limit_pet_activity`;
CREATE TABLE `limit_pet_activity` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `GROUPID` int unsigned NOT NULL DEFAULT '0',
  `PETID` int unsigned NOT NULL DEFAULT '0',
  `START` int unsigned NOT NULL DEFAULT '0',
  `END` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for login_ip_type
-- ----------------------------
DROP TABLE IF EXISTS `login_ip_type`;
CREATE TABLE `login_ip_type` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `IP_TYPE` smallint unsigned NOT NULL DEFAULT '0',
  `IP` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for logininfo
-- ----------------------------
DROP TABLE IF EXISTS `logininfo`;
CREATE TABLE `logininfo` (
  `USERID` int unsigned NOT NULL AUTO_INCREMENT,
  `USERNAME` varchar(49) NOT NULL DEFAULT '',
  `PASSWORD` varchar(33) NOT NULL DEFAULT '',
  `CREATETIME` int unsigned NOT NULL DEFAULT '0',
  `LASTLOGINTIME` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`USERID`),
  UNIQUE KEY `USERNAME` (`USERNAME`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for mail
-- ----------------------------
DROP TABLE IF EXISTS `mail`;
CREATE TABLE `mail` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `SENDERID` int unsigned NOT NULL DEFAULT '0',
  `SENDERNAME` varchar(33) NOT NULL DEFAULT '',
  `RECEIVERID` int unsigned NOT NULL DEFAULT '0',
  `RECEIVERNAME` varchar(33) NOT NULL DEFAULT '',
  `TITILE` varchar(100) NOT NULL DEFAULT '',
  `CONTENT` varchar(250) NOT NULL DEFAULT '',
  `ISREAD` tinyint unsigned NOT NULL DEFAULT '0',
  `ISFETCHATTACH` tinyint unsigned NOT NULL DEFAULT '0',
  `MAILTYPE` tinyint unsigned NOT NULL DEFAULT '0',
  `ATTACHID1` int unsigned NOT NULL DEFAULT '0',
  `ATTACHNUM1` int unsigned NOT NULL DEFAULT '0',
  `ATTACHID2` int unsigned NOT NULL DEFAULT '0',
  `ATTACHNUM2` int unsigned NOT NULL DEFAULT '0',
  `ATTACHID3` int unsigned NOT NULL DEFAULT '0',
  `ATTACHNUM3` int unsigned NOT NULL DEFAULT '0',
  `SENDTIME` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for maintennaceinfo
-- ----------------------------
DROP TABLE IF EXISTS `maintennaceinfo`;
CREATE TABLE `maintennaceinfo` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `MAINTENNACE_BEGIN` int unsigned NOT NULL DEFAULT '0',
  `MAINTENNACE_LASTTIME` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for mall_buy_log
-- ----------------------------
DROP TABLE IF EXISTS `mall_buy_log`;
CREATE TABLE `mall_buy_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `LEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `VIPLEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `SHOPID` int unsigned NOT NULL DEFAULT '0',
  `RESID` int unsigned NOT NULL DEFAULT '0',
  `RESNUM` int unsigned NOT NULL DEFAULT '0',
  `ITEMID` int unsigned NOT NULL DEFAULT '0',
  `ITEMNUM` int unsigned NOT NULL DEFAULT '0',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for mallitem_statics
-- ----------------------------
DROP TABLE IF EXISTS `mallitem_statics`;
CREATE TABLE `mallitem_statics` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `ITEMID` int unsigned NOT NULL DEFAULT '0',
  `CONSUME_DIAMOND` int unsigned NOT NULL DEFAULT '0',
  `TIMESTAMP` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for manorapply
-- ----------------------------
DROP TABLE IF EXISTS `manorapply`;
CREATE TABLE `manorapply` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `MANORID` int unsigned NOT NULL DEFAULT '0',
  `GUILDID` int unsigned NOT NULL DEFAULT '0',
  `CONTRIBUTE` int unsigned NOT NULL DEFAULT '0',
  `CREATETIME` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for manorinfo
-- ----------------------------
DROP TABLE IF EXISTS `manorinfo`;
CREATE TABLE `manorinfo` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `MANORID` smallint unsigned NOT NULL DEFAULT '0',
  `OWNER_GUILDID` int unsigned NOT NULL DEFAULT '0',
  `PK_SIDEA` int unsigned NOT NULL DEFAULT '0',
  `PK_SIDEB` int unsigned NOT NULL DEFAULT '0',
  `LOSER_GUILDID` int unsigned NOT NULL DEFAULT '0',
  `IS_FINISH` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for manorstate
-- ----------------------------
DROP TABLE IF EXISTS `manorstate`;
CREATE TABLE `manorstate` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `STATE` smallint unsigned NOT NULL DEFAULT '0',
  `GIVEREWARD` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for marry_info
-- ----------------------------
DROP TABLE IF EXISTS `marry_info`;
CREATE TABLE `marry_info` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `HUSBAND` int unsigned NOT NULL DEFAULT '0',
  `WIFE` int unsigned NOT NULL DEFAULT '0',
  `HUS_RING` int unsigned NOT NULL DEFAULT '0',
  `WIFE_RING` int unsigned NOT NULL DEFAULT '0',
  `WEDDING_ID` int unsigned NOT NULL DEFAULT '0',
  `EXPIRE_TIME` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for max_charid
-- ----------------------------
DROP TABLE IF EXISTS `max_charid`;
CREATE TABLE `max_charid` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `ZONEID` smallint unsigned NOT NULL DEFAULT '0',
  `MAXID` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for module_history_rank
-- ----------------------------
DROP TABLE IF EXISTS `module_history_rank`;
CREATE TABLE `module_history_rank` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `MODULE_ID` int unsigned NOT NULL DEFAULT '0',
  `INFO` varchar(100) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for module_log
-- ----------------------------
DROP TABLE IF EXISTS `module_log`;
CREATE TABLE `module_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `OP_ID` int unsigned NOT NULL DEFAULT '0',
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `LEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `VIPLV` smallint unsigned NOT NULL DEFAULT '0',
  `MODULE` varchar(20) NOT NULL DEFAULT '',
  `COPYMAPID` int unsigned NOT NULL DEFAULT '0',
  `OTHERCHARID` varchar(50) NOT NULL DEFAULT '',
  `RESULT` tinyint unsigned NOT NULL DEFAULT '0',
  `OLDRANK` int unsigned NOT NULL DEFAULT '0',
  `NEWRANK` int unsigned NOT NULL DEFAULT '0',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for notice
-- ----------------------------
DROP TABLE IF EXISTS `notice`;
CREATE TABLE `notice` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `NOTICEID` int unsigned NOT NULL DEFAULT '0',
  `TITLE` varchar(50) NOT NULL DEFAULT '',
  `CONTENT` varchar(4096) NOT NULL DEFAULT '',
  `ORDERID` smallint unsigned NOT NULL DEFAULT '0',
  `CREATETIME` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for one2one_pvp_got_reward_charid
-- ----------------------------
DROP TABLE IF EXISTS `one2one_pvp_got_reward_charid`;
CREATE TABLE `one2one_pvp_got_reward_charid` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `REWARD_TIMESTAMP` int unsigned NOT NULL DEFAULT '0',
  `BINARYDATA` blob,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for one2onepvp_rank
-- ----------------------------
DROP TABLE IF EXISTS `one2onepvp_rank`;
CREATE TABLE `one2onepvp_rank` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `SCORE` int unsigned NOT NULL DEFAULT '0',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for one2onepvp_rank_lastweek
-- ----------------------------
DROP TABLE IF EXISTS `one2onepvp_rank_lastweek`;
CREATE TABLE `one2onepvp_rank_lastweek` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `RANK` int unsigned NOT NULL DEFAULT '0',
  `SCORE` int unsigned NOT NULL DEFAULT '0',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for online_statics
-- ----------------------------
DROP TABLE IF EXISTS `online_statics`;
CREATE TABLE `online_statics` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `YEAR` int unsigned NOT NULL DEFAULT '0',
  `MONTH` int unsigned NOT NULL DEFAULT '0',
  `DAY` int unsigned NOT NULL DEFAULT '0',
  `TIMESTAMP` int unsigned NOT NULL DEFAULT '0',
  `ONLINENUM` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for open_server_info
-- ----------------------------
DROP TABLE IF EXISTS `open_server_info`;
CREATE TABLE `open_server_info` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `OPEN_TIME` int unsigned NOT NULL DEFAULT '0',
  `OPEN_DAY` int unsigned NOT NULL DEFAULT '0',
  `LASTREFRESHTIME` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for open_svr_power_rank
-- ----------------------------
DROP TABLE IF EXISTS `open_svr_power_rank`;
CREATE TABLE `open_svr_power_rank` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `REWARD` tinyint unsigned NOT NULL DEFAULT '0',
  `BINARYDATA` blob,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for operate_activity
-- ----------------------------
DROP TABLE IF EXISTS `operate_activity`;
CREATE TABLE `operate_activity` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `DATA` blob,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for question_result
-- ----------------------------
DROP TABLE IF EXISTS `question_result`;
CREATE TABLE `question_result` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `QUESTION_ID` int unsigned NOT NULL DEFAULT '0',
  `PROBLEM_ID` int unsigned NOT NULL DEFAULT '0',
  `ANSWER_A` int unsigned NOT NULL DEFAULT '0',
  `ANSWER_B` int unsigned NOT NULL DEFAULT '0',
  `ANSWER_C` int unsigned NOT NULL DEFAULT '0',
  `ANSWER_D` int unsigned NOT NULL DEFAULT '0',
  `ANSWER_E` int unsigned NOT NULL DEFAULT '0',
  `ANSWER_F` int unsigned NOT NULL DEFAULT '0',
  `ANSWER_G` int unsigned NOT NULL DEFAULT '0',
  `ANSWER_H` int unsigned NOT NULL DEFAULT '0',
  `ANSWER_I` int unsigned NOT NULL DEFAULT '0',
  `ANSWER_J` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for recharge_info
-- ----------------------------
DROP TABLE IF EXISTS `recharge_info`;
CREATE TABLE `recharge_info` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `MONEY` int unsigned NOT NULL DEFAULT '0',
  `ITEMID` int unsigned NOT NULL DEFAULT '0',
  `TID` varchar(50) NOT NULL DEFAULT '',
  `OBTAIN` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for recharge_statics
-- ----------------------------
DROP TABLE IF EXISTS `recharge_statics`;
CREATE TABLE `recharge_statics` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `USERS` int unsigned NOT NULL DEFAULT '0',
  `TIMES` int unsigned NOT NULL DEFAULT '0',
  `ACCOUNT` int unsigned NOT NULL DEFAULT '0',
  `CASH` int unsigned NOT NULL DEFAULT '0',
  `DAU` int unsigned NOT NULL DEFAULT '0',
  `PCU` int unsigned NOT NULL DEFAULT '0',
  `TIME` date NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for res_chg_log
-- ----------------------------
DROP TABLE IF EXISTS `res_chg_log`;
CREATE TABLE `res_chg_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `RESNAME` varchar(33) NOT NULL DEFAULT '',
  `ADDNUM` int unsigned NOT NULL DEFAULT '0',
  `OLDVALUE` int unsigned NOT NULL DEFAULT '0',
  `NEWVALUE` int unsigned NOT NULL DEFAULT '0',
  `REASON` varchar(50) NOT NULL DEFAULT '',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for roleinfo
-- ----------------------------
DROP TABLE IF EXISTS `roleinfo`;
CREATE TABLE `roleinfo` (
  `CHARID` int unsigned NOT NULL AUTO_INCREMENT,
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `LEVEL` smallint unsigned NOT NULL DEFAULT '0',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `EXP` int unsigned NOT NULL DEFAULT '0',
  `GOLDCOIN` int unsigned NOT NULL DEFAULT '0',
  `DIAMOND` int unsigned NOT NULL DEFAULT '0',
  `GIVEDIAMOND` int unsigned NOT NULL DEFAULT '0',
  `HEISHIJIEJING` int unsigned NOT NULL DEFAULT '0',
  `REPUTATION` int unsigned NOT NULL DEFAULT '0',
  `HONOUR` int unsigned NOT NULL DEFAULT '0',
  `BRAVEVAL` int unsigned NOT NULL DEFAULT '0',
  `PEACKCERTIFICATE` int unsigned NOT NULL DEFAULT '0',
  `HUFU` int unsigned NOT NULL DEFAULT '0',
  `YUNTIE` int unsigned NOT NULL DEFAULT '0',
  `JUEXINGEXP` int unsigned NOT NULL DEFAULT '0',
  `INTEGRAL` int unsigned NOT NULL DEFAULT '0',
  `SP` int unsigned NOT NULL DEFAULT '0',
  `LILIAN` int unsigned NOT NULL DEFAULT '0',
  `VIPLV` int unsigned NOT NULL DEFAULT '0',
  `SERVERID` int unsigned NOT NULL DEFAULT '0',
  `MAPID` smallint unsigned NOT NULL DEFAULT '0',
  `POSX` int unsigned NOT NULL DEFAULT '0',
  `POSY` int unsigned NOT NULL DEFAULT '0',
  `POSZ` int unsigned NOT NULL DEFAULT '0',
  `LEAVEGUILDTIME` int unsigned NOT NULL DEFAULT '0',
  `BITMASK` int unsigned NOT NULL DEFAULT '0',
  `RECHARGENUM` int unsigned NOT NULL DEFAULT '0',
  `GIVE_RECHARGENUM` int unsigned NOT NULL DEFAULT '0',
  `TOTALCONSUME` bigint unsigned NOT NULL DEFAULT '0',
  `BATTLEPOWER` int unsigned NOT NULL DEFAULT '0',
  `STARNUM` smallint unsigned NOT NULL DEFAULT '0',
  `LASTLOGINTIME` datetime NOT NULL,
  `CREATETIME` datetime NOT NULL,
  `ADVUSER` tinyint unsigned NOT NULL DEFAULT '0',
  `GM` smallint unsigned NOT NULL DEFAULT '0',
  `OP_ID` int unsigned NOT NULL DEFAULT '0',
  `CONTRIBUTE` int unsigned NOT NULL DEFAULT '0',
  `HEADID` int unsigned NOT NULL DEFAULT '0',
  `FACEID` int unsigned NOT NULL DEFAULT '0',
  `NEWBIEGUILDEID` smallint unsigned NOT NULL DEFAULT '0',
  `EQUIP_AVE_RAISELV` smallint unsigned NOT NULL DEFAULT '0',
  `EQUIP_AVE_REFINELV` tinyint unsigned NOT NULL DEFAULT '0',
  `WING_REFINELV` tinyint unsigned NOT NULL DEFAULT '0',
  `FUWEN_AVE_RAISELV` smallint unsigned NOT NULL DEFAULT '0',
  `FUWEN_AVE_QUALITY` tinyint unsigned NOT NULL DEFAULT '0',
  `ONE2ONE_PVP_SCORE` int unsigned NOT NULL DEFAULT '0',
  `SUBNEWBIE_GUIDEID` smallint unsigned NOT NULL DEFAULT '0',
  `BINARYDATA` blob,
  `SEVENDAYCONSUMEDIAMOND` int DEFAULT '0',
  `LUCKDRAWDATA` blob,
  `ENCHANTDATA` blob,
  `DAILY_INVITE_INFO` varchar(60) NOT NULL DEFAULT '',
  `SOUL_FIRE` int unsigned NOT NULL DEFAULT '0',
  `DRAGON_NEST_INFO` varchar(20) NOT NULL DEFAULT '',
  `COUPLE_ID` int unsigned NOT NULL DEFAULT '0',
  `COUPLE_NAME` varchar(33) NOT NULL DEFAULT '',
  `GUILDBLACKMARKET` blob,
  `LIFEESSENCE` int unsigned NOT NULL DEFAULT '0',
  `GUILDCRYSTAL` int unsigned NOT NULL DEFAULT '0',
  `STONECOPYMAPINFO` varchar(20) NOT NULL DEFAULT '',
  `LATE_WALFARE` varchar(20) NOT NULL DEFAULT '',
  `RES_BACK_INFO` varchar(1000) NOT NULL DEFAULT '',
  `MONSTER` blob,
  `MOUNT` blob,
  `PTOTOID` blob,
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  `LEVEL_WFLFARE_OPEN_TIME` varchar(100) NOT NULL DEFAULT '',
  `LEVEL_WFLFARE` varchar(200) NOT NULL DEFAULT '',
  `TRAIN_SP` int unsigned NOT NULL DEFAULT '0',
  `GUAJI_START` int unsigned NOT NULL DEFAULT '0',
  `GUAJI_END` int unsigned NOT NULL DEFAULT '0',
  `NFTVIP` smallint unsigned NOT NULL DEFAULT '0',
  `NFTVIP_GIFTTIME` int unsigned NOT NULL DEFAULT '0',
  `GUAJI_TIME` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`CHARID`),
  UNIQUE KEY `NAME` (`NAME`),
  KEY `ACCOUNTID` (`ACCOUNT`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for server_config
-- ----------------------------
DROP TABLE IF EXISTS `server_config`;
CREATE TABLE `server_config` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CONTENT` varchar(10240) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for server_params
-- ----------------------------
DROP TABLE IF EXISTS `server_params`;
CREATE TABLE `server_params` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `PARAM_NAME` varchar(30) NOT NULL DEFAULT '',
  `PARAM_VALUE` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for serverinfo_statics
-- ----------------------------
DROP TABLE IF EXISTS `serverinfo_statics`;
CREATE TABLE `serverinfo_statics` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `REG_DEVICE` int unsigned NOT NULL DEFAULT '0',
  `LOGIN_DEVICE` int unsigned NOT NULL DEFAULT '0',
  `REG_ACCOUNT` int unsigned NOT NULL DEFAULT '0',
  `LOGIN_ACCOUNT` int unsigned NOT NULL DEFAULT '0',
  `OLD_LOGIN_ACCOUNT` int unsigned NOT NULL DEFAULT '0',
  `NEW_LOGIN_ACCOUNT` int unsigned NOT NULL DEFAULT '0',
  `NEW_REG_ACCOUNT` int unsigned NOT NULL DEFAULT '0',
  `ROLE_NUM` int unsigned NOT NULL DEFAULT '0',
  `CREATE_ROLE_RATE` int unsigned NOT NULL DEFAULT '0',
  `ACU` int unsigned NOT NULL DEFAULT '0',
  `PCU` int unsigned NOT NULL DEFAULT '0',
  `DT` int unsigned NOT NULL DEFAULT '0',
  `RECHARGE_NUM` int unsigned NOT NULL DEFAULT '0',
  `ACC_RECHARGE_NUM` int unsigned NOT NULL DEFAULT '0',
  `NEW_RECHARGE_NUM` int unsigned NOT NULL DEFAULT '0',
  `RECHARGE_TIMES` int unsigned NOT NULL DEFAULT '0',
  `RECHARGE_AMOUNT` int unsigned NOT NULL DEFAULT '0',
  `RECHARGE_RATE` int unsigned NOT NULL DEFAULT '0',
  `ARPU` int unsigned NOT NULL DEFAULT '0',
  `ARPPU` int unsigned NOT NULL DEFAULT '0',
  `HAS_DIAMOND` int unsigned NOT NULL DEFAULT '0',
  `AVE_HAS_DIAMOND` int unsigned NOT NULL DEFAULT '0',
  `RECHARGE_USERS` int unsigned NOT NULL DEFAULT '0',
  `OP_ID` int unsigned NOT NULL DEFAULT '0',
  `TIME` date NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for serverinfo_statics_cache
-- ----------------------------
DROP TABLE IF EXISTS `serverinfo_statics_cache`;
CREATE TABLE `serverinfo_statics_cache` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `REG_DEVICE` int unsigned NOT NULL DEFAULT '0',
  `LOGIN_DEVICE` int unsigned NOT NULL DEFAULT '0',
  `REG_ACCOUNT` int unsigned NOT NULL DEFAULT '0',
  `LOGIN_ACCOUNT` int unsigned NOT NULL DEFAULT '0',
  `OLD_LOGIN_ACCOUNT` int unsigned NOT NULL DEFAULT '0',
  `NEW_LOGIN_ACCOUNT` int unsigned NOT NULL DEFAULT '0',
  `NEW_REG_ACCOUNT` int unsigned NOT NULL DEFAULT '0',
  `ROLE_NUM` int unsigned NOT NULL DEFAULT '0',
  `CREATE_ROLE_RATE` int unsigned NOT NULL DEFAULT '0',
  `ACU` int unsigned NOT NULL DEFAULT '0',
  `PCU` int unsigned NOT NULL DEFAULT '0',
  `DT` int unsigned NOT NULL DEFAULT '0',
  `RECHARGE_NUM` int unsigned NOT NULL DEFAULT '0',
  `ACC_RECHARGE_NUM` int unsigned NOT NULL DEFAULT '0',
  `NEW_RECHARGE_NUM` int unsigned NOT NULL DEFAULT '0',
  `RECHARGE_TIMES` int unsigned NOT NULL DEFAULT '0',
  `RECHARGE_AMOUNT` int unsigned NOT NULL DEFAULT '0',
  `RECHARGE_RATE` int unsigned NOT NULL DEFAULT '0',
  `ARPU` int unsigned NOT NULL DEFAULT '0',
  `ARPPU` int unsigned NOT NULL DEFAULT '0',
  `HAS_DIAMOND` int unsigned NOT NULL DEFAULT '0',
  `AVE_HAS_DIAMOND` int unsigned NOT NULL DEFAULT '0',
  `RECHARGE_USERS` int unsigned NOT NULL DEFAULT '0',
  `OP_ID` int unsigned NOT NULL DEFAULT '0',
  `TIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for skill_chg_log
-- ----------------------------
DROP TABLE IF EXISTS `skill_chg_log`;
CREATE TABLE `skill_chg_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `SKILLID` int unsigned NOT NULL DEFAULT '0',
  `POS` tinyint unsigned NOT NULL DEFAULT '0',
  `OLDVAL` smallint unsigned NOT NULL DEFAULT '0',
  `NEWVAL` smallint unsigned NOT NULL DEFAULT '0',
  `ACTION` varchar(20) NOT NULL DEFAULT '',
  `DOTIME` datetime NOT NULL,
  `DECGOILD` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for sprite_chg_log
-- ----------------------------
DROP TABLE IF EXISTS `sprite_chg_log`;
CREATE TABLE `sprite_chg_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `SPRITEID` int unsigned NOT NULL DEFAULT '0',
  `OLDVAL` smallint unsigned NOT NULL DEFAULT '0',
  `NEWVAL` smallint unsigned NOT NULL DEFAULT '0',
  `ACTION` varchar(20) NOT NULL DEFAULT '',
  `DOTIME` datetime NOT NULL,
  `CONSUME` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for total_recharge_num
-- ----------------------------
DROP TABLE IF EXISTS `total_recharge_num`;
CREATE TABLE `total_recharge_num` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `NUM` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for tx_gift_bag_award
-- ----------------------------
DROP TABLE IF EXISTS `tx_gift_bag_award`;
CREATE TABLE `tx_gift_bag_award` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `TASKID` int unsigned NOT NULL DEFAULT '0',
  `BILLNO` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for user_levelup_log
-- ----------------------------
DROP TABLE IF EXISTS `user_levelup_log`;
CREATE TABLE `user_levelup_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `OLDLV` smallint unsigned NOT NULL DEFAULT '0',
  `NEWLV` smallint unsigned NOT NULL DEFAULT '0',
  `OLDEXP` int unsigned NOT NULL DEFAULT '0',
  `NEWEXP` int unsigned NOT NULL DEFAULT '0',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for user_login_log
-- ----------------------------
DROP TABLE IF EXISTS `user_login_log`;
CREATE TABLE `user_login_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `OP_ID` int unsigned NOT NULL DEFAULT '0',
  `ADVUSER` tinyint unsigned NOT NULL DEFAULT '0',
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `LOGINIP` varchar(40) NOT NULL DEFAULT '',
  `DOTIME` datetime NOT NULL,
  `CREATETIME` datetime NOT NULL,
  `DEVICE_ID` varchar(125) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for user_recharge_log
-- ----------------------------
DROP TABLE IF EXISTS `user_recharge_log`;
CREATE TABLE `user_recharge_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `TID` varchar(50) NOT NULL DEFAULT '',
  `RECHARGENUM` int unsigned NOT NULL DEFAULT '0',
  `NOWDIAMOND` int unsigned NOT NULL DEFAULT '0',
  `DOTIME` datetime NOT NULL,
  `NEW_RECHARGE` tinyint unsigned NOT NULL DEFAULT '0',
  `OP_ID` int unsigned NOT NULL DEFAULT '0',
  `ADDDIAMOND` int unsigned NOT NULL DEFAULT '0',
  `VIPLV` smallint unsigned NOT NULL DEFAULT '0',
  `ITEMID` smallint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for user_viplevelup_log
-- ----------------------------
DROP TABLE IF EXISTS `user_viplevelup_log`;
CREATE TABLE `user_viplevelup_log` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `ACCOUNT` varchar(49) NOT NULL DEFAULT '',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `OLDEXP` int unsigned NOT NULL DEFAULT '0',
  `NEWEXP` int unsigned NOT NULL DEFAULT '0',
  `ADDDAIMOND` int unsigned NOT NULL DEFAULT '0',
  `DOTIME` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for world_boss_buy_info
-- ----------------------------
DROP TABLE IF EXISTS `world_boss_buy_info`;
CREATE TABLE `world_boss_buy_info` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `LASTLEAVETIME` int unsigned NOT NULL DEFAULT '0',
  `BUY_NUM` smallint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for world_boss_rank
-- ----------------------------
DROP TABLE IF EXISTS `world_boss_rank`;
CREATE TABLE `world_boss_rank` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `CHARID` int unsigned NOT NULL DEFAULT '0',
  `NAME` varchar(33) NOT NULL DEFAULT '',
  `DAMAGE` int unsigned NOT NULL DEFAULT '0',
  `JOB` tinyint unsigned NOT NULL DEFAULT '0',
  `SEX` tinyint unsigned NOT NULL DEFAULT '0',
  `USERPHOTOID` smallint unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for worldboss
-- ----------------------------
DROP TABLE IF EXISTS `worldboss`;
CREATE TABLE `worldboss` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `BOSSID` int unsigned NOT NULL DEFAULT '0',
  `BOSSHP` int unsigned NOT NULL DEFAULT '0',
  `STEPID` smallint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for worldboss_activity
-- ----------------------------
DROP TABLE IF EXISTS `worldboss_activity`;
CREATE TABLE `worldboss_activity` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `LEVELID` int unsigned NOT NULL DEFAULT '0',
  `BOSSID` int unsigned NOT NULL DEFAULT '0',
  `BEGINTIME` int unsigned NOT NULL DEFAULT '0',
  `ENDTIME` int unsigned NOT NULL DEFAULT '0',
  `IS_START` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- ----------------------------
-- Table structure for zone_info
-- ----------------------------
DROP TABLE IF EXISTS `zone_info`;
CREATE TABLE `zone_info` (
  `ID` int unsigned NOT NULL AUTO_INCREMENT,
  `SERVERID` int unsigned NOT NULL DEFAULT '0',
  `IP` varchar(50) NOT NULL DEFAULT '',
  `SERVERKEY` varchar(50) NOT NULL DEFAULT '',
  `PORT` smallint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

SET FOREIGN_KEY_CHECKS = 1;
