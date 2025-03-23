<?php
    session_start();

    include 'connect_database.php';

    if (!isset($_SESSION['user']) || !isset($_SESSION['loggedin'])) {
        header('Location: login.php');
        exit();
    }

    $user = select_someone($_SESSION['user']);
    
    function upload_lesson($course_name, $description){
        if (isset($_FILES['file-sample'])) {
            if($_FILES['file-sample']['error'] == 4) {
                create_course($course_name, $description, null);
                header('Location: ../../lesson.php');
                exit();
            }
            $file = $_FILES['file-sample'];
            $file_name = $file['name'];
            $file_tmp = $file['tmp_name'];
            $file_size = $file['size'];
            $file_error = $file['error'];
            $file_type = $file['type'];
            $file_ext = explode('.', $file_name);
            $file_actual_ext = strtolower(end($file_ext));
            $allowed = array('pdf', 'docx', 'pptx', 'xlsx', 'txt');

            if (in_array($file_actual_ext, $allowed)) {
                if ($file_error === 0) {
                    if ($file_size < 10000000) { 
                        if(!file_exists('../courses/')) {
                            mkdir('../courses/');
                        }
                        if(!file_exists('../courses/' . $course_name)) {
                            mkdir('../courses/' . $course_name);
                        }
                        $file_name_new = "download_task" . "." . $file_actual_ext;
                        $file_destination = '../courses/' . $course_name . '/' . $file_name_new;
                        move_uploaded_file($file_tmp, $file_destination);
                        create_course($course_name, $description, $file_name_new);
                    } else {
                        echo 'file_too_big';
                        header('Location: ../../lesson.php');
                        exit();
                    }
                } else {
                    echo 'upload_error';
                    header('Location: ../../lesson.php');
                    exit();
                }
            } else {
                echo 'invalid_file_type';
                header('Location: ../../lesson.php');
                exit();
            }
        }
    }

    function upload_submission(){
        global $user;
        if (isset($_FILES['submission'])) {
            if($_FILES['submission']['error'] == 4) {
                header('Location: ../../lesson.php');
                exit();
            }
            $course_id = $_POST['course_id'];
            $course = get_course($course_id);
            $course_name = $course['course_name'];  
            $file = $_FILES['submission'];
            $file_name = $file['name'];
            $file_tmp = $file['tmp_name'];
            $file_size = $file['size'];
            $file_error = $file['error'];
            $file_type = $file['type'];
            $file_ext = explode('.', $file_name);
            $file_actual_ext = strtolower(end($file_ext));
            $allowed = array('pdf', 'docx', 'pptx', 'xlsx', 'txt');

            if (in_array($file_actual_ext, $allowed)) {
                if ($file_error === 0) {
                    if ($file_size < 10000000) { 
                        if(!file_exists('../courses/')) {
                            mkdir('../courses/');
                        }
                        if(!file_exists('../courses/' . $course_name)) {
                            mkdir('../courses/' . $course_name);
                        }
                        $file_name_new = $user['id'] . "." . $file_actual_ext;
                        $file_destination = '../courses/' . $course_name . '/' . $file_name_new;
                        move_uploaded_file($file_tmp, $file_destination);
                        create_submission($course_id, $user['id'], $file_name_new, TRUE);
                    } else {
                        echo 'file_too_big';
                        header('Location: ../../lesson.php');
                        exit();
                    }
                } else {
                    echo 'upload_error';
                    header('Location: ../../lesson.php');
                    exit();
                }
            } else {
                echo 'invalid_file_type';
                header('Location: ../../lesson.php');
                exit();
            }
        }
    }


    if($_SERVER['REQUEST_METHOD'] == 'POST') {
        if($_POST['course_name'] == null || $_POST['description'] == null) {
            upload_submission();
            header('Location: ../../lesson.php');
        }else{
            $course_name = $_POST['course_name'];
            $description = $_POST['description'];
            upload_lesson($course_name, $description);
            header('Location: ../../lesson.php');
        }
    }

    if($_SERVER['REQUEST_METHOD'] == 'GET') {
        if (isset($_GET['id'])) {
            $course = get_course($_GET['id']);
            if($course == null) {
                header('Location: ../../lesson.php');
                exit();
            }

            header('Location: ../../lesson_detail.php?course_id=' . $course['id']);            
        }
    }

?>