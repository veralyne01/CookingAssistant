#include <windows.h>
#include <winhttp.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#pragma comment(lib, "winhttp.lib")
#include <nlohmann/json.hpp>
using json = nlohmann::json;
class SQLManager {
    void fill_database() {
        std::string name;
        std::getline(std::cin, name);
        std::wstring wStr(name.begin(), name.end());

        HINTERNET session = WinHttpOpen(
            L"CookingAssistant/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0
        );
        HINTERNET connection = WinHttpConnect(
            session,
            L"www.themealdb.com",
            INTERNET_DEFAULT_HTTPS_PORT,
            0
        );
        std::wstring search_string = L"/api/json/v1/1/search.php?s=" + wStr;
        HINTERNET request = WinHttpOpenRequest(
            connection,
            L"GET",
            search_string.c_str(),
            nullptr,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE
        );
        std::string response_data;
        bool success = false;

        if (WinHttpSendRequest(
            request,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0,
            WINHTTP_NO_REQUEST_DATA,
            0,
            0,
            0
        ) &&
            WinHttpReceiveResponse(request, nullptr))
        {
            DWORD statusCode = 0;
            DWORD statusSize = sizeof(statusCode);

            if (WinHttpQueryHeaders(
                request,
                WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                WINHTTP_HEADER_NAME_BY_INDEX,
                &statusCode,
                &statusSize,
                WINHTTP_NO_HEADER_INDEX
            ) && statusCode == 200)
            {
                DWORD available = 0;

                while (WinHttpQueryDataAvailable(request, &available)
                    && available > 0)
                {
                    std::string buffer(available, '\0');
                    DWORD downloaded = 0;

                    if (!WinHttpReadData(
                        request,
                        buffer.data(),
                        available,
                        &downloaded
                    ))
                    {
                        break;
                    }

                    response_data.append(buffer.data(), downloaded);
                }
                std::cout << "Response size: " << response_data.size() << '\n';
                std::cout << response_data << '\n';
                success = !response_data.empty();
            }
        }

        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);

        if (!success)
        {
            std::cerr << "Failed to download JSON\n";
            return;
        }

        std::ofstream file("recipes.json", std::ios::binary);

        if (!file)
        {
            std::cerr << "Failed to create recipes.json\n";
            return;
        }

        file << response_data;

        if (!file)
        {
            std::cerr << "Failed to write JSON file\n";
            return;
        }

        std::cout << "JSON saved to recipes.json\n";
        file.close();
        std::ifstream file2("recipes.json", std::ios::binary);
        std::ostringstream buffer;
        buffer << file2.rdbuf();
        std::string str_data = buffer.str();
        json data = json::parse(str_data);
        const auto& meals = data.at("meals");
        if (meals.is_null() || meals.empty())
        {
            std::cout << "Блюдо не найдено\n";
            return;
        }
        std::cout << "Meal name: " << meals.at(0).at("strMeal").get<std::string>() << '\n';
        std::cout << "Recipe: " << meals.at(0).at("strInstructions").get<std::string>() << '\n';
    }
};