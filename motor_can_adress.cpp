// motor_can_adress.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <iomanip>

#define ad0x18FFA0F3 0
#define ad0x18FFA1F3 1
#define ad0x18FFA2F3 2
#define ad0x18FFA3F3 3
#define ad0x18FFA4F3 4
#define ad0x18FFA5F3 5
#define ad0x18FFA6F3 6
#define ad0x18FFA7F3 7
#define ad0x18FFA8F3 8
#define ad0x18FFAEF3 9
#define ad0x18FFA9F3 10
#define ad0x18FFAAF3 11
#define ad0x18FFACF3 12
#define ad0x18FFAFF3 13
#define ad0x18FECAF3 14
#define ad0x1810A6A0 15
#define ad0x1820F8F4 16
#define ad0x1880F8F4 17
#define ad0x08F000A0 18
#define ad0x08F200A0 19 
#define ad0x041000A0 20
#define ad0x0C20A0A6 21
#define ad0x0CFF08EF 22
#define ad0x0CFF0008 23
#define ad0x0CFF0108 24
#define ad0x0CFF0208 25
#define ad0x0CFF0308 26
#define ad0x0CFF0408 27
#define ad0x0CFF0508 28


typedef struct {
    uint32_t adress;
    uint32_t message[8];
    uint32_t cycle; // время повторения посылки
    uint32_t time_cycle; // время обновления таймера, если 0, то аймера нет
    uint32_t timer_pos; //позиция первого бита в таймере. предполагается, что таймеры 4х битные.
    std::string comment;
}adress_t;
//массив для применения в процедурах выставления одного бита. Нулевой бит самый правый.
uint8_t bits_arr[8] = {
    0b00000001,
    0b00000010,
    0b00000100,
    0b00001000,
    0b00010000,
    0b00100000,
    0b01000000,
    0b10000000
};

typedef uint32_t mes_t[8];
std::vector <adress_t> m;

int printAdress(std::vector<adress_t> a, uint32_t adr) {
    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            //printMessage(a[i]);
            return 1;
        }
    }
    return 0;
}

int printMessageBin(std::vector<adress_t> a, uint32_t adr) {
    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            std::cout << "adr 0x" << std::hex << adr << ": ";
            for (int j = 0; j < 8; j++) {
                std::cout << "0b" << std::bitset<8>(a[i].message[j]) << ", ";
            }
            std::cout << std::endl;
            return 1;
        }
    }
    std::cout << "!!! error to print message in bin by adr = 0x" << std::hex << adr << std::endl;
    return 0;
}

int printMessageHex(std::vector<adress_t> a, uint32_t adr) {
    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            std::cout << "adr 0x" << std::hex << adr << ": ";
            for (int j = 0; j < 8; j++) {
                std::cout << "0x"<< std::hex << std::setfill('0') << std::setw(2) << a[i].message[j] << ", ";
            }
            std::cout << std::endl;
            return 1;
        }
    }
    std::cout << "!!! error to print message in hex by adr = 0x" << std::hex << adr << std::endl;
    return 0;
}

int setMessage(std::vector<adress_t> &a, uint32_t adr, uint32_t mes[8]) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            for (int j = 0; j < 8; j++) {
                a[i].message[j] = mes[j];
            }
            return 0;
        }
    }
    std::cout << "!!!error set msd on adr " << adr;
    return 1;
}

//устанавливает один байт
int setByte(std::vector<adress_t>& a, uint32_t adr, uint32_t byte_pos, uint32_t byte_value) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            a[i].message[byte_pos] = byte_value;
            return 0;
        }
    }
    std::cout << "!!!error set bit on adr " << adr;
    return 1;
}

int setBit(std::vector<adress_t>& a, uint32_t adr, uint32_t bit_pos, uint32_t byte_pos) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            a[i].message[byte_pos] |= bits_arr[bit_pos];
            return 0;
        }
    }
    std::cout << "!!!error set bit on adr " << adr;
    return 1;
}

int setLower4Bites(std::vector<adress_t>& a, uint32_t adr,  uint32_t byte_pos, uint8_t bits_value) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            a[i].message[byte_pos] |= bits_value;
            return 0;
        }
    }
    std::cout << "!!!error setLower4Bites on adr " << adr;
    return 1;
}

int setUpper4Bites(std::vector<adress_t>& a, uint32_t adr, uint32_t byte_pos, uint8_t bits_value) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            a[i].message[byte_pos] |= (bits_value * 0x10);
            return 0;
        }
    }
    std::cout << "!!!error setUpper4Bites on adr " << adr;
    return 1;
}
// 
int set2Bites(std::vector<adress_t>& a, uint32_t adr, uint32_t bite_pos, uint8_t bits_value) {
    if (bits_value > 3 || bits_value > 62){
        std::cout << "!!!error setUpper4Bites by input data on adr " << adr << std::endl;
    }
    uint32_t byte_pos = bite_pos / 8;
    uint32_t bit_start = bite_pos % 8;
    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            switch (bit_start){
            case 0: {
                a[i].message[byte_pos] |= bits_value;
                break;
            }
            case 2: {
                a[i].message[byte_pos] |= (bits_value << 2);
                break;
            }
            case 4: {
                a[i].message[byte_pos] |= (bits_value << 4);
                break;
            }
            case 6: {
                a[i].message[byte_pos] |= (bits_value << 6);
                break;
            }
            }
            return 0;
        }
    }
    std::cout << "!!!error setUpper4Bites on adr " << adr;
    return 1;
}


//меняет старший и младший байты местами. нужно для 16 битных посылок
std::pair<uint32_t, uint32_t>  swapByte(const uint32_t input) {
    uint32_t high_value = 0;
    uint32_t low_value = 0;
    std::pair<uint32_t, uint32_t> ret;
    high_value = input >> 8;//сдвигаем на 8 байт вправо и тем самым получаем старший байт
    low_value = input;
    low_value &= ~(0xff00);//отризаем старший байт и тем самым получаем младший байт
    ret.first = (low_value);
    ret.second =  (high_value);
    return ret;

}

//устанавливает 2 байта в подряд идушье ячейки сообщения. нужно для 16 битных сообщений
int setTwoByte(std::vector<adress_t>& a, uint32_t adr, uint32_t byte_pos, uint32_t byte_value) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            a[i].message[byte_pos] = swapByte(byte_value).first;
            a[i].message[byte_pos + 1] = swapByte(byte_value).second;
            return 0;
        }
    }
    std::cout << "!!!error set bit on adr " << adr;
    return 1;
}

int setCycle(std::vector<adress_t>& a, uint32_t adr, uint32_t cyc) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            a[i].cycle = cyc;
            return 0;
        }
    }
    std::cout << "!!!error set cyc on adr " << adr;
    return 1;
}

int setTimeCycle(std::vector<adress_t>& a, uint32_t adr, uint32_t time_cycle) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            a[i].time_cycle = time_cycle;
            return 0;
        }
    }
    std::cout << "!!!error set time cycle on adr " << adr;
    return 1;
}

int setComment(std::vector<adress_t>& a, uint32_t adr, std::string comment) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            a[i].comment = comment;
            return 0;
        }
    }
    std::cout << "!!!error set time cycle on adr " << adr;
    return 1;
}

int setTimerPos(std::vector<adress_t>& a, uint32_t adr, uint32_t pos) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            a[i].timer_pos = pos;
            return 0;
        }
    }
    std::cout << "!!!error set time cycle on adr " << adr;
    return 1;
}

void printMessage(adress_t a) {
    std::cout << "0x" << std::hex << std::setfill('0') << std::setw(8) << a.adress << " | ";
    for (int i = 0; i < 8; i++) {
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << a.message[i] << ", ";
    }
    std::cout << std::dec << std::setfill('0') << std::setw(4) << a.time_cycle << " | ";
    std::cout << std::dec << std::setfill('0') << std::setw(4) << a.cycle << " | ";
    std::cout << std::dec << std::setfill('0') << std::setw(4) << a.timer_pos << " | ";
    std::cout << a.comment;
}

void printAllData(std::vector<adress_t> a) {
    std::cout << "adress |                         message                         |time_cycle|cycle|timer_pos|comment" << std::endl;
    for (int i = 0; i < a.size(); i++) {
        printMessage(a[i]);
        std::cout << std::endl;
    }
}


int main()
{
    setlocale(LC_ALL, "Russian");
    
    adress_t my; // вектор всех адресов
    my.adress = 0x18FFA0F3;
    for (int i = 0; i < 8; i++) {
        my.message[i] = 0x0; 
    }
    my.cycle = 0;
    my.time_cycle = 0;
    my.timer_pos = 0;
    my.comment = "comment";
    for (int i = 0; i < 29; i++) {
        m.push_back(my);
    }
    m[ad0x18FFA0F3].adress = 0x18FFA0F3;
    m[ad0x18FFA1F3].adress = 0x18FFA1F3;
    m[ad0x18FFA2F3].adress = 0x18FFA2F3;
    m[ad0x18FFA3F3].adress = 0x18FFA3F3;
    m[ad0x18FFA4F3].adress = 0x18FFA4F3;
        m[ad0x18FFA5F3].adress = 0x18FFA5F3;
        m[ad0x18FFA6F3].adress = 0x18FFA6F3;
        m[ad0x18FFA7F3].adress = 0x18FFA7F3;
        m[ad0x18FFA8F3].adress = 0x18FFA8F3;
        m[ad0x18FFAEF3].adress = 0x18FFAEF3;
        m[ad0x18FFA9F3].adress = 0x18FFA9F3;
        m[ad0x18FFAAF3].adress = 0x18FFAAF3;
        m[ad0x18FFACF3].adress = 0x18FFACF3;
        m[ad0x18FFAFF3].adress = 0x18FFAFF3;
        m[ad0x18FECAF3].adress = 0x18FECAF3;
        m[ad0x1810A6A0].adress = 0x1810A6A0;
        m[ad0x1820F8F4].adress = 0x1820F8F4;
        m[ad0x1880F8F4].adress = 0x1880F8F4;
        m[ad0x08F000A0].adress = 0x08F000A0;
        m[ad0x08F200A0].adress = 0x08F200A0;
        m[ad0x041000A0].adress = 0x041000A0;
        m[ad0x0C20A0A6].adress = 0x0C20A0A6;
        m[ad0x0CFF08EF].adress = 0x0CFF08EF;
        m[ad0x0CFF0008].adress = 0x0CFF0008;
        m[ad0x0CFF0108].adress = 0x0CFF0108;
        m[ad0x0CFF0208].adress = 0x0CFF0208;
        m[ad0x0CFF0308].adress = 0x0CFF0308;
        m[ad0x0CFF0408].adress = 0x0CFF0408;
        m[ad0x0CFF0508].adress = 0x0CFF0508;

    std::cout << "size: " << m.size() << std::endl;



    // начало битовой магии
    
    /*------------------------------------------------------------0x18FFA0F3-------------------------------------*/
    setCycle(m, 0x18FFA0F3, 20);   // цикл 20 мс

    mes_t loc = { 1 , 2, 3, 4, 5, 6, 7, 8};
    //setMessage(m, 0x18FFA0F3, loc);  
    setComment(m, 0x18FFA0F3, "главный контактор, питание, напряжение шины, ток шины");
    //в нулевом байте биты 0-1 главный контактор, 1 включить.
    //в нулевом байте биты 2-3 питание , 1 включить
    setBit(m, 0x18FFA0F3, 0, 0); // нуливой байт нуливой бит включение главного контактора
    setBit(m, 0x18FFA0F3, 2, 0); // нуливой байт второй бит включение чего-то

    //биты 8-23 напряжение, то есть первый байт и второй байты.
    float input_vlotage_MCU = 316.9;
    input_vlotage_MCU += 15.5; //суммируем напряжение от ЛБИ и от разетки
    input_vlotage_MCU /= 0.1; //масштабируем напряжение
    uint32_t voltage_hex = (uint32_t)(input_vlotage_MCU);

    setTwoByte(m, 0x18FFA0F3, 1, voltage_hex);
    
    //биты 24-40 переменный ток двигателя, то есть третий и четвертый байты

    float motor_AC = 0.6;// переменный ток двигателя
    motor_AC /= 0.1;
    setTwoByte(m, 0x18FFA0F3, 3, (uint32_t)(motor_AC));

    //printMessageHex(m, 0x18FFA0F3);
    //printMessageBin(m, 0x18FFA0F3);

    /*------------------------------------------------------------0x18FFA1F3-------------------------------------*/

    setCycle(m, 0x18FFA1F3, 20);   // цикл 20 мс
    setTimeCycle(m, 0x18FFA1F3, 100);  // таймера отличен от периуда 
    setTimerPos(m, 0x18FFA1F3, 60);
    setComment(m, 0x18FFA1F3, "информация о батареи и ошибки батареи");

    //биты 0-3 режим батареи нижние биты
    //0 = инициализировать [Соответствует 0, 1 в режиме работы от батареи]
    //1 = Отключить главный контактор(высоковольтный выключатель [Соответствует 2 или 5 в режиме работы от батареи]
    //2 = процесс предварительной зарядки [Соответствует режиму работы от батареи 2]
    //+++3 = замкнуть главный контактор(высокое напряжение замкнуто) 3 соответствует режиму работы от батаре
    //4 = Главный контактор отключен из - за неисправности, Безвозвратно[5 соответствует режиму работы от батареи]
    //5 ~ 15 = зарезервирован
    setLower4Bites(m, 0x18FFA1F3, 0, 3);

    //биты 4-7 режим  работы от  батареи верхние биты
    //0 = загрузка
    //1 = Самодиагностика
    //2 = ждать высокого давления
    //+++3 = разряд
    //4 = Зарезервировано
    //5 = режим проверки
    //6 = режим зарядки постоянным током
    //7 = режим зарядки переменного тока
    //8 = штекерный пистолет постоянного тока не заряжен
    //9 = вилка переменного тока не заряжена
    //10 ~15 = зарезервирован
    setUpper4Bites(m, 0x18FFA1F3, 0, 3);
   

    //биты 8-11 нижние биты первого байта ошибки BMS
    //ошибок нет, все нули
    setLower4Bites(m, 0x18FFA1F3, 1, 0);

    //биты 12-13 или биты 4 и 5 первого байта
    //состояние баланчировки
    //0 = отключить эквализацию
    //+++1 = включить выравнивание
    //2 = зарезервировано
    //3 = зарезервировано
    set2Bites(m, 0x18FFA1F3, 12, 1);

    //биты 16-23 или второй байт
    //SOC
    float soc = 98.56; // значение в процентах
    soc /= 0.5;
    setByte(m, 0x18FFA1F3, 2, (uint32_t)(soc));

    //биты 24-23 или третий байт
    //SOH здрорвье аккумулятора у нас аккум новый и он полон сил, поэтому 100%
    float soh = 100.00;
    soh /= 0.5;
    setByte(m, 0x18FFA1F3, 3, (uint32_t)(soh));

    printMessageBin(m, 0x18FFA1F3);
    printMessageHex(m, 0x18FFA1F3);


    printAllData(m);


}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
