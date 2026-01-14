<?php

use Illuminate\Support\Facades\Route;
use Illuminate\Support\Facades\Http;

Route::get('/', function () {
    $min_microseconds = 0;
    $max_microseconds = 2000000;
    $random_microseconds = rand($min_microseconds, $max_microseconds);
    usleep($random_microseconds);
    return view('hello', [ 'htmlData' => '' ]);
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
    $htmlData = '';
    if ($result) {
      $htmlData .= "<h1>Fruits Table</h1>";
      $htmlData .= "<ul>";
      while ($row = $result->fetch_assoc()) {
          $htmlData .= "<li>{$row['id']}: {$row['name']}</li>";
      }
      $htmlData .= "</ul>";
      $result->free();
    } else {
      $htmlData .= "Query error: " . $mysqli->error;
    }
    $mysqli->close();
    $min_microseconds = 0;
    $max_microseconds = 1000000;
    $random_microseconds = rand($min_microseconds, $max_microseconds);
    usleep($random_microseconds);
    return view('hello', [ 'htmlData' => $htmlData ]);
});

Route::get('/call', function () {
    $response = Http::get('https://jsonplaceholder.typicode.com/posts');
    $htmlData = '<p>Returned from External API Server:</p><div class="pre"><pre>';
    if ($response->successful()) {
        $posts = $response->json();
        $prettyJson = json_encode($posts, JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE);
        $htmlData .= e(mb_substr($prettyJson, 0, 500)) . "...";
    }
    $htmlData .= '</pre></div>';
    if ($response->failed()) {
        $response->throw();
    }
    return view('hello', [ 'htmlData' => $htmlData ]);
});

Route::get('/error', function () {
    1 / 0;
});

Route::get('/query', function () {
  throw new \InvalidArgumentException("Invalid ID: -1");
});
