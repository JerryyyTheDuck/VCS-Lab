import os
import re
import jwt

from controls.database import Database
from flask import Flask, make_response, redirect, render_template, request, session, url_for
from flask import send_from_directory
from werkzeug.utils import secure_filename


app = Flask(__name__)
app.secret_key = os.urandom(32)

JWT_ALG = "HS256"
JWT_KEY = "640ac4e02546bdee1a7fc487b970d3ff"
UPLOAD_FOLDER = 'tmp'
ALLOWED_EXTENSIONS = {'png', 'jpg', 'jpeg', 'gif', 'bmp', 'tiff', 'webp', 'svg','heic', 'heif'}

app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER

app.config['SESSION_COOKIE_HTTPONLY'] = True
app.config['SESSION_COOKIE_SECURE'] = True 
app.config['SESSION_COOKIE_SAMESITE'] = 'Lax'

@app.before_request
def before_request():
    if request.path in ["/login", "/register"]:
        return

    session.clear()

database = Database()


def create_jwt(user):
    header = {
        "alg": JWT_ALG,
        "typ": "JWT"
    }
    payload = {
        "login": True,
        "username": user.username,
        "is_admin": user.is_admin
    }
    return jwt.encode(payload, JWT_KEY, algorithm=JWT_ALG, headers=header)


def is_admin(jwt_payload):
    return jwt_payload.get('is_admin')


def allowed_file(filename):
    return '.' in filename and filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS


@app.route("/register", methods=["GET", "POST"])
def register():
    session.clear()
    if request.method == "GET":
        return render_template("verify.html", title="Register")

    if request.method == "POST":
        username = request.form.get("username")
        password = request.form.get("password")
        verify = request.form.get("confirm_password")
        if verify != password:
            return render_template("error.html", title="Error", error="Passwords do not match", redirect_url="/register"), 400
        
    status = database.create_user(username, password)
    if not status:
        session.clear()
        return render_template("error.html", title="Error", error=f"User {username} already exists",
                               redirect_url="/register"), 400
    else:
        return render_template("success.html", title="Create Successful",
                               message=f"User {username} created successfully", redirect_url="/login")


def validate_input(username, password):
    """Validate username and password."""
    if not username or not password:
        return False, "Missing username or password"

    if not re.match(r"^[a-zA-Z0-9_\-@.]+$", username) or not re.match(r"^[a-zA-Z0-9_\-@.!]+$", password):
        return False, "Invalid characters in username or password"

    return True, None


@app.route("/login", methods=["GET", "POST"])
def login():
    session.clear() 

    if request.method == "GET":
        return render_template("verify.html", title="Log-in")

    if request.method == "POST":
        username = request.form.get("username")
        password = request.form.get("password")

        is_valid, error_message = validate_input(username, password)
        if not is_valid:
            return render_template("error.html", title="Error", error=error_message, redirect_url="/login"), 400

        user, status = database.get_user(username, password)
        if not status or not user:
            return render_template("error.html", title="Error", error="Invalid credentials", redirect_url="/login"), 400

        token = create_jwt(user)
        response = make_response(redirect(url_for("home")))
        response.set_cookie(
            "token",
            token,
            httponly=True, 
            samesite="Strict"
        )
        return response

    return render_template("error.html", title="Error", error="Unknown error", redirect_url="/login"), 500


@app.route("/", methods=["GET", "POST"])
def index():
    session.clear()
    response = make_response(redirect(url_for("login")))
    response.set_cookie("token", "", expires=0)
    return response


@app.route("/logout", methods=["GET"])
def logout():
    session.clear()
    response = make_response(redirect(url_for("login")))
    response.set_cookie("token", "", expires=0)
    return response


@app.route("/home", methods=["GET", "POST"])
def home():
    token = request.cookies.get("token")
    if not token:
        return redirect(url_for("login"))

    try:
        jwt_payload = jwt.decode(token, JWT_KEY, algorithms=[JWT_ALG])
    except jwt.ExpiredSignatureError:
        return redirect(url_for("login"))
    except jwt.InvalidTokenError:
        return redirect(url_for("login"))

    if not jwt_payload.get("login"):
        return redirect(url_for("login"))

    image_files = []
    tmp_dir = "tmp"
    if os.path.exists(tmp_dir):
        for filename in os.listdir(tmp_dir):
            if allowed_file(filename):
                image_files.append(filename)
    return render_template("home.html", title="Welcome to Art Gallery", images=image_files, user=jwt_payload.get("username"))

@app.route('/tmp/<filename>')
def tmp_file(filename):
    return send_from_directory(UPLOAD_FOLDER, filename)


@app.route("/upload", methods=["GET", "POST"])
def upload_image():
    token = request.cookies.get("token")
    if not token:
        return redirect(url_for("login"))

    try:
        jwt_payload = jwt.decode(token, JWT_KEY, algorithms=[JWT_ALG])
    except jwt.ExpiredSignatureError:
        return redirect(url_for("login"))
    except jwt.InvalidTokenError:
        return redirect(url_for("login"))

    if not jwt_payload.get("login"):
        return redirect(url_for("login"))

    if not is_admin(jwt_payload):
        return render_template("error.html", title="Error", error="Not admin", redirect_url="/login"), 403

    if request.method == "GET":
        return render_template("admin.html")

    if request.method == "POST":
        file = request.files.get('file')
        if not file or file.filename == '':
            return render_template("error.html", title="Error", error="No file provided", redirect_url="/upload"), 400

        if not allowed_file(file.filename):
            return render_template("error.html", title="Error", error="Invalid file type", redirect_url="/upload"), 400

        try:
            filename = secure_filename(file.filename)
            new_filename = os.urandom(16).hex() + '.' + filename.rsplit('.', 1)[1].lower()
            file_path = os.path.join(UPLOAD_FOLDER, new_filename)
            file.save(file_path)

            return redirect(url_for("home", user=jwt_payload.get("username")))

        except Exception as e:
            return render_template("error.html", title="Error", error="Internal server error", redirect_url="/upload"), 500

    return render_template("error.html", title="Error", error="Method not allowed", redirect_url="/upload"), 405



if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=11223)
