CFLAGS := -std=c11 -c -xc -mthumb -mcpu=cortex-m3 -O2 -Wall -g -ffunction-sections -fdata-sections -I./.eide/deps -I./lib/cmsis -I./lib -I./rts_os -I./.eide/deps -I./src -DSTM32F10X_MD -D__CHECK_DEVICE_DEFINES -D__CM3_REV
CXXFLAGS := -std=c++14 -c -xc++ -mthumb -mcpu=cortex-m3 -O2 -Wall -g -ffunction-sections -fdata-sections -I./.eide/deps -I./lib/cmsis -I./lib -I./rts_os -I./.eide/deps -I./src -DSTM32F10X_MD -D__CHECK_DEVICE_DEFINES -D__CM3_REV
ASMFLAGS := -c -x assembler-with-cpp -mthumb -mcpu=cortex-m3 -g -ffunction-sections -fdata-sections -I./.eide/deps -I./lib/cmsis -I./lib -I./rts_os -I./.eide/deps -I./src
LDFLAGS := -mthumb -mcpu=cortex-m3 -T "c:/Users/lisha/Desktop/arm/arm/stm32f1x_64KB_flash.ld" --specs=nosys.specs --specs=nano.specs -Wl,--gc-sections -Wl,--print-memory-usage -Wl,-Map=./build/Debug/arm.map
LDLIBS := -lm
