<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Untitled Document</title>
<style type="text/css">
body,td,th {
	font-family: Verdana, Geneva, sans-serif;
	font-size: small;
}
</style>
</head>

<body>

<?php
date_default_timezone_set("Asia/Bangkok");
require "waterconnection.php";

// Define the current date and time first
$cknow = date("Y-m-d H:i:s");

$water_level = isset($_GET["waterLevel"]) ? trim($_GET["waterLevel"]) : null;  // Correct key name
$date = isset($_GET["date"]) ? trim($_GET["date"]) : $cknow;  // Use current time if date not provided

error_reporting(E_ALL & ~E_NOTICE);
echo "data in test...";

if ($water_level !== null) {
    // Fixed sensor ID
    $waterSensorId = "id01";  

    // Use an INSERT ON DUPLICATE KEY UPDATE
    $sql = "INSERT INTO waterlevel (water_level, date, sensor_id) VALUES ('$water_level', '$date', '$waterSensorId')
            ON DUPLICATE KEY UPDATE water_level = '$water_level', date = '$date'";

    if (mysqli_query($conn, $sql)) {
        echo "Record created or updated successfully";
    } else {
        echo "Error: " . $sql . "<br>" . mysqli_error($conn);
    }
}

?>

</body>