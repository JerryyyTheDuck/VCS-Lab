<?php
    require_once 'assets/php_process/edit_utils.php';
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="assets/css/edit.css">
    <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@400;500;600&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/sweetalert2@11"></script>
    <title>Update Profile</title>
</head>
<body>
    <div class="sidebar">
          
        <button>
          <a href="home.php" class="active">
            <i class="fas fa-home"></i> Home
          </a>
        </button>
    
        <button>
            <a href="view_profile.php?name=<?php echo $_SESSION['user']; ?>">
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
        <div class="card">
            <div class="card-header">

            
            <h3 class="card-title">
                <i class="fas fa-user">
                </i>
                Update Profile
                </h3>   
            </div>

            <div class="card-body">
                <form id="submit" method="post" action="assets/php_process/edit_utils.php" enctype="multipart/form-data" >
                    <div class="form-group row mb-4">
                        <label class="col-md-3 col-form-label fw-bold">Profile Picture:</label>
                        <div class="col-md-9">
                            <div class="d-flex align-items-center">
                                <div class="me-3">
                                    <?php
                                    if ($user['avatar'] != null) {
                                        echo '<img src="assets/img/' . $user['avatar'] . '" alt="Profile Picture" class="rounded-circle" style="width: 100px; height: 100px; object-fit: cover;" />';
                                    } else {
                                        echo '<img src="assets/img/default.png" alt="Profile Picture" class="rounded-circle" style="width: 100px; height: 100px; object-fit: cover;" />';
                                    }
                                    ?>
                                </div>
                                <div>
                                    <input
                                        type="file"
                                        name="profile-picture"
                                        id="profile-picture"
                                        class="form-control"
                                        accept="image/*"
                                        onchange="previewImage(event)"
                                    />
                                    <small class="text-muted">Upload a new profile picture (JPEG, PNG, etc.)</small>
                                </div>
                            </div>
                        </div>
                    </div>

                    <div class="form-group row mb-4">
                        <label class="col-md-3 col-form-label fw-bold">Full name:</label>
                        <div class="col-md-9">
                            <?php
                                echo return_default_value('real_name', $user, $_SESSION['is_teacher']);
                            ?>
                        </div>
                    </div>

                    <!-- ID -->
                    <div class="form-group row mb-4">
                        <label class="col-md-3 col-form-label fw-bold">ID</label>
                        <div class="col-md-9">
                            <?php
                                echo return_default_value('id', $user, $_SESSION['is_teacher']);
                            ?>
                        </div>
                    </div>

                    <!-- Username -->
                    <div class="form-group row mb-4">
                        <label class="col-md-3 col-form-label fw-bold">Username</label>
                        <div class="col-md-9">
                            <?php
                                    echo return_default_value('username', $user, $_SESSION['is_teacher']);
                                ?>
                        </div>
                    </div>

                    <div class="form-group row mb-4">
                        <label class="col-md-3 col-form-label fw-bold">Phone Number</label>
                        <div class="col-md-9">
                            <?php
                                echo return_default_value('phone', $user, $_SESSION['is_teacher']);
                            ?>
                        </div>
                    </div>

                    <div class="form-group row mb-4">
                        <label class="col-md-3 col-form-label fw-bold">Email</label>
                        <div class="col-md-9">
                            <?php
                                echo return_default_value('email', $user, $_SESSION['is_teacher']);
                            ?>
                        </div>
                    </div>

                    <div class="form-group row mb-4">
                        <label class="col-md-3 col-form-label fw-bold">New password</label>
                            <div class="col-md-9">
                                <?php
                                    echo return_default_value('password', $user, $_SESSION['is_teacher']);
                                ?>
                            </div>
                    </div>

                    <div class="form-group row mb-4">
                        <label class="col-md-3 col-form-label fw-bold">Re-enter Password</label>
                        <div class="col-md-9">
                            <?php
                                echo return_default_value('re-password', $user, $_SESSION['is_teacher']);
                            ?>
                        </div>
                    </div>

                    <div class="form-group row mb-4">
                        <div class="col-md-9 offset-md-3">
                            <button type="submit" name="submit" class="btn btn-primary">
                                Save
                            </button>
                        </div>
                    </div>
                </form>
            </div>
        </div>
    </div>

    <script src="assets/js/edit.js"></script>
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
        })
    }
</script>
</body>
</html>