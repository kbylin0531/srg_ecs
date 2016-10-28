<?php
use Sharin\ClassLoader;
use Sharin\Utils;
use Sharin\Behaviour;
use Sharin\Core\Dispatcher;
use Sharin\Core\Router;
use Sharin\Developer;
use Sharin\Configger;
use Sharin\Core\Response;
use Sharin\Core\ExceptionHandler;
use Sharin\Exceptions\ParameterInvalidException;
use Sharin\Exceptions\RouteParseFailedException;
use Sharin\Behaviour\HtmlCache;
//---------------------------------------- 获取开始运行时状态 ------------------------------------------------//
$web_begin = [
    $_SERVER['REQUEST_TIME_FLOAT'],
    memory_get_usage(),
];

//---------------------------------------- 通用常量定义 -----------------------------------------------------//
require __DIR__.'/Common/constant.inc';

//---------------------------------------- WEB模式常量定义 --------------------------------------------------//
define('SR_NOW_MICRO',  $_SERVER['REQUEST_TIME_FLOAT']);//(int)($_SERVER['REQUEST_TIME_FLOAT']*1000)//isset($_SERVER['REQUEST_TIME_FLOAT'])? $_SERVER['REQUEST_TIME_FLOAT']:microtime(true)
define('SR_NOW',        $_SERVER['REQUEST_TIME']);
define('SR_IS_AJAX',    ((isset($_SERVER['HTTP_X_REQUESTED_WITH']) and strtoupper($_SERVER['HTTP_X_REQUESTED_WITH']) == 'XMLHTTPREQUEST') ));
define('SR_REQUEST_METHOD',strtoupper($_SERVER['REQUEST_METHOD']));//“GET”, “HEAD”，“POST”，“PUT” ...
define('SR_IS_HTTPS',   isset ($_SERVER ['HTTPS']) and $_SERVER ['HTTPS'] === 'on');
//请求方法
const SR_IS_POST = SR_REQUEST_METHOD === 'POST';
const SR_HTTP_PREFIX = SR_IS_HTTPS ? 'https://' : 'http://' ;
define('SR_SCRIPT_DIR',dirname($_SERVER['SCRIPT_NAME']));
//公共URL
define('SR_PUBLIC_URL',SR_HTTP_PREFIX.$_SERVER['SERVER_NAME'].
        (
            (80 == $_SERVER['SERVER_PORT'])?
                dirname($_SERVER['SCRIPT_NAME']) :
                ":{$_SERVER['SERVER_PORT']}".SR_SCRIPT_DIR
        )
    );

//---------------------------------------- 加载类包 -------------------------------------------------------//
require __DIR__.'/Core/bundle.inc';

//---------------------------------------- 开发模式设置 ---------------------------------------------------//
if(SR_DEBUG_MODE_ON) {
    //开发者模式
    error_reporting(E_ALL);
    ini_set('display_errors', 1);

    Developer::import('web_begin',$web_begin);
    SR_IS_CLI and die('only run in web mode');
    require __DIR__.'/Common/debug_suit.inc';
}else{
    //生产环境
    error_reporting(-1);// version >= 5.3 使用 E_ALL & ~E_NOTICE & ~E_DEPRECATED & ~E_STRICT & ~E_USER_NOTICE & ~E_USER_DEPRECATED
    ini_set('display_errors', 0);
}

/**
 * Class Sharin
 */
final class Sharin {
    /**
     * @var array 常规设置(系统内置的类的惯例配置在此配置)
     */
    private static $convention = [
        'TIMEZONE_ZONE'     => 'Asia/Shanghai',

        //session的memcache支持
        'SESSION_MEMCACHE_ON'   => false,
        'SESSION_MEMCACHE_IP'   => '127.0.0.1',
        'SESSION_MEMCACHE_PORT' => '11211',

        'BEHAVIOUR'        => [
            ON_INIT    => [],
            ON_INITED  => [],
            ON_START   => [
                HtmlCache::class,
            ],
            ON_ROUTE    => [],
            ON_CHECK    => [],
            ON_DISPATCH => [
                HtmlCache::class,
            ],
            ON_STOP     => [
                HtmlCache::class,
            ],
        ],

        'CONFIG_BEHAVIOUR'  => [
            //是否开启Lite
            'USE_LITE'          => true,
            //Lite针对的类名称的列表
            'LITE_CLASS_LIST'   => [
                'Sharin\\Core\\Storage',
                'Sharin\\Core\\Cache',
                'Sharin\\Behaviour\\HtmlCache'
            ],
        ]
    ];

    /**
     * 初始化并注册Sharin的行为
     * @param array $config system configuration
     * @return void
     * @throws ParameterInvalidException
     */
    public static function register(array $config=null){
        Behaviour::register(self::$convention['BEHAVIOUR']);
        Behaviour::listen(ON_INIT);
        $config and self::$convention = Utils::merge(self::$convention,$config);

        //设置初始化时期常量
        date_default_timezone_set(self::$convention['TIMEZONE_ZONE']) or die('Date default timezone set failed!');

        //session控制
        if(self::$convention['SESSION_MEMCACHE_ON']){
            ini_set('session.save_handler', 'memcached') === false and die('session handler init failed');
            ini_set('session.save_path', 'tcp://'.self::$convention['SESSION_MEMCACHE_IP'].':'.self::$convention['SESSION_MEMCACHE_PORT']) === false and die('session save path set failed');
        }

        //行为注册
        ClassLoader::register();
        set_error_handler([self::class,'handleError'],E_ALL);
        set_exception_handler([self::class,'handleException']);
        register_shutdown_function(function (){/* 脚本结束时缓存区域的内容将会自动输出，所以不能把输出控制语句放到这里 */
            //行为监听
            Behaviour::listen(ON_SHUTDOWN);
            //显示trace
            if(SR_DEBUG_MODE_ON and !SR_IS_AJAX) Developer::trace();
        });

        Configger::initialize(self::$convention['CONFIG_BEHAVIOUR']);
        Behaviour::listen(ON_INITED);
    }

    /**
     * Start Application
     * @throws RouteParseFailedException
     */
    public static function start(){
        //执行服务端程序
        Behaviour::listen(ON_START);
        //可以执行一些安全性检查
        //parse uri
        Behaviour::listen(ON_ROUTE);
        $parse_result = Router::parse();
        $rq_module  = $parse_result['m'];
        $rq_contler = $parse_result['c'];
        $rq_action  = $parse_result['a'];
        $rq_params  = $parse_result['p'] and  $_GET = array_merge($_GET,$rq_params); //URL中解析结果合并到$_GET中，$_GET的其他参数不能和之前的一样，否则会被解析结果覆盖,注意到$_GET和$_REQUEST并不同步，当动态添加元素到$_GET中后，$_REQUEST中不会自动添加

        Behaviour::listen(ON_CHECK);
        //dispatch
        $check_result = Dispatcher::check($rq_module,$rq_contler,$rq_action);

        //在执行方法之前定义常量,为了能在控制器的构造函数中使用这三个常量::::define后面不可以接数组
        define('SR_REQUEST_MODULE',     $check_result['m']);//请求的模块
        define('SR_REQUEST_CONTROLLER', $check_result['c']);//请求的控制器
        define('SR_REQUEST_ACTION',     $check_result['a']);//请求的操作

        Behaviour::listen(ON_DISPATCH,[SR_REQUEST_MODULE,SR_REQUEST_CONTROLLER,SR_REQUEST_ACTION]);
        $actionback = Dispatcher::dispatch(SR_REQUEST_MODULE,SR_REQUEST_CONTROLLER,SR_REQUEST_ACTION);
        //exec的结果将用于判断输出缓存，如果为int，表示缓存时间，0表示无限缓存XXX,将来将创造更多的扩展，目前仅限于int

        Behaviour::listen(ON_STOP,[$actionback,[SR_REQUEST_MODULE,SR_REQUEST_CONTROLLER,SR_REQUEST_ACTION]]);
    }


    /**
     * 加载显示模板
     * @param string $tpl template name in folder 'Tpl'
     * @param array|null $vars vars array to extract
     * @param bool $clean it will clean the output cache if set to true
     * @param bool $isfile 判断是否是模板文件
     */
    public static function template(string $tpl,array $vars=null,bool $clean=false,bool $isfile=false) {
        $clean and Response::cleanOutput();
        $isfile or $tpl = SR_PATH_FRAMEWORK."/Template/{$tpl}.php";
        if(!is_file($tpl)){
            $vars['error'] = "'{$tpl}' not found";
            $tpl = SR_PATH_FRAMEWORK.'/Template/systemerror.php';
        }
        $vars and extract($vars, EXTR_OVERWRITE);
        require $tpl;
    }

    /**
     * 处理错误信息
     * @param $code
     * @param $message
     * @param $file
     * @param $line
     * @return true
     */
    public static function handleError($code, $message, $file, $line){
        return ExceptionHandler::handleError($code, $message, $file, $line);
    }

    /**
     * 处理异常
     * @param Throwable $e
     */
    public static function handleException(Throwable $e) {
        return ExceptionHandler::handleException($e);
    }

    /**
     * 取消错误异常等注册并恢复原状
     * @static
     */
    public static function unregister(){
        restore_error_handler();
        restore_exception_handler();
    }

}