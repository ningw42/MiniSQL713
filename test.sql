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

select * from t1;

select a1,a2 from t1;

select * from t1 where a1 > 2;