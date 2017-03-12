<?php

// configure your database connection here:
define('DB_SERVER',"localhost");
define('DB_NAME',"iSpindle");
define('DB_USER',"iSpindle");
define('DB_PASSWORD',"ohyeah");
 
$conn = mysql_connect(DB_SERVER, DB_USER, DB_PASSWORD);
if(is_resource($conn))
{
  mysql_select_db(DB_NAME, $conn);
  mysql_query("SET NAMES 'ascii'", $conn);
  mysql_query("SET CHARACTER SET 'ascii'", $conn);
}
 
define("defaultTimePeriod", 24); // Timeframe for chart
define("defaultReset",  false);  // Flag for Timeframe for chart, 
?>
