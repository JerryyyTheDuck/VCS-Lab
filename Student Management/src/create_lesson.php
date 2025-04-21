<?php
    include 'assets/php_process/lesson_utils.php';

?>

<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Class Management</title>
  <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@400;500;600&display=swap" rel="stylesheet">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css">
  <link rel="stylesheet" href="assets/css/home.css">
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet">

  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js"></script>
  <script src="https://cdn.jsdelivr.net/npm/sweetalert2@11"></script>
  <script src="assets/js/create_lessons.js"></script>
</head>
<body>
  <div class="sidebar">
    <div class="avatar">
      <?php
      if ($user['avatar'] != null) {
          echo '<img src="assets/img/' . $user['avatar'] . '" />';
      } else {
          echo '<img src="assets/img/default.png" />';
      }
      ?>
    </div>
    
    <h2>
      <?php
      echo $user['real_name'] ?? $user['username'];
      ?>
    </h2>

   
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
                <i class="fas fa-book">
                </i>
                CREATE LESSON
                </h3>   
            </div>

            <div class="card-body">
                <form id="submit" method="post" action="assets/php_process/lesson_utils.php" enctype="multipart/form-data" >

                    <div class="form-group row mb-4">
                        <label class="col-md-3 col-form-label fw-bold">Course name</label>
                        <div class="col-md-9">
                            <input name='course_name' type='text' id='course_name' class='form-control' required/>
                        </div>
                    </div>

                    <div class="form-group row mb-4">
                        <label class="col-md-3 col-form-label fw-bold">Description</label>
                        <div class="col-md-9">
                            <input name='description' type='text' id='description' class='form-control' required/>
                        </div>
                    </div>

                    <div class="form-group row mb-4">
                        <label class="col-md-3 col-form-label fw-bold">Upload Sample:</label>
                        <div class="col-md-9">
                            <div class="d-flex align-items-center">
                                <div>
                                    <input
                                        type="file"
                                        name="file-sample"
                                        id="file-sample"
                                        class="form-control"
                                    />
                                    <small class="text-muted">Upload file for Students</small>
                                </div>
                            </div>
                        </div>
                    </div>

                    <div class="form-group row mb-4">
                        <div class="col-md-9 offset-md-3">
                            <button type="submit" name="submit" class="btn btn-primary" >
                                Create
                            </button>
                        </div>
                    </div>
                </form>
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
        })
    }
</script>
</body>
</html>