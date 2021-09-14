# 简单的 Linux 烹饪指南

> 本文是 HPC 高性能计算课程的作业，因为比较简略所以许多细节之处需要再查资料；而且因为写作本文的时候没有在装系统所以没有图也没有实操记录，只是脑海中的碎片知识整理，仅仅用作经验参考。

> 本文更新于：[简单的 Linux 烹饪指南](https://www.chiro.work/Linux-2a6da2dd6a6548edacb299913611e2bd)

## 安装

主要有以下几种安装路径/方法：

1. 虚拟机安装
2. 实体机安装（双系统）
3. WSL2
4. 服务器

### 不需要进入安装界面

1. 使用 Linux Live CD

    由于 Linux 灵活的特性，许多 Linux 发行版在安装盘内有 Live CD 功能，即直接从安装盘启动到内存中，直接体验这款 Linux 发行版，对硬盘可以不做任何更改。

    在 Live CD 内也可以一边体验系统一边安装系统到硬盘上。

2. WSL

    WSL (WSL2) 的安装是十分简(sha)单(gua)的，你只需要访问 Windows Store，然后搜索“Ubuntu”等发行版，然后点击安装，不一会就能装好。建议再安装 Windows Terminal 食用，安装方法同上。

    WSL 本身是一个 Windows 上随开随用的虚拟机，占用资源和性能比再开一个专门的虚拟机要少不少。你可以在这个融入 Windows 的系统中用 vim 等 Linux 下特有的工具，也可以用 Windows 的开发工具和环境，然后把编译工具链安装在 WSL 中。诸如此类的骚操作笔者也在探寻，有好玩的玩法请踢我一脚。

### 进入安装界面

**虚拟机安装**

1. 新建虚拟机，建议硬盘容量 20G 以上。
2. 下载 Linux 发行版镜像，ISO 格式。
3. 把 ISO 镜像作为光盘加载到虚拟机，然后开机。
4. 如果是 VMware 的话可能会有简易安装，其他虚拟机请跟随安装进程继续。

**实体机安装**

1. 下载 Linux 发行版镜像。
2. 使用软碟通（UltraISO）或者 Linux 自带的启动盘创建工具，把发行版镜像写入 U 盘。
3. 打开电源，进入 BIOS 调整系统启动顺序，或者不进入 BIOS 临时调整系统启动顺序，把可移动大容量储存设备列为第一个。
4. 等待系统进入引导。

*注意：*

1. 把 ISO 镜像写入 USB 移动储存设备有可能无法从 USB 设备启动，主要原因可能有：
    1. 烧写工具对当前 Linux 发行版支持不好
    2. BIOS 无法识别 USB 设备
    3. 当前 Linux 发行版的安装光盘可能只有 Legacy 启动方式而 BIOS 关闭了对 Legacy 启动方式的识别

    解决方式依次：

    1. 最好使用 Ubuntu 带的 `usb启动盘制作`来烧写，成功率最高（大概）
    2. 检查接口处是否接触不良，检查 BIOS 中 USB 的相关选项，或者换一个 USB 储存设备
    3. 在 BIOS 中同时打开 UEFI 和 Legacy 两种启动方式，也可以关闭安装启动来提高兼容性

    **Legacy 引导和 UEFI 引导的区别**

    Legacy 和 UEFI 的关系有点像上面说到的 MBR 和 GPT 的关系。

    Legacy (+MBR) 引导的顺序：

    1. 点击开机按钮
    2. BIOS 初始化、硬件自检
    3. 按照设定的顺序找到可启动设备（硬盘、USB储存设备、光驱等）
    4. 读取执行主引导记录（Main Boot Recode, MBR），主引导记录是一段很短的程序，负责找到启动分区并且继续引导到分区引导记录，有 Windows NT 5.x/6.x，GRUB，grub4dos等
    5. 分区引导记录（Partition Boot Record, PBR），用于引导特定的操作系统，有NTLDR、bootmgr、grldr等
    6. 操作系统开始运行

    UEFT (+GPT) 引导顺序：

    ![Untitled](%E7%AE%80%E5%8D%95%E7%9A%84%20Linux%20%E7%83%B9%E9%A5%AA%E6%8C%87%E5%8D%97%204cc52c272d954e44bed4e4a7c1e6af3a/Untitled.png)

    转自

    [](https://blog.csdn.net/gjq_1988/article/details/50593564)

### 设置安装选项

1. 设置分区
    1. 设置分区类型

        如果是对一个全新的硬盘安装，会让你选择硬盘分区的格式：GPT 或者 MBR。

        **MBR 分区和 GPT 分区的区别**

        1. MBR 分区，也称为 msdos 分区，分区表保存在硬盘的第一个扇区（MBR扇区），出现较 GPT 早，主要对比较老的电脑兼容，是从微软的 MSDOS 时代一直沿用至今的一种硬盘分区格式。优点是兼容性强，而且使用简单；缺点是只能存在 4 个主分区，要更多分区只能用逻辑分区实现，也不能识别 2TB 以上的硬盘。此外，对系统备份、系统安全加密等支持也不算好。
        2. GPT 分区解决了 MBR 分区的大多数问题，但是兼容性比 MBR 稍差，需要操作系统支持。
    2. 简易安装：选择该 Linux 发行版自带的推荐的分区方式
    3. 自定义安装
        1. 只添加一个分区并且作为`/`挂载。这种方式最简单而且大概不需要其他的分区操作，除非强制要求再加`/boot` 或者`swap`分区。
        2. 相比上面一个再多分出`/home`分区。这样有助于保护用户数据等，但是如果要调整`/`和`/home`分区的大小需要用`LVM`了。
        3. `swap`分区：交换分区，用于在内存不足时临时存储内存内容，防止系统崩溃。不一定是必选项，但是还是推荐建立这个分区。
        4. 关于`LVM`：

            > 逻辑卷管理提供了比传统的磁盘和分区视图更高级别的计算机系统上磁盘存储的视图。 这使得系统管理员可以更灵活地将存储分配给应用程序和用户。在逻辑卷管理器的控制下创建的存储卷可以随意调整大小和移动，尽管这可能需要对文件系统工具进行一些升级。

2. 设置软件源（一些 Linux 发行版需要）

    软件源可以选择网络上的镜像站，如

    1. 清华大学开源软件镜像站[https://mirrors4.tuna.tsinghua.edu.cn/](https://mirrors4.tuna.tsinghua.edu.cn/)
    2. 阿里源：[http://mirrors.aliyun.com/](http://mirrors.aliyun.com/)
    3. 网易源：[http://mirrors.163.com/](http://mirrors.aliyun.com/)
    4. 哈工大深圳源：[~~mirrors.hitsz.org](http://mirrors.hitsz.org/)~~ 10.249.12.85（仅校园内网）
3. 点击开始安装，中间可能会设置键盘布局、时区等；如果是已经连接网络并且设置了软件源可能会下载完整的语言包并且升级系统等。

## 使用

### 软件包的下载、安装和管理

不同 Linux 发行版的软件包管理的大致流程、命令行参数都差不多：

1. 设置软件源
2. 更新软件列表信息
3. 安装软件 / 更新系统
4. 管理 / 卸载

**Debian 系列操作系统的软件包管理机制**

Debian 系的发行版的包管理器是`apt`，个人感觉是用的最多的软件包管理，也是最方便的包管理器之一。

输入 `apt --help` ，输出大约如下：

```bash
apt 2.0.6 (amd64)                                                            
Usage: apt [options] command                                                 
                                                                             
apt is a commandline package manager and provides commands for               
searching and managing as well as querying information about packages.       
It provides the same functionality as the specialized APT tools,             
like apt-get and apt-cache, but enables options more suitable for            
interactive use by default.                                                  
                                                                             
Most used commands:                                                          
  list - list packages based on package names                                
  search - search in package descriptions                                    
  show - show package details                                                
  install - install packages                                                 
  reinstall - reinstall packages                                             
  remove - remove packages                                                   
  autoremove - Remove automatically all unused packages                      
  update - update list of available packages                                 
  upgrade - upgrade the system by installing/upgrading packages              
  full-upgrade - upgrade the system by removing/installing/upgrading packages
  edit-sources - edit the source information file                            
  satisfy - satisfy dependency strings                                       
                                                                             
See apt(8) for more information about the available commands.                
Configuration options and syntax is detailed in apt.conf(5).                 
Information about how to configure sources can be found in sources.list(5).  
Package and version choices can be expressed via apt_preferences(5).         
Security details are available in apt-secure(8).                             
                                        This APT has Super Cow Powers.
```

最后一行的 `This APT has Super Cow Powers` （“该 APT 具有超级牛力”）是 APT 的一个小彩蛋。输入 `apt moo` ：

```bash
➜  ~ apt moo                             
                 (__)                    
                 (oo)                    
           /------\/                     
          / |    ||                      
         *  /\---/\                      
            ~~   ~~                      
..."Have you mooed today?"...            
➜  ~
```

嗯……好吧真的有牛。 `mooooooooooo~~`

常用的 APT 指令：

1. `apt install <包名>`
2. `apt search <包名或部分包名>`
3. `apt remove <包名>`
4. `apt edit-sources` 编辑软件源