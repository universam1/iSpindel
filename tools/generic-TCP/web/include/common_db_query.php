<?php

/* 
  Visualizer for iSpindle using genericTCP with mySQL 
  Shows mySQL iSpindle data on the browser as a graph via Highcharts:
  http://www.highcharts.com
 
  Data access via mySQL for the charts is defined in here.
 
  For the original project itself, see: https://github.com/universam1/iSpindel  
  
  Tozzi (stephan@sschreiber.de), Feb 22 2017 
*/


// remove last character from a string
function delLastChar($string="")
{
  $t = substr($string, 0, -1);
  return($t);
}
 
// Get values from database for selected spindle, between now and timeframe in hours ago
function getChartValues($iSpindleID='iSpindel000', $timeFrameHours=24)
{
  $q_sql = mysql_query("SELECT Timestamp, UNIX_TIMESTAMP(Timestamp) as unixtime, temperature, angle, battery
                          FROM Data
                          WHERE Name = '".$iSpindleID."' AND Timestamp >= date_sub(NOW(), INTERVAL ".$timeFrameHours." HOUR) and Timestamp <= NOW()
                          ORDER BY Timestamp ASC") or die(mysql_error());
                          

  // retrieve number of rows
  $rows = mysql_num_rows($q_sql);
  if ($rows > 0)
  {
    $valHour = '';
    $valAngle = '';
    $valTemperature = '';
    $valBattery = '';
    
    // retrieve and store the values as CSV lists for HighCharts
    while($r_row = mysql_fetch_array($q_sql))
    {
      $jsTime = $r_row['unixtime'] * 1000;
      $valHour          .= $r_row['Timestamp'].',';
      $valAngle         .= '['.$jsTime.', '.$r_row['angle'].'],';
      $valTemperature   .= '['.$jsTime.', '.$r_row['temperature'].'],';
      $valBattery       .= $r_row['battery'].',';
    }
    
    // remove last comma from each CSV
    $valHour          = delLastChar($valHour);     
    $valAngle         = delLastChar($valAngle);
    $valTemperature   = delLastChar($valTemperature);
    $valBattery       = delLastChar($valBattery);
    return array($valHour, $valAngle, $valTemperature, $valBattery);
  }
} 
?>
