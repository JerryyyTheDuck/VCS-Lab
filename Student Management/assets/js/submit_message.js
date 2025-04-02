async function submitToPHP(event) {
    event.preventDefault();

    const recieve  = document.getElementById('recieve').value;
    const sent  = document.getElementById('sent').value;
    const message  = document.getElementById('message').value;

    const formData = new FormData();
    formData.append('recieve', recieve);
    formData.append('sent', sent);
    formData.append('message', message);
    try {
        const response = await fetch('../assets/php_process/submit_message_utils.php', {
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