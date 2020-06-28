#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CLIENTS 3
#define PORT 1337
#define BUFFER_LEN 256

volatile sig_atomic_t was_sighup = 0;

void sighup_handler(int signal) {
	was_sighup = 1;
}

int main(void) {
	printf("Server's PID: %d\n", getpid());

	int socket_fd;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		perror("Couldn't create socket");
		exit(1);
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) != 0) {
		perror("Couldn't bind socket");
		exit(1);
	}
	listen(socket_fd, MAX_CLIENTS);
	printf("Server listen at 0.0.0.0:%d\n", PORT);


	struct sigaction sa;
	sigaction(SIGHUP, NULL, &sa);
	sa.sa_handler = sighup_handler;
	sa.sa_flags |= SA_RESTART;
	sigaction(SIGHUP, &sa, NULL);

	sigset_t mask;
	sigset_t sighup_mask;
	sigemptyset(&sighup_mask);
	sigaddset(&sighup_mask, SIGHUP);
	sigprocmask(SIG_BLOCK, &sighup_mask, &mask);


	int clients[MAX_CLIENTS] = {0};
	int clients_num = 0;

	char data[BUFFER_LEN] = {0};

	while (1) {
		int nfds = socket_fd;
		fd_set fds;

		FD_ZERO(&fds);
		FD_SET(socket_fd, &fds);
		for (int i = 0; i < clients_num; i++) {
			FD_SET(clients[i], &fds);

			if (clients[i] > nfds) {
				nfds = clients[i];
			}
		}

		int res = pselect(nfds + 1, &fds, NULL, NULL, NULL, &mask);
		if (res < 0 && errno != EINTR) {
			perror("pselect error");
			exit(1);
		}

		if (was_sighup) {
			printf("Signal received\n");
			was_sighup = 0;
			break;
		}

		if (FD_ISSET(socket_fd, &fds)) {
			int conn = accept(socket_fd, NULL, NULL);
			if (conn < 0) {
				perror("Couldn't accept new connection");
				continue;
			}

			if (clients_num + 1 > MAX_CLIENTS) {
				printf("Too many clients\n");
				close(conn);
				continue;
			}

			clients[clients_num] = conn;
			clients_num++;

			printf("New client connected: %d\n", conn);
		}

		for (int i = 0; i < clients_num; i++) {
			if (FD_ISSET(clients[i], &fds)) {
				memset(&data, 0, BUFFER_LEN);

				ssize_t n = read(clients[i], &data, BUFFER_LEN - 1);

				if (n > 0) {
					n--;
					data[n] = '\0';
				}

				if (n <= 0 || !strcmp("quit", data)) {
					close(clients[i]);
					printf("Client %d closed connection\n", clients[i]);

					clients[i] = clients[clients_num - 1];
					clients[clients_num - 1] = 0;

					clients_num--;
					i--;

					continue;
				}

				printf("Client %d sent %ld bytes: %s\n", clients[i], n, data);
			}
		}
	}

	close(socket_fd);

	return EXIT_SUCCESS;
}
