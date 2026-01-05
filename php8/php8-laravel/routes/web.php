<?php

use Illuminate\Support\Facades\Route;
use Illuminate\Support\Facades\Http;

Route::get('/index', function () {
    return view('welcome');
});

Route::get('/', function () {
    $min_microseconds = 0;
    $max_microseconds = 2000000;
    $random_microseconds = rand($min_microseconds, $max_microseconds);
    usleep($random_microseconds);
    return 'Hello World';
});

Route::get('/hello', function () {
    $min_microseconds = 0;
    $max_microseconds = 1000000;
    $random_microseconds = rand($min_microseconds, $max_microseconds);
    usleep($random_microseconds);

    $mysqli = new mysqli('db', 'root', '', 'laravel');
    if ($mysqli->connect_errno) {
      die("MySQL connection failed: " . $mysqli->connect_error);
    }
    $sql = "SELECT id, name FROM fruits";
    $result = $mysqli->query($sql);
    if ($result) {
      echo "<h1>Fruits Table</h1>";
      echo "<ul>";
      while ($row = $result->fetch_assoc()) {
          echo "<li>{$row['id']}: {$row['name']}</li>";
      }
      echo "</ul>";
      $result->free();
    } else {
      echo "Query error: " . $mysqli->error;
    }
    $mysqli->close();
    $min_microseconds = 0;
    $max_microseconds = 1000000;
    $random_microseconds = rand($min_microseconds, $max_microseconds);
    usleep($random_microseconds);

    return 'Hello World';
});

Route::get('/call', function () {
    $response = Http::get('https://jsonplaceholder.typicode.com/posts');
    if ($response->successful()) {
        return $response->json();
    }
    if ($response->failed()) {
        $response->throw();
    }
});

Route::get('/error', function () {
    1 / 0;
});

Route::get('/query', function () {
  throw new \InvalidArgumentException("Invalid ID: -1");
});
