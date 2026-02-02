# APDS-9999 Library Design Document

Design document for implementing the Adafruit_APDS9999 Arduino library using Adafruit BusIO patterns.

**Reference:** VEML6046 library as style template  
**Sensor:** Broadcom APDS-9999 Digital Proximity and RGB Sensor  
**I2C Address:** 0x52 (7-bit)

---

## 1. BusIO Register Pattern Summary (from VEML6046)

The VEML6046 library demonstrates the canonical Adafruit BusIO pattern:

### Core Components
```cpp
Adafruit_I2CDevice *i2c_dev;  // I2C bus interface (created in begin())
```

### Register Access Patterns

**Full Register Read/Write (1-2 bytes):**
```cpp
Adafruit_BusIO_Register reg = 
    Adafruit_BusIO_Register(i2c_dev, REG_ADDR, num_bytes, byte_order);
uint16_t value = reg.read();
reg.write(value);
```

**Bit Field Access:**
```cpp
Adafruit_BusIO_Register reg = 
    Adafruit_BusIO_Register(i2c_dev, REG_ADDR, 1);
Adafruit_BusIO_RegisterBits bits = 
    Adafruit_BusIO_RegisterBits(&reg, num_bits, start_bit);
uint8_t field = bits.read();
bits.write(field);
```

**Multi-byte Bulk Read (for sensor data):**
```cpp
uint8_t buffer[N];
uint8_t reg_addr = START_REG;
i2c_dev->write_then_read(&reg_addr, 1, buffer, N);
```

### Function Naming Convention
- **Setters:** `setPropertyName(value)` → returns `bool`
- **Getters:** `getPropertyName()` → returns typed value or enum
- **Boolean getters:** `isPropertyName()` or `getPropertyEnabled()`
- **Actions:** `enable()`, `trigger()`, `reset()`

---

## 2. Typed Enums Needed for APDS-9999

### Light Sensor Resolution
```cpp
typedef enum {
  APDS9999_LS_RES_20BIT_400MS = 0x00, ///< 20-bit, 400ms conversion
  APDS9999_LS_RES_19BIT_200MS = 0x01, ///< 19-bit, 200ms conversion
  APDS9999_LS_RES_18BIT_100MS = 0x02, ///< 18-bit, 100ms conversion (default)
  APDS9999_LS_RES_17BIT_50MS  = 0x03, ///< 17-bit, 50ms conversion
  APDS9999_LS_RES_16BIT_25MS  = 0x04, ///< 16-bit, 25ms conversion
  APDS9999_LS_RES_13BIT_3MS   = 0x05  ///< 13-bit, 3.125ms conversion
} apds9999_ls_resolution_t;
```

### Light Sensor Measurement Rate
```cpp
typedef enum {
  APDS9999_LS_RATE_25MS   = 0x00, ///< 25ms measurement rate
  APDS9999_LS_RATE_50MS   = 0x01, ///< 50ms measurement rate
  APDS9999_LS_RATE_100MS  = 0x02, ///< 100ms measurement rate (default)
  APDS9999_LS_RATE_200MS  = 0x03, ///< 200ms measurement rate
  APDS9999_LS_RATE_500MS  = 0x04, ///< 500ms measurement rate
  APDS9999_LS_RATE_1000MS = 0x05, ///< 1000ms measurement rate
  APDS9999_LS_RATE_2000MS = 0x06  ///< 2000ms measurement rate
} apds9999_ls_meas_rate_t;
```

### Light Sensor Gain
```cpp
typedef enum {
  APDS9999_LS_GAIN_1X  = 0x00, ///< Gain 1x
  APDS9999_LS_GAIN_3X  = 0x01, ///< Gain 3x (default)
  APDS9999_LS_GAIN_6X  = 0x02, ///< Gain 6x
  APDS9999_LS_GAIN_9X  = 0x03, ///< Gain 9x
  APDS9999_LS_GAIN_18X = 0x04  ///< Gain 18x
} apds9999_ls_gain_t;
```

### Proximity Sensor Resolution
```cpp
typedef enum {
  APDS9999_PS_RES_8BIT  = 0x00, ///< 8-bit resolution (default)
  APDS9999_PS_RES_9BIT  = 0x01, ///< 9-bit resolution
  APDS9999_PS_RES_10BIT = 0x02, ///< 10-bit resolution
  APDS9999_PS_RES_11BIT = 0x03  ///< 11-bit resolution
} apds9999_ps_resolution_t;
```

### Proximity Measurement Rate
```cpp
typedef enum {
  APDS9999_PS_RATE_6MS   = 0x01, ///< 6.25ms measurement rate
  APDS9999_PS_RATE_12MS  = 0x02, ///< 12.5ms measurement rate
  APDS9999_PS_RATE_25MS  = 0x03, ///< 25ms measurement rate
  APDS9999_PS_RATE_50MS  = 0x04, ///< 50ms measurement rate
  APDS9999_PS_RATE_100MS = 0x05, ///< 100ms measurement rate (default)
  APDS9999_PS_RATE_200MS = 0x06, ///< 200ms measurement rate
  APDS9999_PS_RATE_400MS = 0x07  ///< 400ms measurement rate
} apds9999_ps_meas_rate_t;
```

### VCSEL LED Current
```cpp
typedef enum {
  APDS9999_LED_CURRENT_10MA = 0x02, ///< 10mA VCSEL current
  APDS9999_LED_CURRENT_25MA = 0x03  ///< 25mA VCSEL current
} apds9999_led_current_t;
```

### VCSEL Pulse Frequency
```cpp
typedef enum {
  APDS9999_LED_FREQ_60KHZ  = 0x03, ///< 60kHz pulse frequency (default)
  APDS9999_LED_FREQ_70KHZ  = 0x04, ///< 70kHz pulse frequency
  APDS9999_LED_FREQ_80KHZ  = 0x05, ///< 80kHz pulse frequency
  APDS9999_LED_FREQ_90KHZ  = 0x06, ///< 90kHz pulse frequency
  APDS9999_LED_FREQ_100KHZ = 0x07  ///< 100kHz pulse frequency
} apds9999_led_freq_t;
```

### Light Sensor Interrupt Channel Select
```cpp
typedef enum {
  APDS9999_INT_CH_IR    = 0x00, ///< IR channel triggers interrupt
  APDS9999_INT_CH_GREEN = 0x01, ///< Green/ALS channel triggers interrupt (default)
  APDS9999_INT_CH_RED   = 0x02, ///< Red channel triggers interrupt
  APDS9999_INT_CH_BLUE  = 0x03  ///< Blue channel triggers interrupt
} apds9999_ls_int_channel_t;
```

### Light Sensor Variance Threshold
```cpp
typedef enum {
  APDS9999_LS_VAR_8    = 0x00, ///< 8 count variance (default)
  APDS9999_LS_VAR_16   = 0x01, ///< 16 count variance
  APDS9999_LS_VAR_32   = 0x02, ///< 32 count variance
  APDS9999_LS_VAR_64   = 0x03, ///< 64 count variance
  APDS9999_LS_VAR_128  = 0x04, ///< 128 count variance
  APDS9999_LS_VAR_256  = 0x05, ///< 256 count variance
  APDS9999_LS_VAR_512  = 0x06, ///< 512 count variance
  APDS9999_LS_VAR_1024 = 0x07  ///< 1024 count variance
} apds9999_ls_variance_t;
```

---

## 3. Register-by-Register Mapping

### 0x00 - MAIN_CTRL (RW, Default: 0x00)

| Bit | Field | RegisterBits | Setter | Getter |
|-----|-------|--------------|--------|--------|
| 6 | SAI_PS | Yes (1 bit @ 6) | `setPSSleepAfterInt(bool)` | `getPSSleepAfterInt()` |
| 5 | SAI_LS | Yes (1 bit @ 5) | `setLSSleepAfterInt(bool)` | `getLSSleepAfterInt()` |
| 4 | SW_RESET | Yes (1 bit @ 4) | `reset()` | — |
| 2 | RGB_MODE | Yes (1 bit @ 2) | `setRGBMode(bool)` | `getRGBMode()` |
| 1 | LS_EN | Yes (1 bit @ 1) | `enableLightSensor(bool)` | `lightSensorEnabled()` |
| 0 | PS_EN | Yes (1 bit @ 0) | `enableProximitySensor(bool)` | `proximitySensorEnabled()` |

**Notes:** 
- RGB_MODE: false = ALS+IR only, true = full RGB+IR
- SW_RESET: write-only, device does NOT ACK when set

---

### 0x01 - PS_VCSEL (RW, Default: 0x36)

| Bit | Field | RegisterBits | Setter | Getter |
|-----|-------|--------------|--------|--------|
| 6:4 | VCSEL_FREQ | Yes (3 bits @ 4) | `setLEDFrequency(apds9999_led_freq_t)` | `getLEDFrequency()` |
| 2:0 | VCSEL_CURRENT | Yes (3 bits @ 0) | `setLEDCurrent(apds9999_led_current_t)` | `getLEDCurrent()` |

**Notes:** Writing restarts measurements

---

### 0x02 - PS_PULSES (RW, Default: 0x08)

| Bit | Field | RegisterBits | Setter | Getter |
|-----|-------|--------------|--------|--------|
| 7:0 | PS_PULSES | No (full byte) | `setLEDPulses(uint8_t)` | `getLEDPulses()` |

**Notes:** 
- Range: 0-255 pulses
- Default: 8 pulses
- Writing resets PS state machine

---

### 0x03 - PS_MEAS_RATE (RW, Default: 0x05)

| Bit | Field | RegisterBits | Setter | Getter |
|-----|-------|--------------|--------|--------|
| 4:3 | PS_RESOLUTION | Yes (2 bits @ 3) | `setPSResolution(apds9999_ps_resolution_t)` | `getPSResolution()` |
| 2:0 | PS_MEAS_RATE | Yes (3 bits @ 0) | `setPSMeasRate(apds9999_ps_meas_rate_t)` | `getPSMeasRate()` |

**Notes:** Writing restarts measurements

---

### 0x04 - LS_MEAS_RATE (RW, Default: 0x22)

| Bit | Field | RegisterBits | Setter | Getter |
|-----|-------|--------------|--------|--------|
| 6:4 | LS_RESOLUTION | Yes (3 bits @ 4) | `setLSResolution(apds9999_ls_resolution_t)` | `getLSResolution()` |
| 2:0 | LS_MEAS_RATE | Yes (3 bits @ 0) | `setLSMeasRate(apds9999_ls_meas_rate_t)` | `getLSMeasRate()` |

**Notes:** Writing restarts measurements

---

### 0x05 - LS_GAIN (RW, Default: 0x01)

| Bit | Field | RegisterBits | Setter | Getter |
|-----|-------|--------------|--------|--------|
| 2:0 | LS_GAIN_RANGE | Yes (3 bits @ 0) | `setLSGain(apds9999_ls_gain_t)` | `getLSGain()` |

**Notes:** Writing resets LS state machine

---

### 0x06 - PART_ID (R, Default: 0xC2)

| Bit | Field | RegisterBits | Setter | Getter |
|-----|-------|--------------|--------|--------|
| 7:4 | PART_ID | Yes (4 bits @ 4) | — | `getPartID()` |
| 3:0 | REVISION_ID | Yes (4 bits @ 0) | — | `getRevisionID()` |

**Notes:**
- Expected PART_ID = 0x0C (upper nibble of 0xC2)
- Use full byte read and check `== 0xC2` in `begin()`

---

### 0x07 - MAIN_STATUS (R, Default: 0x20)

| Bit | Field | RegisterBits | Setter | Getter |
|-----|-------|--------------|--------|--------|
| 5 | POWER_ON_STATUS | Yes (1 bit @ 5) | — | `getPowerOnStatus()` |
| 4 | LS_INT_STATUS | Yes (1 bit @ 4) | — | `getLSInterruptStatus()` |
| 3 | LS_DATA_STATUS | Yes (1 bit @ 3) | — | `isLSDataReady()` |
| 2 | PS_LOGIC_SIGNAL | Yes (1 bit @ 2) | — | `isObjectNear()` |
| 1 | PS_INT_STATUS | Yes (1 bit @ 1) | — | `getPSInterruptStatus()` |
| 0 | PS_DATA_STATUS | Yes (1 bit @ 0) | — | `isPSDataReady()` |

**Notes:** Status bits are cleared after read

---

### 0x08-0x09 - PS_DATA (R, Default: 0x0000)

| Reg | Bit | Field | Notes |
|-----|-----|-------|-------|
| 0x08 | 7:0 | PS_DATA_0 | LSB of proximity data |
| 0x09 | 2:0 | PS_DATA_1 | MSB of proximity data (11-bit max) |
| 0x09 | 3 | OVERFLOW | Overflow flag |

**Access Method:** 2-byte read, LSBFIRST
**Getter:** `readProximity()` → `uint16_t` (returns value & 0x07FF)
**Getter:** `getProximityOverflow()` → `bool`

---

### 0x0A-0x0C - LS_DATA_IR (R)
### 0x0D-0x0F - LS_DATA_GREEN (R)
### 0x10-0x12 - LS_DATA_BLUE (R)
### 0x13-0x15 - LS_DATA_RED (R)

| Channel | Registers | Data Width |
|---------|-----------|------------|
| IR | 0x0A-0x0C | 20-bit (3 bytes) |
| Green/ALS | 0x0D-0x0F | 20-bit (3 bytes) |
| Blue | 0x10-0x12 | 20-bit (3 bytes) |
| Red | 0x13-0x15 | 20-bit (3 bytes) |

**Access Method:** Bulk read, see Section 4
**Getters:**
- `readIR()` → `uint32_t`
- `readGreen()` → `uint32_t` (alias: `readALS()`)
- `readBlue()` → `uint32_t`
- `readRed()` → `uint32_t`
- `getRGBIRData(uint32_t *r, uint32_t *g, uint32_t *b, uint32_t *ir)` → `bool`

---

### 0x19 - INT_CFG (RW, Default: 0x10)

| Bit | Field | RegisterBits | Setter | Getter |
|-----|-------|--------------|--------|--------|
| 5:4 | LS_INT_SEL | Yes (2 bits @ 4) | `setLSIntChannel(apds9999_ls_int_channel_t)` | `getLSIntChannel()` |
| 3 | LS_VAR_MODE | Yes (1 bit @ 3) | `setLSVarianceMode(bool)` | `getLSVarianceMode()` |
| 2 | LS_INT_EN | Yes (1 bit @ 2) | `enableLSInterrupt(bool)` | `lsInterruptEnabled()` |
| 1 | PS_LOGIC_MODE | Yes (1 bit @ 1) | `setPSLogicMode(bool)` | `getPSLogicMode()` |
| 0 | PS_INT_EN | Yes (1 bit @ 0) | `enablePSInterrupt(bool)` | `psInterruptEnabled()` |

---

### 0x1A - INT_PST (RW, Default: 0x00)

| Bit | Field | RegisterBits | Setter | Getter |
|-----|-------|--------------|--------|--------|
| 7:4 | LS_PERSIST | Yes (4 bits @ 4) | `setLSPersistence(uint8_t)` | `getLSPersistence()` |
| 3:0 | PS_PERSIST | Yes (4 bits @ 0) | `setPSPersistence(uint8_t)` | `getPSPersistence()` |

**Notes:** Value = number of consecutive readings - 1 (0 = 1 reading, 15 = 16 readings)

---

### 0x1B-0x1C - PS_THRES_UP (RW, Default: 0x07FF)

**Access Method:** 2-byte register, LSBFIRST, mask to 11 bits
**Setter:** `setPSThresholdHigh(uint16_t)` → `bool`
**Getter:** `getPSThresholdHigh()` → `uint16_t`

---

### 0x1D-0x1E - PS_THRES_LOW (RW, Default: 0x0000)

**Access Method:** 2-byte register, LSBFIRST, mask to 11 bits
**Setter:** `setPSThresholdLow(uint16_t)` → `bool`
**Getter:** `getPSThresholdLow()` → `uint16_t`

---

### 0x1F-0x20 - PS_CAN (RW, Default: 0x0000)

| Reg | Bit | Field | Notes |
|-----|-----|-------|-------|
| 0x1F | 7:0 | PS_CAN_0 | Digital cancellation LSB |
| 0x20 | 2:0 | PS_CAN_1 | Digital cancellation MSB |
| 0x20 | 7:3 | PS_CAN_ANA | Analog cancellation |

**Access Method:** 
- Digital: 2-byte read, mask lower 11 bits
- Analog: RegisterBits (5 bits @ 3) on 0x20

**Setters:**
- `setPSCancellation(uint16_t)` → `bool` (digital, 11-bit)
- `setPSAnalogCancellation(uint8_t)` → `bool` (5-bit)

**Getters:**
- `getPSCancellation()` → `uint16_t`
- `getPSAnalogCancellation()` → `uint8_t`

---

### 0x21-0x23 - LS_THRES_UP (RW, Default: 0x0FFFFF)

**Access Method:** 3-byte write (see Section 4)
**Setter:** `setLSThresholdHigh(uint32_t)` → `bool` (mask to 20 bits)
**Getter:** `getLSThresholdHigh()` → `uint32_t`

---

### 0x24-0x26 - LS_THRES_LOW (RW, Default: 0x000000)

**Access Method:** 3-byte write (see Section 4)
**Setter:** `setLSThresholdLow(uint32_t)` → `bool` (mask to 20 bits)
**Getter:** `getLSThresholdLow()` → `uint32_t`

---

### 0x27 - LS_THRES_VAR (RW, Default: 0x00)

| Bit | Field | RegisterBits | Setter | Getter |
|-----|-------|--------------|--------|--------|
| 2:0 | LS_THRES_VAR | Yes (3 bits @ 0) | `setLSVariance(apds9999_ls_variance_t)` | `getLSVariance()` |

---

## 4. Multi-Byte Read/Write Notes

### 3-Byte 20-bit Light Data (Read)

Registers 0x07-0x15 are locked during I2C read operations in the range. Read all 12 bytes at once for consistency:

```cpp
bool getRGBIRData(uint32_t *r, uint32_t *g, uint32_t *b, uint32_t *ir) {
  uint8_t buffer[12];
  uint8_t reg = APDS9999_REG_LS_DATA_IR_0;  // 0x0A
  
  if (!i2c_dev->write_then_read(&reg, 1, buffer, 12)) {
    return false;
  }
  
  // Each channel: 3 bytes, LSB first, mask to 20 bits
  *ir    = (buffer[0] | (buffer[1] << 8) | (buffer[2] << 16)) & 0x0FFFFF;
  *g     = (buffer[3] | (buffer[4] << 8) | (buffer[5] << 16)) & 0x0FFFFF;
  *b     = (buffer[6] | (buffer[7] << 8) | (buffer[8] << 16)) & 0x0FFFFF;
  *r     = (buffer[9] | (buffer[10] << 8) | (buffer[11] << 16)) & 0x0FFFFF;
  
  return true;
}
```

### 2-Byte 11-bit Proximity Data (Read)

```cpp
uint16_t readProximity() {
  Adafruit_BusIO_Register ps_data_reg = 
      Adafruit_BusIO_Register(i2c_dev, APDS9999_REG_PS_DATA_0, 2, LSBFIRST);
  uint16_t raw = ps_data_reg.read();
  return raw & 0x07FF;  // Mask to 11 bits, bit 11 is overflow
}

bool getProximityOverflow() {
  Adafruit_BusIO_Register ps_data_reg = 
      Adafruit_BusIO_Register(i2c_dev, APDS9999_REG_PS_DATA_0, 2, LSBFIRST);
  return (ps_data_reg.read() & 0x0800) != 0;
}
```

### 3-Byte 20-bit Threshold (Write)

BusIO_Register supports max 4 bytes. Use direct write:

```cpp
bool setLSThresholdHigh(uint32_t threshold) {
  threshold &= 0x0FFFFF;  // Mask to 20 bits
  uint8_t buffer[4];
  buffer[0] = APDS9999_REG_LS_THRES_UP_0;
  buffer[1] = threshold & 0xFF;
  buffer[2] = (threshold >> 8) & 0xFF;
  buffer[3] = (threshold >> 16) & 0x0F;
  return i2c_dev->write(buffer, 4);
}
```

---

## 5. Suggested Implementation Order

### Phase 1: Core Infrastructure
1. **begin()** - I2C init, device detection
   - Create I2CDevice
   - Read PART_ID register (0x06), verify == 0xC2
   - Return false if wrong ID
   
2. **reset()** - Software reset via MAIN_CTRL[4]
   - Note: No ACK after reset, add small delay

3. **enable/disable functions**
   - `enableLightSensor(bool)`
   - `enableProximitySensor(bool)`
   - `setRGBMode(bool)` - ALS only vs full RGB

### Phase 2: Basic Data Reads
4. **readProximity()** - 2-byte read
5. **getRGBIRData()** - 12-byte bulk read for all light channels
6. **Status checks:**
   - `isPSDataReady()`
   - `isLSDataReady()`
   - `isObjectNear()`

### Phase 3: Configuration Setters
7. **Light sensor config:**
   - `setLSResolution()`
   - `setLSMeasRate()`
   - `setLSGain()`

8. **Proximity sensor config:**
   - `setPSResolution()`
   - `setPSMeasRate()`
   - `setLEDCurrent()`
   - `setLEDFrequency()`
   - `setLEDPulses()`
   - `setPSCancellation()`

### Phase 4: Interrupt Configuration
9. **Interrupt enables:**
   - `enablePSInterrupt()`
   - `enableLSInterrupt()`

10. **Interrupt thresholds:**
    - `setPSThresholdHigh/Low()`
    - `setLSThresholdHigh/Low()`

11. **Advanced interrupt:**
    - `setLSIntChannel()`
    - `setLSVarianceMode()`
    - `setLSVariance()`
    - `setPSLogicMode()`
    - `setPersistence()` for both LS and PS

### Phase 5: Advanced Features
12. **Sleep-after-interrupt:**
    - `setPSSleepAfterInt()`
    - `setLSSleepAfterInt()`

13. **Convenience functions:**
    - `calculateLux()` (requires resolution/gain lookup tables)
    - Individual channel reads: `readRed()`, `readGreen()`, `readBlue()`, `readIR()`

---

## 6. Register Address Constants

```cpp
#define APDS9999_DEFAULT_ADDR      0x52

#define APDS9999_REG_MAIN_CTRL     0x00
#define APDS9999_REG_PS_VCSEL      0x01
#define APDS9999_REG_PS_PULSES     0x02
#define APDS9999_REG_PS_MEAS_RATE  0x03
#define APDS9999_REG_LS_MEAS_RATE  0x04
#define APDS9999_REG_LS_GAIN       0x05
#define APDS9999_REG_PART_ID       0x06
#define APDS9999_REG_MAIN_STATUS   0x07
#define APDS9999_REG_PS_DATA_0     0x08
#define APDS9999_REG_PS_DATA_1     0x09
#define APDS9999_REG_LS_DATA_IR_0  0x0A
#define APDS9999_REG_LS_DATA_IR_1  0x0B
#define APDS9999_REG_LS_DATA_IR_2  0x0C
#define APDS9999_REG_LS_DATA_GREEN_0 0x0D
#define APDS9999_REG_LS_DATA_GREEN_1 0x0E
#define APDS9999_REG_LS_DATA_GREEN_2 0x0F
#define APDS9999_REG_LS_DATA_BLUE_0  0x10
#define APDS9999_REG_LS_DATA_BLUE_1  0x11
#define APDS9999_REG_LS_DATA_BLUE_2  0x12
#define APDS9999_REG_LS_DATA_RED_0   0x13
#define APDS9999_REG_LS_DATA_RED_1   0x14
#define APDS9999_REG_LS_DATA_RED_2   0x15
#define APDS9999_REG_INT_CFG       0x19
#define APDS9999_REG_INT_PST       0x1A
#define APDS9999_REG_PS_THRES_UP_0 0x1B
#define APDS9999_REG_PS_THRES_UP_1 0x1C
#define APDS9999_REG_PS_THRES_LOW_0 0x1D
#define APDS9999_REG_PS_THRES_LOW_1 0x1E
#define APDS9999_REG_PS_CAN_0      0x1F
#define APDS9999_REG_PS_CAN_1      0x20
#define APDS9999_REG_LS_THRES_UP_0 0x21
#define APDS9999_REG_LS_THRES_UP_1 0x22
#define APDS9999_REG_LS_THRES_UP_2 0x23
#define APDS9999_REG_LS_THRES_LOW_0 0x24
#define APDS9999_REG_LS_THRES_LOW_1 0x25
#define APDS9999_REG_LS_THRES_LOW_2 0x26
#define APDS9999_REG_LS_THRES_VAR  0x27

#define APDS9999_PART_ID_VALUE     0xC2
```

---

## 7. Class Structure Summary

```cpp
class Adafruit_APDS9999 {
public:
  Adafruit_APDS9999();
  ~Adafruit_APDS9999();
  
  // Initialization
  bool begin(uint8_t addr = APDS9999_DEFAULT_ADDR, TwoWire *wire = &Wire);
  bool reset();
  uint8_t getPartID();
  uint8_t getRevisionID();
  
  // Enable/Disable
  bool enableLightSensor(bool en);
  bool lightSensorEnabled();
  bool enableProximitySensor(bool en);
  bool proximitySensorEnabled();
  bool setRGBMode(bool rgb);
  bool getRGBMode();
  
  // Data Reading
  uint16_t readProximity();
  bool getProximityOverflow();
  bool getRGBIRData(uint32_t *r, uint32_t *g, uint32_t *b, uint32_t *ir);
  uint32_t readRed();
  uint32_t readGreen();
  uint32_t readBlue();
  uint32_t readIR();
  
  // Status
  bool isPSDataReady();
  bool isLSDataReady();
  bool isObjectNear();
  bool getPowerOnStatus();
  
  // Light Sensor Config
  bool setLSResolution(apds9999_ls_resolution_t res);
  apds9999_ls_resolution_t getLSResolution();
  bool setLSMeasRate(apds9999_ls_meas_rate_t rate);
  apds9999_ls_meas_rate_t getLSMeasRate();
  bool setLSGain(apds9999_ls_gain_t gain);
  apds9999_ls_gain_t getLSGain();
  
  // Proximity Sensor Config
  bool setPSResolution(apds9999_ps_resolution_t res);
  apds9999_ps_resolution_t getPSResolution();
  bool setPSMeasRate(apds9999_ps_meas_rate_t rate);
  apds9999_ps_meas_rate_t getPSMeasRate();
  bool setLEDCurrent(apds9999_led_current_t current);
  apds9999_led_current_t getLEDCurrent();
  bool setLEDFrequency(apds9999_led_freq_t freq);
  apds9999_led_freq_t getLEDFrequency();
  bool setLEDPulses(uint8_t pulses);
  uint8_t getLEDPulses();
  bool setPSCancellation(uint16_t value);
  uint16_t getPSCancellation();
  
  // Interrupts
  bool enablePSInterrupt(bool en);
  bool psInterruptEnabled();
  bool enableLSInterrupt(bool en);
  bool lsInterruptEnabled();
  bool setPSThresholdHigh(uint16_t threshold);
  uint16_t getPSThresholdHigh();
  bool setPSThresholdLow(uint16_t threshold);
  uint16_t getPSThresholdLow();
  bool setLSThresholdHigh(uint32_t threshold);
  uint32_t getLSThresholdHigh();
  bool setLSThresholdLow(uint32_t threshold);
  uint32_t getLSThresholdLow();
  // ... additional interrupt config methods
  
private:
  Adafruit_I2CDevice *i2c_dev;
};
```

---

*Document generated for APDS-9999 library development*  
*Pattern reference: Adafruit_VEML6046*
