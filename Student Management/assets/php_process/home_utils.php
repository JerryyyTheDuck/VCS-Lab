<?php
session_start();
include '../assets/php_process/connect_database.php';


if (!isset($_SESSION['user']) || !isset($_SESSION['loggedin'])) {
    header('Location: login.php');
    exit();
}


$stmt = $conn->prepare("SELECT * FROM info WHERE username = ?");
$stmt->bind_param("s", $_SESSION['user']);
$stmt->execute();
$userResult = $stmt->get_result();
$user = $userResult->fetch_assoc();

$stmt = $conn->prepare("SELECT * FROM info WHERE username IN (SELECT username FROM account WHERE is_teacher = 0)");
$stmt->execute();
$studentsResult = $stmt->get_result();
$students = [];


while ($row = $studentsResult->fetch_assoc()) {
    $students[] = $row;
}

$stmt = $conn->prepare("SELECT * FROM account WHERE is_teacher = 1");
$stmt->execute();
$teachersResult = $stmt->get_result();
$teachers = [];
while ($row = $teachersResult->fetch_assoc()) {
    $teachers[] = $row;
}


$conn->close();


function split_name($name){
    $names = array();
    $names = explode(" ", $name);
    $fist_name = $names[0];
    $last_name = $names[count($names) - 1];
    $middle_name = "";
    for($i = 1; $i < count($names) - 1; $i++){
        $middle_name = $middle_name . " " . $names[$i];
    }

    if ($fist_name == ""){
        $fist_name = "N/A";
    }
    if ($middle_name == ""){
        $middle_name = "N/A";
    }
    if ($last_name == ""){
        $last_name = "N/A";
    }
    if ($name == ""){
        $name = "N/A";
    }


    return [$fist_name, $middle_name, $last_name, $name];
}

?>