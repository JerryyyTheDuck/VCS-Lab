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

function create_submission(){
    
}

?>