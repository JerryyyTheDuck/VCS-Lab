create database stuManager;
use stuManager;

create table account(
    usernamer varchar(20) primary key,
    pass varchar(20),
    real_name nvarchar(255),
    email varchar(255),
    phone varchar(15),
    avatar varchar(100), 
    is_teacher boolean default false
);


insert into account(usernamer, pass) values('student1','123456a@A');
insert into account(usernamer, pass) values('student2','123456a@A');
insert into account(usernamer, pass, is_teacher) values('teacher1','123456a@A', true);
insert into account(usernamer, pass, is_teacher) values('teacher2','123456a@A', true);