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

function update_info($username, $real_name, $phone, $email) {
  global $conn;

  if ($_SESSION['is_teacher']) {
      $stmt = $conn->prepare("UPDATE info SET real_name = ?, phone = ?, email = ? WHERE username = ?");
      if (!$stmt) {
          die("Prepare failed: " . $conn->error);
      }
      $stmt->bind_param("ssss", $real_name, $phone, $email, $username);
  } else {
      $stmt = $conn->prepare("UPDATE info SET phone = ?, email = ? WHERE username = ?");
      if (!$stmt) {
          die("Prepare failed: " . $conn->error);
      }
      $stmt->bind_param("sss", $phone, $email , $username);
  }

  if ($stmt->execute()) {
      return true;
  } else {
      die("Execute failed: " . $stmt->error);
  }
}

?>