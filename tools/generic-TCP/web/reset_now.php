<?php
 include_once("include/common_db.php");
include_once("include/common_db_query.php");

// Check GET parameters 
if(!isset($_GET['name'])) $_GET['name'] = 'iSpindel000'; else $_GET['name'] = $_GET['name'];


$Name = $_GET['name'];

$q_sql = mysql_query( "INSERT INTO Data (Timestamp, Name, resetFlag)
                      VALUES (NOW(),'$Name', true)")
                       or die(mysql_error());
                       
if (! $q_sql){
   echo "Fehler beim Insert";
}
else {
   echo "Reset-Timestamp in Datenbank eingetragen";
}
?>

