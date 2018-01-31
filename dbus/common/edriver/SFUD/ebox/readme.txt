1、添加sfud文件夹下的sfud.c和sfud_sfdp.c两个文件夹到工程目录，添加头文件包含路径（inc目录）
2、添加ebox目录下的ebox_sfud_port.cpp文件
3、设置编译选项：sfud.c和sfud_sfdp.c编译必须使用-C99，KEIL下右键文件选择“option for file”，  
   再选择C/C++选项卡，在“Misc Controls”行输出-C99
4、编译工程，检查是否出错
5、复制例程至main.cpp文件。编译整个工程，下载测试。

6、更新最新的sfud代码，请打开https://github.com/armink/SFUD下载，下载后可直接直接将整个目录覆盖，相同的会替换掉，保留其目录下的ebox目录