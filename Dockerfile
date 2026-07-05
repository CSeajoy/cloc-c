FROM alpine:latest

RUN apk add --no-cache gcc musl-dev

WORKDIR /app

COPY main.c /app/

RUN gcc -Wall -Wextra -o /app/cloc /app/main.c

CMD ["/app/cloc", "/app"]
