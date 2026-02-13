<?php
namespace App\Model\Table;

use Cake\ORM\Table;

class FruitsTable extends Table
{
    public function initialize(array $config): void
    {
        parent::initialize($config);
        $this->setTable('fruits'); // 使用するテーブル名
        $this->setDisplayField('name');
        $this->setPrimaryKey('id');
    }
}
