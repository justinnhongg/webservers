FROM alpine
WORKDIR /webserver
RUN apk add --no-cache clang musl-dev
COPY webserver.c .
COPY webroot/index.html .
RUN clang -o webserver webserver.c
EXPOSE 8080
CMD ./webserver

