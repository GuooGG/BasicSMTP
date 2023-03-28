#pragma once
#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<string.h>
#include<WinSock.h>

void errexit(const char* format, ...);
SOCKET connectsock(const char* host, const char* service, const char* transport);
SOCKET connectudp(const char* host, const char* service);
SOCKET connecttcp(const char* host, const char* service);