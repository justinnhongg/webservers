IMAGE_NAME = webserver
CONTAINER_NAME = webserver_container
PORT = 8080

build:
	docker build -t $(IMAGE_NAME) .

run:
	docker rm -f $(CONTAINER_NAME) 2>/dev/null || true
	docker run -d -p $(PORT):8080 --name $(CONTAINER_NAME) $(IMAGE_NAME)
	docker logs -f $(CONTAINER_NAME)

stop:
	docker stop $(CONTAINER_NAME)
	docker rm $(CONTAINER_NAME)

logs:
	docker logs -f $(CONTAINER_NAME)

exec:
	docker exec -it $(CONTAINER_NAME) /bin/sh

rebuild: stop build run
