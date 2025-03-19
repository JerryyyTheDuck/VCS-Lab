<?php
    require_once '../assets/php_process/connect_database.php';

    if (!isset($_SESSION['user']) || !isset($_SESSION['loggedin'])) {
        header('Location: login.php');
        exit();
    }

    $user = select_someone($_SESSION['user']);
    
    if($user['real_name'] == null) {
        $user['real_name'] = "N/A";
    }

    if($user['email'] == null) {
        $user['email'] = "N/A";
    }

    if($user['phone'] == null) {
        $user['phone'] = "N/A";
    }
?>