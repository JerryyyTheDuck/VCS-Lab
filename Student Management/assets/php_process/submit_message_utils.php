<?php
    session_start();
    include 'connect_database.php';

    if($_SERVER['REQUEST_METHOD'] == 'POST') {
        if($_GET['status'] == 'edit'){
            $id = $_GET['id'];
            $content = $_POST['content'];
        
            if (edit_message($id, $content)) {
                echo "Message updated successfully!";
            } else {
                echo "Failed to update message.";
            }
            exit();
        }else{
            $message = $_POST['message'];
            $user_recieve = $_POST['recieve'];
            $user_sent = $_POST['sent'];
    
            if(create_message($message, $user_recieve, $user_sent)) {
                header('Location: ../../view_profile.php?name=' . $user_recieve);
                exit();
            } else {
                header('Location: ../../view_profile.php?name=' . $user_recieve);
                exit();
            }
            exit();
        }
    }

    
    if($_SERVER['REQUEST_METHOD'] == 'GET') {
        if($_GET['status'] == 'delete') {
            $id = $_GET['id'];
            if(delete_message($id)) {
                header('Location: ../../view_profile.php?name=' . $_SESSION['user']);
                exit();
            } else {
                header('Location: ../../view_profile.php?name=' . $_SESSION['name']);
                exit();
            }
        }
    }
?>