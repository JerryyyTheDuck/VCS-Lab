<?php
    session_start();
    include_once 'connect_database.php';
    
    if (!isset($_SESSION['user']) || !isset($_SESSION['loggedin'])) {
        header('Location: login.php');
        exit();
    }


    $user = select_someone($_SESSION['user']);


    function return_default_value($field, $username, $is_admin) {
        if ($is_admin == 1) {
            if($field == 'password' || $field == 're-password') {
                return "
                    <input name='$field' type='password' id='$field' class='form-control'/>
                ";
            }elseif($field == 'id' ) {
                return "
                    <input name='$field' type='text' id='$field' class='form-control' required value='{$username[$field]}' readonly />
                ";
            }
            return "
                <input name='$field' type='text' id='$field' class='form-control' required value='{$username[$field]}' />
            ";
        } else  {
            if ($field == 'real_name' || $field == 'id' || $field == 'username') {
                return "
                    <input name='$field' type='text' id='$field' class='form-control' required value='{$username[$field]}' readonly />
                ";
            }
            elseif($field == 'password' || $field == 're-password') {
                return "
                    <input name='$field' type='password' id='$field' class='form-control' />
                ";
            } 
            else {
                return "
                    <input name='$field' type='text' id='$field' class='form-control' required value='{$username[$field]}' />
                ";
            }
        }
    }
    
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        
        $jsonData = file_get_contents("php://input");
        $data = json_decode($jsonData, true);

        $name = htmlspecialchars($data['real_name']);
        $username = htmlspecialchars($data['username']);
        $phone = htmlspecialchars($data['phone']);
        $email = htmlspecialchars($data['email']);
        $password = htmlspecialchars($data['password']);
        $re_password = htmlspecialchars($data['confirm_password']);

        if (!preg_match("/^[a-zA-ZàáảãạăắẳẵặâấẩẫậĐđèéẻẽẹêếểễệìíỉĩịòóỏõọôốổỗộơớởỡợùúủũụưứửữựỳýỷỹỵ\s]+$/", $name)) {
            $_SESSION['error'] = 'invalid_name';
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

        if ($password != '' && $password === $re_password) {
            update_password($_SESSION['user'], $password);
            update_info($_SESSION['user'], $name, $phone, $email);
        }else {
            update_info($_SESSION['user'], $name, $phone, $email);
        }
        header('Location: ../../public/edit.php');
        exit();
    }
    
?>