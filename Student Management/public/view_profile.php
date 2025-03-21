<?php
    include_once '../assets/php_process/view_profile_utils.php';
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@400;500;600&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css">
    <link rel="stylesheet" href="../assets/css/profile.css">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/sweetalert2@11"></script>
    
    <title>User Profile</title>
    <style>
        body {
            font-family: 'Poppins', sans-serif;
            background-color: #f8f9fa;
            color: #333;
        }

        .sidebar {
            width: 250px;
            height: 100vh;
            background: linear-gradient(135deg, #d32f2f, #b71c1c);
            color: white;
            position: fixed;
            top: 0;
            left: 0;
            padding: 20px;
            box-shadow: 2px 0 10px rgba(0, 0, 0, 0.1);
            display: flex;
            flex-direction: column;
            gap: 10px;
        }

        .sidebar button {
            background-color: white;
            color: #d32f2f;
            border: none;
            padding: 10px 20px;
            border-radius: 25px;
            font-size: 1rem;
            font-weight: 500;
            transition: background-color 0.3s ease, transform 0.2s ease;
            display: flex;
            align-items: center;
            gap: 8px;
            text-align: left;
        }

        .sidebar button:hover {
            background-color: #f0f0f0;
            transform: translateY(-2px);
        }

        .sidebar button i {
            font-size: 1.2rem;
        }

        .main-content {
            margin-left: 270px;
            padding: 30px;
        }

        .card {
            border: none;
            border-radius: 10px;
            box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
            transition: transform 0.3s ease, box-shadow 0.3s ease;
        }

        .card:hover {
            transform: translateY(-5px);
            box-shadow: 0 6px 15px rgba(0, 0, 0, 0.2);
        }

        .card-header {
            background-color: #d32f2f;
            color: white;
            border-radius: 10px 10px 0 0;
        }

        .card-title {
            font-size: 1.5rem;
            font-weight: 600;
        }

        .card-body {
            padding: 20px;
        }

        .symbol img {
            border-radius: 10px;
        }

        .btn-primary {
            background-color: #d32f2f;
            border: none;
            transition: background-color 0.3s ease;
        }

        .btn-primary:hover {
            background-color: #b71c1c;
        }

        .btn-secondary {
            background-color: #6c757d;
            border: none;
            transition: background-color 0.3s ease;
        }

        .btn-secondary:hover {
            background-color: #5a6268;
        }

        .fw-semibold {
            font-weight: 600;
        }

        .text-muted {
            color: #6c757d !important;
        }
    </style>


</head>
<body>
    <div class="sidebar">
        <button>
            <a href="home.php" class="active">
                <i class="fas fa-home"></i> Home
            </a>
        </button>

        <button>
            <a href="profile.php">
                <i class="fas fa-user"></i> Profile
            </a>
        </button>

        <button>
            <a href="lesson.php">
                <i class="fas fa-book"></i> Lesson
            </a>
        </button>

        <button onclick="logout()">
            <i class="fas fa-sign-out-alt"></i>
            Logout
        </button>
    </div>

    <div class="main-content">
        <div class="card mb-5">
            <div class="card-body">
                <div class="d-flex flex-wrap flex-sm-nowrap align-items-center">
                    <div class="me-7 mb-4">
                        <div class="symbol symbol-100px symbol-lg-160px">
                            <div class="avatar">
                                <?php
                                    echo '<img src="../assets/img/' . $user['avatar'] . '" alt="Student" class="img-thumbnail" />';
                                ?>
                            </div>
                        </div>
                    </div>
                    <div class="flex-grow-1">
                        <div class="d-flex justify-content-between align-items-start flex-wrap mb-2">
                            <div class="d-flex flex-column">
                                <div class="d-flex align-items-center mb-2">
                                    <span class="text-gray-900 text-hover-primary fs-2 fw-bold me-1">
                                        <?php
                                            echo $user['real_name'];
                                        ?>
                                    </span>
                                </div>
                                <div class="d-flex flex-wrap fw-semibold fs-6 mb-4 pe-2">
                                    <span class="d-flex align-items-center text-gray-500 text-hover-primary me-5 mb-2">
                                        <i class="fas fa-id-card fs-4 me-1"></i> 
                                        <?php
                                            echo $user['id'];
                                        ?>
                                    </span>
                                    <span class="d-flex align-items-center text-gray-500 text-hover-primary me-5 mb-2">
                                        <i class="fas fa-phone fs-4 me-1"></i>
                                        <?php
                                            echo $user['phone'];
                                        ?>
                                    </span>
                                    <span class="d-flex align-items-center text-gray-500 text-hover-primary mb-2">
                                        <i class="fas fa-envelope fs-4 me-1"></i> 
                                        <?php
                                            echo $user['email'];
                                        ?>
                                    </span>
                                </div>
                            </div>
                            <?php
                                if ($_SESSION['is_teacher']) {
                                    echo '<div class="d-flex my-4"><a class="btn btn-sm btn-primary me-3" href="../assets/php_process/view_profile_utils.php?name=' . $user['username'] . '&status=update">Update Profile</a></div>';
                                    echo '<div class="d-flex my-4"><a class="btn btn-sm btn-primary me-3" href="../assets/php_process/view_profile_utils.php?name=' . $user['username'] . '&status=delete">Delete Profile</a></div>';
                                }
                            ?>
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <!-- Profile Details -->
        <div class="card mb-5">
            <div class="card-header">
                <div class="card-title m-0">
                    <i class="fas fa-graduation-cap" ></i> Profile Details
                    
                </div>
            </div>
            <div class="card-body p-9">
                <div class="row mb-7">
                    <label class="col-lg-4 fw-semibold text-muted">Full name</label>
                    <div class="col-lg-8"><span class="fw-bold fs-6 text-gray-800">
                        <?php
                            echo $user['real_name'];
                        ?>
                    </span></div>
                </div>
                <div class="row mb-7">
                    <label class="col-lg-4 fw-semibold text-muted">Student ID</label>
                    <div class="col-lg-8"><span class="fw-bold fs-6 text-gray-800">
                        <?php
                            echo $user['id'];
                        ?>
                    </span></div>
                </div>
                <div class="row mb-7">
                    <label class="col-lg-4 fw-semibold text-muted">Username</label>
                    <div class="col-lg-8"><span class="fw-bold fs-6 text-gray-800">
                        <?php
                            echo $user['username'];
                        ?>
                    </span></div>
                </div>
                <div class="row mb-7">
                    <label class="col-lg-4 fw-semibold text-muted">Email</label>
                    <div class="col-lg-8"><span class="fw-bold fs-6 text-gray-800">
                        <?php
                            echo $user['email'];
                        ?>
                    </span></div>
                </div>

                <div class="row mb-7">
                    <label class="col-lg-4 fw-semibold text-muted">Phone</label>
                    <div class="col-lg-8"><span class="fw-bold fs-6 text-gray-800">
                        <?php
                            echo $user['phone'];
                        ?>
                    </span></div>
                </div>

            </div>
        </div>
    </div>

    <script>
        function logout() {
            Swal.fire({
                title: 'Are you sure?',
                text: "You will be logged out!",
                icon: 'warning',
                showCancelButton: true,
                confirmButtonColor: '#3085d6',
                cancelButtonColor: '#d33',
                confirmButtonText: 'Yes, log me out!'
            }).then((result) => {
                if (result.isConfirmed) {
                    location.href = 'logout.php';
                }
            });
        }
    </script>
</body>
</html>