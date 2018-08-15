--
-- Скрипт сгенерирован Devart dbForge Studio for MySQL, Версия 8.0.40.0
-- Домашняя страница продукта: http://www.devart.com/ru/dbforge/mysql/studio
-- Дата скрипта: 15.08.2018 12:18:29
-- Версия сервера: 8.0.11
-- Версия клиента: 4.1
--

-- 
-- Отключение внешних ключей
-- 
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;

-- 
-- Установить режим SQL (SQL mode)
-- 
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

-- 
-- Установка кодировки, с использованием которой клиент будет посылать запросы на сервер
--
SET NAMES 'utf8';

CREATE DATABASE workerplace
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Установка базы данных по умолчанию
--
USE workerplace;

--
-- Создать таблицу `post`
--
CREATE TABLE post (
  CodPost int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name varchar(255) NOT NULL,
  PRIMARY KEY (CodPost)
)
ENGINE = INNODB,
AUTO_INCREMENT = 98,
AVG_ROW_LENGTH = 195,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать индекс `Name` для объекта типа таблица `post`
--
ALTER TABLE post
ADD UNIQUE INDEX Name (Name);

--
-- Создать таблицу `departments`
--
CREATE TABLE departments (
  id int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  parent_id int(11) UNSIGNED NOT NULL DEFAULT 0,
  Name varchar(100) NOT NULL,
  Firm tinyint(1) NOT NULL DEFAULT 0,
  FP tinyint(1) NOT NULL DEFAULT 0,
  LockedUp timestamp NULL DEFAULT NULL,
  LockedBy varchar(20) DEFAULT NULL,
  RV tinyint(255) UNSIGNED DEFAULT 0,
  PRIMARY KEY (id)
)
ENGINE = INNODB,
AUTO_INCREMENT = 119,
AVG_ROW_LENGTH = 431,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать таблицу `users`
--
CREATE TABLE users (
  CodUser int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodAD int(8) DEFAULT NULL,
  LastName varchar(50) DEFAULT NULL,
  Name varchar(50) DEFAULT NULL,
  MiddleName varchar(50) DEFAULT NULL,
  FIOSummary varchar(50) NOT NULL,
  CodPost int(11) UNSIGNED NOT NULL,
  email varchar(50) DEFAULT NULL,
  additionalemail varchar(50) DEFAULT NULL,
  CodOrganization int(11) UNSIGNED NOT NULL,
  LockedUp timestamp NULL DEFAULT NULL,
  LockedBy varchar(20) DEFAULT NULL,
  RV tinyint(255) UNSIGNED DEFAULT 0,
  PRIMARY KEY (CodUser)
)
ENGINE = INNODB,
AUTO_INCREMENT = 154,
AVG_ROW_LENGTH = 125,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать индекс `CodAD` для объекта типа таблица `users`
--
ALTER TABLE users
ADD UNIQUE INDEX CodAD (CodAD);

--
-- Создать внешний ключ
--
ALTER TABLE users
ADD CONSTRAINT FKOrganization FOREIGN KEY (CodOrganization)
REFERENCES departments (id) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE users
ADD CONSTRAINT FKPost FOREIGN KEY (CodPost)
REFERENCES post (codpost) ON UPDATE CASCADE;

--
-- Создать таблицу `workerplace`
--
CREATE TABLE workerplace (
  CodWorkerPlace int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodDepartment int(11) UNSIGNED NOT NULL,
  Name varchar(100) NOT NULL,
  PrimaryUser int(11) UNSIGNED DEFAULT NULL,
  InternalNumber varchar(12) DEFAULT NULL,
  Location varchar(50) DEFAULT NULL,
  warehouse tinyint(1) UNSIGNED NOT NULL DEFAULT 0,
  LockedUp timestamp NULL DEFAULT NULL,
  LockedBy varchar(20) DEFAULT NULL,
  RV tinyint(255) UNSIGNED DEFAULT 0,
  PRIMARY KEY (CodWorkerPlace)
)
ENGINE = INNODB,
AUTO_INCREMENT = 238,
AVG_ROW_LENGTH = 117,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать индекс `FKDivision` для объекта типа таблица `workerplace`
--
ALTER TABLE workerplace
ADD INDEX FKDivision (CodDepartment);

--
-- Создать внешний ключ
--
ALTER TABLE workerplace
ADD CONSTRAINT FKDepartment FOREIGN KEY (CodDepartment)
REFERENCES departments (id) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE workerplace
ADD CONSTRAINT FK_workerplace_users_CodUser FOREIGN KEY (PrimaryUser)
REFERENCES users (coduser) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Создать таблицу `workplaceandusers`
--
CREATE TABLE workplaceandusers (
  CodWorkerPlace int(11) UNSIGNED NOT NULL,
  CodUser int(11) UNSIGNED NOT NULL
)
ENGINE = INNODB,
AVG_ROW_LENGTH = 128,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать внешний ключ
--
ALTER TABLE workplaceandusers
ADD CONSTRAINT FKfromUsers FOREIGN KEY (CodUser)
REFERENCES users (coduser) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE workplaceandusers
ADD CONSTRAINT FKfromWP FOREIGN KEY (CodWorkerPlace)
REFERENCES workerplace (codworkerplace) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать таблицу `historyuseronwp`
--
CREATE TABLE historyuseronwp (
  id int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodUser int(11) UNSIGNED NOT NULL,
  CodWorkerPlace int(11) UNSIGNED NOT NULL,
  DateIn date DEFAULT NULL,
  DateOut date DEFAULT NULL,
  PRIMARY KEY (id)
)
ENGINE = INNODB,
AUTO_INCREMENT = 3,
AVG_ROW_LENGTH = 5461,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать внешний ключ
--
ALTER TABLE historyuseronwp
ADD CONSTRAINT FK_historyuseronwp_users_CodUser FOREIGN KEY (CodUser)
REFERENCES users (coduser) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE historyuseronwp
ADD CONSTRAINT FK_historyuseronwp_workerplace_CodWorkerPlace FOREIGN KEY (CodWorkerPlace)
REFERENCES workerplace (codworkerplace) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать таблицу `tounits`
--
CREATE TABLE tounits (
  CodUser int(11) UNSIGNED NOT NULL,
  CodFilPred int(11) UNSIGNED DEFAULT NULL,
  CodDepartment int(11) UNSIGNED DEFAULT NULL
)
ENGINE = INNODB,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать индекс `CodUser` для объекта типа таблица `tounits`
--
ALTER TABLE tounits
ADD UNIQUE INDEX CodUser (CodUser);

--
-- Создать внешний ключ
--
ALTER TABLE tounits
ADD CONSTRAINT FKToUnitDep FOREIGN KEY (CodDepartment)
REFERENCES departments (id) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE tounits
ADD CONSTRAINT FKToUnitFilPred FOREIGN KEY (CodFilPred)
REFERENCES departments (id) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE tounits
ADD CONSTRAINT FKToUnitUser FOREIGN KEY (CodUser)
REFERENCES users (coduser) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать таблицу `typedevice`
--
CREATE TABLE typedevice (
  CodTypeDevice int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  IconPath varchar(255) DEFAULT NULL,
  Name varchar(100) NOT NULL,
  Type tinyint(1) NOT NULL,
  PRIMARY KEY (CodTypeDevice)
)
ENGINE = INNODB,
AUTO_INCREMENT = 37,
AVG_ROW_LENGTH = 468,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать таблицу `statedev`
--
CREATE TABLE statedev (
  CodState int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name varchar(20) NOT NULL,
  PRIMARY KEY (CodState)
)
ENGINE = INNODB,
AUTO_INCREMENT = 8,
AVG_ROW_LENGTH = 2340,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать индекс `Name` для объекта типа таблица `statedev`
--
ALTER TABLE statedev
ADD UNIQUE INDEX Name (Name);

--
-- Создать таблицу `producer`
--
CREATE TABLE producer (
  CodProducer int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name varchar(100) NOT NULL,
  www varchar(255) DEFAULT NULL,
  Note varchar(255) DEFAULT NULL,
  LockedUp timestamp NULL DEFAULT NULL,
  LockedBy varchar(20) DEFAULT NULL,
  RV tinyint(255) UNSIGNED DEFAULT 0,
  PRIMARY KEY (CodProducer)
)
ENGINE = INNODB,
AUTO_INCREMENT = 50,
AVG_ROW_LENGTH = 334,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать таблицу `po`
--
CREATE TABLE po (
  CodPO int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodGroupPO int(11) UNSIGNED NOT NULL DEFAULT 0,
  Name varchar(100) NOT NULL,
  isGroup tinyint(1) NOT NULL DEFAULT 0,
  CodProducer int(11) UNSIGNED DEFAULT NULL,
  Note varchar(255) DEFAULT NULL,
  Ico varchar(255) DEFAULT NULL,
  PRIMARY KEY (CodPO)
)
ENGINE = INNODB,
AUTO_INCREMENT = 75,
AVG_ROW_LENGTH = 819,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать индекс `FK_po_typePO_CodTypePO` для объекта типа таблица `po`
--
ALTER TABLE po
ADD INDEX FK_po_typePO_CodTypePO (CodGroupPO);

--
-- Создать внешний ключ
--
ALTER TABLE po
ADD CONSTRAINT FK_po_producer_CodProducer FOREIGN KEY (CodProducer)
REFERENCES producer (codproducer) ON UPDATE CASCADE;

--
-- Создать таблицу `domainwg`
--
CREATE TABLE domainwg (
  CodDomainWg int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name varchar(15) NOT NULL,
  PRIMARY KEY (CodDomainWg)
)
ENGINE = INNODB,
AUTO_INCREMENT = 8,
AVG_ROW_LENGTH = 3276,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать индекс `Name` для объекта типа таблица `domainwg`
--
ALTER TABLE domainwg
ADD UNIQUE INDEX Name (Name);

--
-- Создать таблицу `networkdata`
--
CREATE TABLE networkdata (
  CodNetworkData int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodWorkerPlace int(11) UNSIGNED NOT NULL,
  Name varchar(20) NOT NULL,
  CodDomainWg int(11) UNSIGNED NOT NULL,
  PRIMARY KEY (CodNetworkData)
)
ENGINE = INNODB,
AUTO_INCREMENT = 7,
AVG_ROW_LENGTH = 5461,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать индекс `FKMask` для объекта типа таблица `networkdata`
--
ALTER TABLE networkdata
ADD INDEX FKMask (CodDomainWg);

--
-- Создать индекс `Name` для объекта типа таблица `networkdata`
--
ALTER TABLE networkdata
ADD UNIQUE INDEX Name (Name);

--
-- Создать внешний ключ
--
ALTER TABLE networkdata
ADD CONSTRAINT FKWPNetwork FOREIGN KEY (CodWorkerPlace)
REFERENCES workerplace (codworkerplace) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE networkdata
ADD CONSTRAINT FK_networkdata_domainwg_CodDomainWg FOREIGN KEY (CodDomainWg)
REFERENCES domainwg (coddomainwg) ON UPDATE CASCADE;

--
-- Создать таблицу `networkinterface`
--
CREATE TABLE networkinterface (
  CodNetworkInterface int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodNetworkData int(11) UNSIGNED NOT NULL,
  Name varchar(100) NOT NULL,
  IP varchar(15) DEFAULT NULL,
  Mask varchar(15) DEFAULT NULL,
  Gate varchar(15) DEFAULT NULL,
  DNS varchar(100) DEFAULT NULL,
  WINS varchar(100) DEFAULT NULL,
  AutoIP tinyint(1) NOT NULL DEFAULT 1,
  AutoDNS tinyint(1) NOT NULL DEFAULT 1,
  AutoWINS tinyint(1) NOT NULL DEFAULT 1,
  PRIMARY KEY (CodNetworkInterface)
)
ENGINE = INNODB,
AUTO_INCREMENT = 5,
AVG_ROW_LENGTH = 5461,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать внешний ключ
--
ALTER TABLE networkinterface
ADD CONSTRAINT FK_networkinterface_networkdata_CodNetworkData FOREIGN KEY (CodNetworkData)
REFERENCES networkdata (codnetworkdata) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать таблицу `typelicense`
--
CREATE TABLE typelicense (
  CodTypeLicense int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name varchar(100) NOT NULL,
  PRIMARY KEY (CodTypeLicense)
)
ENGINE = INNODB,
AUTO_INCREMENT = 6,
AVG_ROW_LENGTH = 4096,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать таблицу `statepo`
--
CREATE TABLE statepo (
  CodStatePO int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name varchar(100) NOT NULL,
  PRIMARY KEY (CodStatePO)
)
ENGINE = INNODB,
AUTO_INCREMENT = 4,
AVG_ROW_LENGTH = 5461,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать таблицу `provider`
--
CREATE TABLE provider (
  CodProvider int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name varchar(100) NOT NULL,
  Adress varchar(150) DEFAULT NULL,
  Fax varchar(20) DEFAULT NULL,
  Phone varchar(20) DEFAULT NULL,
  email varchar(100) DEFAULT NULL,
  www varchar(255) DEFAULT NULL,
  LockedUp timestamp NULL DEFAULT NULL,
  LockedBy varchar(20) DEFAULT NULL,
  RV tinyint(255) UNSIGNED DEFAULT 0,
  PRIMARY KEY (CodProvider)
)
ENGINE = INNODB,
AUTO_INCREMENT = 6,
AVG_ROW_LENGTH = 4096,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать таблицу `device`
--
CREATE TABLE device (
  id int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  parent_id int(11) UNSIGNED NOT NULL DEFAULT 0,
  CodOrganization int(11) UNSIGNED NOT NULL,
  CodWorkerPlace int(11) UNSIGNED NOT NULL,
  CodTypeDevice int(11) UNSIGNED NOT NULL,
  Name varchar(150) NOT NULL,
  InventoryN varchar(50) DEFAULT NULL,
  SerialN varchar(50) DEFAULT NULL,
  CodProvider int(11) UNSIGNED DEFAULT NULL,
  CodProducer int(11) UNSIGNED DEFAULT NULL,
  DatePurchase date DEFAULT NULL,
  DatePosting date DEFAULT NULL,
  EndGuarantee date DEFAULT NULL,
  Price double(9, 2) UNSIGNED DEFAULT NULL,
  CodState int(11) UNSIGNED NOT NULL,
  Note varchar(255) DEFAULT NULL,
  Type tinyint(1) NOT NULL,
  LockedUp timestamp NULL DEFAULT NULL,
  LockedBy varchar(20) DEFAULT NULL,
  RV tinyint(255) UNSIGNED DEFAULT 0,
  PRIMARY KEY (id)
)
ENGINE = INNODB,
AUTO_INCREMENT = 1697,
AVG_ROW_LENGTH = 106,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать внешний ключ
--
ALTER TABLE device
ADD CONSTRAINT FK_device_departments_id FOREIGN KEY (CodOrganization)
REFERENCES departments (id) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE device
ADD CONSTRAINT FK_device_producer_CodProducer FOREIGN KEY (CodProducer)
REFERENCES producer (codproducer) ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE device
ADD CONSTRAINT FK_device_provider_CodProvider FOREIGN KEY (CodProvider)
REFERENCES provider (codprovider) ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE device
ADD CONSTRAINT FK_device_statedev_CodState FOREIGN KEY (CodState)
REFERENCES statedev (codstate);

--
-- Создать внешний ключ
--
ALTER TABLE device
ADD CONSTRAINT FK_device_typedevice_CodTypeDevice FOREIGN KEY (CodTypeDevice)
REFERENCES typedevice (codtypedevice) ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE device
ADD CONSTRAINT FK_device_workerplace_CodWorkerPlace FOREIGN KEY (CodWorkerPlace)
REFERENCES workerplace (codworkerplace) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать таблицу `licenses`
--
CREATE TABLE licenses (
  `key` int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodDevice int(11) UNSIGNED DEFAULT NULL,
  CodWorkerPlace int(11) UNSIGNED NOT NULL,
  CodPO int(11) UNSIGNED NOT NULL,
  RegName varchar(100) DEFAULT NULL,
  RegKey varchar(255) DEFAULT NULL,
  RegMail varchar(100) DEFAULT NULL,
  KolLicense int(11) NOT NULL DEFAULT 0,
  InvN varchar(100) DEFAULT NULL,
  VersionN varchar(100) DEFAULT NULL,
  CodProvider int(11) UNSIGNED DEFAULT NULL,
  DatePurchase date DEFAULT NULL,
  DateEndLicense date DEFAULT NULL,
  Price double(9, 2) UNSIGNED DEFAULT NULL,
  CodTypeLicense int(11) UNSIGNED NOT NULL,
  CodStatePO int(11) UNSIGNED NOT NULL,
  Note varchar(255) DEFAULT NULL,
  LockedUp timestamp NULL DEFAULT NULL,
  LockedBy varchar(20) DEFAULT NULL,
  RV tinyint(255) UNSIGNED DEFAULT 0,
  PRIMARY KEY (`key`)
)
ENGINE = INNODB,
AUTO_INCREMENT = 138,
AVG_ROW_LENGTH = 130,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать индекс `FK_listpo_orgtexandhardware_id` для объекта типа таблица `licenses`
--
ALTER TABLE licenses
ADD INDEX FK_listpo_orgtexandhardware_id (CodDevice);

--
-- Создать индекс `FKWorkerPlaceListPO` для объекта типа таблица `licenses`
--
ALTER TABLE licenses
ADD INDEX FKWorkerPlaceListPO (CodWorkerPlace);

--
-- Создать внешний ключ
--
ALTER TABLE licenses
ADD CONSTRAINT FKPO FOREIGN KEY (CodPO)
REFERENCES po (codpo) ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE licenses
ADD CONSTRAINT FK_licenses_device_id FOREIGN KEY (CodDevice)
REFERENCES device (id) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE licenses
ADD CONSTRAINT FK_listpo_provider_CodProvider FOREIGN KEY (CodProvider)
REFERENCES provider (codprovider) ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE licenses
ADD CONSTRAINT FK_listpo_statePO_CodStatePO FOREIGN KEY (CodStatePO)
REFERENCES statepo (codstatepo) ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE licenses
ADD CONSTRAINT FK_listpo_typelicense_CodTypeLicense FOREIGN KEY (CodTypeLicense)
REFERENCES typelicense (codtypelicense) ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE licenses
ADD CONSTRAINT FK_listpo_workerplace_CodWorkerPlace FOREIGN KEY (CodWorkerPlace)
REFERENCES workerplace (codworkerplace) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать таблицу `logintype`
--
CREATE TABLE logintype (
  CodLoginType int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name varchar(50) NOT NULL,
  PRIMARY KEY (CodLoginType)
)
ENGINE = INNODB,
AUTO_INCREMENT = 8,
AVG_ROW_LENGTH = 16384,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать индекс `Name` для объекта типа таблица `logintype`
--
ALTER TABLE logintype
ADD UNIQUE INDEX Name (Name);

--
-- Создать таблицу `loginpass`
--
CREATE TABLE loginpass (
  `key` int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodLoginPass int(11) UNSIGNED NOT NULL,
  CodLoginType int(11) UNSIGNED NOT NULL,
  Login varchar(20) NOT NULL,
  Pass varchar(20) DEFAULT NULL,
  Note varchar(50) DEFAULT NULL,
  PRIMARY KEY (`key`)
)
ENGINE = INNODB,
AUTO_INCREMENT = 8,
AVG_ROW_LENGTH = 16384,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать внешний ключ
--
ALTER TABLE loginpass
ADD CONSTRAINT FKCodLoginType FOREIGN KEY (CodLoginType)
REFERENCES logintype (codlogintype) ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE loginpass
ADD CONSTRAINT FKCodUser FOREIGN KEY (CodLoginPass)
REFERENCES users (coduser) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать таблицу `typenumber`
--
CREATE TABLE typenumber (
  CodTypeNumber int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name varchar(50) NOT NULL,
  PRIMARY KEY (CodTypeNumber)
)
ENGINE = INNODB,
AUTO_INCREMENT = 5,
AVG_ROW_LENGTH = 4096,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать индекс `Name` для объекта типа таблица `typenumber`
--
ALTER TABLE typenumber
ADD UNIQUE INDEX Name (Name);

--
-- Создать таблицу `listnumber`
--
CREATE TABLE listnumber (
  `key` int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodUser int(11) UNSIGNED NOT NULL,
  CodTypeNumber int(11) UNSIGNED NOT NULL,
  Number varchar(20) NOT NULL,
  Note varchar(50) DEFAULT NULL,
  PRIMARY KEY (`key`)
)
ENGINE = INNODB,
AUTO_INCREMENT = 3,
AVG_ROW_LENGTH = 16384,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать внешний ключ
--
ALTER TABLE listnumber
ADD CONSTRAINT FKTypeNumber FOREIGN KEY (CodTypeNumber)
REFERENCES typenumber (codtypenumber) ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE listnumber
ADD CONSTRAINT FKUsers FOREIGN KEY (CodUser)
REFERENCES users (coduser) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать таблицу `cause`
--
CREATE TABLE cause (
  CodCause int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name varchar(50) NOT NULL,
  PRIMARY KEY (CodCause)
)
ENGINE = INNODB,
AUTO_INCREMENT = 15,
AVG_ROW_LENGTH = 1092,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать индекс `Name` для объекта типа таблица `cause`
--
ALTER TABLE cause
ADD UNIQUE INDEX Name (Name);

--
-- Создать таблицу `historymoved`
--
CREATE TABLE historymoved (
  id int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodMovedDevice int(11) UNSIGNED DEFAULT NULL,
  CodMovedLicense int(11) UNSIGNED DEFAULT NULL,
  DateMoved datetime NOT NULL,
  OldPlace varchar(255) NOT NULL,
  NewPlace varchar(255) NOT NULL,
  CodCause int(11) UNSIGNED NOT NULL,
  CodPerformer int(11) UNSIGNED DEFAULT NULL,
  Note varchar(255) DEFAULT NULL,
  TypeHistory tinyint(1) NOT NULL DEFAULT 0,
  InventoryN varchar(50) DEFAULT NULL,
  CodOldPlace int(11) UNSIGNED DEFAULT NULL,
  CodNewPlace int(11) UNSIGNED DEFAULT NULL,
  PRIMARY KEY (id)
)
ENGINE = INNODB,
AUTO_INCREMENT = 134,
AVG_ROW_LENGTH = 358,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать внешний ключ
--
ALTER TABLE historymoved
ADD CONSTRAINT FK_historydevicemoved_cause_CodCause FOREIGN KEY (CodCause)
REFERENCES cause (codcause) ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE historymoved
ADD CONSTRAINT FK_historymoved_device_id FOREIGN KEY (CodMovedDevice)
REFERENCES device (id) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE historymoved
ADD CONSTRAINT FK_historymoved_licenses_key FOREIGN KEY (CodMovedLicense)
REFERENCES licenses (`key`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE historymoved
ADD CONSTRAINT FK_historymoved_users_CodUser FOREIGN KEY (CodPerformer)
REFERENCES users (coduser) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE historymoved
ADD CONSTRAINT FK_historymoved_workerplace_CodWorkerPlace FOREIGN KEY (CodOldPlace)
REFERENCES workerplace (codworkerplace) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Создать внешний ключ
--
ALTER TABLE historymoved
ADD CONSTRAINT FK_historymoved_workerplace_CodWorkerPlace2 FOREIGN KEY (CodNewPlace)
REFERENCES workerplace (codworkerplace) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Создать таблицу `tree`
--
CREATE TABLE tree (
  id int(11) UNSIGNED NOT NULL,
  parent_id int(11) UNSIGNED NOT NULL,
  level int(11) UNSIGNED NOT NULL
)
ENGINE = INNODB,
AVG_ROW_LENGTH = 224,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать внешний ключ
--
ALTER TABLE tree
ADD CONSTRAINT FKDepartments FOREIGN KEY (id)
REFERENCES departments (id) ON DELETE CASCADE ON UPDATE CASCADE;

DELIMITER $$

--
-- Создать триггер `NodeInsert`
--
CREATE
DEFINER = 'root'@'%'
TRIGGER NodeInsert
AFTER INSERT
ON departments
FOR EACH ROW
BEGIN
  INSERT INTO tree
  SET id = new.id,
      parent_id = new.id,
      level = 0;
  INSERT INTO tree
    SELECT
      n.id,
      t.parent_id,
      t.level + 1
    FROM (SELECT
             id,
             parent_id
           FROM departments
           WHERE id = new.id) n,
         tree t
    WHERE n.parent_id = t.id;
END
$$

--
-- Создать триггер `NodeUpdate`
--
CREATE
DEFINER = 'root'@'%'
TRIGGER NodeUpdate
AFTER UPDATE
ON departments
FOR EACH ROW
BEGIN
  IF NEW.parent_id != OLD.parent_id THEN
    DROP TEMPORARY TABLE IF EXISTS child;
    CREATE TEMPORARY TABLE child (
      id int,
      level int
    );

    INSERT INTO child
      SELECT
        t.id,
        t.level
      FROM tree t
      WHERE new.id = t.parent_id
      AND t.level > 0;

    DELETE
      FROM tree
    WHERE id IN (SELECT
          id
        FROM child)
      AND parent_id IN (SELECT
          t.parent_id
        FROM (SELECT
            *
          FROM tree) AS t
        WHERE new.id = t.id
        AND t.level > 0);

    DELETE
      FROM tree
    WHERE id = NEW.id
      AND level > 0;

    INSERT INTO tree
      SELECT
        n.id,
        t.parent_id,
        t.level + 1
      FROM (SELECT
               id,
               parent_id
             FROM departments
             WHERE id = new.id) n,
           tree t
      WHERE n.parent_id = t.id;

    INSERT INTO tree
      SELECT
        c.id,
        t.parent_id,
        t.level + c.level
      FROM tree t,
           child c
      WHERE new.id = t.id
      AND t.level > 0;

    DROP TEMPORARY TABLE child;
  END IF;
END
$$

DELIMITER ;

--
-- Создать таблицу `potree`
--
CREATE TABLE potree (
  id int(11) UNSIGNED NOT NULL,
  parent_id int(11) UNSIGNED NOT NULL,
  level int(11) UNSIGNED NOT NULL
)
ENGINE = INNODB,
AVG_ROW_LENGTH = 372,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать внешний ключ
--
ALTER TABLE potree
ADD CONSTRAINT FK_potree_po_CodPO FOREIGN KEY (id)
REFERENCES po (codpo) ON DELETE CASCADE ON UPDATE CASCADE;

DELIMITER $$

--
-- Создать триггер `NodeInsertPo`
--
CREATE
DEFINER = 'root'@'%'
TRIGGER NodeInsertPo
AFTER INSERT
ON po
FOR EACH ROW
BEGIN
  INSERT INTO potree
  SET id = new.CodPO,
      parent_id = new.CodPO,
      level = 0;
  INSERT INTO potree
    SELECT
      n.CodPO,
      t.parent_id,
      t.level + 1
    FROM (SELECT
             CodPO,
             CodGroupPO
           FROM po
           WHERE CodPO = new.CodPO) n,
         potree t
    WHERE n.CodGroupPO = t.id;
END
$$

--
-- Создать триггер `NodeUpdatePo`
--
CREATE
DEFINER = 'root'@'%'
TRIGGER NodeUpdatePo
AFTER UPDATE
ON po
FOR EACH ROW
BEGIN
  IF NEW.CodGroupPO != OLD.CodGroupPO THEN
    DROP TEMPORARY TABLE IF EXISTS childpo;
    CREATE TEMPORARY TABLE childpo (
      id int,
      level int
    );

    INSERT INTO childpo
      SELECT
        t.id,
        t.level
      FROM potree t
      WHERE new.CodPO = t.parent_id
      AND t.level > 0;

    DELETE
      FROM potree
    WHERE id IN (SELECT
          id
        FROM childpo)
      AND parent_id IN (SELECT
          t.parent_id
        FROM (SELECT
            *
          FROM potree) AS t
        WHERE new.CodPO = t.id
        AND t.level > 0);

    DELETE
      FROM potree
    WHERE id = NEW.CodPO
      AND level > 0;

    INSERT INTO potree
      SELECT
        n.CodPO,
        t.parent_id,
        t.level + 1
      FROM (SELECT
               CodPO,
               CodGroupPO
             FROM po
             WHERE CodPO = new.CodPO) n,
           potree t
      WHERE n.CodGroupPO = t.id;

    INSERT INTO potree
      SELECT
        c.id,
        t.parent_id,
        t.level + c.level
      FROM potree t,
           childpo c
      WHERE new.CodPO = t.id
      AND t.level > 0;

    DROP TEMPORARY TABLE childpo;
  END IF;
END
$$

DELIMITER ;

--
-- Создать таблицу `lockreferencebook`
--
CREATE TABLE lockreferencebook (
  id int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name varchar(20) NOT NULL,
  LockedUp timestamp NULL DEFAULT NULL,
  LockedBy varchar(20) DEFAULT NULL,
  PRIMARY KEY (id)
)
ENGINE = INNODB,
AUTO_INCREMENT = 33,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать таблицу `gate`
--
CREATE TABLE gate (
  CodGate int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name varchar(15) NOT NULL,
  PRIMARY KEY (CodGate)
)
ENGINE = INNODB,
AVG_ROW_LENGTH = 16384,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать индекс `Name` для объекта типа таблица `gate`
--
ALTER TABLE gate
ADD UNIQUE INDEX Name (Name);

--
-- Создать таблицу `devtree`
--
CREATE TABLE devtree (
  id int(11) UNSIGNED NOT NULL,
  parent_id int(11) UNSIGNED NOT NULL,
  level int(11) NOT NULL
)
ENGINE = INNODB,
AVG_ROW_LENGTH = 50,
CHARACTER SET utf8,
COLLATE utf8_general_ci;

--
-- Создать внешний ключ
--
ALTER TABLE devtree
ADD CONSTRAINT FK_devtree_device_id FOREIGN KEY (id)
REFERENCES device (id) ON DELETE CASCADE ON UPDATE CASCADE;

DELIMITER $$

--
-- Создать триггер `NodeInsertDevice`
--
CREATE
DEFINER = 'root'@'%'
TRIGGER NodeInsertDevice
AFTER INSERT
ON device
FOR EACH ROW
BEGIN
  INSERT INTO devtree
  SET id = new.id,
      parent_id = new.id,
      level = 0;
  INSERT INTO devtree
    SELECT
      n.id,
      t.parent_id,
      t.level + 1
    FROM (SELECT
             id,
             parent_id
           FROM device
           WHERE id = new.id) n,
         devtree t
    WHERE n.parent_id = t.id;
END
$$

--
-- Создать триггер `NodeUpdateDevice`
--
CREATE
DEFINER = 'root'@'%'
TRIGGER NodeUpdateDevice
AFTER UPDATE
ON device
FOR EACH ROW
BEGIN
  IF NEW.parent_id != OLD.parent_id THEN
    DROP TEMPORARY TABLE IF EXISTS childdev;
    CREATE TEMPORARY TABLE childdev (
      id int,
      level int
    );

    INSERT INTO childdev
      SELECT
        t.id,
        t.level
      FROM devtree t
      WHERE new.id = t.parent_id
      AND t.level > 0;

    DELETE
      FROM devtree
    WHERE id IN (SELECT
          id
        FROM childdev)
      AND parent_id IN (SELECT
          t.parent_id
        FROM (SELECT
            *
          FROM devtree) AS t
        WHERE new.id = t.id
        AND t.level > 0);

    DELETE
      FROM devtree
    WHERE id = NEW.id
      AND level > 0;

    INSERT INTO devtree
      SELECT
        n.id,
        t.parent_id,
        t.level + 1
      FROM (SELECT
               id,
               parent_id
             FROM device
             WHERE id = new.id) n,
           devtree t
      WHERE n.parent_id = t.id;

    INSERT INTO devtree
      SELECT
        c.id,
        t.parent_id,
        t.level + c.level
      FROM devtree t,
           childdev c
      WHERE new.id = t.id
      AND t.level > 0;

    DROP TEMPORARY TABLE childdev;
  END IF;
END
$$

DELIMITER ;

-- 
-- Восстановить предыдущий режим SQL (SQL mode)
-- 
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;

-- 
-- Включение внешних ключей
-- 
/*!40014 SET FOREIGN_KEY_CHECKS = @OLD_FOREIGN_KEY_CHECKS */;