<?php
$mysqli = new mysqli('db', 'root', null, 'laravel');
if ($mysqli->connect_error) {
    die('Connect Error (' . $mysqli->connect_errno . ') ' . $mysqli->connect_error);
}

if ($mysqli->multi_query("CREATE TABLE fruits (
  id INT AUTO_INCREMENT PRIMARY KEY,
  name VARCHAR(50) NOT NULL
);
INSERT INTO fruits (name) VALUES
  ('Apple'),
  ('Orange'),
  ('Grape'),
  ('Banana'),
  ('Strawberry');")) {
} else {
    echo $mysqli->error;
}
