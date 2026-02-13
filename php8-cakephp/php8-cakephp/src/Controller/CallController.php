<?php
namespace App\Controller;

use Cake\Http\Client;

class CallController extends AppController
{
    public function index()
    {
        $http = new Client();
        
        // 外部APIへGETリクエスト
        $response = $http->get('https://jsonplaceholder.typicode.com/posts');

        if ($response->isOk()) {
            // レスポンスをテキストとして取得
            $rawJson = $response->getStringBody();

            // 500文字で切り出し、三点リーダーを付与
            $displayText = mb_strimwidth($rawJson, 0, 500, '...');
        } else {
            $displayText = "APIの取得に失敗しました。";
        }

        // テンプレートに変数を渡す
        $this->set(compact('displayText'));
    }
}
