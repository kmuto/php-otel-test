<h1>Fruits List</h1>
<ul>
    <?php foreach ($fruits as $fruit): ?>
        <li><?= h($fruit->name) ?></li>
    <?php endforeach; ?>
</ul>
