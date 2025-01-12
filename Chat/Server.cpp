#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

// 메세지 버퍼 크기기
#define PACKET_SIZE 1024

// 서버, 클라이언트트 소켓 선언
SOCKET server_socket, client_socket;

using namespace std;

// 메세지 수신 한수수
void recv_msg(){
	char incoming_msg[PACKET_SIZE] = {};
	while(!WSAGetLastError()){
		ZeroMemory(&incoming_msg, PACKET_SIZE);
		recv(client_socket, incoming_msg, PACKET_SIZE, 0);
		cout << "Message from client: " << incoming_msg << '\n';
	}
}

int main(){

	// 윈도우 소켓 초기화
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		cout << "WSAStartup Failed" << '\n';
		return 1;
	}

	// 서버 소켓 생성
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 접속 주소 및 포트 설정정
	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = htonl(INADDR_ANY); 

	// 서버 소켓에 주소 바인딩
	bind(server_socket, (SOCKADDR*)&addr, sizeof(addr));
	
	// 서버 소켓으로 연결 대기
	listen(server_socket, SOMAXCONN);

	// 클라이언트 주소 구조체
	SOCKADDR_IN client = {};
	int client_size = sizeof(client);

	// 클라이언트 연결 수락
	client_socket = accept(server_socket, (SOCKADDR*)&client, &client_size);

	// 클라이언트 연결 성공 시 정보 출력
	if(!WSAGetLastError()){
		cout << "Connection Established" << '\n';
		cout << "Client IP: " << inet_ntoa(client.sin_addr) << '\n';
		cout << "Port: " << ntohs(client.sin_port) << '\n' << '\n';
	}

	// 메세지 수신을 위한 스레드 생성 
	thread receive_msg(recv_msg);

	// 클라이언트로 메세지 전송
	char msg[PACKET_SIZE] = {0};
	while(!WSAGetLastError()){
		ZeroMemory(&msg, PACKET_SIZE);
		cin.getline(msg, PACKET_SIZE);
		send(client_socket, msg, strlen(msg), 0);
	}

	// 수신 스레드 종료 대기
	receive_msg.join();

	// 소켓 닫기
	closesocket(client_socket);
	closesocket(server_socket);

	// 리소스 해제
	WSACleanup();

    return 0;

}