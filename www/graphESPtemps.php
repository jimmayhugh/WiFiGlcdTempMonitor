<?php
  include_once("/var/www/htdocs/esp8266t/accessDatabase.php");
  include_once("/var/www/htdocs/esp8266t/udpRequest.php");

  $arrayTemp0   = 0;
  $arrayTemp1   = 1;
  $arrayTemp2   = 2;
  $arrayTemp3   = 3;

  $fileName = "/var/log/esp8266/graph_esp8266t_update.log";   

  if( ! $fr = fopen($fileName, "a+") )
  {
	  die("Could not open file:".$fileName."\n");
  }else{
    fwrite($fr, "\n");
    fwrite($fr, date("D M j G:i:s Y"));
    fwrite($fr, "\n");
  } 
  
  $tempTime = date("U");
  $deviceQuery = "SELECT * FROM `Addresses` WHERE 1";
  fwrite($fr, $deviceQuery."\n");
//  echo $deviceQuery."\n";
  $devResult = mysqli_query($link, $deviceQuery);
  if($devResult === FALSE)
  {
    fwrite($fr, "device query Failed\n");
    die("device query Failed\n");
  }
  $devCnt = mysqli_num_rows($devResult);
  fwrite($fr, $devCnt." device rows retrieved\n");
//  echo $devCnt." device rows retrieved\n";
  while($devObj = mysqli_fetch_object($devResult))
  {
    $ipAddress = $devObj->ipAddress;
    $udpPort = $devObj->udpPort;
    $in = $getStatus."\n";
    $out = udpRequest($ipAddress, $udpPort, $in);

    $tOut = trim($out);
    if($tOut === "Invalid Command")
    {
      continue;
    }

//    echo "\$in = $in";
//    echo "\$out = $tOut \n";

    $tempArray = explode(",", $tOut);
    $temp0 = trim($tempArray[$arrayTemp0]);
    $temp1 = trim($tempArray[$arrayTemp1]);
    $temp1 = trim($tempArray[$arrayTemp2]);
    $temp1 = trim($tempArray[$arrayTemp3]);
    $tempInsertQuery = "INSERT INTO device SET `ipaddress`=\"$ipAddress\",`port`=$udpPort,`time`=$tempTime,`temp0`=$temp0,`temp1`=$temp1,`temp2`=$temp2,`temp2`=$temp2";
    fwrite($fr, $tempInsertQuery."\n");
//    echo $tempInsertQuery."\n";
    $tempInsertResult = mysqli_query($link, $tempInsertQuery);
    if($tempInsertResult === FALSE)
    {
      fwrite($fr, "ESPTemp Insert Failed\n");
//        die("ESPTemp Insert Failed\n");
    }else{
      fwrite($fr, "ESPTemp Insert Success\n");
    }
    mysqli_free_result($tempInsertResult);
  }
  mysqli_free_result($devResult);
  mysqli_close($link);
  fclose($fr);
?>
