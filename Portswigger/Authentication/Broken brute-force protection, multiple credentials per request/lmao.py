with open('pass.txt', 'r') as f:
    password = f.readlines() 

lmao = []

for i in password:
    lmao.append(i.strip().replace("\'","\""))
    
    
print(lmao)