#ifndef __MOD_AT24CXX_H
#define __MOD_AT24CXX_H
#include "sys.h"



// AT24Cxx系列芯片定义  内存容量
#define AT24C01_CAPACITY     128
#define AT24C02_CAPACITY     256
#define AT24C04_CAPACITY     512
#define AT24C08_CAPACITY     1024
#define AT24C16_CAPACITY     2048

// 页大小定义					页字节数
#define AT24C01_PAGE_SIZE    8
#define AT24C02_PAGE_SIZE    8
#define AT24C04_PAGE_SIZE    16
#define AT24C08_PAGE_SIZE    16
#define AT24C16_PAGE_SIZE    16

// AT24Cxx设备基础地址
#define AT24CXX_BASE_ADDR    0xA0


typedef enum {
	AT24C01 = 0,
	AT24C02,
	AT24C04,
	AT24C08,
	AT24C16
}at24cxx_type;
	
typedef struct{
	at24cxx_type chip_type;   // 芯片类型
	uint8_t i2c_addr;					// I2C设备地址
	uint16_t capacity;        // 容量(字节)
	uint8_t page_size;        // 页大小
	uint8_t addr_bytes;       // 地址字节数(1或2)
}at24cxx_dev;


/**
 * @brief 写入数据到AT24Cxx(自动处理分页)
 * @param dev: 设备结构体指针
 * @param addr: 起始地址
 * @param data: 数据指针
 * @param len: 数据长度
 */
void at24cxx_write_data(at24cxx_dev *dev, at24cxx_type chip_type, uint16_t addr, uint8_t *data, uint16_t len);


/**
 * @brief 从AT24Cxx读取数据
 * @param dev: 设备结构体指针  
 * @param addr: 起始地址
 * @param data: 数据缓冲区指针
 * @param len: 数据长度
 */
void at24cxx_read_data(at24cxx_dev *dev, at24cxx_type chip_type, uint16_t addr, uint8_t *data, uint16_t len);


extern at24cxx_dev eeprom_dev;



//void test_at24c02_simple(void) {
//    uint8_t write_data = 0xAB;
//    uint8_t read_data = 0;
//    
//    // 单字节写入读取
//    at24c02_write_data(0x10, &write_data, 1);
//    delay_ms(10);
//    
//    // 第一次读取
//    at24c02_read_data(0x10, &read_data, 1);
//    LOG_DEBUG("First single byte read: 0x%02X", read_data);
//    
//    // 第二次读取
//    at24c02_read_data(0x10, &read_data, 1);
//    LOG_DEBUG("Second single byte read: 0x%02X", read_data);
//    
//    // 多字节
//    uint8_t multi_write[2] = {0xCD, 0xEF};
//    uint8_t multi_read[2] = {0};
//    
//    at24c02_write_data(0x20, multi_write, 2);
//    delay_ms(10);
//    
//    at24c02_read_data(0x20, multi_read, 2);
//    LOG_DEBUG("First multi read: [0]=0x%02X, [1]=0x%02X", multi_read[0], multi_read[1]);
//    
//    at24c02_read_data(0x20, multi_read, 2);
//    LOG_DEBUG("Second multi read: [0]=0x%02X, [1]=0x%02X", multi_read[0], multi_read[1]);
//}

//// 全局设备变量
//at24cxx_dev eeprom_dev;

//// 示例1：基本读取操作
//void basic_read_example(void) {
//    uint8_t buffer[32];
//    
//    // 从地址0x000读取32字节
//    at24cxx_read_data(&eeprom_dev, AT24C08, 0x000, buffer, sizeof(buffer));
//    
//    // 处理读取的数据...
//    for(int i = 0; i < sizeof(buffer); i++) {
//        printf("Data[%d] = 0x%02X\n", i, buffer[i]);
//    }
//}

//// 示例2：跨边界读取
//void cross_boundary_read_example(void) {
//    uint8_t large_buffer[256];
//    
//    // AT24C08中，这个读取会跨越设备地址边界
//    // 0x0F0-0x1FF (设备地址0xA0) 和 0x100-0x1FF (设备地址0xA2)
//    at24cxx_read_data(&eeprom_dev, AT24C08, 0x0F0, large_buffer, 256);
//}



#endif


