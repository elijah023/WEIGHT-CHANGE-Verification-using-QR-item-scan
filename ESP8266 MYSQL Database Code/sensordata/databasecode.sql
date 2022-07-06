
/*creating a table in the database*/
CREATE TABLE qrlog (
  id int(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
  identity varchar(30) not null,
  reading_time varchar(30) not null,
  intialtime varchar(10)
);

