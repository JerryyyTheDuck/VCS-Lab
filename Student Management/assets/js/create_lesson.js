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

    const file = document.getElementById('file-sample').files[0];
    const course_name = document.getElementById('course_name').value;
    const description = document.getElementById('description').value;

    const formData = new FormData();
    formData.append('course_name', course_name);
    formData.append('description', description);

    if (file) {
        formData.append('file-sample', file);
    }

    try {
        const response = await fetch('../assets/php_process/lesson_utils.php', {
            method: 'POST',
            body: formData,
        });

        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }

        const data = await response.json().text; 

        Swal.fire({
            icon: 'success',
            title: 'Update Successful',
            text: 'Your account has been updated.',
            confirmButtonText: 'OK'
        }).then((result) => {
            if (result.isConfirmed) {
                window.location.href = `edit.php?user=${username}`;
            }
        });

    } catch (error) {
        console.error('Error:', error);
        showError('An error occurred while processing your request.');
    }
}

document.querySelector('#submit').addEventListener('submit', function(event) {
    event.preventDefault();
    submitToPHP(event);
});