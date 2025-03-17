create database stuManager;
use stuManager;

create table student(
    usernamer varchar(20) primary key,
    pass varchar(20),
    real_name nvarchar(255),
    email varchar(255),
    phone varchar(15),
    avatar varchar(100)
);

create table teacher(
    usernamer varchar(20) primary key,
    pass varchar(20),
    real_name nvarchar(255),
    email varchar(255),
    phone varchar(15),
    avatar varchar(100)
);


insert into student values('student1','123456a@A')
insert into student values('student2','123456a@A')
insert into teacher values('teacher1','123456a@A')
insert into teacher values('teacher2','123456a@A')