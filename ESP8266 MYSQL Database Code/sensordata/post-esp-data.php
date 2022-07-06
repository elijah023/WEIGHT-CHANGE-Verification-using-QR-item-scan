<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "elijahqr";

//$conn = mysqli_connect($dbServername, $dbUsername, $dbPassword, $dbName);

$api_key_value = "tPmAT5Ab3j7F9";

$api_key= $sensor ="";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $sensor = test_input($_POST["sensor"]);
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "SELECT id FROM qrlog WHERE identity IN ('" . $sensor . "')";
        
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
            $sql = "INSERT INTO qrlog (identity) VALUES ('" . $sensor . "')";
            $result =mysqli_query($conn,$sql);
        }
    
        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}