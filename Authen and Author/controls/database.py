import os
import bcrypt
from sqlalchemy.orm import sessionmaker
from sqlalchemy import Column, Integer, String, Boolean, create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.exc import SQLAlchemyError

Base = declarative_base()

class Users(Base):
    __tablename__ = "users"
    id = Column(Integer, primary_key=True, autoincrement=True)
    username = Column(String, unique=True, nullable=False)
    password = Column(String, nullable=False)
    is_admin = Column(Boolean, default=False)

class Database:
    def __init__(self):
        self.engine = create_engine("sqlite:///./user.db", echo=False)
        Base.metadata.create_all(self.engine)
        self.Session = sessionmaker(bind=self.engine)
        self.session = self.Session()

        self._create_admin_user()

    def _create_admin_user(self):
        """Create an admin user if none exists."""
        admin = self.session.query(Users).filter(Users.is_admin == True).first()
        if not admin:
            password = os.urandom(32).hex()
            with open("admin_password.txt", "w") as f:
                f.write(password)
            password_hash = self._hash_password(password)
            new_admin = Users(username="administrator", password=password_hash, is_admin=True)
            self.session.add(new_admin)
            self.session.commit()
            print(f"Admin user created with password: {password}")

    def _hash_password(self, password):
        """Hash a password using bcrypt."""
        password_bytes = password.encode("utf-8")
        salt = bcrypt.gensalt()
        return bcrypt.hashpw(password_bytes, salt).decode()

    def create_user(self, username, password):
        """Create a new user."""
        try:
            if self.session.query(Users).filter(Users.username == username).first():
                return False  # User already exists

            password_hash = self._hash_password(password)
            new_user = Users(username=username, password=password_hash)
            self.session.add(new_user)
            self.session.commit()
            return True
        except SQLAlchemyError as e:
            print(f"Error creating user: {e}")
            self.session.rollback()
            return False

    def get_user(self, username, password):
        """Retrieve a user if the username and password match."""
        try:
            user = self.session.query(Users).filter(Users.username == username).first()
            if not user:
                return None, False

            password_bytes = password.encode("utf-8")
            if bcrypt.checkpw(password_bytes, user.password.encode("utf-8")):
                print(user)
                return user, True
            return None, False
        except SQLAlchemyError as e:
            print(f"Error retrieving user: {e}")
            return None, False