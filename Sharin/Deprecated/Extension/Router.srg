<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2015/8/27
 * Time: 14:04
 */
namespace System\Core;
use System\Corax;
use System\Core\Router\DomainCreater;
use System\Core\Router\DomainParser;
use System\Core\Router\RuleCreater;
use System\Core\Router\RuleParser;
use System\Core\Router\URLCreater;
use System\Core\Router\URLParser;
use System\Exception\CoraxException;
use System\Util\SEK;

defined('BASE_PATH') or die('No Permission!');

/**
 * Class Route 路由解析类
 * @package System\Core
 * 相对于ThinkPHP的改变：
 * 解析：
 *      ①common模式,配置时使用数字1
 *      ②pathinfo模式,配置时使用数字2
 *      ③compatible模式：获取pathinfo变量后依据pathinfo模式解析,配置时使用数字3
 *      ④（ThinkPHP框架中使用，本框架中已改为rewrite引擎开关）rewrite模式：语句pathinfo的配置解析，对于解析无影响
 * 创建：
 *      ①common模式
 *      ②pathinfo模式：创建url带上index.php
 *      ③compatible模式：创建url时带上入口文件和pathinfo变量，并语句pathinfo创建规则创建URL
 *      ④（已删除）rewrite模式：根据url模式省略对应的pathinfo变量或者入口文件
 */
class Router{
    /**
     * URL模式
     */
    const URLMODE_COMMON = 1;//最快速的URL访问速度
    const URLMODE_PATHINFO = 2;
    const URLMODE_COMPATIBLE = 3;//兼容模式

    /**
     * 普通模式下的数据源
     */
    const COMMONMODE_SOURCE_GET = 1;
    const COMMONMODE_SOURCE_POST = 2;
    const COMMONMODE_SOURCE_REQUEST = 3;
    const COMMONMODE_SOURCE_INPUT = 4;

    /**
     * 惯例配置配置
     * @var array
     */
    protected static $convention = [
        //直接路由开关
        'DIRECT_ROUTE_ON'    => false,
        //简介路由开关
        'INDIRECT_ROUTE_ON'  => false,
        //直接路由发生在URL解析之前，直接路由如果匹配了URL字符串，则直接链接到指定的模块，否则将进行URL解析和间接路�?
        'DIRECT_ROUTE_RULES'    => [
            //静态路由规则
            'DIRECT_STATIC_ROUTE_RULES' => [],
            //通配符路由规则,具体参考CodeIgniter,内部通过正则表达式实现
            'DIRECT_WILDCARD_ROUTE_RULES' => [],
            //正则表达式规则，
            'DIRECT_REGULAR_ROUTE_RULES' => [],
        ],
        //间接路由在URL解析之后
        'INDIRECT_ROUTE_RULES'   => [],
        //URL创建规则
        'URL_CREATION_RULE'     => [],

        //普通模式 与 兼容模式 获取$_GET变量名称
        'URL_MODULE_VARIABLE'   => '_m',
        'URL_CONTROLLER_VARIABLE'   => '_c',
        'URL_ACTION_VARIABLE'   => '_a',
        'URL_COMPATIBLE_VARIABLE' => '_pathinfo',
        'COMMONMODE_SOURCE' => self::COMMONMODE_SOURCE_GET,

        //兼容模式和PATH_INFO模式下的解析配置，也是URL生成配置
        'MM_BRIDGE'     => '/',//模块与模块之间的连接桥
        'MC_BRIDGE'     => '/',
        'CA_BRIDGE'     => '/',
        'AP_BRIDGE'     => '/co/',//*** 必须保证操作与控制器之间的符号将是$_SERVER['PATH_INFO']字符串中第一个出现的
        'PP_BRIDGE'     => '/',//参数与参数之间的连接桥
        'PKV_BRIDGE'    => '/',//参数的键值对之前的连接桥

        //伪装的后缀，不包括'.'号
        'MASQUERADE_TAIL'   => '.html',
        //重写模式下 消除的部分，对应.htaccess文件下
        'REWRITE_HIDDEN'      => '/index.php',

        //默认的模块，控制器和操作
        'DEFAULT_MODULE'      => 'Home',
        'DEFAULT_CONTROLLER'  => 'Index',
        'DEFAULT_ACTION'      => 'index',

        //是否开启子域名部署
        'DOMAIN_DEPLOY_ON'    => false,
        //解析结果中模块域名是否绑定，false时即使子域名指定了域名，当时URL解析后中还是可以修改的
        'DOMAIN_MODULE_BIND'  => true,
        //子域名部署模式下 的 完整域名
        'FUL_DOMAIN'=>'',
        //使用的协议名称
        'HOST_PROTOCOL' => 'http',
        //使用的端口号，默认为80时会显示为隐藏
        'HOST_PORT' => 80,
        //是否将子域名和模块进行对应
        'SUB_DOMAIN_MODULE_MAPPING_ON'  => true,
        //子域名部署规则
        'SUB_DOMAIN_DEPLOY_RULES' => [
            /**
             * 分别对应子域名模式下 的 [模块、(控制器、(操作、(参数)))]
             * 控制器到参数为可选单元，模块对应着子域名
             * 设置为null是表示不做设置，将使用默认的通用配置
             *
             * 部署规则的反面则对应着 "模块序列"=>"子域名首部" 的键值对
             */
        ],
    ];
    /**
     * 解析结果 组成部件
     * @var array
     */
    private static $parsedResult = [];
    /**
     * 是否已经初始化过
     * @var bool
     */
    protected static $hasInitialized = false;
    /**
     * 域名模块是否绑定
     * 当访问的url匹配了子域名，解析结果中设置了'm'（模块）则认为模块域名进行了绑定操作
     * 绑定之后之后的操作无法修改解析结果中的模块序列
     * @var bool
     */
    protected static $moduleDomainBinded = false;

    /**
     * 不准构造
     */
    protected function __construct(){}

    /**
     * 初始化
     * @param array|null $config 配置，未设置参数或者参数为null时不进行配置
     * @return void
     */
    final public static function init(array $config=null){
        Corax::status('router_init_begin');

        //覆盖自定义配置
        SEK::merge(self::$convention, isset($config) ? $config : Configer::load('route'),true);

        //参数确实时返回默认配置
        self::$parsedResult['m'] = self::$convention['DEFAULT_MODULE'];
        self::$parsedResult['c'] = self::$convention['DEFAULT_CONTROLLER'];
        self::$parsedResult['a'] = self::$convention['DEFAULT_ACTION'];
        self::$parsedResult['p'] = [];

        //重置类的静态属性
        self::$moduleDomainBinded = false;

        //表示已经初始化过了
        self::$hasInitialized = true;

        Corax::status('router_init_done');
    }

    /**
     * 获取解析结果
     * @param string $part
     * @return array|string|null
     */
    final public static function getParsed($part=null){
        self::$hasInitialized or self::init();
        if(isset($part)){
            return isset(self::$parsedResult[$part])?self::$parsedResult[$part]:null;
        }else{
            return self::$parsedResult;
        }
    }

    /**
     * 解析URL中的参数信息，兼容四种模式下的url
     * @param string $hostname 主机名
     * @param string $url pathinfo
     * @param bool|false $forceRefresh
     * @return array 解析结果
     * @throws CoraxException
     */
    final public static function analyse($hostname=null,$url=null,$forceRefresh=false){
        self::$hasInitialized or self::init();
        //检查是否是极速模式
        if(URLMODE_TOPSPEED_ON){
            //极速模式下只使用于common模式,适用于API应用 忽视子域名部署等，直接采用解析结果
            $parsed = URLParser::parse(null,self::URLMODE_COMMON);
            self::pushParsed($parsed);
        }else{

            //在开启子域名部署的情况下，先解析域名
            if(self::$convention['DOMAIN_DEPLOY_ON']){
                $result = DomainParser::getInstance(self::$convention,$forceRefresh)->parse($hostname);
                if(is_array($result)){
                    //MCAP结果合并
                    self::pushParsed($result);
                    //模块域名绑定
                    self::$moduleDomainBinded = (isset($result['m']) and self::$convention['DOMAIN_MODULE_BIND']);
                }
                //else{/*不存在域名对应的结果*/}
            }

            $ruleParser = RuleParser::getInstance(self::$convention,$forceRefresh);
            //解析直接路由
            if(self::$convention['DIRECT_ROUTE_ON']){
                isset($url) or $url = self::getPathInfo();
                $result = $ruleParser->parseDirectRules($url);
                if(null !== $result){
                    if(is_string($result)){
                        $url = $result;//交给URL解析器继续解析
                    }elseif(is_array($result)){
                        return self::pushParsed($result);
                    }else{
                        throw new CoraxException($result); //合理的返回值类型只有string和array，返回非null的其他类型时异常退出
                    }
                }
            }

//            UDK::dumpout($url);

            //解析URL
            $parsed = URLParser::parse($url);
            self::pushParsed($parsed);
            //解析简介路由
            if(self::$convention['INDIRECT_ROUTE_ON']){
                $result = $ruleParser->parseIndirectRules(
                    self::$parsedResult['m'],self::$parsedResult['c'],
                    self::$parsedResult['a'],self::$parsedResult['p']);
                if(null !== $result){
                    return self::pushParsed($result);
                }
            }
        }
        return self::$parsedResult;
    }

    /**
     * 创建当前模式下的URL
     * @param null|string $modulelist 模块列表,未设置参数或者参数为null时，使用当前解析的参数值
     * @param null|string $controller 控制器名称,未设置参数或者参数为null时，使用当前解析的参数值
     * @param null|string $action 操作名称,未设置参数或者参数为null时，使用当前解析的参数值
     * @param array $params 参数列表
     * @param null $mode  URL模式,null时跟随系统
     * @return string URL字符串
     * @throws \Exception
     */
    final public static function build($modulelist=null,$controller=null,$action=null,$params=[],$mode=null){
        self::$hasInitialized or self::init();

        $url = null; //创建的地址

        $moduleback = null; //保存原始模块

        //如果未设置对应的组建或者对应的组建为null，则默认使用当前访问的URI解析的结果
        isset($modulelist) or $modulelist = self::$parsedResult['m'];
        isset($controller) or $controller = self::$parsedResult['c'];
        isset($action)     or $action     = self::$parsedResult['a'];

        if(URLMODE_TOPSPEED_ON){
            //极速模式下忽略路由和域名关系 （注：如果非要在极速模式下跳转到另一个方法，可以在该方法中写入跳转代码）
            $url = URLCreater::create($modulelist,$controller,$action,$params,self::URLMODE_COMMON);
        }else{
            //检查端口号的配置
            $port = (self::$convention['HOST_PORT'] === 80)?'':':'.self::$convention['HOST_PORT'];

            //检查子域名部署情况
            if(self::$convention['DOMAIN_DEPLOY_ON']){
                $hostname = DomainCreater::getInstance(self::$convention)->create($modulelist);
                if(isset($hostname)){
                    //消除模块
                    $moduleback = $modulelist;
                    $modulelist = null;
                    $url = $hostname.$port;
                }
            }

            $protocal = self::$convention['HOST_PROTOCOL'];
            //如果还是null 表示未匹配到子域名
            $url .= isset($url)?"{$protocal}:/{$url}{$port}":"{$protocal}://{$_SERVER['SERVER_NAME']}{$port}";

            //检查路由规则情况(直接路由)
            if(self::$convention['DIRECT_ROUTE_ON']) {
                RuleCreater::getInstance(self::$convention)->create($moduleback,$controller,$action,$params,$modulelist);
            }

            //初始化 链接创建者
            if(URLMODE_TOPSPEED_ON){
                $url .= URLCreater::create($modulelist,$controller,$action,$params,self::URLMODE_COMMON);
            }else{
                //转换成合法的模式常量
                $mode = (null === $mode)?URL_MODE:intval($mode);
                $url .= URLCreater::create($modulelist,$controller,$action,$params,$mode);
            }
        }

        return $url;
    }

    /**
     * 压入解析结果
     * @param   array $result 解析结果
     * @return array
     */
    private static function pushParsed(array &$result) {
        if(!self::$moduleDomainBinded) {
            //未绑定的情况下允许进行模块压入
            isset($result['m']) and self::$parsedResult['m'] = $result['m'];
        }
        isset($result['c']) and self::$parsedResult['c'] = $result['c'];
        isset($result['a']) and self::$parsedResult['a'] = $result['a'];
        isset($result['p']) and SEK::merge(self::$parsedResult['p'],$result['p']);
        //url解析结果的参数释放到$_GET数组中
        $_GET = array_merge(self::$parsedResult['p'],$_GET);
        return self::$parsedResult;
    }



    /**
     * 创建URL组件的key
     * 返回格式："模块序列/控制器名称/操作名称/参数1/参数1的值/参数2/参数2的值/...."
     * @param string|array $modules 模块序列
     * @param string $ctler 控制器名称
     * @param string $action 操作名称
     * @param array $params 参数列表
     * @return string
     * @throws CoraxException
     */
    protected static function buildKey($modules=null,$ctler=null,$action=null,$params=null) {
        $key = '';
        if($modules) $key .= self::toModulesString($modules).'/';
        if($ctler and is_string($ctler)) $key .= "{$ctler}/";
        if($action and is_string($action)) $key .= "{$action}/";
        //参数转换成数组形式
        if($params){
            if(is_array($params)){
                $temp = '';
                foreach($params as $pname=>$pval){
                    $temp .= "{$pname}/{$pval}/";
                }
                $key .= trim($temp,'/');
            }else{
                throw new CoraxException($params);
            }
        }
        return strtolower($key);
    }


    /**
     * 删除伪装的url后缀
     * @param string|array $uri 需要去除尾巴的字符串或者字符串数组（当数组中存在其他元素时忽略）
     * @return string
     * @throws CoraxException
     */
    protected static function stripMasqueradeTail($uri){
        if(is_string($uri)){
            $uri = trim($uri);
            $position = stripos($uri,self::$convention['MASQUERADE_TAIL']);
            //$position === false 表示 不存在伪装的后缀或者相关带嫌疑的url部分
            if(false !== $position and strlen($uri) === ($position + strlen(self::$convention['MASQUERADE_TAIL'])) ){
                //伪装的后缀存在且只出现在最后的位置时
                $uri = substr($uri,0,$position);
            }
        }elseif(is_array($uri)){
            foreach($uri as $key=> &$val){
                $val = self::stripMasqueradeTail($val);
            }
        }else{
            throw new CoraxException($uri);
        }
        return $uri;
    }

    /**
     * 获取 $_SERVER['PATH_INFO'] 的原始值
     * @return string
     */
    protected static function getPathInfo(){
        static $pathinfo = null;
        if(null === $pathinfo){
            if(isset($_SERVER['PATH_INFO'])) {
                $pathinfo = $_SERVER['PATH_INFO'];
            }else{
                //访问的URL：http://localhost:8056/corax/
                //出现的异常时将corax/视为action导致访问的资源是/Home/Index/corax
                //于是会出现调度异常
                //ReflectionException : Method Application\Home\Controller\IndexController::corax() does not exist
//                UDK::dumpout($_SERVER['REQUEST_URI'], $_SERVER['SCRIPT_NAME']);

                if(strlen($_SERVER['REQUEST_URI']) > strlen($_SERVER['SCRIPT_NAME'])){
                    //在不支持PATH_INFO...或者PATH_INFO不存在的情况下(URL省略将被认定为普通模式)
                    //REQUEST_URI获取原生的URL地址进行解析(返回脚本名称后面的部分)
                    $pos = stripos($_SERVER['REQUEST_URI'],$_SERVER['SCRIPT_NAME']);
                    if(0 === $pos){//PATHINFO模式
                        $pathinfo = substr($_SERVER['REQUEST_URI'], strlen($_SERVER['SCRIPT_NAME']));
                    }else{
                        //重写模式
                        $pathinfo = $_SERVER['REQUEST_URI'];
                    }
                }
            }
        }
//        UDK::dumpout($pathinfo,$_SERVER['REQUEST_URI'],$_SERVER['SCRIPT_NAME'],substr($_SERVER['REQUEST_URI'], strlen($_SERVER['SCRIPT_NAME'])));
        return $pathinfo;
    }

    /**
     * 模块序列转换成数组形式
     * 且数组形式的都是大写字母开头的单词形式
     * @param string|array $modules 模块序列
     * @param string $delimiter 模块之间的分割符
     * @return array
     * @throws CoraxException
     */
    protected static function toModulesArray($modules,$delimiter=null){
        isset($delimiter) or $delimiter = self::$convention['MM_BRIDGE'];
        if(is_string($modules)){
            $modules = explode($delimiter,$modules);
        }
        if(!is_array($modules)){
            throw new CoraxException('Parameter should be an array!');
        }
        return array_map(function ($val) {
            return SEK::toJavaStyle($val);
        }, $modules);
    }

    /**
     * 将参数序列装换成参数数组，应用Router模块的配置
     * @param string $params 参数字符串
     * @param string $ppb 参数对之间的分隔符
     * @param string $pkvb 参数键值对之间的分隔符
     * @return array
     */
    protected static function toParametersArray($params,$ppb,$pkvb){//解析字符串成数组
        $pc = [];
        if($ppb !== $pkvb){//使用不同的分割符
            $parampairs = explode($ppb,$params);
            foreach($parampairs as $val){
                $pos = strpos($val,$pkvb);
                if(false === $pos){
                    //非键值对，赋值数字键
                }else{
                    $key = substr($val,0,$pos);
                    $val = substr($val,$pos+strlen($pkvb));
                    $pc[$key] = $val;
                }
            }
        }else{//使用相同的分隔符
            $elements = explode($ppb,$params);
            $count = count($elements);
            for($i=0; $i<$count; $i += 2){
                if(isset($elements[$i+1])){
                    $pc[$elements[$i]] = $elements[$i+1];
                }else{
                    //单个将被投入匿名参数,先废弃
                }
            }
        }
        return $pc;
    }

    /**
     * 模块学列数组转换成模块序列字符串
     * 模块名称全部小写化
     * @param array|string $modules 模块序列
     * @return string
     * @throws CoraxException
     */
    protected static function toModulesString($modules){
        if(is_array($modules)){
            foreach($modules as &$modulename){
                $modulename = SEK::toCStyle($modulename);
            }
            $modules = implode(self::$convention['MM_BRIDGE'],$modules);
        }
        if(!is_string($modules)) throw new CoraxException('Invalid Parameters');
        return trim($modules);
    }

    /**
     * 将参数数组转换成参数序列，应用Router模块的配置
     * @param array $params 参数数组
     * @param string $ppb 参数对之间的分隔符
     * @param string $pkvb 参数键值对之间的分隔符
     * @return string
     */
    protected static function toParametersString(array $params,$ppb,$pkvb){
        //希望返回的是字符串是，返回值是void，直接修改自$params
        $temp = '';
        if($params){
            foreach($params as $key => $val){
                $temp .= "{$key}{$pkvb}{$val}{$ppb}";
            }
            return substr($temp,0,strlen($temp) - strlen($ppb));
        }else{
            return $temp;
        }
    }

}