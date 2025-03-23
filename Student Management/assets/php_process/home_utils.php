<?php
session_start();
include 'assets/php_process/connect_database.php';


if (!isset($_SESSION['user']) || !isset($_SESSION['loggedin'])) {
    header('Location: login.php');
    exit();
}

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


$user = select_someone($_SESSION['user']);


$studentsResult = select_all_student();
$students = [];
while ($row = $studentsResult->fetch_assoc()) {
    $students[] = $row;
}

$teachersResult = select_all_teacher();
$teachers = [];
while ($row = $teachersResult->fetch_assoc()) {
    $teachers[] = $row;
}

?>