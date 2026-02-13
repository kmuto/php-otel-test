<?php
namespace App\Controller;

class HelloController extends AppController
{
    public function index()
    {
        // Fruitsテーブルをロードして全件取得
        $this->loadModel('Fruits');
        $fruits = $this->Fruits->find()->all();

        // テンプレートに変数を渡す
        $this->set(compact('fruits'));
    }
}
