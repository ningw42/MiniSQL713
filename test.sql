create table t1
(
a1 int unique,
a2 float,
a3 char(20),
primary key(a2)
);

insert into t1 values(1, 1.0, 'abc');
insert into t1 values(2, 0.5, 'fuck');
insert into t1 values(3, 10.88, 'you');

create table t2
(
a1 char(30),
a2 float,
a3 int unique,
primary key(a1)
);

insert into t2 values('fuck', 1.0, 1);
insert into t2 values('you', 0.5, 1);
insert into t2 values('hehe', 10.88, 3);

select * from t1;

select a1,a2 from t1;

select * from t1 where a1 > 2;

select * from t1 where a1 > 1 and a1 <3;

select * from t1 where a3 != 'abc';

delete from t1 where a1 > 1;

delete from t1 where a1 > 1 and a1<3;

delete from t1 where a1 > 1 and a2<3.0;