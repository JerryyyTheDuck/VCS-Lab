from flask import Flask, jsonify
from flask_httpauth import HTTPBasicAuth, HTTPDigestAuth

app = Flask(__name__)
app.config['SECRET_KEY'] = 'my_secret_key'  # Needed for Digest authentication

# Basic Authentication Setup
basic_auth = HTTPBasicAuth()

# Digest Authentication Setup
digest_auth = HTTPDigestAuth()

# Sample User Data
users = {
    "admin": "password123",
    "user": "mypassword"
}

@basic_auth.verify_password
def verify_basic(username, password):
    if username in users and users[username] == password:
        return username
    return None

@digest_auth.get_password
def get_digest_password(username):
    return users.get(username)

@app.route('/basic')
@basic_auth.login_required
def basic_auth_route():
    return jsonify({"message": f"Hello, {basic_auth.current_user()}! You used Basic Authentication."})

@app.route('/digest')
@digest_auth.login_required
def digest_auth_route():
    return jsonify({"message": f"Hello, {digest_auth.username()}! You used Digest Authentication."})

if __name__ == '__main__':
    app.run(debug=True)
