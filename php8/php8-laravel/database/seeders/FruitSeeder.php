<?php

namespace Database\Seeders;

use Illuminate\Database\Console\Seeds\WithoutModelEvents;
use Illuminate\Database\Seeder;
use App\Models\Fruit;

class FruitSeeder extends Seeder
{
    /**
     * Run the database seeds.
     */
    public function run(): void
    {
        $fruits = [
            'Apple', 'Banana', 'Orange', 'Strawberry', 'Grape',
            'Watermelon', 'Pineapple', 'Mango', 'Blueberry', 'Peach',
            'Kiwi', 'Melon', 'Cherry', 'Pear', 'Lemon',
            'Grapefruit', 'Raspberry', 'Papaya', 'Fig', 'Avocado'
        ];
  
        foreach ($fruits as $name) {
            Fruit::create([
                'name' => $name,
            ]);
        }
    }
}
