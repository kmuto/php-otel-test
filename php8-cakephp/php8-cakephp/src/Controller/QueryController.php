<?php
namespace App\Controller;

class QueryController extends AppController
{
    public function index()
    {
        throw new \InvalidArgumentException("Invalid ID: -1");
    }
}
