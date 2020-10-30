#include <iostream>
#include <windows.h>
#include <stdio.h>

int main() {
    char filename[] = "tempMappingFile.txt";
    char data[1024];

    // Получаем данные для файла
    std::cout << "Enter data: " << std::endl;
    std::cin >> data;

    // Создаем файл
    HANDLE fileHandle = CreateFile(
            filename,
            GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
    );

    if (fileHandle != INVALID_HANDLE_VALUE) {
        std::cout << "*** tempMappingFile.txt was created successfully ***" << std::endl;
    }

    // Получаем дескриптор глобального объекта "проецируемый файл" (проекции файла)
    HANDLE mapFileHandle = CreateFileMapping(fileHandle, nullptr, PAGE_READWRITE, 0, 1024, "mappedFile");

    if (mapFileHandle != INVALID_HANDLE_VALUE) {
        std::cout << "*** File mapping was created successfully ***" << std::endl;
    }

    /*
     * Делаем "проекцию в память".
     * При этом получаем регион в вирутальном адресном пространстве.
     * Функция вернет стартовый адрес региона ко всем страницам которого можно обращаться непосредственно.
     * Страницы в состоянии committed (загрузка в реальную память происходит по мере необходимости).
     */
    LPVOID mapAddress = MapViewOfFile(mapFileHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    // Копируем данные
    memcpy(mapAddress, data, strlen(data));
    std::cout << "Map Address: " << mapAddress << std::endl;

    // Возвращаем ресурсы системе
    char answer;
    std::cout << "Close Mapping? (y/n) ";
    std::cin >> answer;
    // "Разрушаем" связь спроецированного региона и его ресурсом
    if (answer == 'y') {
        UnmapViewOfFile(mapAddress);
    }

    // Закрываем HANDLE'ы
    CloseHandle(mapFileHandle);
    CloseHandle(fileHandle);

    return 0;
}
