<?php

namespace App\Http\Controllers;

use App\Models\Fruit;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Log;

class FruitController extends Controller
{
    public function index()
    {
        Log::info("Fetching fruits for index page");
        $fruits = \App\Models\Fruit::orderBy('name', 'asc')->get();
        $htmlData = $fruits->implode('name', ', ');
        $maxId = \App\Models\Fruit::max('id');
        $targetId = rand(1, $maxId);
        $randomFruit = \App\Models\Fruit::where('id', '>=', $targetId)->first();
        $htmlData .= "<p>今日のおすすめ：<strong>{$randomFruit->name}</strong></p>";
        return view('hello', compact('htmlData'));
    }
}
