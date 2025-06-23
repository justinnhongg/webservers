IMAGE_NAME = webserver
CONTAINER_NAME = webserver_container
PORT = 4433

build:
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
