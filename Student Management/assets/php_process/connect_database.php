<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "stuManager";


$conn = mysqli_connect($servername, $username, $password, $dbname);

if (!$conn) {
  die("Connection failed: " . mysqli_connect_error());
}

function select_all_student(){
  global $conn;
  $stmt = $conn->prepare("SELECT * FROM info WHERE username IN (SELECT username FROM account WHERE is_teacher = 0)");
  $stmt->execute();
  return $stmt->get_result();
}

function select_all_teacher(){
  global $conn;
  $stmt = $conn->prepare("SELECT * FROM info WHERE username IN (SELECT username FROM account WHERE is_teacher = 1)");
  $stmt->execute();
  return $stmt->get_result();
}

function select_someone($username){
  global $conn;
  $stmt = $conn->prepare("SELECT * FROM info WHERE username = ?");
  if (!$stmt) {
      die("Prepare failed: " . $conn->error);
  }
  $stmt->bind_param("s", $username);
  
  if($stmt->execute()){
      return $stmt->get_result()->fetch_assoc();
  } else {
      die("Execute failed: " . $stmt->error);
  }
  
}

function update_password($username, $password) {
  global $conn;

  $stmt = $conn->prepare("UPDATE account SET pass = ? WHERE username = ?");
  if (!$stmt) {
      die("Prepare failed: " . $conn->error);
  }

  $stmt->bind_param("ss", $password, $username);
  if ($stmt->execute()) {
      return true; 
  } else {
      die("Execute failed: " . $stmt->error);
  }
}

function update_info($username, $real_name, $phone, $email, $image) {
  global $conn;

  if ($_SESSION['is_teacher']) {
      $stmt = $conn->prepare("UPDATE info SET real_name = ?, phone = ?, email = ?, avatar = ? WHERE username = ?");
      if (!$stmt) {
          die("Prepare failed: " . $conn->error);
      }
      $stmt->bind_param("sssss", $real_name, $phone, $email, $image, $username);
  } else {
      $stmt = $conn->prepare("UPDATE info SET phone = ?, email = ?, avatar = ? WHERE username = ?");
      if (!$stmt) {
          die("Prepare failed: " . $conn->error);
      }
      $stmt->bind_param("ssss", $phone, $email , $image,$username);
  }

  if ($stmt->execute()) {
      return true;
  } else {
      die("Execute failed: " . $stmt->error);
  }
}

function delete_student($username) {
  global $conn;
  $stmt = $conn->prepare("DELETE FROM account WHERE username = ?");
  if (!$stmt) {
      die("Prepare failed: " . $conn->error);
  }
  $stmt->bind_param("s", $username);
  if ($stmt->execute()) {
      return true;
  } else {
      die("Execute failed: " . $stmt->error);
  }
}

function create_account($username, $password, $is_teacher) {
  global $conn;
  $is_teacher = (int)$is_teacher;
  $stmt = $conn->prepare("INSERT INTO account (username, pass, is_teacher) VALUES (?, ?, ?)");
  if (!$stmt) {
      die("Prepare failed: " . $conn->error);
  }
  $stmt->bind_param("ssi", $username, $password, $is_teacher);
  if ($stmt->execute()) {
      return true;
  } else {
      die("Execute failed: " . $stmt->error);
  }
}

function create_info($username, $real_name, $email, $phone, $ava){
    global $conn;
    $stmt = $conn->prepare("INSERT INTO info (username, real_name, email, phone, avatar) VALUES (?, ?, ?, ?, ?)");
    if (!$stmt) {
        die("Prepare failed: " . $conn->error);
    }
    $stmt->bind_param("sssss", $username, $real_name, $email, $phone, $ava);
    if ($stmt->execute()) {
        return true;
    } else {
        die("Execute failed: " . $stmt->error);
    }
}

function list_course(){
    global $conn;
    $stmt = $conn->prepare("SELECT * FROM course");
    $stmt->execute();
    return $stmt->get_result();
}

function create_course($course_name, $description, $file_name_new){
    global $conn;
    $stmt = $conn->prepare("INSERT INTO course (course_name, description, attachment) VALUES (?, ?, ?)");
    if (!$stmt) {
        die("Prepare failed: " . $conn->error);
    }
    $stmt->bind_param("sss", $course_name, $description, $file_name_new);
    if ($stmt->execute()) {
        return true;
    } else {
        die("Execute failed: " . $stmt->error);
    }
}

function get_course($id){
    global $conn;
    $stmt = $conn->prepare("SELECT * FROM course WHERE id = ?");
    if (!$stmt) {
        die("Prepare failed: " . $conn->error);
    }
    $stmt->bind_param("s", $id);
    if ($stmt->execute()) {
        return $stmt->get_result()->fetch_assoc();
    } else {
        die("Execute failed: " . $stmt->error);
    }
}

function create_submission($courseID, $studentID, $attachment){
    global $conn;

    $stmt = $conn->prepare("SELECT * FROM student_submits WHERE course_id = ? AND student_id = ?");
    if (!$stmt) {
        die("Prepare failed: " . $conn->error);
    }
    $stmt->bind_param("ss", $courseID, $studentID);
    $stmt->execute();
    $result = $stmt->get_result();
    if($result->num_rows == 0){
        $stmt = $conn->prepare("INSERT INTO student_submits (course_id, student_id, attachment) VALUES (?, ?, ?)");
        if (!$stmt) {
            die("Prepare failed: " . $conn->error);
        }
        $stmt->bind_param("sss", $courseID, $studentID, $attachment);
        if ($stmt->execute()) {
            return true;
        } else {
            die("Execute failed: " . $stmt->error);
        }
    }else{
        $stmt = $conn->prepare("UPDATE student_submits SET attachment = ? WHERE course_id = ? AND student_id = ?");
        if (!$stmt) {
            die("Prepare failed: " . $conn->error);
        }
        $stmt->bind_param("sss", $attachment, $courseID, $studentID);
        $original = explode('.',$result->fetch_assoc()['attachment']);
        $directory = "../courses/" . get_course($courseID)['course_name'] . "/";
        $pattern = $directory . $original[0] . "*";
        $files = glob($pattern);
        foreach($files as $file){
            if($file != $directory.$original[0].".".$original[1]){
                unlink($file);
            }
        }
        if ($stmt->execute()) {
            return true;
        } else {
            die("Execute failed: " . $stmt->error);
        }
    }
}


function list_all_submission($courseID){
    global $conn;
    $stmt = $conn->prepare("SELECT * FROM student_submits WHERE course_id = ?");
    if (!$stmt) {
        die("Prepare failed: " . $conn->error);
    }
    $stmt->bind_param("s", $courseID);
    $stmt->execute();
    return $stmt->get_result();
}

function create_message($content, $user_receive, $user_sent){
    global $conn;
    $stmt = $conn->prepare("INSERT INTO note (content, user_receive, user_sent) VALUES (?, ?, ?)");
    if (!$stmt) {
        die("Prepare failed: " . $conn->error);
    }
    $stmt->bind_param("sss", $content, $user_receive, $user_sent);
    if ($stmt->execute()) {
        return true;
    } else {
        die("Execute failed: " . $stmt->error);
    }
}

function message_sent($user_sent){
    global $conn;
    $stmt = $conn->prepare("SELECT * FROM note WHERE user_sent = ?");
    if (!$stmt) {
        die("Prepare failed: " . $conn->error);
    }
    $stmt->bind_param("s", $user_sent);
    $stmt->execute();
    return $stmt->get_result();
}

function message_receive($user_receive){
    global $conn;
    $stmt = $conn->prepare("SELECT * FROM note WHERE user_receive = ?");
    if (!$stmt) {
        die("Prepare failed: " . $conn->error);
    }
    $stmt->bind_param("s", $user_receive);
    $stmt->execute();
    return $stmt->get_result();
}

function delete_message($id){
    global $conn;
    $stmt = $conn->prepare("DELETE FROM note WHERE id = ?");
    if (!$stmt) {
        die("Prepare failed: " . $conn->error);
    }
    $stmt->bind_param("s", $id);
    if ($stmt->execute()) {
        return true;
    } else {
        die("Execute failed: " . $stmt->error);
    }
}

function edit_message($id, $content){
    global $conn;
    $stmt = $conn->prepare("UPDATE note SET content = ? WHERE id = ?");
    if (!$stmt) {
        die("Prepare failed: " . $conn->error);
    }
    $stmt->bind_param("ss", $content, $id);
    if ($stmt->execute()) {
        return true;
    } else {
        die("Execute failed: " . $stmt->error);
    }
}
?>