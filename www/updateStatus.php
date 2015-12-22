<?php

  include_once("udpRequest.php");
  include_once("accessDatabase.php");

  $query = "SELECT * FROM `Addresses` WHERE 1";
//  echo "\$query = $query <br />";
  if($result = mysqli_query($link, $query))
  {
    $numAddresses = mysqli_num_rows($result);
//    printf("Select returned %d rows.\n", mysqli_num_rows($result));
  }

  $dataTimer = date("s");
  $setDebug = 0;

  for($x = 0; $x < $numAddresses; $x++)
  {
    $ipAddressObj = mysqli_fetch_object($result);
    $ip_Address = $ipAddressObj->ipAddress;
    $udp_Port = $ipAddressObj->udpPort;
//    echo "\$ip_Address = ".$ipAddressObj->ipAddress.", \$udp_Port = ".$ipAddressObj->udpPort."<br />";
    $in = $getStatus."\n";
    $udpStatus = udpRequest($ip_Address, $udp_Port, $in);
//    echo "\$udpStatus = ".$udpStatus."<br />";

    $udpType = explode(",", $udpStatus);
    $tempStr =
    "<div id=\"temp\"; position: relative; width: 100%>
       <td align=\"center\" width=\"20%\">
         <form method=\"post\" action=\"modifySettings.php\">
           <input type=\"hidden\" name=\"ip_address\" value=\"".$ip_Address."\">
           <input type=\"hidden\" name=\"udp_port\" value=\"".$udp_Port."\">
           <input type=\"hidden\" name=\"device_name\" value=\"".$udpType[4]."\">
           <input type=\"hidden\" name=\"temptype\" value=\"tempc\">
           <input type=\"submit\" value=\"MODIFY\">
         </form>
         <font size=\"10\"><strong>".$udpType[4]."</strong></font><br />
         <form method=\"post\" action=\"SensorStatus.php\">
           <input type=\"hidden\" name=\"ip_address\" value=\"".$ip_Address."\">
           <input type=\"hidden\" name=\"udp_port\" value=\"".$udp_Port."\">
           <input type=\"submit\" name=\"remove\" value=\"REMOVE\">
         </form>
        </td>
       <td align=\"center\" width=\"20%\">
         Temperature
         <br />
         <form method=\"post\" action=\"plotData.php\">
           <input type=\"hidden\" name=\"ip_address\" value=\"".$ip_Address."\">
           <input type=\"hidden\" name=\"udp_port\" value=\"".$udp_Port."\">
           <input type=\"hidden\" name=\"device_name\" value=\"".$udpType[4]."\">
           <input type=\"hidden\" name=\"temptype\" value=\"temp0\">
           <input type=\"submit\" value=\"GRAPH\">
         </form>
         <font size=\"10\"><strong>".$udpType[0]."&deg; C</strong></font>
         <br /><br />
       </td>
       <td align=\"center\" width=\"20%\">
         Temperature
         <br />
         <form method=\"post\" action=\"plotData.php\">
           <input type=\"hidden\" name=\"ip_address\" value=\"".$ip_Address."\">
           <input type=\"hidden\" name=\"device_name\" value=\"".$udpType[4]."\">
           <input type=\"hidden\" name=\"temptype\" value=\"temp1\">
           <input type=\"submit\" value=\"GRAPH\">
         </form>
         <font size=\"10\"><strong>".$udpType[1]."&deg; F</strong></font>
         <br /><br />
       </td>
       <td align=\"center\" width=\"20%\">
         Temperature
         <br />
         <form method=\"post\" action=\"plotData.php\">
           <input type=\"hidden\" name=\"ip_address\" value=\"".$ip_Address."\">
           <input type=\"hidden\" name=\"device_name\" value=\"".$udpType[4]."\">
           <input type=\"hidden\" name=\"temptype\" value=\"temp2\">
           <input type=\"submit\" value=\"GRAPH\">
         </form>
         <font size=\"10\"><strong>".$udpType[2]."&deg; F</strong></font>
         <br /><br />
       </td>
       <td align=\"center\" width=\"20%\">
         Temperature
         <br />
         <form method=\"post\" action=\"plotData.php\">
           <input type=\"hidden\" name=\"ip_address\" value=\"".$ip_Address."\">
           <input type=\"hidden\" name=\"device_name\" value=\"".$udpType[4]."\">
           <input type=\"hidden\" name=\"temptype\" value=\"temp3\">
           <input type=\"submit\" value=\"GRAPH\">
         </form>
         <font size=\"10\"><strong>".$udpType[3]."&deg; F</strong></font>
         <br /><br />
       </td>";
    echo "<table width=\"100%\" align=\"center\" border=\"2\">\n<tr>\n$tempStr</tr>\n</table>";
    if($setDebug > 0)
      echo "\$dataTimer = $dataTimer\n";
    if($dataTimer == "00" || $dataTimer == "01")
    {
      $time = time("U");
      $switch1 = trim($udpType[2]);
      $switch2 = trim($udpType[3]);
      $insertQuery = "INSERT INTO device SET `ipaddress`=\"$ip_Address\",`port`=$udp_Port,`time`=$time,`temp0`=$udpType[0],`temp1`=$udpType[1],`temp2`=$udpType[2],`temp3`=$udpType[3]";
      if($setDebug != 0)
        echo $insertQuery."\n";
      $insertResult = mysqli_query($link, $insertQuery);
      if($insertResult === FALSE && $setDebug != 0)
      {
        echo "Data Insert Failed\n";
        printf("Error: %s\n", mysqli_error($link));
      }

      if($insertResult === TRUE && $setDebug != 0)
      {
        echo "Data Insert Success\n";
      }
      mysqli_free_result($insertResult);
    }
  }
/* free result set */
mysqli_free_result($result);
?>
