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

    const file = document.getElementById('submission').files[0];
    const course_id = document.getElementById('course_id').value;

    const formData = new FormData();
    formData.append('course_id', course_id);
    if (file) {
        formData.append('submission', file);
    }else{
        showError('Please select a file to upload.');
        return;
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
        switch (data) {
            case 'upload_error':
                showError('An error occurred while uploading your file.');
            case 'invalid_file_type':
                showError('Invalid file type.');
            case 'file_too_big':
                showError('File is too big.');
            default:
                Swal.fire({
                    icon: 'success',
                    title: 'Upload Successful',
                    text: 'Your file has been submitted.',
                    confirmButtonText: 'OK'
                }).then((result) => {
                    if (result.isConfirmed) {
                        window.location.href = `lesson.php`;
                    }
                });
        }

    } catch (error) {
        console.error('Error:', error);
        showError('An error occurred while processing your request.');
    }
}

document.querySelector('submit').addEventListener('submit', function(event) {
    event.preventDefault();
    submitToPHP(event);
});