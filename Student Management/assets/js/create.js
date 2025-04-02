function previewImage(event) {
    const input = event.target;
    const preview = document.querySelector('img.rounded-circle');

    if (input.files && input.files[0]) {
        const reader = new FileReader();

        reader.onload = function (e) {
            preview.src = e.target.result;
        };
        reader.readAsDataURL(input.files[0]);
    }
}

function showError(message) {
    console.log("Showing error:", message);
    Swal.fire({
        icon: 'error',
        title: 'Update Failed',
        text: message,
        confirmButtonText: 'Try Again'
    });
}

function showSuccess(message) {
    console.log("Showing success:", message);
    Swal.fire({
        icon: 'success',
        title: 'Update Successful',
        text: message,
        confirmButtonText: 'OK'
    });
}

async function submitToPHP(event) {
    event.preventDefault();

    const image = document.getElementById('profile-picture').files[0];
    const real_name = document.getElementById('real_name').value;
    const username = document.getElementById('username').value;
    const email = document.getElementById('email').value;
    const phone = document.getElementById('phone').value;
    const password = document.getElementById('password').value;
    const confirm_password = document.getElementById('re-password').value;
    const is_teacher = document.getElementById('is_teacher').checked;
    const formData = new FormData();


    formData.append('real_name', real_name);
    formData.append('username', username);
    formData.append('email', email);
    formData.append('phone', phone);
    formData.append('password', password);
    formData.append('confirm_password', confirm_password);
    formData.append('is_teacher', is_teacher)
    if (image) {
        formData.append('profile-picture', image);
    }

    try {
        const response = await fetch('../assets/php_process/create_utils.php', {
            method: 'POST',
            body: formData,
        });

        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }

        const data = await response.text();

        switch (data) {
            case 'invalid_phone':
                showError("Invalid phone number. Please follow the format 123-456-7890.");
                break;
            case 'invalid_email':
                showError('Invalid email address.');
                break;
            case 'invalid_password':
                showError('Invalid password.');
                break;
            case 'invalid_name':
                showError('Invalid full name. Only letters and spaces are allowed.');
                break;
            case 'invalid_username':
                showError('Invalid username. Only letters, numbers, and underscores are allowed.');
                break;
            case 'database_error':
                showError('An error occurred while processing your request.');
                break;
            default:
                showSuccess('Your account has been updated.');
                Swal.fire({
                    icon: 'success',
                    title: 'Update Successful',
                    text: 'Your account has been updated.',
                    confirmButtonText: 'OK'
                }).then((result) => {
                    if (result.isConfirmed) {
                        alert('edit.php?user=' + username)
                        window.location.href = 'edit.php?user=' + username;
                    }
                });
        }
    } catch (error) {
        console.error('Error:', error);
        showError('An error occurred while processing your request.');
    }
}


document.querySelector('#submit').addEventListener('submit', function(event) {
    event.preventDefault();
    submitToPHP(event);
});