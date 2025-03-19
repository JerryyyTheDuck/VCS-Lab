<?php
    session_start();
    include_once 'connect_database.php';

    if (!isset($_SESSION['user']) || !isset($_SESSION['loggedin'])) {
        header('Location: login.php');  
        exit();
    }
    if(($_SERVER['REQUEST_METHOD'] === 'GET')) {
        try {
            $user = select_someone($_GET['name']);
            if ($user == null){
                header('Location: ../public/home.php');
            }
            echo "lmao";
        } catch (Exception $e) {
            echo $e->getMessage();
        }
    }


    function delete(){

    }

    function update(){

    }


?>