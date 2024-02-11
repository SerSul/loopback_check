#include <winsock2.h>
#include <iphlpapi.h>
#include <iostream>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

int main() {
    WSADATA wsaData;
    DWORD dwRetVal = 0;
    ULONG outBufLen = 15000;
    PIP_ADAPTER_ADDRESSES pAddresses = NULL, pCurrAddresses = NULL;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed." << std::endl;
        return 1;
    }

    do {
        pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
        if (pAddresses == NULL) {
            std::cout << "Memory allocation failed for IP_ADAPTER_ADDRESSES struct." << std::endl;
            return 1;
        }

        dwRetVal = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            free(pAddresses);
            pAddresses = NULL;
        }
        else {
            break;
        }
    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW));

    if (dwRetVal == NO_ERROR) {
        pCurrAddresses = pAddresses;
        bool loopbackFound = false;
        while (pCurrAddresses) {
            if (pCurrAddresses->IfType == IF_TYPE_SOFTWARE_LOOPBACK) {
                loopbackFound = true;
                std::cout << "Loopback Interface found: " << pCurrAddresses->AdapterName << std::endl;
                break;
            }
            pCurrAddresses = pCurrAddresses->Next;
        }

        if (!loopbackFound) {
            std::cout << "No Loopback Interface found." << std::endl;
        }
    }
    else {
        std::cout << "GetAdaptersAddresses failed with error: " << dwRetVal << std::endl;
    }

    if (pAddresses) {
        free(pAddresses);
    }

    WSACleanup(); 
    return 0;
}
