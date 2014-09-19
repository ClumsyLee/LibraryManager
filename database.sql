-- MySQL dump 10.13  Distrib 5.6.20, for Linux (x86_64)
--
-- Host: localhost    Database: library
-- ------------------------------------------------------
-- Server version	5.6.20

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Book`
--

DROP TABLE IF EXISTS `Book`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Book` (
  `isbn` bigint(13) unsigned NOT NULL,
  `call_num` varchar(20) NOT NULL,
  `title` varchar(255) NOT NULL,
  `author` varchar(128) NOT NULL,
  `imprint` varchar(128) NOT NULL,
  `abstract` text,
  `table_of_contents` text,
  PRIMARY KEY (`isbn`),
  UNIQUE KEY `call_num` (`call_num`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Book`
--

LOCK TABLES `Book` WRITE;
/*!40000 ALTER TABLE `Book` DISABLE KEYS */;
INSERT INTO `Book` VALUES (9787111165057,'TP393 K066C3','计算机网络: 自顶向下方法与Internet特色','(美) James F. Kurose, Keith W. Ross著; 陈鸣等译','北京: 机械工业出版社, 2005.6',NULL,NULL),(9787121201257,'TP312C M269A3B2','Effective STL中文版: 50条有效使用STL的经验','(美) Scott Meyers著; 潘爱民, 陈铭, 邹开红译','北京: 电子工业出版社, 2013.5',NULL,NULL);
/*!40000 ALTER TABLE `Book` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Borrow`
--

DROP TABLE IF EXISTS `Borrow`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Borrow` (
  `reader_id` int(10) unsigned NOT NULL,
  `copy_id` char(9) NOT NULL,
  `begin_date` date NOT NULL,
  `due_date` date NOT NULL,
  `renew_times` tinyint(3) unsigned NOT NULL,
  `recalled` tinyint(1) NOT NULL,
  PRIMARY KEY (`reader_id`,`copy_id`),
  KEY `copy_id` (`copy_id`),
  CONSTRAINT `Borrow_ibfk_1` FOREIGN KEY (`reader_id`) REFERENCES `User` (`id`),
  CONSTRAINT `Borrow_ibfk_2` FOREIGN KEY (`copy_id`) REFERENCES `Copy` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Borrow`
--

LOCK TABLES `Borrow` WRITE;
/*!40000 ALTER TABLE `Borrow` DISABLE KEYS */;
INSERT INTO `Borrow` VALUES (2013011187,'C2010095T','2014-09-18','2014-12-17',0,0),(2013011189,'C27415768','2014-09-18','2014-12-17',0,0);
/*!40000 ALTER TABLE `Borrow` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Copy`
--

DROP TABLE IF EXISTS `Copy`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Copy` (
  `id` char(9) NOT NULL,
  `isbn` bigint(13) unsigned NOT NULL,
  `volumn` tinyint(3) unsigned NOT NULL,
  `status` enum('ON_SHELF','LENT','AT_LOAN_DESK','LOST','LIB_USE_ONLY') NOT NULL,
  PRIMARY KEY (`id`),
  KEY `isbn` (`isbn`),
  CONSTRAINT `Copy_ibfk_1` FOREIGN KEY (`isbn`) REFERENCES `Book` (`isbn`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Copy`
--

LOCK TABLES `Copy` WRITE;
/*!40000 ALTER TABLE `Copy` DISABLE KEYS */;
INSERT INTO `Copy` VALUES ('C2010095T',9787111165057,0,'LENT'),('C27415768',9787121201257,0,'LENT');
/*!40000 ALTER TABLE `Copy` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Temporary view structure for view `CopyInfo`
--

DROP TABLE IF EXISTS `CopyInfo`;
/*!50001 DROP VIEW IF EXISTS `CopyInfo`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE VIEW `CopyInfo` AS SELECT 
 1 AS `id`,
 1 AS `title`,
 1 AS `isbn`,
 1 AS `call_num`,
 1 AS `status`,
 1 AS `due_date`,
 1 AS `request_num`*/;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `Request`
--

DROP TABLE IF EXISTS `Request`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Request` (
  `reader_id` int(10) unsigned NOT NULL,
  `copy_id` char(9) NOT NULL,
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`reader_id`,`copy_id`),
  KEY `copy_id` (`copy_id`),
  CONSTRAINT `Request_ibfk_1` FOREIGN KEY (`reader_id`) REFERENCES `User` (`id`),
  CONSTRAINT `Request_ibfk_2` FOREIGN KEY (`copy_id`) REFERENCES `Copy` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Request`
--

LOCK TABLES `Request` WRITE;
/*!40000 ALTER TABLE `Request` DISABLE KEYS */;
INSERT INTO `Request` VALUES (2013011188,'C27415768','2014-09-18 11:44:16');
/*!40000 ALTER TABLE `Request` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `User`
--

DROP TABLE IF EXISTS `User`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `User` (
  `id` int(10) unsigned NOT NULL,
  `name` varchar(32) DEFAULT NULL,
  `password` varchar(255) NOT NULL,
  `salt` varchar(255) NOT NULL,
  `admin_priv` tinyint(1) NOT NULL DEFAULT '0',
  `reader_priv` tinyint(1) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `User`
--

LOCK TABLES `User` WRITE;
/*!40000 ALTER TABLE `User` DISABLE KEYS */;
INSERT INTO `User` VALUES (1,'admin','8a4a49704582814952872966eed8ff9e902ce06c470b3dc79d9943c79a48591923620b213cf14c08a141c444b1643a3759597473bca4eea4bb83662538f5d0ff','9db9f19203136f553dc73ed50a31188e8a7012e02794569be7599fb2df4241e1807ff1ff4e3a4af32ff0219a252717957d9ff80a420095e6c05001d2b604bae3',1,1),(2013011187,'李思涵','4d51dc912acefde68e25d54e3d0e2667e67751f12e65256200fcfecd4eea4668e5c97d93f3f00086c92722504b1920fe0f796235c91258d6f871ab8785dbd691','ca5dd7a0f081942d0e82bf57988b451a240dad515e0f8a5d26d5840ba74f7d6cad6e61652f757d69c8bb51ef3c8a8cf4795e4abe65ff96bef18b545315d10b2b',0,1),(2013011188,'常增禄','60b97991085938a57caf0bdadfa20e92c06ec0c28470f7468da0bc9a5d3d33218975e5d6fbfe809bacd6fa6ee5d42e216cd27419a494a9fcd84e1a13761c1523','36cd484aa0bf5959ab438b1c6d6449d9f6b95963bf8bc8e08095ffc480947b59ed6ca06cd8a51d740965de00b8e723a5d3d7c0be5077a2e80bd85f6674a22ffb',0,1),(2013011189,'郭一隆','76a6a8d0d19ea87f2ef28b16a24a4d96b4a87bfe2b7fdd0c9dc21dacb9476380b12b10d84050329911d5ad8af54adf897140902da7164b53f4b53a4862eed42b','9a24cb9d43d2636d1fb35c485d4013d4772af6f8f47b8c299110a5a5b5d99248f728ff4bc5c4174d62fe44af846a421164a999ab6c4222e80b3ad4c4093d5e5c',0,1);
/*!40000 ALTER TABLE `User` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Final view structure for view `CopyInfo`
--

/*!50001 DROP VIEW IF EXISTS `CopyInfo`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`thomas`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `CopyInfo` AS select `Copy`.`id` AS `id`,`Book`.`title` AS `title`,`Book`.`isbn` AS `isbn`,`Book`.`call_num` AS `call_num`,`Copy`.`status` AS `status`,`Borrow`.`due_date` AS `due_date`,count(`Request`.`reader_id`) AS `request_num` from (((`Copy` join `Book` on((`Copy`.`isbn` = `Book`.`isbn`))) left join `Borrow` on((`Borrow`.`copy_id` = `Copy`.`id`))) left join `Request` on((`Request`.`copy_id` = `Copy`.`id`))) group by `Copy`.`id` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2014-09-20  2:15:53
