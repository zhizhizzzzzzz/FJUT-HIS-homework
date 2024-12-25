DROP SCHEMA IF EXISTS `payrolldb`;
CREATE SCHEMA `payrolldb`;
USE `payrolldb`;

-- Table structure for table `Company`
DROP TABLE IF EXISTS `company`;
CREATE TABLE `company` (
  `department` varchar(255) DEFAULT NULL,
  `dept_short` varchar(255) DEFAULT NULL,
  `designation` varchar(255) DEFAULT NULL,
  `design_short` varchar(255) DEFAULT NULL,
  `salary` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Dumping data for table `Company`
LOCK TABLES `company` WRITE;
INSERT INTO `company` (`department`, `dept_short`, `designation`, `design_short`, `salary`) VALUES
('Human Resources', 'HR', 'Manager', 'Mgr', '5000'),
('Human Resources', 'HR', 'Cleaner', 'Clr', '1000'),
('Information Technology', 'IT', 'Developer', 'Dev', '4000'),
('Finance', 'Fin', 'Analyst', 'Anly', '4500'),
('Marketing', 'Mkt', 'Director', 'Dir', '6000'),
('Sales', 'Sales', 'Representative', 'Rep', '3500'),
('Research and Development', 'R&D', 'Scientist', 'Sci', '5500'),
('Customer Support', 'CS', 'Agent', 'Ag', '3000'),
('Operations', 'Ops', 'Coordinator', 'Coord', '3700');
UNLOCK TABLES;

-- Table structure for table `Employee`
DROP TABLE IF EXISTS `employee`;
CREATE TABLE `employee` (
  `id` INT PRIMARY KEY AUTO_INCREMENT,
  `emp_id` VARCHAR(255) DEFAULT NULL,
  `name` VARCHAR(255) DEFAULT NULL,
  `dob` DATE DEFAULT NULL, -- Date of birth yyyy-mm-dd
  `gender` VARCHAR(255) DEFAULT NULL,
  `father` VARCHAR(255) DEFAULT NULL,
  `email` VARCHAR(255) DEFAULT NULL,
  `phone` VARCHAR(255) DEFAULT NULL,
  `address` VARCHAR(255) DEFAULT NULL,
  `department` VARCHAR(255) DEFAULT NULL, -- Department
  `designation` VARCHAR(255) DEFAULT NULL, -- Designation
  `doj` DATE DEFAULT NULL, -- Date of Joining yyyy-mm-dd
  `salary` DECIMAL(10,2) DEFAULT NULL,
  `type` VARCHAR(255) DEFAULT NULL -- Fresher or Experienced
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Dumping data for table `Employee`
LOCK TABLES `employee` WRITE;
INSERT INTO `employee` (`emp_id`, `name`, `dob`, `gender`, `father`, `email`, `phone`, `address`, `department`, `designation`, `doj`, `salary`, `type`) VALUES
('HR/Mgr/1', 'John Doe', '1980-01-01', 'Male', 'Richard Doe', 'john.doe@example.com', '1234567890', '123 Main St', 'HR', 'Mgr', '2000-01-01', '5000.00', 'Experienced'),
('IT/Dev/2', 'Jane Smith', '1985-05-10', 'Female', 'Robert Smith', 'jane.smith@example.com', '0987654321', '456 Elm St', 'IT', 'Dev', '2010-05-10', '4000.00', 'Experienced'),
('Fin/Anly/3', 'Alice Brown', '1990-07-15', 'Female', 'Thomas Brown', 'alice.brown@example.com', '1122334455', '789 Oak St', 'Fin', 'Anly', '2015-07-15', '4500.00', 'Experienced'),
('Mkt/Dir/4', 'Michael Lee', '1975-02-25', 'Male', 'George Lee', 'michael.lee@example.com', '4455667788', '101 Pine St', 'Mkt', 'Dir', '2010-02-25', '6000.00', 'Experienced'),
('Sales/Rep/5', 'Emily Clark', '1992-11-15', 'Female', 'David Clark', 'emily.clark@example.com', '5566778899', '202 Birch St', 'Sales', 'Rep', '2018-11-15', '3500.00', 'Fresher'),
('R&D/Sci/6', 'James White', '1987-06-30', 'Male', 'Frank White', 'james.white@example.com', '6677889900', '303 Cedar St', 'R&D', 'Sci', '2013-06-30', '5500.00', 'Experienced'),
('CS/Ag/7', 'Sophie Turner', '1995-04-20', 'Female', 'Henry Turner', 'sophie.turner@example.com', '7788990011', '404 Spruce St', 'CS', 'Ag', '2020-04-20', '3000.00', 'Fresher'),
('Ops/Coord/8', 'Daniel Kim', '1983-08-12', 'Male', 'James Kim', 'daniel.kim@example.com', '8899001122', '505 Maple St', 'Ops', 'Coord', '2008-08-12', '3700.00', 'Experienced');
UNLOCK TABLES;