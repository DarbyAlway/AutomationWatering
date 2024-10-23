<?php
require "waterconnection.php";

// Prepare and execute the SQL query
$sql = "SELECT AVG(water_level) as water_level, DATE_FORMAT(date, '%Y-%m-%d %H:00:00') as date 
        FROM waterlevel 
        GROUP BY date 
        ORDER BY date ASC";
$result = $conn->query($sql);

// Initialize arrays to hold data
$water_levels = [];
$dates = [];

// Fetch data from the result set
if ($result && $result->num_rows > 0) {
    while ($row = $result->fetch_assoc()) {
        // Store water levels and dates
        $water_levels[] = $row['water_level'];
        $dates[] = $row['date'];
    }
} else {
    echo "0 results or query error";
}

// Close the database connection
$conn->close();
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Water Level Graph</title>

    <!-- Chart.js and date adapter -->
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/chartjs-adapter-date-fns/dist/chartjs-adapter-date-fns.bundle.min.js"></script>

    <style>
        body {
            font-family: Arial, sans-serif;
        }
        .container {
            width: 75%;
            margin: 0 auto;
        }
        #waterLevelChart {
            max-width: 100%;
            height: auto;
        }
        .btn-container {
            margin-top: 20px;
            text-align: center;
        }
        button {
            padding: 10px 20px;
            font-size: 16px;
            background-color: #28a745;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }
        button:hover {
            background-color: #218838;
        }
    </style>
</head>

<body>
    <div class="container">
        <h1>Water Level Monitoring</h1>
        <canvas id="waterLevelChart"></canvas>

        <!-- Button to activate pump -->
        <div class="btn-container">
            <button id="activatePump">Activate Pump</button>
        </div>
    </div>

    <script>
        // PHP arrays passed to JavaScript
        var waterLevels = <?php echo json_encode($water_levels); ?>;
        var dates = <?php echo json_encode($dates); ?>;

        // Create the chart
        var ctx = document.getElementById('waterLevelChart').getContext('2d');
        var chart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: dates,
                datasets: [{
                    label: 'Water Level (in cm)',
                    data: waterLevels,
                    borderColor: 'rgba(75, 192, 192, 1)',
                    borderWidth: 2,
                    fill: false,
                    pointRadius: 3,
                    tension: 0.1
                }]
            },
            options: {
                responsive: true,
                plugins: {
                    title: {
                        display: true,
                        text: 'Water Level Over Time (Hourly)'
                    },
                    tooltip: {
                        mode: 'index',
                        intersect: false
                    }
                },
                scales: {
                    x: {
                        type: 'time',
                        time: {
                            unit: 'hour', // Change to 'hour' for hourly intervals
                            tooltipFormat: 'PP HH:mm' // Display format for tooltips
                        },
                        title: {
                            display: true,
                            text: 'Date and Time'
                        }
                    },
                    y: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: 'Water Level (cm)'
                        }
                    }
                }
            }
        });

        // Button click event to activate the pump
        document.getElementById('activatePump').addEventListener('click', function() {
            fetch('http://192.168.0.170/activatePump')  // Replace with your ESP32's actual IP address
                .then(response => {
                    if (response.ok) {
                        alert('Pump activated');
                    } else {
                        alert('Failed to activate pump');
                    }
                })
                .catch(error => {
                    console.error('Error:', error);
                    alert('Error occurred while trying to activate the pump.'); 
                });
        }); 
    </script>

</body> 
</html>
