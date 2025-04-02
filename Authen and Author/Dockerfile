FROM python:3.10-slim

RUN apt-get update && \
    apt-get install -y --no-install-recommends curl wget  supervisor build-essential && \
    rm -rf /var/lib/apt/lists/*

RUN adduser -u 1000 jerryyy
RUN chown jerryyy:jerryyy /tmp

RUN python -m pip install --upgrade pip
RUN python -m pip install --upgrade setuptools

USER root
RUN mkdir -p /app
WORKDIR /app
COPY requirements.txt /app/requirements.txt
RUN pip install -r requirements.txt

COPY . /app

EXPOSE 11223

ENV PYTHONDONTWRITEBYTECODE=1
ENV FLASK_APP=app.py
ENV FLASK_RUN_HOST=0.0.0.0
ENV FLASK_RUN_PORT=11223

CMD ["flask", "run"]
