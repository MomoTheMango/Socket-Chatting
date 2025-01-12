#include <iostream>
#include <winsock2.h>
#include <thread>

// 메세지 버퍼 크기
#define PACKET_SIZE 1024

// 소켓 선언
SOCKET client_socket;

using namespace std;

// 메세지 수신 함수 
void recv_msg(){
	char incoming_msg[PACKET_SIZE] = {};
	while(!WSAGetLastError()){
		ZeroMemory(&incoming_msg, PACKET_SIZE);
		recv(client_socket, incoming_msg, PACKET_SIZE, 0);
		cout << "Message from server: " << incoming_msg << '\n';
	}
}

int main(){

	// 윈도우 소켓 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		cout << "WSAStartup failed" << '\n';
		return 1;
	}

	// 소켓 생성
	client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 접속 주소 및 포트 설정 
	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// 서버에 연결 시도
	cout << "Establishing connection to server..." << '\n';
	while(1){
		if(connect(client_socket, (SOCKADDR*)&addr, sizeof(addr)) != SOCKET_ERROR)
			break;
	}

	// 연결 성공 안내
	cout << "Connection established" << '\n' << '\n';

	// 메세지 수신을 위한 스레드 생성
	thread receive_msg(recv_msg);

	// 서버로 메세지 전송송
	char msg[PACKET_SIZE] = {};
	while(!WSAGetLastError()){
		ZeroMemory(&msg, PACKET_SIZE);
		cin.getline(msg, PACKET_SIZE);
		send(client_socket, msg, strlen(msg), 0);
	}

	// 수신 스레드 종료 대기
	receive_msg.join();

	// 소켓 닫기
	closesocket(client_socket);

	// 리소스 해제
	WSACleanup();
	
	return 0;

}