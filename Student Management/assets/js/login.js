// Function to show SweetAlert for login errors
function showLoginError(message) {
    Swal.fire({
        icon: 'error',
        title: 'Login Failed',
        text: message,
        confirmButtonText: 'Try Again'
    });
}

// Function to handle form submission
function handleLoginFormSubmission(event) {
    event.preventDefault(); // Prevent the form from submitting normally

    const formData = new FormData(event.target);
    const user = formData.get('user');
    const pass = formData.get('pass');

    // Validate inputs
    if (!user || !pass) {
        showLoginError('Please fill in all fields.');
        return;
    }

    // Send data to the server using Fetch API
    fetch('login.php', {
        method: 'POST',
        body: formData
    })
    .then(response => response.text())
    .then(data => {
        if (data.includes('location.href')) {
            window.location.href = 'home.php';
        } else if (data === 'invalid_password') {
            showLoginError('Invalid password.');
        } else if (data === 'invalid_username') {
            showLoginError('Invalid username.');
        } else if (data === 'database_error') {
            showLoginError('An error occurred while processing your request.');
        } else {
            showLoginError('Unknown error occurred.');
        }
    })
    .catch(error => {
        console.error('Error:', error);
        showLoginError('An error occurred while processing your request.');
    });
}

document.addEventListener('DOMContentLoaded', function () {
    const loginForm = document.getElementById('login_form');
    if (loginForm) {
        loginForm.addEventListener('submit', handleLoginFormSubmission);
    }
});