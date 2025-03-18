<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="../assets/css/login.css">
    <script src="https://cdn.jsdelivr.net/npm/sweetalert2@11"></script>
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

        include 'connect_database.php';

        if(isset($_POST['user']) && isset($_POST['pass'])) {
            $user = $_POST['user'];
            $pass = $_POST['pass'];
            $conn = new mysqli('localhost', 'root', '', 'login');
            $sql = "SELECT * FROM account WHERE user = '$user' AND pass = '$pass'";
            $result = $conn->query($sql);
            if($result->num_rows > 0) {
                echo "
                <script>
                    Swal.fire({
                        icon: 'success',
                        title: 'Login Successful!',
                        text: 'You have logged in successfully.',
                        confirmButtonText: 'OK'
                    });
                </script>";
            } else {
                echo "
                <script>
                    Swal.fire({
                        icon: 'error',
                        title: 'Login Failed',
                        text: 'Invalid username or password.',
                        confirmButtonText: 'Try Again'
                    });
                </script>";
                header('Location: login.php');
                exit();
            }
        } else {
            echo "
            <script>
                Swal.fire({
                    icon: 'error',
                    title: 'Login Failed',
                    text: 'Invalid username or password.',
                    confirmButtonText: 'Try Again'
                });
            </script>";
            header('Location: login.php');
            exit();
        }
    ?>
</body>
</html>
