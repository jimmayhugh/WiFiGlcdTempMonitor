<?php
$username="esp8266t";
$password="esp8266t";
$database="esp8266t";

$link = mysqli_connect("localhost",$username,$password);
@mysqli_select_db($link,$database) or die( "Unable to select database");

?>
