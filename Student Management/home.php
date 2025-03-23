<?php
  include 'assets/php_process/home_utils.php';
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
    <h1>Class Management</h1>
    <h3 class="text-gray-800 fw-bold mb-4">VCS Passport - Incident Response</h3>
    
    <div class="student-list">
      <h2><i class="fas fa-graduation-cap"></i> Students

      <?php
        if ($_SESSION['is_teacher'] == '1') {
            echo '<a class="btn btn-sm btn-primary me-3" style="margin-left: auto;" href="create.php">Create New Users</a>';
        }
      ?>
      </h2>

      <div class="card">
        <div class="card-body">
            <div class="table-responsive">
                <table class="table table-striped table-rounded border gs-7">
                    <?php
                    if (!empty($students)) {
                        echo '<thead>
                                <tr class="fw-semibold fs-6 text-gray-800 border-bottom border-gray-200">
                                    <th>No</th>
                                    <th>Username</th>
                                    <th>Surname</th>
                                    <th>Middle Name</th>
                                    <th>Given Name</th>
                                    <th>Full Name</th>
                                </tr>
                              </thead>
                              <tbody>';
                        $count = 1;
                        foreach ($students as $student) {
                          list($first_name, $middle_name, $last_name, $name) = split_name($student['real_name']);
                      
                          echo '<tr class="">
                                  <td>' . $count . '</td>
                                  <td class="fw-bold text-primary">
                                      <a href="view_profile.php?name=' . $student['username'] . '"style="text-decoration:none;" >' . $student['username'] . '</a>
                                  </td>
                                  <td class="fw-semibold">' . $first_name . '</td>
                                  <td class="fw-semibold">' . $middle_name . '</td>
                                  <td class="fw-semibold">' . $last_name . '</td>
                                  <td class="fw-semibold">' . $name . '</td>
                                </tr>';
                          $count++;
                      }
                        echo '</tbody>';
                    } else {
                        echo '<h3>No student found</h3>';
                    }
                    ?>
                </table>
            </div>
        </div>
      </div>
    </div>

    <div class="teacher-list">
      <h2><i class="fas fa-chalkboard-teacher"></i> Teachers</h2>
      <div class="card">
        <div class="card-body">
            <div class="table-responsive">
                <table class="table table-striped table-rounded border gs-7">
                    <?php
                    if (!empty($teachers)) {
                        echo '<thead>
                                <tr class="fw-semibold fs-6 text-gray-800 border-bottom border-gray-200">
                                    <th>No</th>
                                    <th>Username</th>
                                    <th>Surname</th>
                                    <th>Middle Name</th>
                                    <th>Given Name</th>
                                    <th>Full Name</th>
                                </tr>
                              </thead>
                              <tbody>';
                        $count = 1;
                        foreach ($teachers as $teacher) {
                            list($first_name, $middle_name, $last_name, $real_name) = split_name($teacher['real_name']);
                            echo '<tr class="">
                                    <td>' . $count . '</td>
                                    <td class="fw-bold text-primary">
                                      <a href="view_profile.php?name=' . $teacher['username'] . '"style="text-decoration:none;" >' . $teacher['username'] . '</a>
                                  </td>

                                    <td class="fw-semibold">' . $first_name . '</td>
                                    <td class="fw-semibold">' . $middle_name . '</td>
                                    <td class="fw-semibold">' . $last_name . '</td>
                                    <td class="fw-semibold">' . $real_name . '</td>
                                  </tr>';
                            $count++;
                        }
                        echo '</tbody>';
                    } else {
                        echo '<h3>No teacher found</h3>';
                    }
                    ?>
                </table>
            </div>
        </div>
      </div>
    </div>
  </div>
</body>
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
</html>