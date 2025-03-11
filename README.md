# YODA Otonom Araç Takımı Workspace

### Kurulum

Workspace kurmak istediğiniz dosyanın terminalinde source yaptıktan sonra terminale aşağıdaki komutu girin.

``` git clone https://github.com/YildizOtonomDizaynArastirma/yoda_ws.git ```

Ardından src/yoda_cpp_package dosyası içindeki /home/safa/... şeklinde olan uzantıları kendi username değerinize göre değiştirin.

```
colcon build 
source /path/to/install/setup.bash 
ros2 launch yoda_cpp_package yoda_launch.py
```



