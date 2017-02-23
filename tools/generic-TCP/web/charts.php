<?php

// Show the Angle/Temperature chart
// GET Parameters:
// hours = number of hours before now() to be displayed
// name = iSpindle name
 
include_once("include/common_db.php");
include_once("include/common_db_query.php");

// Check GET parameters (for now: Spindle name and Timeframe to display) 
if(!isset($_GET['hours'])) $_GET['hours'] = 24; else $_GET['hours'] = $_GET['hours'];
if(!isset($_GET['name'])) $_GET['name'] = 'iSpindel000'; else $_GET['name'] = $_GET['name'];

list($hour, $angle, $temperature, $battery) = getChartValues($_GET['name'], $_GET['hours']);

?>

<!DOCTYPE html>
<html>
<head>
  <title>iSpindle Data</title>
  <meta http-equiv="refresh" content="120">
  <meta name="Keywords" content="iSpindle, iSpindel, Chart, genericTCP">
  <meta name="Description" content="iSpindle Fermentation Chart">
  <script src="include/jquery-3.1.1.min.js"></script>

<script type="text/javascript">
$(function () 
{
  var chart;
 
  $(document).ready(function() 
  {        
    chart = new Highcharts.Chart(
    {
      chart: 
      {
        renderTo: 'container'
      },
      title: 
      {
        text: 'iSpindel: <?php echo $_GET['name'];?>'
      },
      subtitle: 
      {
        text: 'Temperatur und Winkel der letzten <?php echo $_GET['hours'];?> Stunden'
      },
      xAxis: 
      {
	type: 'datetime',
	title:
        {
          text: 'Uhrzeit'
        }
      },      
      yAxis: [
      {   
        title: 
        {
          text: 'Winkel'         
        },      
        labels: 
        {
          align: 'left',
          x: 3,
          y: 16,
          formatter: function() 
          {
            return this.value +'°'
          }
        },
	showFirstLabel: false
      },{
         linkedTo: 0,
         gridLineWidth: 0,
         opposite: true,
         title: {
            text: 'Temperatur'
         },
         labels: {
            align: 'right',
            x: -3,
            y: 16,
          formatter: function() 
          {
            return this.value +'°C'
          }
         },
	showFirstLabel: false
        }
      ],
      
      tooltip: 
      {
	crosshairs: [true, true],
        formatter: function() 
        {
	   if(this.series.name == 'Temperatur') {
           	return '<b>'+ this.series.name +' </b>um '+ Highcharts.dateFormat('%H:%M', new Date(this.x)) +' Uhr:  '+ this.y +'°C';
	   } else {
	   	return '<b>'+ this.series.name +' </b>um '+ Highcharts.dateFormat('%H:%M', new Date(this.x)) +' Uhr:  '+ this.y +'°';
	   }
        }
      },  
      legend: 
      {
        enabled: true
      },
      credits:
      {
        enabled: false
      },
      series:
      [
	  {
          name: 'Winkel', 
	  color: '#FF0000',
          data: [<?php echo $angle;?>],
          marker: 
          {
            symbol: 'square',
            enabled: false,
            states: 
            {
              hover: 
              {
                symbol: 'square',
                enabled: true,
                radius: 8
              }
            }
          }    
          },
	  {
          name: 'Temperatur', 
	  color: '#0000FF',
          data: [<?php echo $temperature;?>],
          marker: 
          {
            symbol: 'square',
            enabled: false,
            states: 
            {
              hover: 
              {
                symbol: 'square',
                enabled: true,
                radius: 8
              }
            }
          }    
        
        }
      ] //series      
    });
  });  
});
</script>
</head>
<body>
 
<div id="wrapper">
  <script src="include/highcharts.js"></script>
  <div id="container" style="width: 760px; height: 360px; margin: 0 auto"></div>
</div>
 
</body>
</html>
