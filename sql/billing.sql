/*
 Navicat Premium Data Transfer

 Source Server         : 海牙rds
 Source Server Type    : MySQL
 Source Server Version : 80034
 Source Host           : rm-bp15fg96gd03xcpu8wo.mysql.rds.aliyuncs.com:3306
 Source Schema         : billing

 Target Server Type    : MySQL
 Target Server Version : 80034
 File Encoding         : 65001

 Date: 05/07/2024 14:49:50
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for pay_brandroid_universal
-- ----------------------------
DROP TABLE IF EXISTS `pay_brandroid_universal`;
CREATE TABLE `pay_brandroid_universal` (
  `pay_id` int NOT NULL AUTO_INCREMENT,
  `server_id` varchar(60) NOT NULL,
  `character_id` varchar(60) NOT NULL,
  `transaction_id` varchar(60) NOT NULL,
  `order_id` varchar(60) NOT NULL,
  `amount` decimal(16,2) NOT NULL,
  `product_id` varchar(60) NOT NULL,
  `api_app_id` varchar(100) NOT NULL,
  `api_channel_id` varchar(100) NOT NULL,
  `api_pay_type` varchar(100) NOT NULL,
  `api_account_id` varchar(100) NOT NULL,
  `api_role_id` varchar(100) NOT NULL,
  `api_server_id` varchar(100) NOT NULL,
  `api_trans_type` varchar(100) NOT NULL,
  `create_date` datetime NOT NULL,
  `pay_date` datetime NOT NULL,
  `stat` char(1) NOT NULL,
  PRIMARY KEY (`pay_id`),
  UNIQUE KEY `idx_pay_quilk_transaction_id` (`transaction_id`),
  UNIQUE KEY `idx_pay_quilk_order_id` (`order_id`)
) ENGINE=InnoDB AUTO_INCREMENT=108 DEFAULT CHARSET=utf8mb3;

-- ----------------------------
-- Records of pay_brandroid_universal
-- ----------------------------
BEGIN;
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (9, '121', '25215836', '1716884941', '00020240528162242516347523', 30.00, 'com.ssh.30', '', '', '', 'kugame', '', '', '', '2024-05-28 16:29:01', '2024-05-28 16:29:01', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (10, '121', '25215836', '1716884942', '00020240528162228279590813', 30.00, 'com.ssh.30', '', '', '', 'kugame', '', '', '', '2024-05-28 16:29:02', '2024-05-28 16:29:02', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (11, '121', '25215836', '1716884967', '00020240528162927860413623', 6.00, 'com.ssh.28', '', '', '', 'kugame', '', '', '', '2024-05-28 16:29:27', '2024-05-28 16:29:27', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (12, '121', '25215836', '1716884980', '00020240528162940544277973', 30.00, 'com.ssh.30', '', '', '', 'kugame', '', '', '', '2024-05-28 16:29:40', '2024-05-28 16:29:40', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (13, '121', '25215836', '1716884997', '00020240528162957883049083', 68.00, 'com.ssh.31', '', '', '', 'kugame', '', '', '', '2024-05-28 16:29:57', '2024-05-28 16:29:57', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (14, '121', '25215836', '1716885003', '00020240528162217809177703', 18.00, 'com.ssh.29', '', '', '', 'kugame', '', '', '', '2024-05-28 16:30:03', '2024-05-28 16:30:03', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (15, '121', '25215836', '1716885042', '00020240528163042587492293', 198.00, 'com.ssh.33', '', '', '', 'kugame', '', '', '', '2024-05-28 16:30:42', '2024-05-28 16:30:42', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (16, '121', '25215836', '1716885057', '00020240528163057626898673', 198.00, 'com.ssh.33', '', '', '', 'kugame', '', '', '', '2024-05-28 16:30:57', '2024-05-28 16:30:57', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (17, '121', '25215836', '1716885062', '00020240528162115381021683', 6.00, 'com.ssh.28', '', '', '', 'kugame', '', '', '', '2024-05-28 16:31:02', '2024-05-28 16:31:02', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (18, '121', '25215836', '1716885122', '00020240528162101474553923', 6.00, 'com.ssh.28', '', '', '', 'kugame', '', '', '', '2024-05-28 16:32:02', '2024-05-28 16:32:03', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (19, '121', '25215836', '1716886075', '00020240528164755274691133', 30.00, 'com.ssh.43', '', '', '', 'kugame', '', '', '', '2024-05-28 16:47:55', '2024-05-28 16:47:56', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (20, '121', '25215836', '1716886109', '00020240528164829507876053', 30.00, 'com.ssh.43', '', '', '', 'kugame', '', '', '', '2024-05-28 16:48:29', '2024-05-28 16:48:30', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (21, '121', '25215836', '1716886115', '00020240528164835636091483', 648.00, 'com.ssh.45', '', '', '', 'kugame', '', '', '', '2024-05-28 16:48:35', '2024-05-28 16:48:35', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (22, '121', '25215836', '1716886125', '00020240528164845396524193', 198.00, 'com.ssh.44', '', '', '', 'kugame', '', '', '', '2024-05-28 16:48:45', '2024-05-28 16:48:45', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (23, '121', '25215836', '1716886169', '00020240528164929574507463', 30.00, 'com.ssh.43', '', '', '', 'kugame', '', '', '', '2024-05-28 16:49:29', '2024-05-28 16:49:29', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (24, '121', '25215836', '1716886172', '00020240528164932096948083', 30.00, 'com.ssh.43', '', '', '', 'kugame', '', '', '', '2024-05-28 16:49:32', '2024-05-28 16:49:32', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (25, '121', '25215836', '1716886177', '00020240528164937863827363', 30.00, 'com.ssh.43', '', '', '', 'kugame', '', '', '', '2024-05-28 16:49:37', '2024-05-28 16:49:37', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (26, '121', '25215836', '1716886181', '00020240528164941036537553', 30.00, 'com.ssh.43', '', '', '', 'kugame', '', '', '', '2024-05-28 16:49:41', '2024-05-28 16:49:41', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (27, '121', '25215836', '1717040619', '00020240530114338249460333', 198.00, 'com.ssh.44', '', '', '', 'kugame', '', '', '', '2024-05-30 11:43:39', '2024-05-30 11:43:39', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (28, '121', '25215840', '1717041322', '00020240530115522556393913', 68.00, 'com.ssh.44', '', '', '', 'maxcai', '', '', '', '2024-05-30 11:55:22', '2024-05-30 11:55:22', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (29, '121', '25215840', '1717049944', '00020240530141904716887893', 30.00, 'com.ssh.43', '', '', '', 'maxcai', '', '', '', '2024-05-30 14:19:04', '2024-05-30 14:19:05', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (30, '121', '25215840', '1717049954', '00020240530141914719401203', 30.00, 'com.ssh.43', '', '', '', 'maxcai', '', '', '', '2024-05-30 14:19:14', '2024-05-30 14:19:14', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (31, '121', '25215840', '1717049979', '00020240530141939207335263', 30.00, 'com.ssh.43', '', '', '', 'maxcai', '', '', '', '2024-05-30 14:19:39', '2024-05-30 14:19:40', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (32, '121', '25215840', '1717050036', '00020240530142036576605623', 648.00, 'com.ssh.45', '', '', '', 'maxcai', '', '', '', '2024-05-30 14:20:36', '2024-05-30 14:20:36', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (33, '121', '25215836', '1717132562', '00020240531131602173237753', 98.00, 'com.ssh.51', '', '', '', 'kugame', '', '', '', '2024-05-31 13:16:02', '2024-05-31 13:16:02', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (34, '121', '25215855', '1717134868', '00020240531135428283265913', 98.00, 'com.ssh.51', '', '', '', 'wwwwww', '', '', '', '2024-05-31 13:54:28', '2024-05-31 13:54:28', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (35, '121', '25215855', '1717140051', '00020240531152051404276023', 98.00, 'com.ssh.51', '', '', '', 'wwwwww', '', '', '', '2024-05-31 15:20:51', '2024-05-31 15:20:51', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (36, '121', '25215858', '1717140476', '00020240531152756235497063', 98.00, 'com.ssh.51', '', '', '', 'kugame2', '', '', '', '2024-05-31 15:27:56', '2024-05-31 15:27:56', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (37, '121', '25215858', '1717142615', '00020240531160335219861953', 98.00, 'com.ssh.51', '', '', '', 'kugame2', '', '', '', '2024-05-31 16:03:35', '2024-05-31 16:03:35', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (38, '121', '25215858', '1717142862', '00020240531160742292022783', 98.00, 'com.ssh.51', '', '', '', 'kugame2', '', '', '', '2024-05-31 16:07:42', '2024-05-31 16:07:42', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (39, '121', '25215859', '1717145658', '00020240531165418166599823', 98.00, 'com.ssh.51', '', '', '', 'kugame3', '', '', '', '2024-05-31 16:54:18', '2024-05-31 16:54:18', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (40, '121', '25215860', '1717153996', '00020240531191316039225503', 98.00, 'com.ssh.51', '', '', '', 'kugame4', '', '', '', '2024-05-31 19:13:16', '2024-05-31 19:13:16', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (41, '121', '25215860', '1717208150', '00020240601101549385839003', 6.00, 'com.ssh.28', '', '', '', 'kugame4', '', '', '', '2024-06-01 10:15:50', '2024-06-01 10:15:50', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (42, '121', '25215860', '1717208474', '00020240601102114690216343', 18.00, 'com.ssh.29', '', '', '', 'kugame4', '', '', '', '2024-06-01 10:21:14', '2024-06-01 10:21:14', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (43, '121', '25215860', '1717208494', '00020240601102134742009353', 98.00, 'com.ssh.32', '', '', '', 'kugame4', '', '', '', '2024-06-01 10:21:34', '2024-06-01 10:21:34', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (44, '121', '25215836', '1717248567', '00020240601212927199123033', 98.00, 'com.ssh.51', '', '', '', 'kugame', '', '', '', '2024-06-01 21:29:27', '2024-06-01 21:29:27', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (45, '121', '25215836', '1717257643', '00020240602000043810393763', 98.00, 'com.ssh.51', '', '', '', 'kugame', '', '', '', '2024-06-02 00:00:43', '2024-06-02 00:00:43', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (46, '121', '25215863', '1717259174', '00020240602002614116575333', 98.00, 'com.ssh.51', '', '', '', 'kugame5', '', '', '', '2024-06-02 00:26:14', '2024-06-02 00:26:14', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (47, '121', '25215836', '1717490042', '00020240604163201595932613', 98.00, 'com.ssh.52', '', '', '', 'kugame', '', '', '', '2024-06-04 16:34:02', '2024-06-04 16:34:02', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (48, '121', '25215836', '1717490103', '00020240604162412311435863', 98.00, 'com.ssh.52', '', '', '', 'kugame', '', '', '', '2024-06-04 16:35:03', '2024-06-04 16:35:03', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (49, '121', '25215836', '1717490161', '00020240604162215296180403', 98.00, 'com.ssh.52', '', '', '', 'kugame', '', '', '', '2024-06-04 16:36:01', '2024-06-04 16:36:01', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (50, '121', '25215871', '1717497892', '00020240604184452283805253', 648.00, 'com.ssh.35', '', '', '', 'kugame2', '', '', '', '2024-06-04 18:44:52', '2024-06-04 18:44:52', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (51, '121', '25215871', '1717498079', '00020240604184759433675733', 98.00, 'com.ssh.51', '', '', '', 'kugame2', '', '', '', '2024-06-04 18:47:59', '2024-06-04 18:47:59', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (52, '121', '25215871', '1717498135', '00020240604184855120989773', 98.00, 'com.ssh.52', '', '', '', 'kugame2', '', '', '', '2024-06-04 18:48:55', '2024-06-04 18:48:55', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (53, '121', '25215836', '1717568072', '00020240605141432606743253', 6.00, 'com.ssh.28', '', '', '', 'kugame', '', '', '', '2024-06-05 14:14:32', '2024-06-05 14:14:32', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (54, '121', '25215836', '1717568078', '00020240605141438761053353', 18.00, 'com.ssh.29', '', '', '', 'kugame', '', '', '', '2024-06-05 14:14:38', '2024-06-05 14:14:38', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (55, '121', '25215836', '1717568091', '00020240605141451518974233', 68.00, 'com.ssh.44', '', '', '', 'kugame', '', '', '', '2024-06-05 14:14:51', '2024-06-05 14:14:51', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (56, '121', '25215836', '1717572866', '00020240605153426322023023', 648.00, 'com.ssh.45', '', '', '', 'kugame', '', '', '', '2024-06-05 15:34:26', '2024-06-05 15:34:26', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (57, '121', '25215836', '1717587208', '00020240605193328233496843', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 19:33:28', '2024-06-05 19:33:29', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (58, '121', '25215836', '1717589202', '00020240605200642309488693', 30.00, 'com.ssh.54', '', '', '', 'kugame', '', '', '', '2024-06-05 20:06:42', '2024-06-05 20:06:42', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (59, '121', '25215836', '1717590265', '00020240605202425784424233', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 20:24:25', '2024-06-05 20:24:25', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (60, '121', '25215836', '1717590291', '00020240605202451182261813', 30.00, 'com.ssh.54', '', '', '', 'kugame', '', '', '', '2024-06-05 20:24:51', '2024-06-05 20:24:51', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (61, '121', '25215836', '1717590319', '00020240605202519757638813', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 20:25:19', '2024-06-05 20:25:19', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (62, '121', '25215836', '1717590816', '00020240605203336505784973', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 20:33:36', '2024-06-05 20:33:36', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (63, '121', '25215836', '1717594058', '00020240605212738126408003', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 21:27:38', '2024-06-05 21:27:39', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (64, '121', '25215836', '1717594610', '00020240605213650676380983', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 21:36:50', '2024-06-05 21:36:51', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (65, '121', '25215836', '1717594614', '00020240605213653072559083', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 21:36:54', '2024-06-05 21:36:54', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (66, '121', '25215836', '1717595227', '00020240605214707183199343', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 21:47:07', '2024-06-05 21:47:07', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (67, '121', '25215836', '1717596520', '00020240605220840914740043', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 22:08:40', '2024-06-05 22:08:40', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (68, '121', '25215836', '1717597235', '00020240605222035736142033', 30.00, 'com.ssh.54', '', '', '', 'kugame', '', '', '', '2024-06-05 22:20:35', '2024-06-05 22:20:35', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (69, '121', '25215836', '1717597840', '00020240605223040459325113', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 22:30:40', '2024-06-05 22:30:40', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (70, '121', '25215836', '1717597845', '00020240605223045558463353', 30.00, 'com.ssh.54', '', '', '', 'kugame', '', '', '', '2024-06-05 22:30:45', '2024-06-05 22:30:45', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (71, '121', '25215836', '1717597850', '00020240605223050687940203', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 22:30:50', '2024-06-05 22:30:50', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (72, '121', '25215836', '1717598756', '00020240605224556184296143', 30.00, 'com.ssh.54', '', '', '', 'kugame', '', '', '', '2024-06-05 22:45:56', '2024-06-05 22:45:56', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (73, '121', '25215836', '1717598806', '00020240605224645384754453', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 22:46:46', '2024-06-05 22:46:46', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (74, '121', '25215836', '1717640369', '00020240606101928335455583', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-06 10:19:29', '2024-06-06 10:19:29', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (75, '121', '25215836', '1717640691', '00020240606102451391159923', 15.00, 'com.ssh.55', '', '', '', 'kugame', '', '', '', '2024-06-06 10:24:51', '2024-06-06 10:24:51', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (76, '121', '25215836', '1717640695', '00020240606102455098294943', 18.00, 'com.ssh.56', '', '', '', 'kugame', '', '', '', '2024-06-06 10:24:55', '2024-06-06 10:24:55', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (77, '121', '25215836', '1717640709', '00020240606102509145864043', 6.00, 'com.ssh.28', '', '', '', 'kugame', '', '', '', '2024-06-06 10:25:09', '2024-06-06 10:25:09', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (78, '121', '25215836', '1717640716', '00020240606102516131237483', 18.00, 'com.ssh.56', '', '', '', 'kugame', '', '', '', '2024-06-06 10:25:16', '2024-06-06 10:25:17', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (79, '121', '25215836', '1717640727', '00020240606102527196138063', 18.00, 'com.ssh.56', '', '', '', 'kugame', '', '', '', '2024-06-06 10:25:27', '2024-06-06 10:25:27', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (80, '121', '25215836', '1717666850', '00020240606174050161585063', 15.00, 'com.ssh.55', '', '', '', 'kugame', '', '', '', '2024-06-06 17:40:50', '2024-06-06 17:40:50', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (81, '121', '25215836', '1717666882', '00020240606174122149916273', 18.00, 'com.ssh.56', '', '', '', 'kugame', '', '', '', '2024-06-06 17:41:22', '2024-06-06 17:41:22', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (82, '121', '25215878', '1717667034', '00020240606174354213008403', 1000.00, 'com.ssh.36', '', '', '', 'kugame1', '', '', '', '2024-06-06 17:43:54', '2024-06-06 17:43:55', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (83, '121', '25215871', '1717667210', '00020240606174650600092523', 6.00, 'com.ssh.53', '', '', '', 'kugame2', '', '', '', '2024-06-06 17:46:50', '2024-06-06 17:46:50', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (84, '121', '25215871', '1717667223', '00020240606174703825901413', 18.00, 'com.ssh.56', '', '', '', 'kugame2', '', '', '', '2024-06-06 17:47:03', '2024-06-06 17:47:03', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (85, '121', '25215871', '1717673763', '00020240606193603590035973', 6.00, 'com.ssh.53', '', '', '', 'kugame2', '', '', '', '2024-06-06 19:36:03', '2024-06-06 19:36:03', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (86, '121', '25215871', '1717673770', '00020240606193610777107093', 6.00, 'com.ssh.53', '', '', '', 'kugame2', '', '', '', '2024-06-06 19:36:10', '2024-06-06 19:36:10', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (87, '121', '25215871', '1717673774', '00020240606193614647232643', 6.00, 'com.ssh.53', '', '', '', 'kugame2', '', '', '', '2024-06-06 19:36:14', '2024-06-06 19:36:14', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (88, '121', '25215871', '1717673776', '00020240606193616609826933', 6.00, 'com.ssh.53', '', '', '', 'kugame2', '', '', '', '2024-06-06 19:36:16', '2024-06-06 19:36:16', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (89, '121', '25215871', '1717758382', '00020240607190622536618133', 98.00, 'com.ssh.51', '', '', '', 'kugame2', '', '', '', '2024-06-07 19:06:22', '2024-06-07 19:06:22', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (90, '121', '25215836', '1718086739', '00020240611141859576751233', 98.00, 'com.ssh.51', '', '', '', 'kugame', '', '', '', '2024-06-11 14:18:59', '2024-06-11 14:18:59', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (91, '121', '25215836', '1718163764', '00020240612114244897155723', 98.00, 'com.ssh.51', '', '', '', 'kugame', '', '', '', '2024-06-12 11:42:44', '2024-06-12 11:42:45', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (92, '121', '25215836', '1718164183', '00020240612114942105459303', 98.00, 'com.ssh.52', '', '', '', 'kugame', '', '', '', '2024-06-12 11:49:43', '2024-06-12 11:49:43', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (93, '121', '25215836', '1718960434', '00020240621170034434800223', 6.00, 'com.ssh.28', '', '', '', 'kugame', '', '', '', '2024-06-21 17:00:34', '2024-06-21 17:00:34', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (94, '121', '25215836', '1718960523', '00020240621170203279905523', 18.00, 'com.ssh.29', '', '', '', 'kugame', '', '', '', '2024-06-21 17:02:03', '2024-06-21 17:02:03', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (95, '123', '25948475', '1719456493', 'BR202406271048139596624905919', 6.00, 'com.ssh.28', '', '', '', '501562792', '', '', '', '2024-06-27 10:48:13', '2024-06-27 10:48:13', 'N');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (96, '123', '25948475', '1719456647', 'BR202406271050469595437349422', 6.00, 'com.ssh.28', '', '', '', '501562792', '', '', '', '2024-06-27 10:50:47', '2024-06-27 10:50:47', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (97, '123', '25948475', '1719456666', 'BR202406271051069599920921428', 6.00, 'com.ssh.53', '', '', '', '501562792', '', '', '', '2024-06-27 10:51:06', '2024-06-27 10:51:07', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (98, '123', '25948475', '1719456692', 'BR202406271051329592516783429', 98.00, 'com.ssh.51', '', '', '', '501562792', '', '', '', '2024-06-27 10:51:32', '2024-06-27 10:51:32', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (99, '123', '25948475', '1719458058', 'BR202406271114189597473705919', 10.00, 'com.ssh.54', '', '', '', '501562792', '', '', '', '2024-06-27 11:14:18', '2024-06-27 11:14:18', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (100, '123', '25948475', '1719458173', 'BR202406271114429591652922235', 10.00, 'com.ssh.54', '', '', '', '501562792', '', '', '', '2024-06-27 11:16:13', '2024-06-27 11:16:13', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (101, '123', '25948475', '1719465930', 'BR202406271325309596540364926', 6.00, 'com.ssh.53', '', '', '', '501562792', '', '', '', '2024-06-27 13:25:30', '2024-06-27 13:25:30', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (102, '123', '25948477', '1719887162', 'BR202407021026022465065999821', 6.00, 'com.ssh.28', '', '', '', '501648574', '', '', '', '2024-07-02 10:26:02', '2024-07-02 10:26:02', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (103, '123', '25948477', '1720061592', 'BR202407041053122464421778333', 6.00, 'com.ssh.28', '', '', '', '501648574', '', '', '', '2024-07-04 10:53:12', '2024-07-04 10:53:12', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (104, '121', '25948477', '1720144356', 'BR202407050952362462154099124', 6.00, 'com.ssh.28', '', '', '', '501648574', '', '', '', '2024-07-05 09:52:36', '2024-07-05 09:52:37', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (105, '121', '25948475', '1720151230', 'BR202407051147099599231654523', 6.00, 'com.ssh.53', '', '', '', '501562792', '', '', '', '2024-07-05 11:47:10', '2024-07-05 11:47:10', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (106, '121', '25948475', '1720160153', 'BR202407051411269593064634534', 6.00, 'com.ssh.28', '', '', '', '501562792', '', '', '', '2024-07-05 14:15:53', '2024-07-05 14:15:53', 'S');
INSERT INTO `pay_brandroid_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (107, '121', '25948475', '1720161262', 'BR202407051434229597615785817', 6.00, 'com.ssh.28', '', '', '', '501562792', '', '', '', '2024-07-05 14:34:22', '2024-07-05 14:34:23', 'S');
COMMIT;

-- ----------------------------
-- Table structure for pay_common_universal
-- ----------------------------
DROP TABLE IF EXISTS `pay_common_universal`;
CREATE TABLE `pay_common_universal` (
  `pay_id` int NOT NULL AUTO_INCREMENT,
  `server_id` varchar(60) NOT NULL,
  `character_id` varchar(60) NOT NULL,
  `transaction_id` varchar(60) NOT NULL,
  `order_id` varchar(60) NOT NULL,
  `amount` decimal(16,2) NOT NULL,
  `product_id` varchar(60) NOT NULL,
  `api_app_id` varchar(100) NOT NULL,
  `api_channel_id` varchar(100) NOT NULL,
  `api_pay_type` varchar(100) NOT NULL,
  `api_account_id` varchar(100) NOT NULL,
  `api_role_id` varchar(100) NOT NULL,
  `api_server_id` varchar(100) NOT NULL,
  `api_trans_type` varchar(100) NOT NULL,
  `create_date` datetime NOT NULL,
  `pay_date` datetime NOT NULL,
  `stat` char(1) NOT NULL,
  PRIMARY KEY (`pay_id`),
  UNIQUE KEY `idx_pay_common_transaction_id` (`transaction_id`),
  UNIQUE KEY `idx_pay_common_order_id` (`order_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- ----------------------------
-- Records of pay_common_universal
-- ----------------------------
BEGIN;
COMMIT;

-- ----------------------------
-- Table structure for pay_laosiji_universal
-- ----------------------------
DROP TABLE IF EXISTS `pay_laosiji_universal`;
CREATE TABLE `pay_laosiji_universal` (
  `pay_id` int NOT NULL AUTO_INCREMENT,
  `server_id` varchar(60) NOT NULL,
  `character_id` varchar(60) NOT NULL,
  `transaction_id` varchar(60) NOT NULL,
  `order_id` varchar(60) NOT NULL,
  `amount` decimal(16,2) NOT NULL,
  `product_id` varchar(60) NOT NULL,
  `api_app_id` varchar(100) NOT NULL,
  `api_channel_id` varchar(100) NOT NULL,
  `api_pay_type` varchar(100) NOT NULL,
  `api_account_id` varchar(100) NOT NULL,
  `api_role_id` varchar(100) NOT NULL,
  `api_server_id` varchar(100) NOT NULL,
  `api_trans_type` varchar(100) NOT NULL,
  `create_date` datetime NOT NULL,
  `pay_date` datetime NOT NULL,
  `stat` char(1) NOT NULL,
  PRIMARY KEY (`pay_id`),
  UNIQUE KEY `idx_pay_laosiji_transaction_id` (`transaction_id`),
  UNIQUE KEY `idx_pay_laosiji_order_id` (`order_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- ----------------------------
-- Records of pay_laosiji_universal
-- ----------------------------
BEGIN;
COMMIT;

-- ----------------------------
-- Table structure for pay_quilk_universal
-- ----------------------------
DROP TABLE IF EXISTS `pay_quilk_universal`;
CREATE TABLE `pay_quilk_universal` (
  `pay_id` int NOT NULL AUTO_INCREMENT,
  `server_id` varchar(60) NOT NULL,
  `character_id` varchar(60) NOT NULL,
  `transaction_id` varchar(60) NOT NULL,
  `order_id` varchar(60) NOT NULL,
  `amount` decimal(16,2) NOT NULL,
  `product_id` varchar(60) NOT NULL,
  `api_app_id` varchar(100) NOT NULL,
  `api_channel_id` varchar(100) NOT NULL,
  `api_pay_type` varchar(100) NOT NULL,
  `api_account_id` varchar(100) NOT NULL,
  `api_role_id` varchar(100) NOT NULL,
  `api_server_id` varchar(100) NOT NULL,
  `api_trans_type` varchar(100) NOT NULL,
  `create_date` datetime NOT NULL,
  `pay_date` datetime NOT NULL,
  `stat` char(1) NOT NULL,
  PRIMARY KEY (`pay_id`),
  UNIQUE KEY `idx_pay_quilk_transaction_id` (`transaction_id`),
  UNIQUE KEY `idx_pay_quilk_order_id` (`order_id`)
) ENGINE=InnoDB AUTO_INCREMENT=95 DEFAULT CHARSET=utf8mb3;

-- ----------------------------
-- Records of pay_quilk_universal
-- ----------------------------
BEGIN;
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (9, '121', '25215836', '1716884941', '00020240528162242516347523', 30.00, 'com.ssh.30', '', '', '', 'kugame', '', '', '', '2024-05-28 16:29:01', '2024-05-28 16:29:01', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (10, '121', '25215836', '1716884942', '00020240528162228279590813', 30.00, 'com.ssh.30', '', '', '', 'kugame', '', '', '', '2024-05-28 16:29:02', '2024-05-28 16:29:02', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (11, '121', '25215836', '1716884967', '00020240528162927860413623', 6.00, 'com.ssh.28', '', '', '', 'kugame', '', '', '', '2024-05-28 16:29:27', '2024-05-28 16:29:27', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (12, '121', '25215836', '1716884980', '00020240528162940544277973', 30.00, 'com.ssh.30', '', '', '', 'kugame', '', '', '', '2024-05-28 16:29:40', '2024-05-28 16:29:40', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (13, '121', '25215836', '1716884997', '00020240528162957883049083', 68.00, 'com.ssh.31', '', '', '', 'kugame', '', '', '', '2024-05-28 16:29:57', '2024-05-28 16:29:57', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (14, '121', '25215836', '1716885003', '00020240528162217809177703', 18.00, 'com.ssh.29', '', '', '', 'kugame', '', '', '', '2024-05-28 16:30:03', '2024-05-28 16:30:03', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (15, '121', '25215836', '1716885042', '00020240528163042587492293', 198.00, 'com.ssh.33', '', '', '', 'kugame', '', '', '', '2024-05-28 16:30:42', '2024-05-28 16:30:42', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (16, '121', '25215836', '1716885057', '00020240528163057626898673', 198.00, 'com.ssh.33', '', '', '', 'kugame', '', '', '', '2024-05-28 16:30:57', '2024-05-28 16:30:57', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (17, '121', '25215836', '1716885062', '00020240528162115381021683', 6.00, 'com.ssh.28', '', '', '', 'kugame', '', '', '', '2024-05-28 16:31:02', '2024-05-28 16:31:02', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (18, '121', '25215836', '1716885122', '00020240528162101474553923', 6.00, 'com.ssh.28', '', '', '', 'kugame', '', '', '', '2024-05-28 16:32:02', '2024-05-28 16:32:03', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (19, '121', '25215836', '1716886075', '00020240528164755274691133', 30.00, 'com.ssh.43', '', '', '', 'kugame', '', '', '', '2024-05-28 16:47:55', '2024-05-28 16:47:56', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (20, '121', '25215836', '1716886109', '00020240528164829507876053', 30.00, 'com.ssh.43', '', '', '', 'kugame', '', '', '', '2024-05-28 16:48:29', '2024-05-28 16:48:30', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (21, '121', '25215836', '1716886115', '00020240528164835636091483', 648.00, 'com.ssh.45', '', '', '', 'kugame', '', '', '', '2024-05-28 16:48:35', '2024-05-28 16:48:35', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (22, '121', '25215836', '1716886125', '00020240528164845396524193', 198.00, 'com.ssh.44', '', '', '', 'kugame', '', '', '', '2024-05-28 16:48:45', '2024-05-28 16:48:45', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (23, '121', '25215836', '1716886169', '00020240528164929574507463', 30.00, 'com.ssh.43', '', '', '', 'kugame', '', '', '', '2024-05-28 16:49:29', '2024-05-28 16:49:29', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (24, '121', '25215836', '1716886172', '00020240528164932096948083', 30.00, 'com.ssh.43', '', '', '', 'kugame', '', '', '', '2024-05-28 16:49:32', '2024-05-28 16:49:32', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (25, '121', '25215836', '1716886177', '00020240528164937863827363', 30.00, 'com.ssh.43', '', '', '', 'kugame', '', '', '', '2024-05-28 16:49:37', '2024-05-28 16:49:37', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (26, '121', '25215836', '1716886181', '00020240528164941036537553', 30.00, 'com.ssh.43', '', '', '', 'kugame', '', '', '', '2024-05-28 16:49:41', '2024-05-28 16:49:41', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (27, '121', '25215836', '1717040619', '00020240530114338249460333', 198.00, 'com.ssh.44', '', '', '', 'kugame', '', '', '', '2024-05-30 11:43:39', '2024-05-30 11:43:39', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (28, '121', '25215840', '1717041322', '00020240530115522556393913', 68.00, 'com.ssh.44', '', '', '', 'maxcai', '', '', '', '2024-05-30 11:55:22', '2024-05-30 11:55:22', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (29, '121', '25215840', '1717049944', '00020240530141904716887893', 30.00, 'com.ssh.43', '', '', '', 'maxcai', '', '', '', '2024-05-30 14:19:04', '2024-05-30 14:19:05', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (30, '121', '25215840', '1717049954', '00020240530141914719401203', 30.00, 'com.ssh.43', '', '', '', 'maxcai', '', '', '', '2024-05-30 14:19:14', '2024-05-30 14:19:14', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (31, '121', '25215840', '1717049979', '00020240530141939207335263', 30.00, 'com.ssh.43', '', '', '', 'maxcai', '', '', '', '2024-05-30 14:19:39', '2024-05-30 14:19:40', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (32, '121', '25215840', '1717050036', '00020240530142036576605623', 648.00, 'com.ssh.45', '', '', '', 'maxcai', '', '', '', '2024-05-30 14:20:36', '2024-05-30 14:20:36', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (33, '121', '25215836', '1717132562', '00020240531131602173237753', 98.00, 'com.ssh.51', '', '', '', 'kugame', '', '', '', '2024-05-31 13:16:02', '2024-05-31 13:16:02', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (34, '121', '25215855', '1717134868', '00020240531135428283265913', 98.00, 'com.ssh.51', '', '', '', 'wwwwww', '', '', '', '2024-05-31 13:54:28', '2024-05-31 13:54:28', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (35, '121', '25215855', '1717140051', '00020240531152051404276023', 98.00, 'com.ssh.51', '', '', '', 'wwwwww', '', '', '', '2024-05-31 15:20:51', '2024-05-31 15:20:51', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (36, '121', '25215858', '1717140476', '00020240531152756235497063', 98.00, 'com.ssh.51', '', '', '', 'kugame2', '', '', '', '2024-05-31 15:27:56', '2024-05-31 15:27:56', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (37, '121', '25215858', '1717142615', '00020240531160335219861953', 98.00, 'com.ssh.51', '', '', '', 'kugame2', '', '', '', '2024-05-31 16:03:35', '2024-05-31 16:03:35', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (38, '121', '25215858', '1717142862', '00020240531160742292022783', 98.00, 'com.ssh.51', '', '', '', 'kugame2', '', '', '', '2024-05-31 16:07:42', '2024-05-31 16:07:42', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (39, '121', '25215859', '1717145658', '00020240531165418166599823', 98.00, 'com.ssh.51', '', '', '', 'kugame3', '', '', '', '2024-05-31 16:54:18', '2024-05-31 16:54:18', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (40, '121', '25215860', '1717153996', '00020240531191316039225503', 98.00, 'com.ssh.51', '', '', '', 'kugame4', '', '', '', '2024-05-31 19:13:16', '2024-05-31 19:13:16', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (41, '121', '25215860', '1717208150', '00020240601101549385839003', 6.00, 'com.ssh.28', '', '', '', 'kugame4', '', '', '', '2024-06-01 10:15:50', '2024-06-01 10:15:50', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (42, '121', '25215860', '1717208474', '00020240601102114690216343', 18.00, 'com.ssh.29', '', '', '', 'kugame4', '', '', '', '2024-06-01 10:21:14', '2024-06-01 10:21:14', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (43, '121', '25215860', '1717208494', '00020240601102134742009353', 98.00, 'com.ssh.32', '', '', '', 'kugame4', '', '', '', '2024-06-01 10:21:34', '2024-06-01 10:21:34', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (44, '121', '25215836', '1717248567', '00020240601212927199123033', 98.00, 'com.ssh.51', '', '', '', 'kugame', '', '', '', '2024-06-01 21:29:27', '2024-06-01 21:29:27', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (45, '121', '25215836', '1717257643', '00020240602000043810393763', 98.00, 'com.ssh.51', '', '', '', 'kugame', '', '', '', '2024-06-02 00:00:43', '2024-06-02 00:00:43', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (46, '121', '25215863', '1717259174', '00020240602002614116575333', 98.00, 'com.ssh.51', '', '', '', 'kugame5', '', '', '', '2024-06-02 00:26:14', '2024-06-02 00:26:14', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (47, '121', '25215836', '1717490042', '00020240604163201595932613', 98.00, 'com.ssh.52', '', '', '', 'kugame', '', '', '', '2024-06-04 16:34:02', '2024-06-04 16:34:02', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (48, '121', '25215836', '1717490103', '00020240604162412311435863', 98.00, 'com.ssh.52', '', '', '', 'kugame', '', '', '', '2024-06-04 16:35:03', '2024-06-04 16:35:03', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (49, '121', '25215836', '1717490161', '00020240604162215296180403', 98.00, 'com.ssh.52', '', '', '', 'kugame', '', '', '', '2024-06-04 16:36:01', '2024-06-04 16:36:01', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (50, '121', '25215871', '1717497892', '00020240604184452283805253', 648.00, 'com.ssh.35', '', '', '', 'kugame2', '', '', '', '2024-06-04 18:44:52', '2024-06-04 18:44:52', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (51, '121', '25215871', '1717498079', '00020240604184759433675733', 98.00, 'com.ssh.51', '', '', '', 'kugame2', '', '', '', '2024-06-04 18:47:59', '2024-06-04 18:47:59', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (52, '121', '25215871', '1717498135', '00020240604184855120989773', 98.00, 'com.ssh.52', '', '', '', 'kugame2', '', '', '', '2024-06-04 18:48:55', '2024-06-04 18:48:55', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (53, '121', '25215836', '1717568072', '00020240605141432606743253', 6.00, 'com.ssh.28', '', '', '', 'kugame', '', '', '', '2024-06-05 14:14:32', '2024-06-05 14:14:32', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (54, '121', '25215836', '1717568078', '00020240605141438761053353', 18.00, 'com.ssh.29', '', '', '', 'kugame', '', '', '', '2024-06-05 14:14:38', '2024-06-05 14:14:38', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (55, '121', '25215836', '1717568091', '00020240605141451518974233', 68.00, 'com.ssh.44', '', '', '', 'kugame', '', '', '', '2024-06-05 14:14:51', '2024-06-05 14:14:51', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (56, '121', '25215836', '1717572866', '00020240605153426322023023', 648.00, 'com.ssh.45', '', '', '', 'kugame', '', '', '', '2024-06-05 15:34:26', '2024-06-05 15:34:26', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (57, '121', '25215836', '1717587208', '00020240605193328233496843', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 19:33:28', '2024-06-05 19:33:29', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (58, '121', '25215836', '1717589202', '00020240605200642309488693', 30.00, 'com.ssh.54', '', '', '', 'kugame', '', '', '', '2024-06-05 20:06:42', '2024-06-05 20:06:42', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (59, '121', '25215836', '1717590265', '00020240605202425784424233', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 20:24:25', '2024-06-05 20:24:25', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (60, '121', '25215836', '1717590291', '00020240605202451182261813', 30.00, 'com.ssh.54', '', '', '', 'kugame', '', '', '', '2024-06-05 20:24:51', '2024-06-05 20:24:51', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (61, '121', '25215836', '1717590319', '00020240605202519757638813', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 20:25:19', '2024-06-05 20:25:19', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (62, '121', '25215836', '1717590816', '00020240605203336505784973', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 20:33:36', '2024-06-05 20:33:36', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (63, '121', '25215836', '1717594058', '00020240605212738126408003', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 21:27:38', '2024-06-05 21:27:39', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (64, '121', '25215836', '1717594610', '00020240605213650676380983', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 21:36:50', '2024-06-05 21:36:51', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (65, '121', '25215836', '1717594614', '00020240605213653072559083', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 21:36:54', '2024-06-05 21:36:54', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (66, '121', '25215836', '1717595227', '00020240605214707183199343', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 21:47:07', '2024-06-05 21:47:07', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (67, '121', '25215836', '1717596520', '00020240605220840914740043', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 22:08:40', '2024-06-05 22:08:40', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (68, '121', '25215836', '1717597235', '00020240605222035736142033', 30.00, 'com.ssh.54', '', '', '', 'kugame', '', '', '', '2024-06-05 22:20:35', '2024-06-05 22:20:35', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (69, '121', '25215836', '1717597840', '00020240605223040459325113', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 22:30:40', '2024-06-05 22:30:40', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (70, '121', '25215836', '1717597845', '00020240605223045558463353', 30.00, 'com.ssh.54', '', '', '', 'kugame', '', '', '', '2024-06-05 22:30:45', '2024-06-05 22:30:45', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (71, '121', '25215836', '1717597850', '00020240605223050687940203', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 22:30:50', '2024-06-05 22:30:50', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (72, '121', '25215836', '1717598756', '00020240605224556184296143', 30.00, 'com.ssh.54', '', '', '', 'kugame', '', '', '', '2024-06-05 22:45:56', '2024-06-05 22:45:56', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (73, '121', '25215836', '1717598806', '00020240605224645384754453', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-05 22:46:46', '2024-06-05 22:46:46', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (74, '121', '25215836', '1717640369', '00020240606101928335455583', 6.00, 'com.ssh.53', '', '', '', 'kugame', '', '', '', '2024-06-06 10:19:29', '2024-06-06 10:19:29', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (75, '121', '25215836', '1717640691', '00020240606102451391159923', 15.00, 'com.ssh.55', '', '', '', 'kugame', '', '', '', '2024-06-06 10:24:51', '2024-06-06 10:24:51', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (76, '121', '25215836', '1717640695', '00020240606102455098294943', 18.00, 'com.ssh.56', '', '', '', 'kugame', '', '', '', '2024-06-06 10:24:55', '2024-06-06 10:24:55', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (77, '121', '25215836', '1717640709', '00020240606102509145864043', 6.00, 'com.ssh.28', '', '', '', 'kugame', '', '', '', '2024-06-06 10:25:09', '2024-06-06 10:25:09', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (78, '121', '25215836', '1717640716', '00020240606102516131237483', 18.00, 'com.ssh.56', '', '', '', 'kugame', '', '', '', '2024-06-06 10:25:16', '2024-06-06 10:25:17', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (79, '121', '25215836', '1717640727', '00020240606102527196138063', 18.00, 'com.ssh.56', '', '', '', 'kugame', '', '', '', '2024-06-06 10:25:27', '2024-06-06 10:25:27', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (80, '121', '25215836', '1717666850', '00020240606174050161585063', 15.00, 'com.ssh.55', '', '', '', 'kugame', '', '', '', '2024-06-06 17:40:50', '2024-06-06 17:40:50', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (81, '121', '25215836', '1717666882', '00020240606174122149916273', 18.00, 'com.ssh.56', '', '', '', 'kugame', '', '', '', '2024-06-06 17:41:22', '2024-06-06 17:41:22', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (82, '121', '25215878', '1717667034', '00020240606174354213008403', 1000.00, 'com.ssh.36', '', '', '', 'kugame1', '', '', '', '2024-06-06 17:43:54', '2024-06-06 17:43:55', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (83, '121', '25215871', '1717667210', '00020240606174650600092523', 6.00, 'com.ssh.53', '', '', '', 'kugame2', '', '', '', '2024-06-06 17:46:50', '2024-06-06 17:46:50', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (84, '121', '25215871', '1717667223', '00020240606174703825901413', 18.00, 'com.ssh.56', '', '', '', 'kugame2', '', '', '', '2024-06-06 17:47:03', '2024-06-06 17:47:03', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (85, '121', '25215871', '1717673763', '00020240606193603590035973', 6.00, 'com.ssh.53', '', '', '', 'kugame2', '', '', '', '2024-06-06 19:36:03', '2024-06-06 19:36:03', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (86, '121', '25215871', '1717673770', '00020240606193610777107093', 6.00, 'com.ssh.53', '', '', '', 'kugame2', '', '', '', '2024-06-06 19:36:10', '2024-06-06 19:36:10', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (87, '121', '25215871', '1717673774', '00020240606193614647232643', 6.00, 'com.ssh.53', '', '', '', 'kugame2', '', '', '', '2024-06-06 19:36:14', '2024-06-06 19:36:14', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (88, '121', '25215871', '1717673776', '00020240606193616609826933', 6.00, 'com.ssh.53', '', '', '', 'kugame2', '', '', '', '2024-06-06 19:36:16', '2024-06-06 19:36:16', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (89, '121', '25215871', '1717758382', '00020240607190622536618133', 98.00, 'com.ssh.51', '', '', '', 'kugame2', '', '', '', '2024-06-07 19:06:22', '2024-06-07 19:06:22', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (90, '121', '25215836', '1718086739', '00020240611141859576751233', 98.00, 'com.ssh.51', '', '', '', 'kugame', '', '', '', '2024-06-11 14:18:59', '2024-06-11 14:18:59', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (91, '121', '25215836', '1718163764', '00020240612114244897155723', 98.00, 'com.ssh.51', '', '', '', 'kugame', '', '', '', '2024-06-12 11:42:44', '2024-06-12 11:42:45', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (92, '121', '25215836', '1718164183', '00020240612114942105459303', 98.00, 'com.ssh.52', '', '', '', 'kugame', '', '', '', '2024-06-12 11:49:43', '2024-06-12 11:49:43', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (93, '121', '25215836', '1718960434', '00020240621170034434800223', 6.00, 'com.ssh.28', '', '', '', 'kugame', '', '', '', '2024-06-21 17:00:34', '2024-06-21 17:00:34', 'S');
INSERT INTO `pay_quilk_universal` (`pay_id`, `server_id`, `character_id`, `transaction_id`, `order_id`, `amount`, `product_id`, `api_app_id`, `api_channel_id`, `api_pay_type`, `api_account_id`, `api_role_id`, `api_server_id`, `api_trans_type`, `create_date`, `pay_date`, `stat`) VALUES (94, '121', '25215836', '1718960523', '00020240621170203279905523', 18.00, 'com.ssh.29', '', '', '', 'kugame', '', '', '', '2024-06-21 17:02:03', '2024-06-21 17:02:03', 'S');
COMMIT;

-- ----------------------------
-- Procedure structure for lp_pay_brandroid_universal_request
-- ----------------------------
DROP PROCEDURE IF EXISTS `lp_pay_brandroid_universal_request`;
delimiter ;;
CREATE PROCEDURE `lp_pay_brandroid_universal_request`(p_server_id VARCHAR(60), p_character_id VARCHAR(60), p_transaction_id VARCHAR(60), p_order_id VARCHAR(60), p_amount DECIMAL(16,2), p_product_id VARCHAR(60), 
						p_api_app_id VARCHAR(100), p_api_channel_id VARCHAR(100), p_api_pay_type VARCHAR(100), p_api_account_id VARCHAR(100), 
						p_api_role_id VARCHAR(100), p_api_server_id VARCHAR(100), p_api_trans_type VARCHAR(100))
LABEL_PROC:

BEGIN	
	SET @pay_id = 0;
	
	# check parameter
	IF p_amount <= 0 THEN
		SELECT -1500001 AS return_code, 'amount error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_server_id = '' THEN
		SELECT -1500002 AS return_code, 'server_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_character_id = '' THEN
		SELECT -1500003 AS return_code, 'character_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	IF p_transaction_id = '' THEN
		SELECT -1500004 AS return_code, 'transaction_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_order_id = '' THEN
		SELECT -1500005 AS return_code, 'order_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	
	



	# select pay_brandroid_universal from order_id
	SET @row_order = 1;
	SELECT transaction_id INTO @transaction_id FROM pay_brandroid_universal WHERE order_id = p_order_id;
	IF FOUND_ROWS() = 0 THEN
		SET @row_order = 0;
	ELSEIF ISNULL(@transaction_id) THEN
		SET @transaction_id = '';
	END IF;
	

	
	# select pay_brandroid_universal from transaction_id
	SET @row_transaction = 1;
	SELECT pay_id, order_id, stat INTO @pay_id, @order_id, @stat FROM pay_brandroid_universal WHERE transaction_id = p_transaction_id;
	IF FOUND_ROWS() = 0 THEN
		SET @row_transaction = 0;
	ELSEIF ISNULL(@order_id) THEN
		SET @order_id = '';
	END IF;
	


	
	#insert 
	IF @row_order = 0 AND @row_transaction = 0 THEN
	
		INSERT INTO pay_brandroid_universal (server_id, character_id, transaction_id, order_id, amount, product_id, 
						api_app_id, api_channel_id, api_pay_type, api_account_id, api_role_id, api_server_id, api_trans_type, 
						create_date, pay_date, stat)
			VALUES (p_server_id, p_character_id, p_transaction_id, p_order_id, p_amount, p_product_id, 
				p_api_app_id, p_api_channel_id, p_api_pay_type, p_api_account_id, p_api_role_id, p_api_server_id, p_api_trans_type, 
				NOW(), NOW(), 'N');
	
		SET @pay_id = @@IDENTITY; 
	#order is completed
	ELSEIF @transaction_id = p_transaction_id AND @order_id = p_order_id THEN
	
		IF UPPER(@stat) = 'S' THEN
			SELECT 1 AS return_code, 0 AS pay_id;
			LEAVE LABEL_PROC;
		END IF;	
	#check transaction_id and order_id
	ELSE
		SELECT -1500011 AS return_code, 'transaction_id or order_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	
	
	# return info
	SELECT 1 AS return_code, @pay_id AS pay_id;
	

END LABEL_PROC
;;
delimiter ;

-- ----------------------------
-- Procedure structure for lp_pay_brandroid_universal_result
-- ----------------------------
DROP PROCEDURE IF EXISTS `lp_pay_brandroid_universal_result`;
delimiter ;;
CREATE PROCEDURE `lp_pay_brandroid_universal_result`(p_pay_id INT)
LABEL_PROC:

BEGIN	
	# check parameter
	IF p_pay_id <= 0 THEN
		SELECT -1500006 AS return_code, 'pay_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	
	SELECT stat INTO @stat FROM pay_brandroid_universal WHERE pay_id = p_pay_id;
	IF FOUND_ROWS() = 0 THEN
		SELECT -1500013 AS return_code, 'table pay_brandroid_universal row not found' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF UPPER(@stat) = 'S' THEN
		SELECT 1 AS return_code;
		LEAVE LABEL_PROC;
	END IF;
	
	
	# update user_pay
	UPDATE pay_brandroid_universal SET 
		pay_date = NOW(), 
		stat = 'S' 
		WHERE pay_id = p_pay_id;
		
	SELECT 1 AS return_code;
	

END LABEL_PROC
;;
delimiter ;

-- ----------------------------
-- Procedure structure for lp_pay_common_universal_request
-- ----------------------------
DROP PROCEDURE IF EXISTS `lp_pay_common_universal_request`;
delimiter ;;
CREATE PROCEDURE `lp_pay_common_universal_request`(p_server_id VARCHAR(60), p_character_id VARCHAR(60), p_transaction_id VARCHAR(60), p_order_id VARCHAR(60), p_amount DECIMAL(16,2), p_product_id VARCHAR(60), 
						p_api_app_id VARCHAR(100), p_api_channel_id VARCHAR(100), p_api_pay_type VARCHAR(100), p_api_account_id VARCHAR(100), 
						p_api_role_id VARCHAR(100), p_api_server_id VARCHAR(100), p_api_trans_type VARCHAR(100))
LABEL_PROC:

BEGIN	
	SET @pay_id = 0;
	
	# check parameter
	IF p_amount <= 0 THEN
		SELECT -1500001 AS return_code, 'amount error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_server_id = '' THEN
		SELECT -1500001 AS return_code, 'server_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_character_id = '' THEN
		SELECT -1500001 AS return_code, 'character_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	IF p_transaction_id = '' THEN
		SELECT -1500001 AS return_code, 'transaction_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_order_id = '' THEN
		SELECT -1500001 AS return_code, 'order_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	
	



	# select pay_common_universal from order_id
	SET @row_order = 1;
	SELECT transaction_id INTO @transaction_id FROM pay_common_universal WHERE order_id = p_order_id;
	IF FOUND_ROWS() = 0 THEN
		SET @row_order = 0;
	ELSEIF ISNULL(@transaction_id) THEN
		SET @transaction_id = '';
	END IF;
	

	
	# select pay_common_universal from transaction_id
	SET @row_transaction = 1;
	SELECT pay_id, order_id, stat INTO @pay_id, @order_id, @stat FROM pay_common_universal WHERE transaction_id = p_transaction_id;
	IF FOUND_ROWS() = 0 THEN
		SET @row_transaction = 0;
	ELSEIF ISNULL(@order_id) THEN
		SET @order_id = '';
	END IF;
	


	
	#insert 
	IF @row_order = 0 AND @row_transaction = 0 THEN
	
		INSERT INTO pay_common_universal (server_id, character_id, transaction_id, order_id, amount, product_id, 
						api_app_id, api_channel_id, api_pay_type, api_account_id, api_role_id, api_server_id, api_trans_type, 
						create_date, pay_date, stat)
			VALUES (p_server_id, p_character_id, p_transaction_id, p_order_id, p_amount, p_product_id, 
				p_api_app_id, p_api_channel_id, p_api_pay_type, p_api_account_id, p_api_role_id, p_api_server_id, p_api_trans_type, 
				NOW(), NOW(), 'N');
	
		SET @pay_id = @@IDENTITY; 
	#order is completed
	ELSEIF @transaction_id = p_transaction_id AND @order_id = p_order_id THEN
	
		IF UPPER(@stat) = 'S' THEN
			SELECT 1 AS return_code, 0 AS pay_id;
			LEAVE LABEL_PROC;
		END IF;	
	#check transaction_id and order_id
	ELSE
		SELECT -1500011 AS return_code, 'transaction_id or order_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	
	
	# return info
	SELECT 1 AS return_code, @pay_id AS pay_id;
	

END LABEL_PROC
;;
delimiter ;

-- ----------------------------
-- Procedure structure for lp_pay_common_universal_result
-- ----------------------------
DROP PROCEDURE IF EXISTS `lp_pay_common_universal_result`;
delimiter ;;
CREATE PROCEDURE `lp_pay_common_universal_result`(p_pay_id INT)
LABEL_PROC:

BEGIN	
	# check parameter
	IF p_pay_id <= 0 THEN
		SELECT -1500001 AS return_code, 'pay_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	
	SELECT stat INTO @stat FROM pay_common_universal WHERE pay_id = p_pay_id;
	IF FOUND_ROWS() = 0 THEN
		SELECT -1500011 AS return_code, 'table pay_common_universal row not found' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF UPPER(@stat) = 'S' THEN
		SELECT 1 AS return_code;
		LEAVE LABEL_PROC;
	END IF;
	
	
	# update user_pay
	UPDATE pay_common_universal SET 
		pay_date = NOW(), 
		stat = 'S' 
		WHERE pay_id = p_pay_id;
		
	SELECT 1 AS return_code;
	

END LABEL_PROC
;;
delimiter ;

-- ----------------------------
-- Procedure structure for lp_pay_h365_universal_request
-- ----------------------------
DROP PROCEDURE IF EXISTS `lp_pay_h365_universal_request`;
delimiter ;;
CREATE PROCEDURE `lp_pay_h365_universal_request`(p_server_id VARCHAR(60), p_character_id VARCHAR(60), p_transaction_id VARCHAR(60), p_order_id VARCHAR(60), p_amount DECIMAL(16,2), p_product_id VARCHAR(60), 
						p_api_app_id VARCHAR(100), p_api_channel_id VARCHAR(100), p_api_pay_type VARCHAR(100), p_api_account_id VARCHAR(100), 
						p_api_role_id VARCHAR(100), p_api_server_id VARCHAR(100), p_api_trans_type VARCHAR(100))
LABEL_PROC:

BEGIN	
	SET @pay_id = 0;
	
	# check parameter
	IF p_amount <= 0 THEN
		SELECT -1500001 AS return_code, 'amount error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_server_id = '' THEN
		SELECT -1500001 AS return_code, 'server_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_character_id = '' THEN
		SELECT -1500001 AS return_code, 'character_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	IF p_transaction_id = '' THEN
		SELECT -1500001 AS return_code, 'transaction_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_order_id = '' THEN
		SELECT -1500001 AS return_code, 'order_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	
	



	# select pay_h365_universal from order_id
	SET @row_order = 1;
	SELECT transaction_id INTO @transaction_id FROM pay_h365_universal WHERE order_id = p_order_id;
	IF FOUND_ROWS() = 0 THEN
		SET @row_order = 0;
	ELSEIF ISNULL(@transaction_id) THEN
		SET @transaction_id = '';
	END IF;
	

	
	# select pay_h365_universal from transaction_id
	SET @row_transaction = 1;
	SELECT pay_id, order_id, stat INTO @pay_id, @order_id, @stat FROM pay_h365_universal WHERE transaction_id = p_transaction_id;
	IF FOUND_ROWS() = 0 THEN
		SET @row_transaction = 0;
	ELSEIF ISNULL(@order_id) THEN
		SET @order_id = '';
	END IF;
	


	
	#insert 
	IF @row_order = 0 AND @row_transaction = 0 THEN
	
		INSERT INTO pay_h365_universal (server_id, character_id, transaction_id, order_id, amount, product_id, 
						api_app_id, api_channel_id, api_pay_type, api_account_id, api_role_id, api_server_id, api_trans_type, 
						create_date, pay_date, stat)
			VALUES (p_server_id, p_character_id, p_transaction_id, p_order_id, p_amount, p_product_id, 
				p_api_app_id, p_api_channel_id, p_api_pay_type, p_api_account_id, p_api_role_id, p_api_server_id, p_api_trans_type, 
				NOW(), NOW(), 'N');
	
		SET @pay_id = @@IDENTITY; 
	#order is completed
	ELSEIF @transaction_id = p_transaction_id AND @order_id = p_order_id THEN
	
		IF UPPER(@stat) = 'S' THEN
			SELECT 1 AS return_code, 0 AS pay_id;
			LEAVE LABEL_PROC;
		END IF;	
	#check transaction_id and order_id
	ELSE
		SELECT -1500011 AS return_code, 'transaction_id or order_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	
	
	# return info
	SELECT 1 AS return_code, @pay_id AS pay_id;
	

END LABEL_PROC
;;
delimiter ;

-- ----------------------------
-- Procedure structure for lp_pay_h365_universal_result
-- ----------------------------
DROP PROCEDURE IF EXISTS `lp_pay_h365_universal_result`;
delimiter ;;
CREATE PROCEDURE `lp_pay_h365_universal_result`(p_pay_id INT)
LABEL_PROC:

BEGIN	
	# check parameter
	IF p_pay_id <= 0 THEN
		SELECT -1500001 AS return_code, 'pay_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	
	SELECT stat INTO @stat FROM pay_h365_universal WHERE pay_id = p_pay_id;
	IF FOUND_ROWS() = 0 THEN
		SELECT -1500011 AS return_code, 'table pay_h365_universal row not found' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF UPPER(@stat) = 'S' THEN
		SELECT 1 AS return_code;
		LEAVE LABEL_PROC;
	END IF;
	
	
	# update user_pay
	UPDATE pay_h365_universal SET 
		pay_date = NOW(), 
		stat = 'S' 
		WHERE pay_id = p_pay_id;
		
	SELECT 1 AS return_code;
	

END LABEL_PROC
;;
delimiter ;

-- ----------------------------
-- Procedure structure for lp_pay_laosiji_universal_request
-- ----------------------------
DROP PROCEDURE IF EXISTS `lp_pay_laosiji_universal_request`;
delimiter ;;
CREATE PROCEDURE `lp_pay_laosiji_universal_request`(p_server_id VARCHAR(60), p_character_id VARCHAR(60), p_transaction_id VARCHAR(60), p_order_id VARCHAR(60), p_amount DECIMAL(16,2), p_product_id VARCHAR(60), 
						p_api_app_id VARCHAR(100), p_api_channel_id VARCHAR(100), p_api_pay_type VARCHAR(100), p_api_account_id VARCHAR(100), 
						p_api_role_id VARCHAR(100), p_api_server_id VARCHAR(100), p_api_trans_type VARCHAR(100))
LABEL_PROC:

BEGIN	
	SET @pay_id = 0;
	
	# check parameter
	IF p_amount <= 0 THEN
		SELECT -1500001 AS return_code, 'amount error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_server_id = '' THEN
		SELECT -1500001 AS return_code, 'server_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_character_id = '' THEN
		SELECT -1500001 AS return_code, 'character_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	IF p_transaction_id = '' THEN
		SELECT -1500001 AS return_code, 'transaction_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_order_id = '' THEN
		SELECT -1500001 AS return_code, 'order_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	
	



	# select pay_laosiji_universal from order_id
	SET @row_order = 1;
	SELECT transaction_id INTO @transaction_id FROM pay_laosiji_universal WHERE order_id = p_order_id;
	IF FOUND_ROWS() = 0 THEN
		SET @row_order = 0;
	ELSEIF ISNULL(@transaction_id) THEN
		SET @transaction_id = '';
	END IF;
	

	
	# select pay_laosiji_universal from transaction_id
	SET @row_transaction = 1;
	SELECT pay_id, order_id, stat INTO @pay_id, @order_id, @stat FROM pay_laosiji_universal WHERE transaction_id = p_transaction_id;
	IF FOUND_ROWS() = 0 THEN
		SET @row_transaction = 0;
	ELSEIF ISNULL(@order_id) THEN
		SET @order_id = '';
	END IF;
	


	
	#insert 
	IF @row_order = 0 AND @row_transaction = 0 THEN
	
		INSERT INTO pay_laosiji_universal (server_id, character_id, transaction_id, order_id, amount, product_id, 
						api_app_id, api_channel_id, api_pay_type, api_account_id, api_role_id, api_server_id, api_trans_type, 
						create_date, pay_date, stat)
			VALUES (p_server_id, p_character_id, p_transaction_id, p_order_id, p_amount, p_product_id, 
				p_api_app_id, p_api_channel_id, p_api_pay_type, p_api_account_id, p_api_role_id, p_api_server_id, p_api_trans_type, 
				NOW(), NOW(), 'N');
	
		SET @pay_id = @@IDENTITY; 
	#order is completed
	ELSEIF @transaction_id = p_transaction_id AND @order_id = p_order_id THEN
	
		IF UPPER(@stat) = 'S' THEN
			SELECT 1 AS return_code, 0 AS pay_id;
			LEAVE LABEL_PROC;
		END IF;	
	#check transaction_id and order_id
	ELSE
		SELECT -1500011 AS return_code, 'transaction_id or order_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	
	
	# return info
	SELECT 1 AS return_code, @pay_id AS pay_id;
	

END LABEL_PROC
;;
delimiter ;

-- ----------------------------
-- Procedure structure for lp_pay_laosiji_universal_result
-- ----------------------------
DROP PROCEDURE IF EXISTS `lp_pay_laosiji_universal_result`;
delimiter ;;
CREATE PROCEDURE `lp_pay_laosiji_universal_result`(p_pay_id INT)
LABEL_PROC:

BEGIN	
	# check parameter
	IF p_pay_id <= 0 THEN
		SELECT -1500001 AS return_code, 'pay_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	
	SELECT stat INTO @stat FROM pay_laosiji_universal WHERE pay_id = p_pay_id;
	IF FOUND_ROWS() = 0 THEN
		SELECT -1500011 AS return_code, 'table pay_laosiji_universal row not found' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF UPPER(@stat) = 'S' THEN
		SELECT 1 AS return_code;
		LEAVE LABEL_PROC;
	END IF;
	
	
	# update user_pay
	UPDATE pay_laosiji_universal SET 
		pay_date = NOW(), 
		stat = 'S' 
		WHERE pay_id = p_pay_id;
		
	SELECT 1 AS return_code;
	

END LABEL_PROC
;;
delimiter ;

-- ----------------------------
-- Procedure structure for lp_pay_quilk_universal_request
-- ----------------------------
DROP PROCEDURE IF EXISTS `lp_pay_quilk_universal_request`;
delimiter ;;
CREATE PROCEDURE `lp_pay_quilk_universal_request`(p_server_id VARCHAR(60), p_character_id VARCHAR(60), p_transaction_id VARCHAR(60), p_order_id VARCHAR(60), p_amount DECIMAL(16,2), p_product_id VARCHAR(60), 
						p_api_app_id VARCHAR(100), p_api_channel_id VARCHAR(100), p_api_pay_type VARCHAR(100), p_api_account_id VARCHAR(100), 
						p_api_role_id VARCHAR(100), p_api_server_id VARCHAR(100), p_api_trans_type VARCHAR(100))
LABEL_PROC:

BEGIN	
	SET @pay_id = 0;
	
	# check parameter
	IF p_amount <= 0 THEN
		SELECT -1500001 AS return_code, 'amount error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_server_id = '' THEN
		SELECT -1500002 AS return_code, 'server_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_character_id = '' THEN
		SELECT -1500003 AS return_code, 'character_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	IF p_transaction_id = '' THEN
		SELECT -1500004 AS return_code, 'transaction_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF p_order_id = '' THEN
		SELECT -1500005 AS return_code, 'order_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	
	



	# select pay_quilk_universal from order_id
	SET @row_order = 1;
	SELECT transaction_id INTO @transaction_id FROM pay_quilk_universal WHERE order_id = p_order_id;
	IF FOUND_ROWS() = 0 THEN
		SET @row_order = 0;
	ELSEIF ISNULL(@transaction_id) THEN
		SET @transaction_id = '';
	END IF;
	

	
	# select pay_quilk_universal from transaction_id
	SET @row_transaction = 1;
	SELECT pay_id, order_id, stat INTO @pay_id, @order_id, @stat FROM pay_quilk_universal WHERE transaction_id = p_transaction_id;
	IF FOUND_ROWS() = 0 THEN
		SET @row_transaction = 0;
	ELSEIF ISNULL(@order_id) THEN
		SET @order_id = '';
	END IF;
	


	
	#insert 
	IF @row_order = 0 AND @row_transaction = 0 THEN
	
		INSERT INTO pay_quilk_universal (server_id, character_id, transaction_id, order_id, amount, product_id, 
						api_app_id, api_channel_id, api_pay_type, api_account_id, api_role_id, api_server_id, api_trans_type, 
						create_date, pay_date, stat)
			VALUES (p_server_id, p_character_id, p_transaction_id, p_order_id, p_amount, p_product_id, 
				p_api_app_id, p_api_channel_id, p_api_pay_type, p_api_account_id, p_api_role_id, p_api_server_id, p_api_trans_type, 
				NOW(), NOW(), 'N');
	
		SET @pay_id = @@IDENTITY; 
	#order is completed
	ELSEIF @transaction_id = p_transaction_id AND @order_id = p_order_id THEN
	
		IF UPPER(@stat) = 'S' THEN
			SELECT 1 AS return_code, 0 AS pay_id;
			LEAVE LABEL_PROC;
		END IF;	
	#check transaction_id and order_id
	ELSE
		SELECT -1500011 AS return_code, 'transaction_id or order_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	
	
	# return info
	SELECT 1 AS return_code, @pay_id AS pay_id;
	

END LABEL_PROC
;;
delimiter ;

-- ----------------------------
-- Procedure structure for lp_pay_quilk_universal_result
-- ----------------------------
DROP PROCEDURE IF EXISTS `lp_pay_quilk_universal_result`;
delimiter ;;
CREATE PROCEDURE `lp_pay_quilk_universal_result`(p_pay_id INT)
LABEL_PROC:

BEGIN	
	# check parameter
	IF p_pay_id <= 0 THEN
		SELECT -1500006 AS return_code, 'pay_id error' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;

	
	SELECT stat INTO @stat FROM pay_quilk_universal WHERE pay_id = p_pay_id;
	IF FOUND_ROWS() = 0 THEN
		SELECT -1500013 AS return_code, 'table pay_quilk_universal row not found' AS return_msg;
		LEAVE LABEL_PROC;
	END IF;
	
	IF UPPER(@stat) = 'S' THEN
		SELECT 1 AS return_code;
		LEAVE LABEL_PROC;
	END IF;
	
	
	# update user_pay
	UPDATE pay_quilk_universal SET 
		pay_date = NOW(), 
		stat = 'S' 
		WHERE pay_id = p_pay_id;
		
	SELECT 1 AS return_code;
	

END LABEL_PROC
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
