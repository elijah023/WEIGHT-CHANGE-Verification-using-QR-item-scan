<?php
// host = localhost because database hosted on the same server where PHP files are hosted
//everything prefixed with $ is a PHP variable
    $host = "localhost"; 
    $dbname = "elijahqr";    // Database name
    $username = "root";                    // Database username
    $password = "";                        // Database password
    // Establish connection to MySQL database, using the inbuilt MySQLi library.
    $conn = new mysqli($host, $username, $password, $dbname);
    // Check if connection established successfully
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    } else {
        echo "Connected to mysql database. ";
    }
 //$_POST is a PHP Superglobal that assists us to collect/access the data, that arrives in the form of a post request made to this script.
  // If values sent by NodeMCU are not empty then insert into MySQL database table
  if(!empty($_POST['api_key']) && !empty($_POST['sensor']) ){
        // "sendval" and "sendval2" are query parameters accessed from the HTTP POST Request made by the NodeMCU.
            $api_key = $_POST['api_key'];
            $sensor = $_POST['sensor'];
        // Update your tablename here
        // A SQL query to insert data into table -> INSERT INTO table_name (col1, col2, ..., colN) VALUES (' " .$col1. " ', '".col2."', ..., ' ".colN." ')
        $sql = "SELECT id FROM qrlog WHERE identity IN ('" . $sensor . "');";
                        if ($conn->query($sql) === TRUE) {
                            // If the query returns true, it means it ran successfully
                            echo "Values inserted in MySQL database table.";
                        } else {
                            echo "Error: " . $sql . "<br>" . $conn->error;
                        }
            }
    // Close MySQL connection
    $conn->close();
    ?>