FROM nvidia/cuda:12.9.1-devel-ubuntu22.04

RUN apt-get update && apt-get install -y \
    python3 python3-pip python3-venv python3-dev \
    git curl build-essential cmake ninja-build \
    libglib2.0-0 libsm6 libxext6 libxrender-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

RUN python3 -m venv .env
ENV VIRTUAL_ENV=/app/.env
ENV PATH="$VIRTUAL_ENV/bin:$PATH"

RUN pip install --upgrade pip setuptools wheel

COPY requirements.txt /app/requirements.txt

RUN pip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu121

RUN pip install --no-cache-dir -r requirements.txt

COPY app.py /app/app.py

CMD ["python3", "app.py"]
