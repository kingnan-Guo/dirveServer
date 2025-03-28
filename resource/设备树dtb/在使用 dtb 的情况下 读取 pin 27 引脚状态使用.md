    读取 pin 27 引脚状态使用 ,在使用 dtb 的情况下

    分支
        dirver_raspberry_input_button_v0.1.1
        dirver_raspberry_input_button_v0.1.0


   

    insmod my_chip_board_button.ko 
    insmod my_drv.ko

    cat /proc/devices  

    ls -l /dev/my_board_*

    ./main /dev/my_board_button_0
    
    rmmod my_chip_board_button.ko 
    rmmod my_drv.ko


