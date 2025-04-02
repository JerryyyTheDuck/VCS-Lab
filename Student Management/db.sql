create database stuManager;
use stuManager;

create table account(
    username varchar(20) primary key,
    pass varchar(20),
    is_teacher boolean default false
);

create table info(
    id varchar(20) default uuid() primary key,
    username varchar(20),
    real_name nvarchar(255),
    email varchar(255),
    phone varchar(15),
    avatar varchar(100) default 'default.png',
    foreign key(username) references account(username) on delete cascade
);

CREATE TABLE course (
    id CHAR(36) PRIMARY KEY DEFAULT (UUID()), 
    course_name NVARCHAR(255), 
    description NVARCHAR(65536), 
    attachment NVARCHAR(100)
);


create table student_submits(
    id varchar(20) default uuid() primary key,
    course_id  CHAR(36),
    student_id varchar(20),
    attachment varchar(100),
    is_submited boolean default false,
    status boolean default false,
    foreign key(course_id) references course(id) on delete cascade,
    foreign key(student_id) references info(id) on delete cascade
);

create table note(
    id varchar(20) default uuid() primary key,
    user_sent varchar(20),
    user_receive varchar(20),
    content nvarchar(65536),
    foreign key(user_sent) references account(username) on delete cascade,
    foreign key(user_receive) references account(username) on delete cascade
);

INSERT INTO account (username, pass, is_teacher) VALUES
('john_doe', 'password123', false),
('alice_smith', 'alice2025', true),
('bob_jones', 'securepass987', false),
('mary_williams', 'mysecretpass', true),
('susan_lee', '1234567890', false),
('admin', 'admin', true),
('test', 'test', false),
('teacher1', '123456a@A', true),
('teacher2', '123456a@A', true),
('student1', '123456a@A', false),
('student2', '123456a@A', false);

INSERT INTO info (username, real_name, email, phone, avatar) VALUES
('john_doe', 'John Doe', 'john.doe@example.com', '555-1234', 'default.png'),
('alice_smith', 'Alice Smith', 'alice.smith@school.com', '555-5678', 'default.png'),
('bob_jones', 'Bob Jones', 'bob.jones@example.com', '555-8765', 'default.png'),
('mary_williams', 'Mary Williams', 'mary.williams@university.edu', '555-4321', 'default.png'),
('susan_lee', 'Susan Lee', 'susan.lee@example.com', '555-9876', 'default.png'), 
('admin', 'Admin', 'admin@gmail.com', '555-0000', 'default.png'),
('test', 'Test', 'test@test.com', '555-1111', 'default.png'),
('teacher1', 'John Smith', 'john.smith@school.edu', '555-1234', 'default.png'),
('teacher2', 'Emily Johnson', 'emily.johnson@school.edu', '555-5678', 'default.png'),
('student1', 'Michael Brown', 'michael.brown@student.edu', '555-8765', 'default.png'),
('student2', 'Sarah Wilson', 'sarah.wilson@student.edu', '555-4321', 'default.png');