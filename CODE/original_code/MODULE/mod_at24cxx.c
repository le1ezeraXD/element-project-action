#include "mod_at24cxx.h"
#include "sys_i2c.h"
#include "param.h"

/**
	******************************************************************************
	* @file    mod_at24cxx.c
	* @author  zyj
	* @version V0.0.1
	* @date    22-10-2025
	* @brief   at24cxx
	******************************************************************************
	*	驱动代码: at24cxx
	* ic: at24cxx  
	******************************************************************************
	*/
	
at24cxx_dev eeprom_dev;

/**
 * @brief  根据芯片类型和A2 A1 A0引脚配置计算设备地址
 * @param  chip_type: 芯片类型
 * @param  addr:  要写入数据的地址
 *				 ax_state:  A2 A1 A0引脚状态(0-7)，AT24C01/02使用全部3位，AT24C04使用A2A1，AT24C08使用A2，AT24C16不使用
 * @return 设备地址
 */
uint8_t at24cxx_get_device_addr(at24cxx_type chip_type, uint16_t addr) {
	  uint8_t at24xx_addr = AT24CXX_BASE_ADDR;
		uint8_t ax_state = addr >> 8;
	
		switch(chip_type) {
			case AT24C01:
			case AT24C02:
				// 1010 A2 A1 A0  ax默认接GND - 0
//				at24xx_addr |= ((ax_state & 0x07) << 1);
				break;
			case AT24C04:
				// 1010 A2 A1 P0  ax默认接GND - 0  
				at24xx_addr |= ((ax_state & 0x01) << 1);  // 只使用A2A1，忽略A0
				break;
			case AT24C08:
				// 1010 A2 P1 P0  ax默认接GND - 0
				at24xx_addr |= ((ax_state & 0x03) << 1);  // 只使用A2，忽略A1A0
				break;
			case AT24C16:
				// 1010 P2 P1 P0
				// P2P1P0 = A10A9A8
				at24xx_addr |= ((ax_state & 0x07) << 1);  // P2P1P0位
				break;
			default:
				break;
		}
		
		return at24xx_addr;
}

/**
 * @brief 初始化AT24Cxx设备
 * @param dev: 设备结构体指针
 * @param chip_type: 芯片类型
 * @param addr: 要写入数据的地址
 * @param ax_state: 地址引脚配置 A0 A1 A2
 */
void at24cxx_init(at24cxx_dev *dev, at24cxx_type chip_type, uint16_t addr) {
	
	dev->chip_type = chip_type;
	dev->i2c_addr  = at24cxx_get_device_addr(chip_type, addr);
	
	// 设置容量和页大小
		switch(chip_type) {
			case AT24C01:
				dev->capacity   = AT24C01_CAPACITY;
				dev->page_size  = AT24C01_PAGE_SIZE;
			  dev->addr_bytes = 1;
			  break;
			case AT24C02:
				dev->capacity   = AT24C02_CAPACITY;
				dev->page_size  = AT24C02_PAGE_SIZE;
			  dev->addr_bytes = 1;
				break;
			case AT24C04:
				dev->capacity   = AT24C04_CAPACITY;
				dev->page_size  = AT24C04_PAGE_SIZE;
			  dev->addr_bytes = 1;  // 使用1字节地址+设备地址中的P0位
				break;
			case AT24C08:
				dev->capacity   = AT24C08_CAPACITY;
				dev->page_size  = AT24C08_PAGE_SIZE;
			  dev->addr_bytes = 1;  // 使用1字节地址+设备地址中的P1  P0位
				break;
			case AT24C16:
				dev->capacity   = AT24C16_CAPACITY;
				dev->page_size  = AT24C16_PAGE_SIZE;
			  dev->addr_bytes = 1;  // 使用1字节地址+设备地址中的P2  P1  P0位
				break;
			default:
				// 默认按AT24C02处理
				dev->capacity  = AT24C02_CAPACITY;
				dev->page_size = AT24C02_PAGE_SIZE;
				dev->addr_bytes = 1;
				break;
		}
}


/**
 * @brief 	计算实际的内存地址(考虑芯片的地址位分布)
 * @param 	dev: 设备结构体指针
 * @param 	addr: 逻辑地址
 * @return 	实际要发送的地址值
 */
uint16_t at24cxx_calc_mem_addr(at24cxx_dev *dev, uint16_t addr) {
    // 确保地址不超出芯片容量
    if(addr >= dev->capacity) {
        addr = dev->capacity - 1;
    }
    
    // 对于不同芯片，地址处理方式不同
    switch(dev->chip_type) {
        case AT24C01:
        case AT24C02:
            // 直接使用8位地址
            return addr & 0xFF;
            
        case AT24C04:
            // 地址位分布: 设备地址包含P0(A8)，内存地址使用A7-A0
            return addr & 0xFF;
            
        case AT24C08:
            // 地址位分布: 设备地址包含P1P0(A9A8)，内存地址使用A7-A0  
            return addr & 0xFF;
            
        case AT24C16:
            // 使用16位地址
            return addr;
            
        default:
            return addr & 0xFF;
    }
}


/**
 * @brief 写入数据到AT24Cxx(自动处理分页)
 * @param dev: 设备结构体指针
 * @param addr: 起始地址
 * @param data: 数据指针
 * @param len: 数据长度
 */
void at24cxx_write_data(at24cxx_dev *dev, at24cxx_type chip_type, uint16_t addr, uint8_t *data, uint16_t len) {
    PROJ_FUNC_ENTER;
	
		//初始化EEPROM  得到设备地址
		at24cxx_init(dev, chip_type, addr);

    uint8_t ret;
    uint16_t bytes_written = 0;
    uint16_t current_addr = addr;
    
    // 循环写入，处理跨页情况
    while(bytes_written < len) {
			
			  // 如果地址变化超过当前设备地址范围，需要重新初始化
        if(current_addr != addr) {
            at24cxx_init(dev, chip_type, current_addr);
        }
			
        uint16_t page_boundary = (current_addr / dev->page_size + 1) * dev->page_size;  
        uint16_t bytes_to_write = len - bytes_written;   
        
        // 如果跨页，则计算当前页剩余空间
        if(current_addr + bytes_to_write > page_boundary) {
            bytes_to_write = page_boundary - current_addr;  
        }
        
        // 计算实际内存地址
        uint16_t mem_addr = at24cxx_calc_mem_addr(dev, current_addr);
        
        i2c_device at24cxx = {
            .dev_addr_w = dev->i2c_addr,
            .reg_addr 	= mem_addr,  
            .pdata			= &data[bytes_written],
            .data_len 	= bytes_to_write  
        };
        

        ret = sys_i2c_write(I2C2, &at24cxx);
        if(ret) {
            LOG_ERROR("at24cxx write failed at addr 0x%04X", current_addr);
            break;
        }
        
        // EEPROM写入需要时间
        delay_ms(5);
        
        bytes_written += bytes_to_write; 
        current_addr += bytes_to_write;  
    }
    
    PROJ_FUNC_EXIT;
}


/**
 * @brief 从AT24Cxx读取数据
 * @param dev: 设备结构体指针  
 * @param addr: 起始地址
 * @param data: 数据缓冲区指针
 * @param len: 数据长度
 */
void at24cxx_read_data(at24cxx_dev *dev, at24cxx_type chip_type, uint16_t addr, uint8_t *data, uint16_t len) {
    PROJ_FUNC_ENTER;
    
    uint8_t ret;
    uint16_t bytes_read = 0;       //记录已读取的字节数
    uint16_t current_addr = addr;  //当前读取地址
    
    // 参数检查
    if(addr >= dev->capacity) {
        LOG_ERROR("Start address 0x%04X exceeds chip capacity 0x%04X", addr, dev->capacity);
        PROJ_FUNC_EXIT;
        return;
    }
    
    // 调整长度
    if(addr + len > dev->capacity) {
        len = dev->capacity - addr;
        LOG_ERROR("Adjust read length to %u bytes", len);
    }
    
    // 预先计算所有分段（避免在循环中重复计算）  按块分段
    if(chip_type >= AT24C04) {
        // 分段读取：预先计算所有设备地址边界
        while(bytes_read < len) {
            uint16_t segment_start = current_addr;
            uint8_t current_dev_addr = at24cxx_get_device_addr(chip_type, current_addr);
            
            // 找到当前设备地址的连续范围
            uint16_t segment_end = segment_start;
            while(segment_end < addr + len - 1) {
                uint8_t next_dev_addr = at24cxx_get_device_addr(chip_type, segment_end + 1);
                if(next_dev_addr != current_dev_addr) break;
                segment_end++;
            }
            
            uint16_t segment_len = segment_end - segment_start + 1;
            uint16_t remaining_len = len - bytes_read;
            uint16_t bytes_to_read = (segment_len < remaining_len) ? segment_len : remaining_len;
            
            // 初始化并读取当前分段
            at24cxx_init(dev, chip_type, segment_start);
            uint16_t mem_addr = at24cxx_calc_mem_addr(dev, segment_start);
            
            i2c_device at24cxx = {
                .dev_addr_w = dev->i2c_addr,
                .dev_addr_r = dev->i2c_addr | 0x01,
                .reg_addr   = mem_addr,
                .pdata      = &data[bytes_read],
                .data_len   = bytes_to_read
            };
            
            ret = sys_i2c_write_read(I2C2, &at24cxx);
            if(ret) {
                LOG_ERROR("Read failed at 0x%04X, dev_addr=0x%02X, error: %d", 
                         segment_start, dev->i2c_addr, ret);
                break;
            }
            
            bytes_read += bytes_to_read;
            current_addr = segment_end + 1;
        }
    } else {
        // AT24C01/02 - 直接单次读取（不会跨设备地址）
        at24cxx_init(dev, chip_type, addr);
        uint16_t mem_addr = at24cxx_calc_mem_addr(dev, addr);
        
        i2c_device at24cxx = {
            .dev_addr_w = dev->i2c_addr,
            .dev_addr_r = dev->i2c_addr | 0x01,
            .reg_addr   = mem_addr,
            .pdata      = data,
            .data_len   = len
        };
        
        ret = sys_i2c_write_read(I2C2, &at24cxx);
        if(ret) {
            LOG_ERROR("Read failed at 0x%04X, dev_addr=0x%02X, error: %d", 
                     addr, dev->i2c_addr, ret);
        }
    }
    
    PROJ_FUNC_EXIT;
}

#if 0
#define AT24C02_W_DATA   0xA0
#define AT24C02_R_DATA   0xA1


void at24c02_write_data(uint8_t addr, uint8_t *data, uint8_t len) {
	PROJ_FUNC_ENTER;
	
	uint8_t ret;
	
	i2c_device at24c02 = {
		.dev_addr_w = AT24C02_W_DATA, 		
		.reg_addr	 	= addr,       				
		.pdata 			= data,          		
		.data_len   = len
	};
	
	ret = sys_i2c_write(I2C2, &at24c02);
	if(ret){
		LOG_ERROR("at24c02 write failed");
	}
	
	delay_ms(10);
	
	PROJ_FUNC_EXIT;
}


void at24c02_read_data(uint8_t addr, uint8_t *data, uint8_t len) {
	PROJ_FUNC_ENTER;
	
	uint8_t ret;
	
	i2c_device at24c02 = {
		.dev_addr_w = AT24C02_W_DATA, 	
		.dev_addr_r = AT24C02_R_DATA,    	
		.reg_addr	 	= addr,       				
		.pdata 			= data,          		
		.data_len   = len
	};
	
	ret = sys_i2c_write_read(I2C2, &at24c02);
	if(ret){
		LOG_ERROR("at24c02 write failed");
	}
	
	PROJ_FUNC_EXIT;
}
#endif







