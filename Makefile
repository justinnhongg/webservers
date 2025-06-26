IMAGE_NAME = webserver
CONTAINER_NAME = webserver_container
PORT = 4433
KEY = key.pem
CERT = cert.pem

all: build

$(CERT) $(KEY):
	openssl req -x509 -newkey rsa:2048 -nodes \
	    -keyout $(KEY) -out $(CERT) -days 365 \
	    -subj "/C=US/ST=NY/L=NYC/O=Example/OU=Dev/CN=localhost"

build: $(CERT) $(KEY)
	docker build -t $(IMAGE_NAME) .

run:
	docker rm -f $(CONTAINER_NAME) 2>/dev/null || true
	docker run -d -p 0.0.0.0:$(PORT):4433 --name $(CONTAINER_NAME) $(IMAGE_NAME)
	docker logs -f $(CONTAINER_NAME)

stop:
	docker rm -f $(CONTAINER_NAME)

logs:
	docker logs -f $(CONTAINER_NAME)

exec:
	docker exec -it $(CONTAINER_NAME) /bin/sh

rebuild: stop build run

clean:
	rm -f $(KEY) $(CERT)

