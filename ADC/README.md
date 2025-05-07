利用多通道 ADC 连续转换模式，读取电位器电压，NTC 热敏电阻，内部温度传感器，内部参考电压，并利用 DMA 循环模式搬运数据。

### 注意

- 使用 ADC 前进行一次校准

- 单次扫描模式将启动 ADC 放入循环内，单通道可以通过 HAL_ADC_PollForConversion() 检测转换完成，多通道可以利用中断
 
- 连续转换模式将启动 ADC 放在循环外，随时可以读取数据



### 附

NTC 热敏电阻的阻值和温度计算：
```c
float ADC2Resistance(uint32_t adc_value) {
    return (adc_value / (4095.0f - adc_value)) * 10000.0f;
}

float resistance2Temperature(float R1) {
    float B = 3950.0f;
    float R2 = 10000.0f;
    float T2 = 25.0f;
    return (1.0 / ((1.0 / B) * log(R1 / R2) + (1.0 / (T2 + 273.15))) - 273.15);
}
```