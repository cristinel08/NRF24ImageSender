#include "CameraOV2640.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "pico/binary_info.h"
#include "ov2640.h"

ArduCamOv2640::ArduCamOv2640(
	void
)
{
  sensor_model = OV2640;
  sensor_addr = 0x30;
}

void ArduCamOv2640::InitCAM(
	void
)
{
 
  switch (sensor_model)
  {
    case OV2640:
        WriteI2cReg(0xff, 0x01);
        WriteI2cReg(0x12, 0x80);
        sleep_ms(100);
        if (m_fmt == JPEG)
        {
          WriteI2cRegMulti(OV2640_JPEG_INIT);
          WriteI2cRegMulti(OV2640_YUV422);
          WriteI2cRegMulti(OV2640_JPEG);
          WriteI2cReg(0xff, 0x01);
          WriteI2cReg(0x15, 0x00);
		  WriteI2cRegMulti(OV2640_1600x1200_JPEG);
        }
        else
        {
          WriteI2cRegMulti(OV2640_QVGA);
        }
        break;
    default:
      break;
  }
}

void ArduCamOv2640::CS_HIGH(
	void
)
{
	 sbi(P_CS, B_CS);	
}
void ArduCamOv2640::CS_LOW(
	void
)
{
	 cbi(P_CS, B_CS);	
}

void ArduCamOv2640::FlushFifo(
	void
)
{
	WriteSpiReg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

void ArduCamOv2640::StartCapture(
	void
)
{
	WriteSpiReg(ARDUCHIP_FIFO, FIFO_START_MASK);
}


void ArduCamOv2640::ClearFifoFlag(
	void
)
{
	WriteSpiReg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
}

uint8_t ArduCamOv2640::ReadSpiReg(
	uint8_t const& addr
)
{
  	uint8_t value = addr& 0x7f;
  	cbi(P_CS, B_CS);
  	spi_write_blocking(SPI_PORT, &value, 1);
  	spi_read_blocking(SPI_PORT, 0, &value, 1);
  	sbi(P_CS, B_CS);
	return value;
}



void ArduCamOv2640::WriteSpiReg(
	uint8_t const& addr, 
	uint8_t const& data
)
{
    uint8_t buf[2];
    buf[0] = addr|WRITE_BIT ;  // remove read bit as this is a write
    buf[1] = data;
    cbi(P_CS, B_CS);
    spi_write_blocking(SPI_PORT, buf, 2);
    sbi(P_CS, B_CS);
}


uint32_t ArduCamOv2640::ReadFifoLength(
	void
)
{
	uint32_t len1,len2,len3,length=0;
	len1 = ReadSpiReg(FIFO_SIZE1);
  	len2 = ReadSpiReg(FIFO_SIZE2);
  	len3 = ReadSpiReg(FIFO_SIZE3) & 0x7f;
  	length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
	return length;	
}

void ArduCamOv2640::SetFifoBurst(
	void
)
{
    uint8_t value;
    spi_read_blocking(SPI_PORT, BURST_FIFO_READ, &value, 1);	
}

//Set corresponding bit  
void ArduCamOv2640::SetBit(
	uint8_t const& addr, 
	uint8_t const& bit
)
{
	uint8_t temp;
	temp = ReadSpiReg(addr);
	WriteSpiReg(addr, temp | bit);
}


//Clear corresponding bit 
void ArduCamOv2640::ClearBit(
	uint8_t const& addr, 
	uint8_t const& bit
)
{
	uint8_t temp;
	temp = ReadSpiReg(addr);
	WriteSpiReg(addr, temp & (~bit));
}


//Get corresponding bit status
uint8_t ArduCamOv2640::GetBit(
	uint8_t const& addr, 
	uint8_t const& bit
)
{
  uint8_t temp;
  temp = ReadSpiReg(addr);
  temp = temp & bit;
  return temp;
}




	// Write 8 bit values to 8 bit register address
void ArduCamOv2640::WriteI2cRegMulti(
	struct sensor_reg const reglist[]
)
{
	  uint16_t reg_addr = 0;
	  uint16_t reg_val = 0;
	  const struct sensor_reg *next = reglist;
	  while ((reg_addr != 0xff) | (reg_val != 0xff))
	  {
	    reg_addr = next->reg;
	    reg_val = next->val;
	    WriteI2cReg(reg_addr, reg_val);
	    next++;
	  }
}

// Read/write 8 bit value to/from 8 bit register address	
void ArduCamOv2640::WriteI2cReg(
	uint8_t const& regID, 
	uint8_t const& regDat
)
{
	uint8_t buf[2];
    buf[0] = regID;
    buf[1] = regDat;
    i2c_write_blocking(I2C_PORT, sensor_addr, buf,  2, true );	
}

void ArduCamOv2640::SetSpecialEffects(
	uint8_t const& Special_effect
)
{
	switch(Special_effect)
	{
		case Antique:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x18);
			WriteI2cReg(0x7c, 0x05);
			WriteI2cReg(0x7d, 0x40);
			WriteI2cReg(0x7d, 0xa6);
			break;
		case Bluish:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x18);
			WriteI2cReg(0x7c, 0x05);
			WriteI2cReg(0x7d, 0xa0);
			WriteI2cReg(0x7d, 0x40);
			break;
		case Greenish:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x18);
			WriteI2cReg(0x7c, 0x05);
			WriteI2cReg(0x7d, 0x40);
			WriteI2cReg(0x7d, 0x40);
			break;
		case Reddish:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x18);
			WriteI2cReg(0x7c, 0x05);
			WriteI2cReg(0x7d, 0x40);
			WriteI2cReg(0x7d, 0xc0);
			break;
		case BW:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x18);
			WriteI2cReg(0x7c, 0x05);
			WriteI2cReg(0x7d, 0x80);
			WriteI2cReg(0x7d, 0x80);
			break;
		case Negative:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x40);
			WriteI2cReg(0x7c, 0x05);
			WriteI2cReg(0x7d, 0x80);
			WriteI2cReg(0x7d, 0x80);
			break;
		case BWnegative:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x58);
			WriteI2cReg(0x7c, 0x05);
			WriteI2cReg(0x7d, 0x80);
			WriteI2cReg(0x7d, 0x80);
			break;
		case Normal:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x00);
			WriteI2cReg(0x7c, 0x05);
			WriteI2cReg(0x7d, 0x80);
			WriteI2cReg(0x7d, 0x80);
			break;
				
	}
}

void ArduCamOv2640::GetFrame(
	uint8_t** image, 
	uint32_t const& length
)
{
	CS_LOW();
	SetFifoBurst();//Set fifo burst mode
	spi_read_blocking(SPI_PORT, BURST_FIFO_READ, *image, length);
	CS_HIGH();
}

void ArduCamOv2640::SetContrast(
	uint8_t const& Contrast
)
{
	switch(Contrast)
	{
		case Contrast2:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x04);
			WriteI2cReg(0x7c, 0x07);
			WriteI2cReg(0x7d, 0x20);
			WriteI2cReg(0x7d, 0x28);
			WriteI2cReg(0x7d, 0x0c);
			WriteI2cReg(0x7d, 0x06);
			break;
		case Contrast1:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x04);
			WriteI2cReg(0x7c, 0x07);
			WriteI2cReg(0x7d, 0x20);
			WriteI2cReg(0x7d, 0x24);
			WriteI2cReg(0x7d, 0x16);
			WriteI2cReg(0x7d, 0x06); 
			break;
		case Contrast0:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x04);
			WriteI2cReg(0x7c, 0x07);
			WriteI2cReg(0x7d, 0x20);
			WriteI2cReg(0x7d, 0x20);
			WriteI2cReg(0x7d, 0x20);
			WriteI2cReg(0x7d, 0x06); 
			break;
		case Contrast_1:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x04);
			WriteI2cReg(0x7c, 0x07);
			WriteI2cReg(0x7d, 0x20);
			WriteI2cReg(0x7d, 0x20);
			WriteI2cReg(0x7d, 0x2a);
			WriteI2cReg(0x7d, 0x06);	
			break;
		case Contrast_2:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x04);
			WriteI2cReg(0x7c, 0x07);
			WriteI2cReg(0x7d, 0x20);
			WriteI2cReg(0x7d, 0x18);
			WriteI2cReg(0x7d, 0x34);
			WriteI2cReg(0x7d, 0x06);
			break;
	}	
}
bool ArduCamOv2640::CheckOV2640Camera(
	void
)
{
	uint8_t cameraCommand{};
	uint8_t vid{};
	uint8_t pid{};
    while (1)
    {
        //Check if the ArduCAM SPI bus is OK
        WriteSpiReg(ARDUCHIP_TEST1, 0x55);
        cameraCommand = ReadSpiReg(ARDUCHIP_TEST1);
        if (cameraCommand != 0x55) 
        {
            printf(" SPI interface Error!");
            sleep_ms(1000); continue;
        } else 
        {
            printf("ACK CMD SPI interface OK.END"); break;
        }
    }
  
	while (1) 
    {
		//Check if the camera module type is OV2640
		WriteI2cReg(0xff, 0x01);
		ReadI2cReg(OV2640_CHIPID_HIGH, &vid);
		ReadI2cReg(OV2640_CHIPID_LOW, &pid);
		if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 )))
        {
			printf("Can't find OV2640 module!");
			sleep_ms(1000); 
            continue;
		}
		else 
        {
			printf("OV2640 detected.END"); 
            break;
		}
    }
	return true;
}

void ArduCamOv2640::SetQsFactor(
	uint8_t const& factor
)
{
	WriteI2cReg(0xff, 0x00);
	WriteI2cReg(0x44, factor);
	WriteI2cReg(0xff, 0xff);
}

void ArduCamOv2640::SetBrightness(
	uint8_t const& Brightness
)
{
	switch(Brightness)
	{
		case Brightness2:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x04);
			WriteI2cReg(0x7c, 0x09);
			WriteI2cReg(0x7d, 0x40);
			WriteI2cReg(0x7d, 0x00);
			break;
		case Brightness1:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x04);
			WriteI2cReg(0x7c, 0x09);
			WriteI2cReg(0x7d, 0x30);
			WriteI2cReg(0x7d, 0x00);
			break;	
		case Brightness0:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x04);
			WriteI2cReg(0x7c, 0x09);
			WriteI2cReg(0x7d, 0x20);
			WriteI2cReg(0x7d, 0x00);
			break;
		case Brightness_1:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x04);
			WriteI2cReg(0x7c, 0x09);
			WriteI2cReg(0x7d, 0x10);
			WriteI2cReg(0x7d, 0x00);
			break;
		case Brightness_2:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x04);
			WriteI2cReg(0x7c, 0x09);
			WriteI2cReg(0x7d, 0x00);
			WriteI2cReg(0x7d, 0x00);
			break;	
	}			
}

void ArduCamOv2640::SetColorSaturation(
	uint8_t const& Color_Saturation
)
{
	switch(Color_Saturation)
	{
		case Saturation2:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x02);
			WriteI2cReg(0x7c, 0x03);
			WriteI2cReg(0x7d, 0x68);
			WriteI2cReg(0x7d, 0x68);
			break;
		case Saturation1:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x02);
			WriteI2cReg(0x7c, 0x03);
			WriteI2cReg(0x7d, 0x58);
			WriteI2cReg(0x7d, 0x58);
			break;
		case Saturation0:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x02);
			WriteI2cReg(0x7c, 0x03);
			WriteI2cReg(0x7d, 0x48);
			WriteI2cReg(0x7d, 0x48);
			break;
		case Saturation_1:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x02);
			WriteI2cReg(0x7c, 0x03);
			WriteI2cReg(0x7d, 0x38);
			WriteI2cReg(0x7d, 0x38);
			break;
		case Saturation_2:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0x7c, 0x00);
			WriteI2cReg(0x7d, 0x02);
			WriteI2cReg(0x7c, 0x03);
			WriteI2cReg(0x7d, 0x28);
			WriteI2cReg(0x7d, 0x28);
			break;	
	}
}

void ArduCamOv2640::SetLightMode(
	uint8_t const& Light_Mode
)
{
	switch(Light_Mode)
	{
	
		case Auto:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0xc7, 0x00); //AWB on
			break;
		case Sunny:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0xc7, 0x40); //AWB off
			WriteI2cReg(0xcc, 0x5e);
			WriteI2cReg(0xcd, 0x41);
			WriteI2cReg(0xce, 0x54);
			break;
		case Cloudy:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0xc7, 0x40); //AWB off
			WriteI2cReg(0xcc, 0x65);
			WriteI2cReg(0xcd, 0x41);
			WriteI2cReg(0xce, 0x4f);  
			break;
		case Office:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0xc7, 0x40); //AWB off
			WriteI2cReg(0xcc, 0x52);
			WriteI2cReg(0xcd, 0x41);
			WriteI2cReg(0xce, 0x66);
			break;
		case Home:
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0xc7, 0x40); //AWB off
			WriteI2cReg(0xcc, 0x42);
			WriteI2cReg(0xcd, 0x3f);
			WriteI2cReg(0xce, 0x71);
			break;
		default :
			WriteI2cReg(0xff, 0x00);
			WriteI2cReg(0xc7, 0x00); //AWB on
			break; 
	}	
}

void ArduCamOv2640::ReadI2cReg(
	uint8_t const& regID, uint8_t* regDat
)
{	
  i2c_write_blocking(I2C_PORT, sensor_addr, &regID, 1, true );
  i2c_read_blocking(I2C_PORT, sensor_addr, regDat,  1, false );
}


void ArduCamOv2640::SetJpegSize(
	uint8_t const& size
)
{
	switch(size)
	{
		case OV2640_160x120:
			WriteI2cRegMulti(OV2640_160x120_JPEG);
			break;
		case OV2640_176x144:
			WriteI2cRegMulti(OV2640_176x144_JPEG);
			break;
		case OV2640_320x240:
			WriteI2cRegMulti(OV2640_320x240_JPEG);
			break;
		case OV2640_352x288:
	  	WriteI2cRegMulti(OV2640_352x288_JPEG);
			break;
		case OV2640_640x480:
			WriteI2cRegMulti(OV2640_640x480_JPEG);
			break;
		case OV2640_800x600:
			WriteI2cRegMulti(OV2640_800x600_JPEG);
			break;
		case OV2640_1024x768:
			WriteI2cRegMulti(OV2640_1024x768_JPEG);
			break;
		case OV2640_1280x1024:
			WriteI2cRegMulti(OV2640_1280x1024_JPEG);
			break;
		case OV2640_1600x1200:
			WriteI2cRegMulti(OV2640_1600x1200_JPEG);
			break;
		default:
			WriteI2cRegMulti(OV2640_320x240_JPEG);
			break;
	}
}


void ArduCamOv2640::SetFormat(
	byte const& fmt
)
{
  if (fmt == BMP)
    m_fmt = BMP;
  else if(fmt == RAW)
    m_fmt = RAW;
  else
    m_fmt = JPEG;
}

void ArduCamOv2640:: ArduCamInit(
	uint8_t const& CS
)
{
	B_CS = CS;
	*P_CS=CS;
  	sbi(P_CS, B_CS);
	i2c_init(I2C_PORT, 100 * 1000);
	gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
	gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(PIN_SDA);
	gpio_pull_up(PIN_SCL);
	// Make the I2C pins available to picotool
	bi_decl( bi_2pins_with_func(PIN_SDA, PIN_SCL, GPIO_FUNC_I2C));
	// This example will use SPI0 at 0.5MHz.
	spi_init(SPI_PORT, 8 * 1000*1000);
	gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
	gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
	gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
	gpio_init(CS);
    gpio_set_dir(CS, GPIO_OUT);
    gpio_put(CS, 1);
	WriteSpiReg(0x07, 0x80);
    sleep_ms(100);
    WriteSpiReg(0x07, 0x00);
    sleep_ms(100);
}
