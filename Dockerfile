FROM alpine
WORKDIR /webserver
RUN apk add --no-cache clang musl-dev openssl-dev
COPY . .
RUN clang -o webserver webserver.c -lssl -lcrypto
EXPOSE 4433
CMD ["./webserver"]
