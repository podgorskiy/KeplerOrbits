<html>
<head>
	<title>Solar System</title>
	<!-- link calendar resources -->
	<link rel="stylesheet" type="text/css" href="tcal.css" />
	<script type="text/javascript" src="tcal.js"></script> 
</head>
<body>
<?php
	$size=$_GET['m'];
	$date=$_GET['date'];
	$time=$_GET['time'];
?>
	<form method="get" action="solarsystem.php">
		<!-- add class="tcal" to your input field -->
		<p><b>Укажите дату. Формат mm/dd/yyyy .  Оставьте поле пустым для текущей даты</b><br>
<?php
	echo "	<div><input type=\"text\" name=\"date\" class=\"tcal\" value=\"$date\" /></div><br>";
	echo "	<p><b>Укажите время. Формат hh:mm . Время следует указывать по Гринвичу! Оставьте поле пустым для текущего времени.</b><br>";
	echo "	<div><input type=\"text\" name=\"time\" value=\"$time\" /></div><br>";
	echo "	<p><b>Масштаб. Укажите ширину окна в астрономических единицах. Укажите значение 100 для полного обзора солнечной системы.</b><br>";
	echo "	<input name=\"m\" type=\"text\" size=\"4\" value=\"$size\"><br>\n";
?>
		<div><input type="submit" value="Обновить"/></div>
	</form>
<?php
	echo "<img src=\"image.php?m=$size&date=$date&time=$time\">\n";
?>
</body>
</html>
