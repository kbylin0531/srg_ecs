注意：
   ①项目中之一写明变量类型等详细注释，这有帮助于编辑器检查变量类型，可以帮助避免低级错误
   ②获取可变参数的第二种方法
    ** PHP版本需要支持到5.6 **
       function sum(...$numbers) {
            $acc = 0;
            foreach ($numbers as $n) {
                $acc += $n;
            }
            return $acc;
        }
        echo sum(1, 2, 3, 4);//输出 10

        //...可用于数组解包
        function add($a, $b) {
            return $a + $b;
        }
        echo add(...[1, 2])."\n";//输出 3
        $a = [1, 2];
        echo add(...$a);//输出 3
   ?OB缓存有一定的大小，如果超出默认的4096字节则直接输出的浏览器，这时候使用ob_end_clean();不能达到隐藏的效果
   ④项目有配置文件集中在Configure目录可以依据目录更新时间决定是否重新加载Runtime目录下的集中配置文件
   ⑤在线把图片转换成Base64 ，网址：http://imgbase64.duoshitong.com/
   ⑥模板引擎选自smarty无变动，如果需要更新smarty版本，到目录"System\Extension\smarty"下替换
   ⑦exit(12)同样表示为终止程序的作用，但没有输出脚本值，因为如果exit函数的形参为整形数据,那么就代表一个退出的状态号，退出状态号的标准取值范围是：0-254之间，所以exit(12)也表示终止程序的作用。用整形数据的状态用法为：exit(0-254)；终止并输出脚本的用法为：exit("终止程序")；学习愉快！！！
   ⑧获取静态方法调用的类名称使用get_called_class,对象用get_class
   ⑨
        //测试strrposde得到结论:从前往后是从0开始的，从后往前是-1开始的
        //        Util::dump(strrpos('bsabab','ab'));//4
        //        Util::dump(strrpos('bsabab','ab',-1));//4
        //        Util::dump(strrpos('bsabab','ab',-2));//4
        //        Util::dump(strrpos('bsabab','ab',-3));//2
        //        Util::dump(strrpos('bsabab','ab',-4));//2
    ⑩Runtime目录下存放了 Mistight.lite.php文件，其中集中了大量的核心类，可以避免大量访问磁盘的IO消耗
       每次修改Core文件需要判断是否需要更新

   11.URL导向与ThinkPHP的差异是，后者从模块开始向操作解析，Mist则是从操作向模块解析
      优点:URL地址可以很简短，只需要直接输入操作名称就可以访问(在单个模块下优点突出)
      缺点:操作与参数之间的分割符需要着重设置，否则会出现解析歧义

   12.二维码PHP开源项目：http://sourceforge.net/projects/phpqrcode/?source=top3_dlp_t5
   13.文件密钥， 以一个文件(通常是图片)的md5值为密码，使用文件进行登陆（前提是开启文件登陆，并养成良好的文件管理习惯）
   14.Configure目录下的Auto目录下存放了系统自动生成的配置
   15.stdClass内置对象
      $condition = new stdClass();
      $condition->name = 'thinkphp';
   16.PHP红色的类似函数的部分是语言结构，区别于函数的时效率非常高
   17. 批量属性赋值
        示例代码：
   			foreach (array('csrf_expire', 'csrf_token_name', 'csrf_cookie_name') as $key)
   			{
   				if (NULL !== ($val = config_item($key)))
   				{
   					$this->{'_'.$key} = $val;
   				}
   			}
   18.ctype_digit 如果 text 字符串是一个十进制数字，就返回 TRUE  ；反之就返回 FALSE
   19.如果不特别声明，form表单默认提交的方法是get
   20.由于php对于方法不区分大小写，所以对于
            <h1><a href="{U url='admin/member/index/indexmain' }">MatAdmin</a></h1>
            <h1><a href="{U url='admin/member/index/indexMain' }">MatAdmin</a></h1>
        将产生同样的URL效果

   21.Router.class.php中第 379 行注释道：
        $_GET和$_REQUEST并不同步，当动态添加元素到$_GET中后，$_REQUEST中不会自动添加，不要理所当然地认为$_REQUEST每时每刻都是
        $_GET和$_POST的并集合，他们只是保存HTTP服务器解析时候的状态
国语英文对照：
divider - 分割线
  22.允许行为控制中的：如果未给指定的action传递参数，默认为null，避免thinkphp的异常提示
  23.
     访问如http://localhost/MinShuttler/Public时
       REQUEST_URI : /MinShuttler/Public/
       SCRIPT_NAME : /MinShuttler/Public/index.php
  
Sharingan:
    24:PHP标准库 (SPL) http://php.net/manual/zh/book.spl.php
    25:Vendor与Plugins都引用了第三方的库，区别时Plugin对原先的库作了修改
    26:Sharin目录下的类都是可以内置到lite文件中的
    28:文件后缀改为srg 16.09.23
    29:#!/usr/bin/env php 加入后可以直接输入PHP文件名像shell脚本那样执行（该命令的意思是使用环境变量中的php命令解析下面的脚本程序）
    30.当出现目录不可写等错误时，需要检查文件权限，通常可以修改所有者来获取访问权限 "chown -R asus.asus *"
    31.类的注释中尽量使用完整名称而避免在注释之外使用use语句(可以提高一部分性能) property Sharin\Core\Logger $log 日志记录类
    32.限制函数返回值可以是：
        
        public function func(string $str): array
        {
            return [];
        }
        

    33. Sharin
    34.通常认为抛出异常是程序无法继续执行或者执行下去没有意义的清空下执行的操作，通常清空下返回false即可代替通常的错误，这么做也是为节省开销作考虑
    
    35. 可执行的PHP文件将被命名为 XXX.go 
        单体文件将被命名为 YYY.psrg 
        过程文件将被命名为 XXX.inc