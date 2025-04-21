<?php
    session_start();
    include_once 'connect_database.php';

    if (!isset($_SESSION['user']) || !isset($_SESSION['loggedin'])) {
        header('Location: login.php');  
        exit();
    }

    
    if(($_SERVER['REQUEST_METHOD'] === 'GET')) {
        if (isset($_GET['name']) && isset($_GET['status'])) {
            $user = select_someone($_GET['name']);
            if($user === null){
                return "no_user";
            }else{
                if ($_GET['status'] === 'delete'){
                    delete_student($_GET['name']);
                    header('Location: ../../home.php');
                    exit();
                    
                }else if ($_GET['status'] === 'update'){
                    header('Location: ../../edit.php?user='.$_GET['name']);
                    exit();
                }
            }
        }

        try {
            $user = select_someone($_GET['name']);
            if ($user == null){
                header('Location: ../../home.php');
            }
        } catch (Exception $e) {
            echo $e->getMessage();
        }
    }
?>