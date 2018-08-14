-- Скрипт сгенерирован Devart dbForge Studio for MySQL, Версия 5.0.76.1
-- Домашняя страница продукта: http://www.devart.com/ru/dbforge/mysql/studio
-- Дата скрипта: 18.06.2012 16:48:34
-- Версия сервера: 5.5.17
-- Версия клиента: 4.1

-- 
-- Отключение внешних ключей
-- 
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;

-- 
-- Установка кодировки, с использованием которой клиент будет посылать запросы на сервер
--
SET NAMES 'utf8';

--
-- Описание для базы данных proba
--
DROP DATABASE IF EXISTS proba;
CREATE DATABASE IF NOT EXISTS proba
	CHARACTER SET utf8
	COLLATE utf8_general_ci;

-- 
-- Установка базы данных по умолчанию
--
USE proba;

--
-- Описание для таблицы cause
--
CREATE TABLE IF NOT EXISTS cause (
  CodCause INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name VARCHAR(50) NOT NULL,
  PRIMARY KEY (CodCause),
  UNIQUE INDEX Name (Name)
)
ENGINE = INNODB
AUTO_INCREMENT = 2
AVG_ROW_LENGTH = 16384
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы departments
--
CREATE TABLE IF NOT EXISTS departments (
  id INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  parent_id INT(11) UNSIGNED NOT NULL DEFAULT 0,
  Name VARCHAR(100) NOT NULL,
  Firm TINYINT(1) NOT NULL DEFAULT 0,
  FP TINYINT(1) NOT NULL DEFAULT 0,
  PRIMARY KEY (id)
)
ENGINE = INNODB
AUTO_INCREMENT = 103
AVG_ROW_LENGTH = 564
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы domainwg
--
CREATE TABLE IF NOT EXISTS domainwg (
  CodDomainWg INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name VARCHAR(15) NOT NULL,
  PRIMARY KEY (CodDomainWg),
  UNIQUE INDEX Name (Name)
)
ENGINE = INNODB
AUTO_INCREMENT = 6
AVG_ROW_LENGTH = 4096
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы gate
--
CREATE TABLE IF NOT EXISTS gate (
  CodGate INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name VARCHAR(15) NOT NULL,
  PRIMARY KEY (CodGate),
  UNIQUE INDEX Name (Name)
)
ENGINE = INNODB
AUTO_INCREMENT = 2
AVG_ROW_LENGTH = 16384
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы logintype
--
CREATE TABLE IF NOT EXISTS logintype (
  CodLoginType INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name VARCHAR(50) NOT NULL,
  PRIMARY KEY (CodLoginType),
  UNIQUE INDEX Name (Name)
)
ENGINE = INNODB
AUTO_INCREMENT = 8
AVG_ROW_LENGTH = 16384
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы post
--
CREATE TABLE IF NOT EXISTS post (
  CodPost INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name VARCHAR(255) NOT NULL,
  PRIMARY KEY (CodPost),
  UNIQUE INDEX Name (Name)
)
ENGINE = INNODB
AUTO_INCREMENT = 92
AVG_ROW_LENGTH = 207
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы producer
--
CREATE TABLE IF NOT EXISTS producer (
  CodProducer INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name VARCHAR(100) NOT NULL,
  www VARCHAR(255) DEFAULT NULL,
  Note VARCHAR(255) DEFAULT NULL,
  PRIMARY KEY (CodProducer)
)
ENGINE = INNODB
AUTO_INCREMENT = 34
AVG_ROW_LENGTH = 512
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы provider
--
CREATE TABLE IF NOT EXISTS provider (
  CodProvider INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name VARCHAR(100) NOT NULL,
  Adress VARCHAR(150) DEFAULT NULL,
  Fax VARCHAR(20) DEFAULT NULL,
  Phone VARCHAR(20) DEFAULT NULL,
  email VARCHAR(100) DEFAULT NULL,
  www VARCHAR(255) DEFAULT NULL,
  PRIMARY KEY (CodProvider)
)
ENGINE = INNODB
AUTO_INCREMENT = 5
AVG_ROW_LENGTH = 4096
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы statedev
--
CREATE TABLE IF NOT EXISTS statedev (
  CodState INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name VARCHAR(20) NOT NULL,
  PRIMARY KEY (CodState),
  UNIQUE INDEX Name (Name)
)
ENGINE = INNODB
AUTO_INCREMENT = 7
AVG_ROW_LENGTH = 2730
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы statepo
--
CREATE TABLE IF NOT EXISTS statepo (
  CodStatePO INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name VARCHAR(100) NOT NULL,
  PRIMARY KEY (CodStatePO)
)
ENGINE = INNODB
AUTO_INCREMENT = 4
AVG_ROW_LENGTH = 5461
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы typedevice
--
CREATE TABLE IF NOT EXISTS typedevice (
  CodTypeDevice INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  IconPath VARCHAR(255) DEFAULT NULL,
  Name VARCHAR(100) NOT NULL,
  Type TINYINT(1) NOT NULL,
  PRIMARY KEY (CodTypeDevice)
)
ENGINE = INNODB
AUTO_INCREMENT = 25
AVG_ROW_LENGTH = 682
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы typelicense
--
CREATE TABLE IF NOT EXISTS typelicense (
  CodTypeLicense INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name VARCHAR(100) NOT NULL,
  PRIMARY KEY (CodTypeLicense)
)
ENGINE = INNODB
AUTO_INCREMENT = 5
AVG_ROW_LENGTH = 4096
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы typenumber
--
CREATE TABLE IF NOT EXISTS typenumber (
  CodTypeNumber INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  Name VARCHAR(50) NOT NULL,
  PRIMARY KEY (CodTypeNumber),
  UNIQUE INDEX Name (Name)
)
ENGINE = INNODB
AUTO_INCREMENT = 5
AVG_ROW_LENGTH = 4096
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы po
--
CREATE TABLE IF NOT EXISTS po (
  CodPO INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodGroupPO INT(11) UNSIGNED NOT NULL DEFAULT 0,
  Name VARCHAR(100) NOT NULL,
  isGroup TINYINT(1) NOT NULL DEFAULT 0,
  CodProducer INT(11) UNSIGNED DEFAULT NULL,
  Note VARCHAR(255) DEFAULT NULL,
  Ico VARCHAR(255) DEFAULT NULL,
  PRIMARY KEY (CodPO),
  INDEX FK_po_typePO_CodTypePO (CodGroupPO),
  CONSTRAINT FK_po_producer_CodProducer FOREIGN KEY (CodProducer)
    REFERENCES producer(CodProducer) ON DELETE RESTRICT ON UPDATE CASCADE
)
ENGINE = INNODB
AUTO_INCREMENT = 68
AVG_ROW_LENGTH = 963
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы tree
--
CREATE TABLE IF NOT EXISTS tree (
  id INT(11) UNSIGNED NOT NULL,
  parent_id INT(11) UNSIGNED NOT NULL,
  level INT(11) UNSIGNED NOT NULL,
  CONSTRAINT FKDepartments FOREIGN KEY (id)
    REFERENCES departments(id) ON DELETE CASCADE ON UPDATE CASCADE
)
ENGINE = INNODB
AVG_ROW_LENGTH = 292
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы users
--
CREATE TABLE IF NOT EXISTS users (
  CodUser INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodAD INT(8) DEFAULT NULL,
  LastName VARCHAR(50) DEFAULT NULL,
  Name VARCHAR(50) DEFAULT NULL,
  MiddleName VARCHAR(50) DEFAULT NULL,
  FIOSummary VARCHAR(50) NOT NULL,
  CodPost INT(11) UNSIGNED NOT NULL,
  email VARCHAR(50) DEFAULT NULL,
  additionalemail VARCHAR(50) DEFAULT NULL,
  CodOrganization INT(11) UNSIGNED NOT NULL,
  PRIMARY KEY (CodUser),
  UNIQUE INDEX CodAD (CodAD),
  CONSTRAINT FKOrganization FOREIGN KEY (CodOrganization)
    REFERENCES departments(id) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT FKPost FOREIGN KEY (CodPost)
    REFERENCES post(CodPost) ON DELETE RESTRICT ON UPDATE CASCADE
)
ENGINE = INNODB
AUTO_INCREMENT = 132
AVG_ROW_LENGTH = 144
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы listnumber
--
CREATE TABLE IF NOT EXISTS listnumber (
  `key` INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodUser INT(11) UNSIGNED NOT NULL,
  CodTypeNumber INT(11) UNSIGNED NOT NULL,
  Number VARCHAR(20) NOT NULL,
  Note VARCHAR(50) DEFAULT NULL,
  PRIMARY KEY (`key`),
  CONSTRAINT FKTypeNumber FOREIGN KEY (CodTypeNumber)
    REFERENCES typenumber(CodTypeNumber) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT FKUsers FOREIGN KEY (CodUser)
    REFERENCES users(CodUser) ON DELETE CASCADE ON UPDATE CASCADE
)
ENGINE = INNODB
AUTO_INCREMENT = 1
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы loginpass
--
CREATE TABLE IF NOT EXISTS loginpass (
  `key` INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodLoginPass INT(11) UNSIGNED NOT NULL,
  CodLoginType INT(11) UNSIGNED NOT NULL,
  Login VARCHAR(20) NOT NULL,
  Pass VARCHAR(20) DEFAULT NULL,
  Note VARCHAR(50) DEFAULT NULL,
  PRIMARY KEY (`key`),
  CONSTRAINT FKCodLoginType FOREIGN KEY (CodLoginType)
    REFERENCES logintype(CodLoginType) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT FKCodUser FOREIGN KEY (CodLoginPass)
    REFERENCES users(CodUser) ON DELETE CASCADE ON UPDATE CASCADE
)
ENGINE = INNODB
AUTO_INCREMENT = 1
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы potree
--
CREATE TABLE IF NOT EXISTS potree (
  id INT(11) UNSIGNED NOT NULL,
  parent_id INT(11) UNSIGNED NOT NULL,
  level INT(11) UNSIGNED NOT NULL,
  CONSTRAINT FK_potree_po_CodPO FOREIGN KEY (id)
    REFERENCES po(CodPO) ON DELETE CASCADE ON UPDATE CASCADE
)
ENGINE = INNODB
AVG_ROW_LENGTH = 455
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы tounits
--
CREATE TABLE IF NOT EXISTS tounits (
  CodUser INT(11) UNSIGNED NOT NULL,
  CodFilPred INT(11) UNSIGNED DEFAULT NULL,
  CodDepartment INT(11) UNSIGNED DEFAULT NULL,
  UNIQUE INDEX CodUser (CodUser),
  CONSTRAINT FKToUnitDep FOREIGN KEY (CodDepartment)
    REFERENCES departments(id) ON DELETE SET NULL ON UPDATE CASCADE,
  CONSTRAINT FKToUnitFilPred FOREIGN KEY (CodFilPred)
    REFERENCES departments(id) ON DELETE SET NULL ON UPDATE CASCADE,
  CONSTRAINT FKToUnitUser FOREIGN KEY (CodUser)
    REFERENCES users(CodUser) ON DELETE CASCADE ON UPDATE CASCADE
)
ENGINE = INNODB
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы workerplace
--
CREATE TABLE IF NOT EXISTS workerplace (
  CodWorkerPlace INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodDepartment INT(11) UNSIGNED NOT NULL,
  Name VARCHAR(100) NOT NULL,
  PrimaryUser INT(11) UNSIGNED DEFAULT NULL,
  InternalNumber VARCHAR(12) DEFAULT NULL,
  Location VARCHAR(50) DEFAULT NULL,
  warehouse TINYINT(1) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (CodWorkerPlace),
  INDEX FKDivision (CodDepartment),
  CONSTRAINT FK_workerplace_users_CodUser FOREIGN KEY (PrimaryUser)
    REFERENCES users(CodUser) ON DELETE SET NULL ON UPDATE CASCADE,
  CONSTRAINT FKDepartment FOREIGN KEY (CodDepartment)
    REFERENCES departments(id) ON DELETE CASCADE ON UPDATE CASCADE
)
ENGINE = INNODB
AUTO_INCREMENT = 206
AVG_ROW_LENGTH = 131
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы device
--
CREATE TABLE IF NOT EXISTS device (
  id INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  parent_id INT(11) UNSIGNED NOT NULL DEFAULT 0,
  CodOrganization INT(11) UNSIGNED NOT NULL,
  CodWorkerPlace INT(11) UNSIGNED NOT NULL,
  CodTypeDevice INT(11) UNSIGNED NOT NULL,
  Name VARCHAR(150) NOT NULL,
  InventoryN VARCHAR(50) DEFAULT NULL,
  SerialN VARCHAR(50) DEFAULT NULL,
  CodProvider INT(11) UNSIGNED DEFAULT NULL,
  CodProducer INT(11) UNSIGNED DEFAULT NULL,
  DatePurchase DATE DEFAULT NULL,
  DatePosting DATE DEFAULT NULL,
  EndGuarantee DATE DEFAULT NULL,
  Price DOUBLE(9, 2) UNSIGNED DEFAULT NULL,
  CodState INT(11) UNSIGNED NOT NULL,
  Note VARCHAR(255) DEFAULT NULL,
  Type TINYINT(1) NOT NULL,
  PRIMARY KEY (id),
  CONSTRAINT FK_device_departments_id FOREIGN KEY (CodOrganization)
    REFERENCES departments(id) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT FK_device_producer_CodProducer FOREIGN KEY (CodProducer)
    REFERENCES producer(CodProducer) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT FK_device_provider_CodProvider FOREIGN KEY (CodProvider)
    REFERENCES provider(CodProvider) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT FK_device_statedev_CodState FOREIGN KEY (CodState)
    REFERENCES statedev(CodState) ON DELETE RESTRICT ON UPDATE RESTRICT,
  CONSTRAINT FK_device_typedevice_CodTypeDevice FOREIGN KEY (CodTypeDevice)
    REFERENCES typedevice(CodTypeDevice) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT FK_device_workerplace_CodWorkerPlace FOREIGN KEY (CodWorkerPlace)
    REFERENCES workerplace(CodWorkerPlace) ON DELETE CASCADE ON UPDATE CASCADE
)
ENGINE = INNODB
AUTO_INCREMENT = 1200
AVG_ROW_LENGTH = 129
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы networkdata
--
CREATE TABLE IF NOT EXISTS networkdata (
  CodNetworkData INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodWorkerPlace INT(11) UNSIGNED NOT NULL,
  Name VARCHAR(20) NOT NULL,
  CodDomainWg INT(11) UNSIGNED NOT NULL,
  PRIMARY KEY (CodNetworkData),
  INDEX FKMask (CodDomainWg),
  UNIQUE INDEX Name (Name),
  CONSTRAINT FK_networkdata_domainwg_CodDomainWg FOREIGN KEY (CodDomainWg)
    REFERENCES domainwg(CodDomainWg) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT FKWPNetwork FOREIGN KEY (CodWorkerPlace)
    REFERENCES workerplace(CodWorkerPlace) ON DELETE CASCADE ON UPDATE CASCADE
)
ENGINE = INNODB
AUTO_INCREMENT = 1
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы workplaceandusers
--
CREATE TABLE IF NOT EXISTS workplaceandusers (
  CodWorkerPlace INT(11) UNSIGNED NOT NULL,
  CodUser INT(11) UNSIGNED NOT NULL,
  CONSTRAINT FKfromUsers FOREIGN KEY (CodUser)
    REFERENCES users(CodUser) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT FKfromWP FOREIGN KEY (CodWorkerPlace)
    REFERENCES workerplace(CodWorkerPlace) ON DELETE CASCADE ON UPDATE CASCADE
)
ENGINE = INNODB
AVG_ROW_LENGTH = 174
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы devtree
--
CREATE TABLE IF NOT EXISTS devtree (
  id INT(11) UNSIGNED NOT NULL,
  parent_id INT(11) UNSIGNED NOT NULL,
  level INT(11) NOT NULL,
  CONSTRAINT FK_devtree_device_id FOREIGN KEY (id)
    REFERENCES device(id) ON DELETE CASCADE ON UPDATE CASCADE
)
ENGINE = INNODB
AVG_ROW_LENGTH = 57
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы licenses
--
CREATE TABLE IF NOT EXISTS licenses (
  `key` INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodDevice INT(11) UNSIGNED DEFAULT NULL,
  CodWorkerPlace INT(11) UNSIGNED NOT NULL,
  CodPO INT(11) UNSIGNED NOT NULL,
  RegName VARCHAR(100) DEFAULT NULL,
  RegKey VARCHAR(255) DEFAULT NULL,
  RegMail VARCHAR(100) DEFAULT NULL,
  KolLicense INT(11) NOT NULL DEFAULT 0,
  InvN VARCHAR(100) DEFAULT NULL,
  VersionN VARCHAR(100) DEFAULT NULL,
  CodProvider INT(11) UNSIGNED DEFAULT NULL,
  DatePurchase DATE DEFAULT NULL,
  DateEndLicense DATE DEFAULT NULL,
  Price DOUBLE(9, 2) UNSIGNED DEFAULT NULL,
  CodTypeLicense INT(11) UNSIGNED NOT NULL,
  CodStatePO INT(11) UNSIGNED NOT NULL,
  Note VARCHAR(255) DEFAULT NULL,
  PRIMARY KEY (`key`),
  INDEX FK_listpo_orgtexandhardware_id (CodDevice),
  INDEX FKWorkerPlaceListPO (CodWorkerPlace),
  CONSTRAINT FK_licenses_device_id FOREIGN KEY (CodDevice)
    REFERENCES device(id) ON DELETE SET NULL ON UPDATE CASCADE,
  CONSTRAINT FK_listpo_provider_CodProvider FOREIGN KEY (CodProvider)
    REFERENCES provider(CodProvider) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT FK_listpo_statePO_CodStatePO FOREIGN KEY (CodStatePO)
    REFERENCES statepo(CodStatePO) ON DELETE NO ACTION ON UPDATE CASCADE,
  CONSTRAINT FK_listpo_typelicense_CodTypeLicense FOREIGN KEY (CodTypeLicense)
    REFERENCES typelicense(CodTypeLicense) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT FK_listpo_workerplace_CodWorkerPlace FOREIGN KEY (CodWorkerPlace)
    REFERENCES workerplace(CodWorkerPlace) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT FKPO FOREIGN KEY (CodPO)
    REFERENCES po(CodPO) ON DELETE RESTRICT ON UPDATE CASCADE
)
ENGINE = INNODB
AUTO_INCREMENT = 103
AVG_ROW_LENGTH = 168
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы networkinterface
--
CREATE TABLE IF NOT EXISTS networkinterface (
  CodNetworkInterface INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodNetworkData INT(11) UNSIGNED NOT NULL,
  Name VARCHAR(100) NOT NULL,
  IP VARCHAR(15) DEFAULT NULL,
  Mask VARCHAR(15) DEFAULT NULL,
  Gate VARCHAR(15) DEFAULT NULL,
  DNS VARCHAR(100) DEFAULT NULL,
  WINS VARCHAR(100) DEFAULT NULL,
  AutoIP TINYINT(1) NOT NULL DEFAULT 1,
  AutoDNS TINYINT(1) NOT NULL DEFAULT 1,
  AutoWINS TINYINT(1) NOT NULL DEFAULT 1,
  PRIMARY KEY (CodNetworkInterface),
  CONSTRAINT FK_networkinterface_networkdata_CodNetworkData FOREIGN KEY (CodNetworkData)
    REFERENCES networkdata(CodNetworkData) ON DELETE CASCADE ON UPDATE CASCADE
)
ENGINE = INNODB
AUTO_INCREMENT = 1
CHARACTER SET utf8
COLLATE utf8_general_ci;

--
-- Описание для таблицы historymoved
--
CREATE TABLE IF NOT EXISTS historymoved (
  id INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  CodMovedDevice INT(11) UNSIGNED DEFAULT NULL,
  CodMovedLicense INT(11) UNSIGNED DEFAULT NULL,
  DateMoved DATETIME NOT NULL,
  OldPlace VARCHAR(255) NOT NULL,
  NewPlace VARCHAR(255) NOT NULL,
  CodCause INT(11) UNSIGNED NOT NULL,
  CodPerformer INT(11) UNSIGNED DEFAULT NULL,
  Note VARCHAR(255) DEFAULT NULL,
  TypeHistory TINYINT(1) NOT NULL DEFAULT 0,
  InventoryN VARCHAR(50) DEFAULT NULL,
  PRIMARY KEY (id),
  CONSTRAINT FK_historydevicemoved_cause_CodCause FOREIGN KEY (CodCause)
    REFERENCES cause(CodCause) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT FK_historymoved_device_id FOREIGN KEY (CodMovedDevice)
    REFERENCES device(id) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT FK_historymoved_licenses_key FOREIGN KEY (CodMovedLicense)
    REFERENCES licenses(`key`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT FK_historymoved_users_CodUser FOREIGN KEY (CodPerformer)
    REFERENCES users(CodUser) ON DELETE SET NULL ON UPDATE CASCADE
)
ENGINE = INNODB
AUTO_INCREMENT = 7
AVG_ROW_LENGTH = 2730
CHARACTER SET utf8
COLLATE utf8_general_ci;

DELIMITER $$

--
-- Описание для триггера NodeInsert
--
CREATE 
	DEFINER = 'root'@'%'
TRIGGER NodeInsert
	AFTER INSERT
	ON departments
	FOR EACH ROW
BEGIN
  INSERT INTO tree
  SET
    id = new.id, parent_id = new.id, level = 0;
  INSERT INTO tree
  SELECT
    n.id, t.parent_id, t.level + 1
  FROM
    (SELECT
      id, parent_id
    FROM
      departments
    WHERE
      id = new.id) n
    , tree t
  WHERE
    n.parent_id = t.id;
END
$$

--
-- Описание для триггера NodeUpdate
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
    CREATE TEMPORARY TABLE child(
      id INT,
      level INT
    );

    INSERT INTO child
    SELECT
      t.id, t.level
    FROM
      tree t
    WHERE
      new.id = t.parent_id AND t.level > 0;

    DELETE
    FROM
      tree
    WHERE
      id IN
      (SELECT
        id
      FROM
        child)
      AND parent_id IN
      (SELECT
        t.parent_id
      FROM
        (SELECT
          *
        FROM
          tree) AS t
      WHERE
        new.id = t.id AND t.level > 0);

    DELETE
    FROM
      tree
    WHERE
      id = NEW.id AND level > 0;

    INSERT INTO tree
    SELECT
      n.id, t.parent_id, t.level + 1
    FROM
      (SELECT
        id, parent_id
      FROM
        departments
      WHERE
        id = new.id) n
      , tree t
    WHERE
      n.parent_id = t.id;

    INSERT INTO tree
    SELECT
      c.id, t.parent_id, t.level + c.level
    FROM
      tree t, child c
    WHERE
      new.id = t.id AND t.level > 0;

    DROP TEMPORARY TABLE child;
  END IF;
END
$$

--
-- Описание для триггера NodeInsertDevice
--
CREATE 
	DEFINER = 'root'@'%'
TRIGGER NodeInsertDevice
	AFTER INSERT
	ON device
	FOR EACH ROW
BEGIN
  INSERT INTO devtree
SET
  id = new.id, parent_id = new.id, level = 0;
  INSERT INTO devtree
SELECT n.id
     , t.parent_id
     , t.level + 1
FROM
  (SELECT id
        , parent_id
   FROM
     device
   WHERE
     id = new.id) n
  , devtree t
WHERE
  n.parent_id = t.id;
END
$$

--
-- Описание для триггера NodeUpdateDevice
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
    CREATE TEMPORARY TABLE childdev(
  id INT,
  level INT
);

    INSERT INTO childdev
SELECT t.id
     , t.level
FROM
  devtree t
WHERE
  new.id = t.parent_id
  AND t.level > 0;

    DELETE
FROM
  devtree
WHERE
  id IN
  (SELECT id
   FROM
     childdev)
  AND parent_id IN
  (SELECT t.parent_id
   FROM
     (SELECT *
      FROM
        devtree) AS t
   WHERE
     new.id = t.id
     AND t.level > 0);

    DELETE
FROM
  devtree
WHERE
  id = NEW.id
  AND level > 0;

    INSERT INTO devtree
SELECT n.id
     , t.parent_id
     , t.level + 1
FROM
  (SELECT id
        , parent_id
   FROM
     device
   WHERE
     id = new.id) n
  , devtree t
WHERE
  n.parent_id = t.id;

    INSERT INTO devtree
SELECT c.id
     , t.parent_id
     , t.level + c.level
FROM
  devtree t, childdev c
WHERE
  new.id = t.id
  AND t.level > 0;

    DROP TEMPORARY TABLE childdev;
  END IF;
END
$$

--
-- Описание для триггера NodeInsertPo
--
CREATE 
	DEFINER = 'root'@'%'
TRIGGER NodeInsertPo
	AFTER INSERT
	ON po
	FOR EACH ROW
BEGIN
  INSERT INTO potree
  SET
    id = new.CodPO, parent_id = new.CodPO, level = 0;
  INSERT INTO potree
  SELECT
    n.CodPO, t.parent_id, t.level + 1
  FROM
    (SELECT
      CodPO, CodGroupPO
    FROM
      po
    WHERE
      CodPO = new.CodPO) n
    , potree t
  WHERE
    n.CodGroupPO = t.id;
END
$$

--
-- Описание для триггера NodeUpdatePo
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
    CREATE TEMPORARY TABLE childpo(
      id INT,
      level INT
    );

    INSERT INTO childpo
    SELECT
      t.id, t.level
    FROM
      potree t
    WHERE
      new.CodPO = t.parent_id AND t.level > 0;

    DELETE
    FROM
      potree
    WHERE
      id IN
      (SELECT
        id
      FROM
        childpo)
      AND parent_id IN
      (SELECT
        t.parent_id
      FROM
        (SELECT
          *
        FROM
          potree) AS t
      WHERE
        new.CodPO = t.id AND t.level > 0);

    DELETE
    FROM
      potree
    WHERE
      id = NEW.CodPO AND level > 0;

    INSERT INTO potree
    SELECT
      n.CodPO, t.parent_id, t.level + 1
    FROM
      (SELECT
        CodPO, CodGroupPO
      FROM
        po
      WHERE
        CodPO = new.CodPO) n
      , potree t
    WHERE
      n.CodGroupPO = t.id;

    INSERT INTO potree
    SELECT
      c.id, t.parent_id, t.level + c.level
    FROM
      potree t, childpo c
    WHERE
      new.CodPO = t.id AND t.level > 0;

    DROP TEMPORARY TABLE childpo;
  END IF;
END
$$

DELIMITER ;

-- 
-- Включение внешних ключей
-- 
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;