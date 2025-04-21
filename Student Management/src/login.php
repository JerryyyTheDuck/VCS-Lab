<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="assets/css/login.css">
    <script src="jquery-3.3.1.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/sweetalert2@11"></script>
    <script src="assets/js/login.js"></script>
    <title>Login</title>
</head>
<body>
    <div class='box'>
        <div class='box-form'>
            <div class='box-login-tab'></div>
            <div class='box-login-title'>
                <div class='i i-login'></div>
                <h2>LOGIN</h2>
            </div>
            <div class='box-login'>
                <div class='fieldset-body' id='login_form'>
                    <form method="POST">
                        <p class='field'>
                            <label for='user'>USERNAME</label>
                            <input type='text' id='user' name='user' title='Username' required />
                            <span id='valida' class='i i-warning'></span>
                        </p>
                        <p class='field'>
                            <label for='pass'>PASSWORD</label>
                            <input type='password' id='pass' name='pass' title='Password' required />
                            <span id='valida' class='i i-close'></span>
                        </p>
                        <input type='submit' id='do_login' value='GET STARTED' title='Get Started' />
                    </form>
                </div>
            </div>
        </div>
    </div>
    <?php
        session_start();
        include 'assets/php_process/connect_database.php';

        if ($_SERVER['REQUEST_METHOD'] === 'POST') {
            if (isset($_POST['user']) && isset($_POST['pass'])) {
                $user = $_POST['user'];
                $pass = $_POST['pass'];
                
                try {
                    $stmt = $conn->prepare("SELECT * FROM account WHERE username = ?");
                    $stmt->bind_param("s", $user);
                    $stmt->execute();
                    $result = $stmt->get_result();

                    if ($result->num_rows > 0) {
                        $row = $result->fetch_assoc();
                        if (trim($pass) === trim($row['pass'])) {
                            $_SESSION['user'] = $user;
                            $_SESSION['is_teacher'] = $row['is_teacher'];
                            $_SESSION['loggedin'] = true;
                            echo '<script>location.href="home.php";</script>';
                            exit();
                        } else {
                            echo 'invalid_password';
                        }
                    } else {
                        echo 'invalid_username';
                    }
                } catch (Exception $e) {
                    echo 'database_error';
                }
            }
        }
    ?>
</body>
<>
</html>