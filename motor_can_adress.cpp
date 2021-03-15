﻿// motor_can_adress.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
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
#define ad0x1811A6A0 29


#define BYTE_0 0
#define BYTE_1 1
#define BYTE_2 2
#define BYTE_3 3
#define BYTE_4 4
#define BYTE_5 5
#define BYTE_6 6
#define BYTE_7 7

#define BIT_0 0 
#define BIT_1 1 
#define BIT_2 2 
#define BIT_3 3 
#define BIT_4 4 
#define BIT_5 5 
#define BIT_6 6 
#define BIT_7 6 




typedef struct {
    uint32_t adress;
    uint32_t message[8];
    uint32_t cycle; // время повторения посылки
    uint32_t time_cycle; // время обновления таймера, если 0, то аймера нет
    uint32_t timer_pos; //позиция первого бита в таймере. предполагается, что таймеры 4х битные.
    std::string comment;
}adress_t;

typedef struct {
    uint8_t byte;
    uint8_t bit;
}pair_bits_t;
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

pair_bits_t  bitToByte(const uint32_t input_bit);
int setLower4Bites(std::vector<adress_t>& a, uint32_t adr, uint32_t byte_pos, uint8_t bits_value);
int setPairOf12Bit(std::vector<adress_t>& a, const uint32_t adr, const uint32_t byte_pos, const uint32_t byte_value_1, const uint32_t byte_value_2);

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
int setByte(
    std::vector<adress_t>& a, 
    const uint32_t adr, 
    const uint32_t byte_pos, 
    const uint32_t byte_value) {

    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            a[i].message[byte_pos] = byte_value;
            return 0;
        }
    }
    std::cout << "!!!error set bit on adr " << adr;
    return 1;
}

int set12Bits(std::vector<adress_t>& a, uint32_t adr, uint32_t bit_pos, uint32_t byte_value) {
    if (bit_pos > 50 || byte_value > 0xFFF) {
        std::cout << "!!!error set12Bits on adr " << adr;
        return 1;
    }
    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            uint32_t loc_byte_pos = bitToByte(bit_pos).byte;
            uint32_t high_half_byte = byte_value >> 8;
            uint32_t low_byte = byte_value % 0x100;
            setByte(a, adr, loc_byte_pos, low_byte);
            setLower4Bites(a, adr, loc_byte_pos + 1, high_half_byte);
            return 0;
        }
    }


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

int setPairOf12Bit(
    std::vector<adress_t>& a, 
    const uint32_t adr, 
    const uint32_t byte_pos, 
    const uint32_t byte_value_1, 
    const uint32_t byte_value_2
) {

    if (byte_value_1 > 0xFFF || byte_value_2 > 0xFFF || byte_pos > 5) {
        std::cout << "!!!error setPairOf12Bit by input value on adr " << adr;
        return 1;
    }
    for (int i = 0; i < a.size(); i++) {
        if (a[i].adress == adr) {
            uint32_t high_half_byte_1 = byte_value_1 >> 8;
            uint32_t low_byte_1 = byte_value_1 % 0x100;
            uint32_t low_half_byte_2 = byte_value_2 % 0x10;
            uint32_t high_byte_2 = byte_value_2 >> 4;
            setByte(a, adr, byte_pos, low_byte_1);
            setLower4Bites(a, adr, byte_pos + 1, high_half_byte_1);
            setByte(a, adr, byte_pos + 2, high_byte_2);
            setUpper4Bites(a, adr, byte_pos + 1, low_half_byte_2);
            return 0;
        }
    }
    std::cout << "!!!error setPairOf12Bit by wrong adress on adr " << adr;
    return 1;
}

void printMessage(adress_t a) {
    std::cout << "0x" << std::hex << std::setfill('0') << std::setw(8) << a.adress << " | ";
    for (int i = 0; i < 7; i++) {
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << a.message[i] << ", ";
    }
    std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << a.message[7] << " | ";
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

//переводит значение бит в пару из значений байт(0-7) и бит(0-7)
pair_bits_t  bitToByte(const uint32_t input_bit) {
    pair_bits_t ret;
    ret.byte = input_bit / 8;
    ret.bit = input_bit % 8;
    return ret;
}

int main()
{
    //test comment
    //test comment 2
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
    for (int i = 0; i < 30; i++) {
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
        m[ad0x1811A6A0].adress = 0x1811A6A0;

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
    setLower4Bites(m, 0x18FFA1F3, BYTE_0, 3);

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
    setUpper4Bites(m, 0x18FFA1F3, BYTE_0, 3);
   

    //биты 8-11 нижние биты первого байта ошибки BMS
    //ошибок нет, все нули
    setLower4Bites(m, 0x18FFA1F3, BYTE_1, 0);

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
    setByte(m, 0x18FFA1F3, BYTE_2, (uint32_t)(soc));

    //биты 24-31 или третий байт
    //SOH здрорвье аккумулятора у нас аккум новый и он полон сил, поэтому 100%
    float soh = 100.00;
    soh /= 0.5;
    setByte(m, 0x18FFA1F3, BYTE_3, (uint32_t)(soh));



    //биты 32-47 байты 4 и 5
    //сопративление батареи
    float battary_resistance = 0.005; //сопративление батареи 5 мОм
    battary_resistance /= 0.001;
    setTwoByte(m, 0x18FFA1F3, bitToByte(32).byte, (uint32_t)(battary_resistance));

    //биты 48-59 
    //остаточная емкость
    float residual_capacity = 2.2; //ампер часа
    residual_capacity /= 0.1;
    //set12Bits(m, 0x18FFA1F3, 48, (uint32_t)(0xABC));
    set12Bits(m, 0x18FFA1F3, 48, (uint32_t)(residual_capacity));
    

    //биты 60-63
    //счетчик каждые 100 мс
    //!!!!!!!!!!!!!!!-----------------------СЧЕТЧИК 0-14, 15 ЗАРЕЗЕРВИРОВАНО
    setUpper4Bites(m, 0x18FFA1F3, bitToByte(60).byte, 0xe);


    /*---------------------------------------------------------0x18FFA2F3-------------------------------------*/

    setCycle(m, 0x18FFA2F3, 20);
    setComment(m, 0x18FFA2F3, "напряжение, ток, мощность АКБ");
    //подряд 2 посылки 0-11 и 12-23
    //напряжение АКБ, выходное напряжение в вольтах
    const float battary_voltage = 1.8;
    //battary_voltage /= 0.2;
    const float output_voltage = 316.9;
    //output_voltage /= 0.2;
    setPairOf12Bit(m, 0x18FFA2F3, BYTE_0, (uint32_t)(battary_voltage / 0.2), (uint32_t)(output_voltage / 0.2));


    //биты 24-39 или байты 3 и 4
    //ток батареи в амперах
    const float battary_current = 486.32;
    //battary_current = (battary_current + 500.0) / 0.02;
    setTwoByte(m, 0x18FFA2F3, BYTE_3, (uint32_t)((battary_current + 500.0) / 0.02));


    //биты 40-55 или байты 5 и 6
    //мощьность батареи в кВт
    const float battary_power = (output_voltage * battary_current) / 1000;
    setTwoByte(m, 0x18FFA2F3, BYTE_5, (uint32_t)((battary_power + 325) / 0.01));


    /*---------------------------------------------------------0x18FFA3F3-------------------------------------*/
    
    setCycle(m, 0x18FFA3F3, 1000);
    setComment(m, 0x18FFA3F3, "онлайн статусы, количество АКБ ящиков, номер ведомого, версия ПО");
    
    //биты 0-31 или байты 0-3 
    //задаются номера онлайн устройств. Списано с логов 
    setByte(m, 0x18FFA3F3, BYTE_0, 0x0f); // списано с логов. может быть 0x00
    setByte(m, 0x18FFA3F3, BYTE_1, 0x00); // списано с логов.
    setByte(m, 0x18FFA3F3, BYTE_2, 0x00); // списано с логов.
    setByte(m, 0x18FFA3F3, BYTE_3, 0x00); // списано с логов.

    //биты 32-39 или байт 4
    //количество аккумуляторнх ящиков списано с логов значения в диапазоне 1-16
    setByte(m, 0x18FFA3F3, BYTE_4, 0x02); // списано с логов. 2 ящика

    //биты 40-48 или байт 5
    //номер ведомого списано с логов значения в диапазоне 1-32
    setByte(m, 0x18FFA3F3, BYTE_5, 0x04); // списано с логов. ведовый четвертый

    //бит 48 статус программного обеспечения
    //биты 49-63 номер версии ПО
    //чтобы не плодить функции типа setNBits напишим все руками

    //0 представляет статус образца
    //1 представляет статус партии
    bool software_status = 0; //статус образца
    uint32_t software_version = 1; //версия 1;
    setTwoByte(m, 0x18FFA3F3, BYTE_6, (uint32_t)(software_status + (software_version << 1)));

    /*---------------------------------------------------------0x18FFA4F3-------------------------------------*/
    
    setCycle(m, 0x18FFA4F3, 100);
    setComment(m, 0x18FFA4F3, "описание ячеек АКБ");

    //биты 0-15 байты 0 и 1
    //сумма напряжения ячеек
    const float battary_cell_voltage_summ = output_voltage;
    setTwoByte(m, 0x18FFA4F3, BYTE_0, (uint32_t)(battary_cell_voltage_summ / 0.01));
    
    //биты 16-27 и биты 28-39 байты 2-4
    //максимальное и минимальное напряжение ячеек в вольтах
    const float min_voltage_per_cell = 2.2;
    const float max_voltage_per_cell = 2.2;
    setPairOf12Bit(m, 0x18FFA4F3, BYTE_2, (uint32_t)(max_voltage_per_cell / 0.0015),
        (uint32_t)(min_voltage_per_cell / 0.0015));

    //биты 40-47 ячейка с максимальным напряжением
    //биты 48-55 ячейка с минимальным напряжением
    setByte(m, 0x18FFA4F3, bitToByte(40).byte, 1); //у первой ячейки максимальное напряжение
    setByte(m, 0x18FFA4F3, bitToByte(48).byte, 2); //у второй ячейки минимальное напряжение

    //биты 56-59 системный номер ячецки с самым высоким напряжением
    setLower4Bites(m, 0x18FFA4F3, bitToByte(56).byte, 1);//первыя ячейка, как в логах

    //биты 60-63 системный номер ячецки с самым низким напряжением
    setUpper4Bites(m, 0x18FFA4F3, bitToByte(60).byte, 3);//третья ячейка, как в логах

    /*---------------------------------------------------------0x18FFA4F3-------------------------------------*/

    printMessageHex(m, 0x18FFA4F3);
    
    
    //printAllData(m);


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
