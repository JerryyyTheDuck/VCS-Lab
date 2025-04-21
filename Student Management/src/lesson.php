<?php
    include 'assets/php_process/lesson_utils.php';
    include_once 'assets/php_process/connect_database.php';

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
        <div class="container">
        <div class="row">
            <div class="col-md-12">
            <h1>Lesson Management</h1>
            <?php
                if ($_SESSION['is_teacher'] == '1') {
                    echo '<a class="btn btn-primary" href="create_lesson.php">Create Course</a>';
                }
            ?>
            <table class="table table-striped">
                <thead>
                <tr>
                    <th scope="col">ID</th>
                    <th scope="col">Class Name</th>
                    <th scope="col">Description</th>
                </tr>
                </thead>
                <tbody>
                  <?php
                    $courses = list_course();
                    foreach ($courses as $index => $course) {
                        echo '<tr>';
                        echo '<th scope="row">' . ($index + 1) . '</th>';
                        echo '<td><a href="assets/php_process/lesson_utils.php?id='. $course['id'] .'">' . $course['course_name'] . '</a></td>';
                        if (strlen($course['description']) > 150) {
                            echo '<td>' . substr($course['description'], 0, 50) . '...</td>';
                        } else {
                            echo '<td>' . $course['description'] . '</td>';
                        }
                        echo '</tr>';
                    }
                    ?>
                </tbody>
            </table>
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