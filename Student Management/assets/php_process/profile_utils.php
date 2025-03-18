<?php
    include '../assets/php_process/connect_database.php';

    if (!isset($_SESSION['user']) || !isset($_SESSION['loggedin'])) {
        header('Location: login.php');
        exit();
    }

    $stmt = $conn->prepare("SELECT * FROM account WHERE username = ?");
    $stmt->bind_param("s", $_SESSION['user']);
    $stmt->execute();
    $userResult = $stmt->get_result();
    $user = $userResult->fetch_assoc();
    
    if($user['real_name'] == null) {
        $user['real_name'] = "N/A";
    }

    if($user['email'] == null) {
        $user['email'] = "N/A";
    }

    if($user['phone'] == null) {
        $user['phone'] = "N/A";
    }
    
    
    $conn->close();
?>