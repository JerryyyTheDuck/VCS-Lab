function previewImage(event) {
    const reader = new FileReader();
    const profilePicture = document.getElementById('profile-picture');

    reader.onload = function() {
        profilePicture.src = reader.result;
    };

    if (event.target.files[0]) {
        reader.readAsDataURL(event.target.files[0]);
    }
}

function showError(message) {
    console.log("Showing error:", message); // Debugging
    Swal.fire({
        icon: 'error',
        title: 'Update Failed',
        text: message,
        confirmButtonText: 'Try Again'
    });
}

function showSuccess(message) {
    console.log("Showing success:", message); // Debugging
    Swal.fire({
        icon: 'success',
        title: 'Update Successful',
        text: message,
        confirmButtonText: 'OK'
    });
}

function submitToPHP(event) {
    event.preventDefault(); // Prevent the default form submission

    const real_name = document.getElementById('real_name').value;
    const username = document.getElementById('username').value;
    const id = document.getElementById('id').value;
    const email = document.getElementById('email').value;
    const phone = document.getElementById('phone').value;
    const password = document.getElementById('password').value;
    const confirm_password = document.getElementById('re-password').value;

    const data = {
        real_name: real_name,
        username: username,
        id: id,
        email: email,
        phone: phone,
        password: password,
        confirm_password: confirm_password
    };

    fetch('../assets/php_process/edit_utils.php', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(data),
    })
    .then(response => response.text())
    .then(data => {
        if (data === 'invalid_phone') {
            showError("Invalid phone number. Please follow the format 123-456-7890.");
        } else if (data === 'invalid_email') {
            showError('Invalid email address.');
        } else if (data === "invalid_password") {
            showError('Invalid password.'); 
        } else if (data === 'invalid_name') {
            showError('Invalid full name. Only letters and spaces are allowed.');
        } else if (data === 'invalid_username') {
            showError('Invalid username. Only letters, numbers, and underscores are allowed.');
        } else if (data === 'database_error') {
            showError('An error occurred while processing your request.');
        } else {
            showSuccess('Your account has been updated.');
            Swal.fire({
                icon: 'success',
                title: 'Update Successful',
                text: 'Your account has been updated.',
                confirmButtonText: 'OK'
            }).then((result) => {
                if (result.isConfirmed) {
                    window.location.href = 'edit.php';
                }
            });
        }
    }).catch(error => {
        showError('An error occurred while processing your request.');
    });
}

// Attach the submit event listener to the form
document.querySelector('#submit').addEventListener('submit', function(event) {
    console.log("Form submission intercepted"); // Debugging
    event.preventDefault(); // Prevent the default form submission
    submitToPHP(event); // Call your custom submit function
});