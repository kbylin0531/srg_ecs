<?php
/**
* 系统预置的核心类库
*/
namespace Sharin ;
use Exception as E;
use Sharin\Core\Storage;
use Sharin\Exceptions\ConfigParseFailedException;
use Sharin\Library\Helper\Ini;
use Sharin\Library\Helper\XMLer;
use Sharin\Library\Trace;
use Sharin\Exceptions\MethodNotFoundException;
use Sharin\Exceptions\ParameterInvalidException;

/**
 * Class Utils 内置工具类
 * @package Sharin
 */
final class Utils {
    /**
     * 将C风格字符串转换成JAVA风格字符串
     * C风格      如： sub_string
     * JAVA风格   如： SubString
     * @param string $str
     * @param bool $orien 转换方向，默认C 风格转 Java 风格
     * @return string 返回风格转换后的类型
     */
    public static function styleStr(string $str,bool $orien=true){
        return $orien? ucfirst(preg_replace_callback('/_([a-zA-Z])/',function($match){return strtoupper($match[1]);},$str)):
            strtolower(ltrim(preg_replace('/[A-Z]/', '_\\0', $str), '_'));
    }

    /**
     * 检查继承关系
     * @param string $myname
     * @param string $parentnm
     * @return bool
     */
    public static function checkExtends(string $myname,string $parentnm){
        if(!class_exists($myname)) return false;
        $list = class_parents($myname,false);
        return isset($list[$parentnm]);
    }

    /**
     * 检查接口实现关系
     * @param $myname
     * @param $interfacename
     * @return bool
     */
    public static function checkImplement(string $myname,string $interfacename){
        if(!class_exists($myname)) return false;
        $list = class_implements($myname,false);
        return isset($list[$interfacename]);
    }

    /**
     * 检查trait使用关系
     * @param $myname
     * @param $traitname
     * @return bool
     */
    public static function checkTraitUse(string $myname,string $traitname){
        if(!class_exists($myname)) return false;
        $list = class_uses($myname,false);
        return isset($list[$traitname]);
    }

    /**
     * 将参数二的配置合并到参数一种，如果存在参数一数组不存在的配置项，跳过其设置
     * (不会覆盖)
     * @param array $dest 被覆盖者
     * @param array $sourse 覆盖者
     * @param bool $covermode 为true的情况下无论目标健是否存在否会覆盖，否则会先检测目标是否存在
     * @return mixed
     */
    public static function merge(array $dest, array $sourse,bool $covermode=true){
        foreach($sourse as $key=>$val){
            if($covermode){
                //fugei mose
                if (isset($dest[$key]) and is_array($val)) {
                    $dest[$key] = self::merge($dest[$key],$val,true);/* se shuju,zo erbi tyi */
                }else{
                    if(is_int($key)){
                        if(!in_array($val,$dest)) $dest[] = $val;
                    }else{
                        $dest[$key] = $val;
                    }
                }
            }else{
                //kno saiqu wumao sai
                if(isset($dest[$key])){
                    if (is_array($val)) {
                        $dest[$key] = self::merge($dest[$key],$val,false);
                    }else{
                        if(is_int($key)){
                            if(!in_array($val,$dest)) $dest[] = $val;
                        }else{
                            $dest[$key] = $val;
                        }
                    }
                }
            }
        }
        return $dest;
    }

    /**
     * 过滤掉数组中与参数二计算值相等的值，可以是保留也可以是剔除
     * @param array $array
     * @param callable|array|mixed $comparer
     * @param bool $leave
     * @return void
     */
    public static function filter(array &$array, $comparer=null, $leave=true){
        static $result = [];
        $flag = is_callable($comparer);
        $flag2 = is_array($comparer);
        foreach ($array as $key=>$val){
            if($flag?$comparer($key,$val):($flag2?in_array($val,$comparer):($comparer === $val))){
                if($leave){
                    unset($array[$key]);
                }else{
                    $result[$key] = $val;
                }
            }
        }
        $leave or $array = $result;
    }

    /**
     * 从字面商判断$path是否被包含在$scope的范围内
     * @param string $path 路径
     * @param string $scope 范围
     * @return bool
     */
    public static function checkInScope($path, $scope) {
        if (false !== strpos($path, '\\')) $path = str_replace('\\', '/', $path);
        if (false !== strpos($scope, '\\')) $scope = str_replace('\\', '/', $scope);
        $path = rtrim($path, '/');
        $scope = rtrim($scope, '/');
        return (SR_IS_WIN ? stripos($path, $scope) : strpos($path, $scope)) === 0;
    }
}

/**
 * Class Behaviour 行为控制
 * 各个行为组成了应用的生命周期
 * @package Sharin
 */
final class Behaviour {
    /**
     * 标签集合
     * key可能是某种有意义的方法，也可能是一个标识符
     * value可能是闭包函数或者类名称
     * 如果是value类名称，则key可能是其调用的方法的名称（此时会检查这个类中是否存在这个方法），也可能是一个标识符
     * @var array
     */
    private static $tags = [];

    public static function init(array $tags=null){
        if(null !== $tags){
            self::$tags = $tags;
        }
    }

    /**
     * 动态注册行为
     * @param string|array $tag 标签名称，如果是数组则意味着批量导入
     * @param mixed|array $behavior 行为名称,为array类型时将进行批量注册
     * @return void
     */
    public static function register($tag, $behavior=null) {
        if(is_array($tag)){
            self::$tags = array_merge(self::$tags,$tag);
        }else{
            if (!isset(self::$tags[$tag]))  self::$tags[$tag] = [];
            if (is_array($behavior)) {
                self::$tags[$tag] = array_merge(self::$tags[$tag], $behavior);
            } else {
                self::$tags[$tag][] = $behavior;
            }
        }
    }

    /**
     * 监听标签的行为
     * @param string $tag 标签名称
     * @param mixed $params 传入回调闭包或者对象方法的参数
     * @return void
     */
    public static function listen($tag, $params = null) {
        if(SR_DEBUG_MODE_ON) Developer::status($tag);
        if (!empty(self::$tags[$tag])) foreach(self::$tags[$tag] as $name){
            if (false === self::exec($name, $tag, $params)) break; // 如果返回false 则中断行为执行
        }
    }

    /**
     * 执行某个行为
     * @param string|callable $callableorclass 闭包或者类名称
     * @param string $tag 方法名（标签名）
     * @param mixed $params 方法的参数
     * @return mixed
     * @throws ParameterInvalidException
     */
    private static function exec($callableorclass,string $tag , $params = null) {
        static $_instances = [];
        if (is_callable($callableorclass)) {
            //如果是闭包，则直接执行闭包函数
            return $callableorclass($params);
        }elseif(is_string($callableorclass)){
            if(!isset($_instances[$callableorclass])){
                $_instances[$callableorclass] = new $callableorclass();
            }
            $obj = $_instances[$callableorclass];
            if(!is_callable([$obj, $tag])) $tag = 'run';//tag默认是方法名称
            return $obj->$tag($params,$tag);
        }
        throw new ParameterInvalidException($callableorclass,['Closure','string']);
    }
}

/**
 * Class Developer
 * 开发者工具
 * @package Sharin
 */
final class Developer {

    private static $showTrace = SR_DEBUG_MODE_ON;

    /**
     * @var array
     */
    private static $highlightes = [];
    /**
     * @var array
     */
    private static $_status = [];
    /**
     * @var array
     */
    private static $_traces = [];

    /**
     * Open the page trace
     * @return void
     */
    public static function openTrace(){
        self::$showTrace = true;
    }

    /**
     * Close the page trace
     * @return void
     */
    public static function closeTrace(){
        self::$showTrace = false;
    }

    /**
     * record the runtime's time and memory usage
     * @param null|string $tag tag of runtime point
     * @return void
     */
    public static function status($tag){
        SR_DEBUG_MODE_ON and self::$_status[$tag] = [
            microtime(true),
            memory_get_usage(),
        ];
    }

    /**
     * import status
     * @param string $tag
     * @param array $status
     */
    public static function import($tag,array $status){
        self::$_status[$tag] = $status;
    }

    /**
     * 记录下跟踪信息
     * @param string|mixed $message
     * @param ...
     * @return void
     */
    public static function trace($message=null){
        static $index = 0;
        if(!SR_DEBUG_MODE_ON) return;
        if(null === $message){
            !SR_IS_CLI and self::$showTrace and Trace::show(self::$highlightes,self::$_traces,self::$_status);
        }else{
            $location = debug_backtrace();
            if(isset($location[0])){
                $location = "{$location[0]['file']}:{$location[0]['line']}";
            }else{
                $location = $index ++;
            }
            if(func_num_args() > 1) $message = var_export(func_get_args(),true);
            if(!is_string($message)) $message = var_export($message,true);
            //同一个位置可能trace多次
            if(isset(self::$_traces[$location])){
                $index ++;
                $location = "$location ({$index})";
            }
            self::$_traces[$location] = $message;
        }
    }
}

/**
 * Class ClassLoader 类加载器
 * @package Sharin
 */
final class ClassLoader {
    /**
     * 加载规则
     * [
     *      0   => '',//加载路径，相对于SR_PATH_BASE
     *      1   => '',//文件后缀名称
     * ]
     * @var array
     */
    protected static $rules = [
        0           => [
            'path'  => 'Library',
        ],
        'Sharin'    => [
            'ext'   => 'psrg',
        ],
        'Explorer'  => [
            'ext'   => 'class.php',
        ],
    ];

    /**
     * @var array array of key-valure pairs (name to relative path)
     */
    private static $map = [];

    /**
     * import classes from outer
     * @param array $map
     */
    public static function import(array $map){
        $map and self::$map = array_merge(self::$map,$map);
    }

    /**
     * default loader for this system
     * @param string $clsnm class name
     * @return void
     */
    public static function load($clsnm){
        if(isset(self::$map[$clsnm])) {
            include_once self::$map[$clsnm];
        }else{
            $pos = strpos($clsnm,'\\');
            if($pos === false){
                $rel = $clsnm;
                $index = 0;
            }else{
                $rel = str_replace('\\', '/', $clsnm);
                $index = substr($clsnm,0,$pos);
            }

            $path = isset(self::$rules[$index]['path'])? self::$rules[$index]['path'] : '';
            $ext  = isset(self::$rules[$index]['ext'])? self::$rules[$index]['ext'] : 'php';//兼容
            $path = SR_PATH_BASE."/{$path}/{$rel}.{$ext}";
            is_file($path) and require(self::$map[$clsnm] = $path);
        }
        //检查是否存在继承关系
        if(class_exists($clsnm,false) and Utils::checkExtends($clsnm,Component::class)){
            call_user_func([$clsnm,'initializationize']);
        }
    }

    /**
     * register class autoloader
     * @param callable $autoloader
     * @throws Exception
     */
    public static function register(callable $autoloader=null){
        if(!spl_autoload_register($autoloader?$autoloader:[static::class,'load'],false,true)){
            Exception::throwing('Faile to register class autoloader!');
        }
    }

}

/**
 * Class Exception 异常错误类
 * 同时负责管理错误和异常的管理
 * @package Sharin
 */
class Exception extends E {

    /**
     * Exception constructor.
     */
    public function __construct(){
        $message = count($args = func_get_args()) > 1? var_export($args,true) : $args[0];
        $this->message = is_string($message)?$message:var_export($message,true);
    }

    /**
     * 直接抛出异常信息
     * @param ...
     * @return mixed
     * @throws Exception
     */
    public static function throwing(){
        $clsnm = static::class;//extend class name
        throw new $clsnm(func_get_args());
    }
}

/**
 * Class Configger 配置管理类
 * @package Sharin\Core
 */
final class Configger {
    /**
     * 配置类型
     * 值使用字符串而不是效率更高的数字是处于可以直接匹配后缀名的考虑
     */
    const TYPE_PHP     = 'php';
    const TYPE_INI     = 'ini';//Initialization File - 初始化文件,是windows的系统配置文件所采用的存储格式，统管windows的各项配置
    const TYPE_YAML    = 'yaml';
    const TYPE_XML     = 'xml';
    const TYPE_JSON    = 'json';
    const TYPE_SERIAL  = 'serial';

    /**
     * @var array 本类的配置
     */
    private static $config = [
        //是否开启Lite
        'USE_LITE'          => true,
        //Lite针对的类名称的列表
        'LITE_CLASS_LIST'   => [
            'Sharin\\Core\\Storage',
            'Sharin\\Core\\Cache',
        ],
        //运行时配置目录（其下的配置文件只在运行时由程序生成）
        'RUNTIME_CONF_PATH'  => SR_PATH_DATA.'/Configuration/',
    ];

    /**
     * @var array 配置缓存
     */
    private static $_cache = [];

    /**
     * 初始化，配置管理类
     * 允许多次调用
     * @param array|null $config
     */
    public static function initialize(array $config=null){
        null !== $config and self::$config = Utils::merge(self::$config,$config,true);

        //清空之前的配置
        self::$_cache = [];

        if(self::$config['USE_LITE']){
            //预处理后的文件
            $litefile = SR_PATH_RUNTIME.'/configger.php';
//            if(!Storage::has($litefile,Storage::READ_ACCESS) or Storage::mtime(SR_PATH_CONFIG) > Storage::mtime($litefile)){ //不能使用Storage的原因是避免Storage过早地出场
            if(!is_file($litefile) or !is_readable($litefile) or (filemtime(SR_PATH_CONFIG) > filemtime($litefile))) {
                //缓存文件不存在，或者已经过期（配置目录比缓存文件新旧表示过期）需要重新编译
                if(!empty(self::$config['LITE_CLASS_LIST'])) foreach (self::$config['LITE_CLASS_LIST'] as $clsnm){
                    self::load($clsnm);
                }
                // Closure is not suggest in config file due to var_export could not do well with closure
                // it will be translated to 'method Closure::__set_state()'
                self::store($litefile,self::$_cache);
//                Storage::write($litefile,'<?php return '.var_export(self::$_cache,true).';') or Exception::throwing("配置无法写入文件'{$litefile}'");
//                    dumpout('缓存文件不存在，或者已经过期（配置目录比缓存文件新旧表示过期），已经完成lite文件的建立工作', self::$_cache);
            }else{
                //直接读取lite文件
                self::$_cache = self::parse($litefile,self::TYPE_PHP);
//                    dumpout('直接读取lite文件',class_exists('Sharin\\Core\\Storage',false)? 'Storage已经加载到内存中' : 'Storage未载入内存' ,self::$_cache);
            }
        }else{
            //设么都不做，每次通过load方法加载外部配置都会将配置加入到缓存中
//                dumpout('设么都不做，每次通过load方法加载外部配置都会将配置加入到缓存中',self::$_cache);
        }
    }
//------------------------------------ 类配置方法集合 : load <== loadInner + loadOuter -------------------------------------------------------------------------------------
    /**
     * 获取类的完整配置
     * @param string $clsnm 类名称
     * @param bool $is_component 该类是否继承自Component类
     * @return array
     */
    public static function load(string $clsnm,bool $is_component=null){
        Developer::trace(isset(self::$_cache[$clsnm]) ? self::$_cache[$clsnm] : $clsnm);
        if(!isset(self::$_cache[$clsnm])){
            null === $is_component and $is_component = Utils::checkExtends($clsnm,Component::class);
            self::$_cache[$clsnm] = self::loadInner($clsnm,$is_component);
            self::$_cache[$clsnm] = Utils::merge(self::$_cache[$clsnm],self::loadOuter($clsnm,$is_component),true);//内部的配置即完整的，不需要外部额外添加，所以参数三设置为false(错误的，因为针对健为数字的情况，外部配置可能会无效)
        }
        return self::$_cache[$clsnm];
    }

    /**
     * 获取类的内置配置
     *  注：只有Component类及子类才能通过convention方法获取内置配置，其他类只能获取到空的数组，如果类不存在时也会返回空数组
     * @param string $clsnm 类名称
     * @param bool $is_component 该类是否继承自Component类
     * @return array 返回类内部的配置
     */
    public static function loadInner(string $clsnm,bool $is_component=null){
        null === $is_component and $is_component = Utils::checkExtends($clsnm,Component::class);
        return $is_component ? call_user_func([$clsnm,'convention']) : [];
    }

    /**
     * 获取类；类的外部配置（包括外部全局和外部应用两部分）
     * @param string $clsnm 类名称
     * @param bool $is_component 该类是否继承自Component类
     * @return array
     */
    public static function loadOuter(string $clsnm,bool $is_component=null) {
        null === $is_component and $is_component = Utils::checkExtends($clsnm,Component::class);

        //获取类的标识符名称
        $idname = $is_component ?
            call_user_func([$clsnm,'getName']):
            strtolower(str_replace('\\','/',ltrim($clsnm,'\\')));
        //类的短名称与长完整名称添加对应
//        self::$_map[$idname] = $clsnm;

        strpos($idname,'.') !== false and $idname = str_replace('.', '/' ,$idname);

        $path = SR_PATH_BASE."/Config/{$idname}.php";

//            dumpout($idname,$path,is_readable($path),self::parse($path,self::TYPE_PHP) );
        //读取全局配置
        $result = is_readable($path) ? self::parse($path,self::TYPE_PHP) : [];
        if(defined('SR_PATH_APP')){
            //应用配置读取
            $appconf = SR_PATH_APP."/Config/{$idname}.php";
            if(is_readable($appconf) and $appconf = self::parse($appconf,self::TYPE_PHP)){
                is_array($appconf) and $result = Utils::merge($result,$appconf,true);
            }
        }
        return $result;
    }

//------------------------------------ user config of dynamic -------------------------------------------------------------------------------------
    /**
     * read the user-defined config in SR_PATH_RUNTIME
     * @param string $identify config identify
     * @param mixed $replacement
     * @return array
     */
    public static function read(string $identify,$replacement=[]){
        $path = self::_id2path($identify,true);//读取时必须保证文件是存在的
        if($path and Storage::has($path,Storage::READ_ACCESS)){
            return self::parse($path,self::TYPE_PHP);
        }
        return $replacement;
    }

    /**
     * write user-config to file
     * @param string $identify
     * @param array $config
     * @return bool
     */
    public static function write(string $identify,array $config){
        $path = self::_id2path($identify,false);
        return self::store($path,$config,self::TYPE_PHP);
    }

    /**
     * 将配置项转换成配置文件路径
     * 配置项中的"."会被替换为目录分隔符"/"
     * @param string $item 配置项
     * @param mixed $check 检查文件是否存在
     * @return false|string 返回配置文件路径，参数二位true并且文件不存在时返回null
     */
    private static function _id2path($item,$check=true){
        strpos($item,'.') and $item = str_replace('.',DIRECTORY_SEPARATOR,$item);
        $path = self::$config['RUNTIME_CONF_PATH']."/{$item}.php";
        return !$check || is_readable($path)?$path:false;
    }


//------------------------------------ 通用工具 -------------------------------------------------------------------------------------

    public static function store(string $path,array $data,string $type=''){
        $type or $type = pathinfo($path, PATHINFO_EXTENSION);
        switch ($type) {
            case self::TYPE_PHP:
                return Storage::write($path,'<?php return '.var_export($data,true).';');
                break;
            case self::TYPE_INI:
                return Storage::write($path,Ini::create($data));
                break;
            case self::TYPE_YAML:
                return yaml_emit_file($path,$data,YAML_UTF8_ENCODING);
                break;
            case self::TYPE_XML:
                return Storage::write($path,XMLer::arrayToXml($data));
                break;
            case self::TYPE_JSON:
                return Storage::write($path,json_encode($data));
                break;
            case self::TYPE_SERIAL:
                return Storage::write($path,serialize($data));
                break;
        }
        return false;
    }

    /**
     * 解析配置文件爱呢
     * @param string $path 配置文件的路径
     * @param string|null $type 配置文件的类型,参数为null时根据文件名称后缀自动获取
     * @param callable $parser 配置解析方法 有些格式需要用户自己解析
     * @return array
     * @throws ConfigParseFailedException
     */
    public static function parse(string $path,string $type='',callable $parser=null){
        if(!is_readable($path)) return [];//文件不存在或者不可以读取
        $type or $type = pathinfo($path, PATHINFO_EXTENSION);
        switch ($type) {
            case self::TYPE_PHP:
                $result = include $path;
                break;
            case self::TYPE_INI:
                return Ini::parse($path);
                break;
            case self::TYPE_YAML:
                $result = yaml_parse_file($path);
                break;
            case self::TYPE_XML:
                $result = (array)simplexml_load_file($path);
                break;
            case self::TYPE_JSON:
                $result = json_decode(file_get_contents($path), true);
                break;
            case self::TYPE_SERIAL:
                $content = file_get_contents($path);
                $result = unserialize($content);
                break;
            default:
                if($parser){
                    $result = $parser($path);
                }else{
                    throw new ConfigParseFailedException($path,$type,$parser);
                }
        }
        return is_array($result)? $result : [];
    }
}

/**
 * Class Component 组件类
 *
 * 作为系统各个核心类的基类，实现了自动初始化的行为和驱动管理的功能
 *
 * 调用静态方法等于直接使用系统内置的驱动，较实例化驱动高效
 *
 * 规则：
 *  一、驱动中存在与实例名称一致的方法
 *      调用示例如下：
 *          Component::{method}();
 *          Component::instantiate()->{method}();
 *  二、静态调用可以基于实例，需要设置
 *      设置了 BASE_ON_INSTANCE = true 时，类加载时会自动初始化驱动
 *
 * @package Sharin
 */
abstract class Component {
    /**
     * 继承类配置
     * @var array
     */
    private static $_conventions = [];

    /**
     * @var object[] 静态驱动列表
     */
    private static $_static_drivers = [];

    /**
     * @var object[] 驱动列表
     */
    private static $_drivers = [];
    /**
     * @var string 组件名称（是该组件的唯一标识符），为空时表示自动获取，否则取该类的静态name属性作为短名称
     */
    protected static $name = null;

    /**
     * @var string 错误信息，只能内部设置
     */
    protected static $_error = '';
    /**
     * 获取组件名称
     * 组件名称是类名称的变体，将"\\"改为"."，并且全部转为小写
     * @return string
     */
    public static function getName(){
        null === static::$name and static::$name = strtolower(str_replace('\\','.',ltrim(static::class,'\\')));
        return static::$name;
    }

    /**
     * 获取错误信息
     * 获取一次后错误信息将被重置
     * @return string
     */
    public static function getError(){
        $error = static::$_error;
        static::$_error = '';
        return $error;
    }
    /**
     * 类自动加载时自动调用
     * 需要注意的是继承类中需要调用__initialization来获取Component的行为
     * @param array|null $config 参数非空是留给继承类覆盖时调用的
     * @return void
     */
    public static function initializationize(array $config = null){
        static $_convention = [ /* jito wule tigo xinen */
            BASE_ON_DRIVER        => FALSE,   //静态方法是否基于驱动
            BASED_DRIVER_INDEX    => '',      //静态方法基于的驱动角标

            DRIVER_CLASS_LIST     => [],      //驱动类的列表
            DRIVER_CONFIG_LIST    => [],      //驱动类列表参数
        ];

        $clsnm = static::class;

        //类配置设置  覆盖优先级 $config > $full > $_convention
        $convention = $_convention;
        $full = Configger::load($clsnm,true);
//            dumpout($full,Utils::merge($convention,$full),$config);
        $config and $convention = Utils::merge($convention,$config);//覆盖方法中传递的
        $full   and $convention = Utils::merge($convention,$full);//外部定义的
        self::$_conventions[$clsnm] = $convention;

    }

    /**
     * 获取静态驱动
     * @return object|false 返回驱动，驱动不存在时返回false
     */
    final public static function getStaticDriver(){
        $static = static::class;
        if(!isset(self::$_static_drivers[$static])){
            $instance = self::instance(self::convention(BASED_DRIVER_INDEX,-1));/* -1 kennia mao gei*/
            if(false === $instance){
                return false;
            }
            self::$_static_drivers[$static] = $instance;
        }
        return self::$_static_drivers[$static];
    }

    /**
     * 设置惯例配置
     * @param string $name
     * @param mixed $value
     * @return bool
     */
    final public static function setConvention(string $name,$value){
        $clsnm = static::class;
        if(empty(self::$_conventions[$clsnm])){
            return false;
        }
        if(strpos($name,'.') !== false){
            $target = &self::$_conventions[$clsnm];
            $name = explode('.',$name);
            $len = count($name);
            $flag = $len -1 ;
            for ($i = 0 ; $i < $len ; $i ++) {
                $key = $name[$i];
                if($flag === $len){
                    $target[$key] = $value;
                }else{
                    $target[$key] = [];
                    $target = & $target[$key];
                }
            }
        }else{
            self::$_conventions[$clsnm][$name] = $value;
        }
        return true;
    }


    /**
     * 获取配置（规矩）
     * @param string $name 配置名称，涉及到子配置时使用"."连接
     * @param mixed $alternate 替补，如果目标配置不存在
     * @return mixed
     */
    final public static function convention(string $name='',$alternate=null){
        $clsnm = static::class;
        isset(self::$_conventions[$clsnm]) or self::$_conventions[$clsnm] = [];
        if('' === $name){
            return self::$_conventions[$clsnm];
        }elseif(strpos($name,'.') !== false){/* ummo yule "."ao qudo lofyi xinen  */
            $target = self::$_conventions[$clsnm];
            $name = explode('.',$name);
            foreach ($name as $nm) {
                if(isset($target[$nm])){
                    $target = $target[$nm];
                }else{
                    return $alternate;
                }
            }
            return $target;
        }else{
            return isset(self::$_conventions[$clsnm][$name]) ? self::$_conventions[$clsnm][$name] : $alternate;
        }
    }

    /**
     * 获取自身的一个驱动实例
     * 来实现自身的操作
     * @param int|string|null $index 角标，如果是null时表示自动获取
     * @return object|false
     */
    public static function instance($index=null){
        $static = static::class;
        isset(self::$_drivers[$static]) or self::$_drivers[$static] = [];
        if(!isset(self::$_drivers[$static][$index])){
            $classes = self::convention('DRIVER_CLASS_LIST',[]);
            if(null === $index){
                $clsnm = reset($classes);
                if(false === $clsnm) {
                    return false;
                }
            } else {
                if(!isset($classes[$index])){
                    return false;
                }
                $clsnm = $classes[$index];
            }
            $plist = self::convention('DRIVER_CONFIG_LIST',[]);
            $params = isset($plist[$index])? $plist[$index] : reset($plist);
            self::$_drivers[$static][$index] = $params ? new $clsnm($params) : new $clsnm();
        }
        return self::$_drivers[$static][$index];
    }

    /**
     * 调用不存在的静态方法时
     * @param string $method method name
     * @param array $arguments method arguments
     * @return mixed
     * @throws MethodNotFoundException
     */
    public static function __callStatic(string $method,array $arguments) {
        if($driver = static::instance(null) and is_callable($callable = [$driver, $method])) {
            return call_user_func_array($callable, $arguments);
        }
        Developer::trace('MethodNotFoundException:'.static::class.' '.$method);
        return false;
    }
}


/**
 * Interface BehaviourInterface
 * 行为接口
 * @package Sharin\Interfaces
 */
interface BehaviourInterface {
    /**
     * @param string $tag 行为接触点
     * @param mixed $parameters 外部传入参数
     * @return mixed
     */
    public function run($tag,$parameters);

}