<?php
    session_start();
    
    include 'connect_database.php';

    if (!isset($_SESSION['user']) || !isset($_SESSION['loggedin'])) {
        header('Location: login.php');
        exit();
    }

    if ($_SESSION['is_teacher'] == '0') {
        header('Location: home.php');
        exit();
    }
    
    $user = select_someone($_SESSION['user']);
    

    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        $name = htmlspecialchars($_POST['real_name'] ?? '');
        $username = htmlspecialchars($_POST['username'] ?? '');
        $phone = htmlspecialchars($_POST['phone'] ?? '');
        $email = htmlspecialchars($_POST['email'] ?? '');
        $password = $_POST['password'] ?? '';
        $re_password = $_POST['confirm_password'] ?? '';
        $is_teacher = $_POST['is_teacher'] ?? '';

        if (!preg_match("/^[a-zA-ZàáảãạăắẳẵặâấẩẫậĐđèéẻẽẹêếểễệìíỉĩịòóỏõọôốổỗộơớởỡợùúủũụưứửữựỳýỷỹỵ\s]+$/", $name)) {
            echo 'invalid_name';
            exit();
        }
        
        if (!preg_match("/^[A-Za-z0-9_]+$/", $username)) {
            echo "invalid_username";
            exit();
        }
    
        if (!preg_match("/^\d{10,11}$/", $phone)) {
            echo "invalid_phone";
            exit();
        }
    
        if (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
            echo "indvaild_email";
            exit();
        }

        if ($password !== $re_password) {
            echo "invalid_password";
            exit();
        }

        
        if (isset($_FILES['profile-picture'])) {
            $profile_picture = $_FILES['profile-picture'];
            $profile_picture_name = $profile_picture['name'];
            $profile_picture_tmp = $profile_picture['tmp_name'];
            $profile_picture_size = $profile_picture['size'];
            $profile_picture_error = $profile_picture['error'];
            $profile_picture_type = $profile_picture['type'];
            $profile_picture_ext = explode('.', $profile_picture_name);
            $profile_picture_actual_ext = strtolower(end($profile_picture_ext));
            $allowed = array('jpg', 'jpeg', 'png');

            if (in_array($profile_picture_actual_ext, $allowed)) {
                if ($profile_picture_error === 0) {
                    if ($profile_picture_size < 10000000) { 
                        $profile_picture_name_new = $username . "." . $profile_picture_actual_ext;
                        $profile_picture_destination = '../img/' . $profile_picture_name_new;
                        move_uploaded_file($profile_picture_tmp, $profile_picture_destination);
                    } else {
                        echo 'file_too_big';
                        header('Location: ../../edit.php?user='.$username);
                        exit();
                    }
                } else {
                    echo 'upload_error';
                    header('Location: ../../edit.php?user='.$username);
                    exit();
                }
            } else {
                echo 'invalid_file_type';
                header('Location: ../../edit.php?user='.$username);
                exit();
            }
        }


        create_account($username, $password, $is_teacher);
        create_info($username, $name, $email, $phone, $profile_picture_name_new);
        header('Location: ../../edit.php?user='.$username);
        exit();
    }
?>