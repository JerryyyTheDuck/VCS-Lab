<?php
    include 'assets/php_process/lesson_utils.php';
    include_once 'assets/php_process/connect_database.php';

    if (!isset($_SESSION['user']) || !isset($_SESSION['loggedin'])) {
        header('Location: login.php');
        exit();
    }

    if ($_SERVER['REQUEST_METHOD'] == 'GET') {
        $course_id = $_GET['course_id'];
        $course = get_course($course_id);
        if ($course == null) {
            header('Location: lesson.php');
            exit();
        }
    }
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Class Management</title>

    <!-- External CSS and Fonts -->
    <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@400;500;600&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css">
    <link rel="stylesheet" href="assets/css/home.css">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/sweetalert2@11"></script>
    <script src="assets/js/lesson_detail.js"></script>
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
            <?php echo $user['real_name'] ?? $user['username']; ?>
        </h2>

        <button><a href="home.php" class="active"><i class="fas fa-home"></i> Home</a></button>
        <button><a href="profile.php"><i class="fas fa-user"></i> Profile</a></button>
        <button><a href="lesson.php"><i class="fas fa-book"></i> Lesson</a></button>
        <button onclick="logout()"><i class="fas fa-sign-out-alt"></i> Logout</button>
    </div>

    
    <div class="main-content">
        <div class="col-md-12">
            <h1>Lesson Management</h1>
        </div>

        <!-- Course Details -->
        <div class="teacher-list">
            <h2><i class="fas fa-chalkboard-teacher"></i> <?php echo $course['course_name']; ?></h2>
            <div class="card border-0">
                <div class="card-body">
                    <div class="table-responsive">
                        <div class="mb-5">
                            <h4 class="mb-3 text-primary"><i class="fas fa-tasks me-2"></i> Task Description</h4>
                            
                            <div class="bg-light p-4 rounded">
                                <p class="mb-0"><?php echo htmlspecialchars($course['description']); ?></p>
                            </div>
                        </div>

                        <div>
                            <h4 class="mb-3 text-primary"><i class="fas fa-paperclip me-2"></i> Attachment</h4>
                            <div class="bg-light p-4 rounded">
                                <?php if ($course['attachment'] != null): ?>
                                    <a href="assets/courses/<?php echo htmlspecialchars($course['course_name']); ?>/<?php echo htmlspecialchars($course['attachment']); ?>" 
                                       class="btn btn-outline-primary btn-sm" 
                                       download>
                                        <i class="fas fa-download me-2"></i>
                                        <?php echo htmlspecialchars($course['attachment']); ?>
                                    </a>
                                <?php else: ?>
                                    <p class="mb-0 text-muted">No attachment</p>
                                <?php endif; ?>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <div class="teacher-list">
            <h2><i class="fas fa-book"></i> Submission</h2>
            <div class="card border-0">
                <div class="card-body">
                    <div class="table-responsive">
                    <table class="table table-striped table-rounded border gs-7 bg-light p-4 rounded">
                            <?php if ($_SESSION['is_teacher'] != 1): ?>
                              <form id="submit" method="post" action="assets/php_process/lesson_utils.php" enctype="multipart/form-data">
                                  <div class="form-group row mb-4">
                                      <div class="col-md-9">
                                          <div class="d-flex align-items-center">
                                              <input type="hidden" name="course_id" value="<?php echo htmlspecialchars($_GET['course_id']); ?>" />
                                              <div style="display: flex; justify-content: space-between; width: 100%;">
                                                  <input type="file" name="submission" id="submission" class="form-control" />
                                                  <button type="submit" name="submit" class="btn btn-primary" style="margin-left: 10%;">Create</button>
                                              </div>
                                          </div>
                                      </div>
                                  </div>
                              </form>
                            <?php else: ?>
                                <?php 
                                $count = 1;
                                $submissions = list_all_submission($course_id);

                                if ($submissions->num_rows > 0) {
                                    echo '<thead>
                                    <tr class="fw-semibold fs-6 text-gray-800 border-bottom border-gray-200">
                                        <th>No</th>
                                        <th>Full name</th>
                                        <th>Attachment</th>
                                    </tr>
                                    </thead>
                                    <tbody>';
                                    while ($submission = $submissions->fetch_assoc()) {
                                        $stmt = $conn->prepare("SELECT * FROM info WHERE id = ?");
                                        if (!$stmt) {
                                            die("Prepare failed: " . $conn->error);
                                        }
                                        $stmt->bind_param("s", $submission['student_id']);
                                        $stmt->execute();
                                        $result = $stmt->get_result();
                                
                                        if ($result->num_rows > 0) {
                                            $student_info = $result->fetch_assoc();
                                            $name = htmlspecialchars($student_info['real_name']); 
                                        } else {
                                            $name = "Unknown";
                                        }
                                        echo '<tr>
                                                <td>' . $count . '</td>
                                                <td>' . $name . '</td>
                                                <td>
                                                    <a href="assets/courses/'. htmlspecialchars($course['course_name']) . "/" . htmlspecialchars($submission['attachment']) . '" class="btn btn-outline-primary btn-sm" download>
                                                        <i class="fas fa-download me-2"></i>
                                                        ' . htmlspecialchars($submission['attachment']) . '
                                                    </a>
                                                </td>
                                              </tr>';
                                        $count++;
                                    }
                                } else {
                                    echo '<p class="mb-0 text-muted">No submission</p>';
                                }
                                ?>
                            <?php endif; ?>
                        </table>

                    </div>
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
        })
    }
  </script>
</body>
</html>
