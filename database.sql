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
INSERT INTO `Borrow` VALUES (2013011187,'C2010095T','2014-06-28','2014-09-24',0,0),(2013011187,'C27415768','2014-05-09','2014-09-24',0,0);
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
  `is_admin` bool NOT NULL DEFAULT false,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `User`
--

LOCK TABLES `User` WRITE;
/*!40000 ALTER TABLE `User` DISABLE KEYS */;
INSERT INTO `User` VALUES (2013011187,'李思涵','1234', '1234', false);
/*!40000 ALTER TABLE `User` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Request`
--

DROP TABLE IF EXISTS `Request`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Request` (
  `reader_id` int(10) unsigned NOT NULL,
  `copy_id` char(9) NOT NULL,
  `time` timestamp NOT NULL,
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
/*!40000 ALTER TABLE `Request` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2014-09-11 18:58:26
