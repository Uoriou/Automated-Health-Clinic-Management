FROM gcc:latest 

RUN apt-get update && apt-get install -y \
    g++ \
    libsqlite3-dev \
    git \
    build-essential \
    libboost-system-dev \
    libboost-thread-dev \
    libboost-date-time-dev \
    libasio-dev

WORKDIR /app

RUN git clone --depth=1 https://github.com/CrowCpp/Crow.git || \
(echo "Crow clone failed! Retrying..." && sleep 3 && git clone --depth=1 https://github.com/CrowCpp/Crow.git)
COPY . /app
RUN g++ -o myfirstprogram myfirstprogram.cpp -std=c++17 -pthread -l sqlite3 -I/app/Crow/include -I/usr/include/asio -DASIO_STANDALONE
RUN ls -l myfirstprogram && file myfirstprogram
EXPOSE 8080
CMD ["./myfirstprogram"]