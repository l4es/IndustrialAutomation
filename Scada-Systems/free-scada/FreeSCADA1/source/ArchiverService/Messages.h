//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: ERR_CREATE_THREAD
//
// MessageText:
//
//  Ошибка при создании потока.
//
#define ERR_CREATE_THREAD                0xC0000001L

//
// MessageId: ERR_DBACCESSOR
//
// MessageText:
//
//  Ошибка при работе с FreeSCADA DBAccessor.
//
#define ERR_DBACCESSOR                   0xC0000002L

//
// MessageId: ERR_OPEN_DATABASE
//
// MessageText:
//
//  Ошибка при открытии базы данных. Проверьте правильность ввода имени пользователя и пароля.
//  Убедитесь в корректности базы данных (существует ли, сохранена ли целостность).
//
#define ERR_OPEN_DATABASE                0xC0000003L

//
// MessageId: ERR_GET_CHANNELS
//
// MessageText:
//
//  Ошибка при доступе к данным каналов (интерфейс IChanels).
//
#define ERR_GET_CHANNELS                 0xC0000004L

//
// MessageId: ERR_OPCCONNECT_OUTOFMEMORY
//
// MessageText:
//
//  Недостаточно памяти у OPC сервера.
//
#define ERR_OPCCONNECT_OUTOFMEMORY       0xC0000005L

//
// MessageId: ERR_OPCCONNECT_INVALIDARG
//
// MessageText:
//
//  Неправильные аргументы при вызове OPC сервера.
//
#define ERR_OPCCONNECT_INVALIDARG        0xC0000006L

//
// MessageId: ERR_OPCCONNECT_FAIL
//
// MessageText:
//
//  Ошибка на OPC сервере.
//
#define ERR_OPCCONNECT_FAIL              0xC0000007L

//
// MessageId: ERR_OPCCONNECT_FAIL_UNKN
//
// MessageText:
//
//  Неизвестная ошибка на OPC сервере.
//
#define ERR_OPCCONNECT_FAIL_UNKN         0xC0000008L

//
// MessageId: ERR_OPCCONNECT_CANT_CREATE_INSTANCE
//
// MessageText:
//
//  Невозможно подключиться к OPC серверу. Сервер не найден.
//
#define ERR_OPCCONNECT_CANT_CREATE_INSTANCE 0xC0000009L

//
// MessageId: ERR_OPCCONNECT_CANT_QUERY_IOPCSERVER
//
// MessageText:
//
//  Невозможно подключиться к OPC серверу. На сервере отсутствует стандартный интерфейс IOPCServer.
//
#define ERR_OPCCONNECT_CANT_QUERY_IOPCSERVER 0xC000000AL

//
// MessageId: ERR_OPCCONNECT_CANT_ADD_GROUP
//
// MessageText:
//
//  Невозможно добавить группу на OPC сервере.
//
#define ERR_OPCCONNECT_CANT_ADD_GROUP    0xC000000BL

//
// MessageId: ERR_OPCCONNECT_CANT_QUERY_IOPCITEMMGT
//
// MessageText:
//
//  На сервере отсутствует стандартный интерфейс IOPCItemMgt.
//
#define ERR_OPCCONNECT_CANT_QUERY_IOPCITEMMGT 0xC000000CL

//
// MessageId: ERR_OPCCONNECT_CANT_SUBSCRIBE_CALLBACK
//
// MessageText:
//
//  На сервере не реализован механизм Connection Point.
//
#define ERR_OPCCONNECT_CANT_SUBSCRIBE_CALLBACK 0xC000000DL

//
// MessageId: ERR_OPCCONNECT_ACCESSDENIED
//
// MessageText:
//
//  Неудается установить соединение с сервером. Отказано в доступе.
//
#define ERR_OPCCONNECT_ACCESSDENIED      0xC000000EL

//
// MessageId: ERR_TERMINATE_THREAD
//
// MessageText:
//
//  Поток был принудительно завершен.
//
#define ERR_TERMINATE_THREAD             0x80000001L

//
// MessageId: ERR_OPCCONNECT_INVALIDITEMID
//
// MessageText:
//
//  Неправильный ItemID (синтаксическая ошибка) при подключении к OPC серверу.
//
#define ERR_OPCCONNECT_INVALIDITEMID     0x80000002L

//
// MessageId: ERR_OPCCONNECT_UNKNOWNITEMID
//
// MessageText:
//
//  Неизвестный ItemID для OPC сервера.
//
#define ERR_OPCCONNECT_UNKNOWNITEMID     0x80000003L

//
// MessageId: ERR_OPCCONNECT_LOST_CONNECTION
//
// MessageText:
//
//  Истек интервал ожидания данных с сервера. Производится переподключение.
//
#define ERR_OPCCONNECT_LOST_CONNECTION   0x80000004L

//
// MessageId: ERR_OPCCONNECT_TOO_FAST
//
// MessageText:
//
//  OPC сервер не может выдать данные с запрошенным интервалом обновления. 
//  Архиватор уменьшает скорость до допустимого минимума.
//
#define ERR_OPCCONNECT_TOO_FAST          0x80000005L

