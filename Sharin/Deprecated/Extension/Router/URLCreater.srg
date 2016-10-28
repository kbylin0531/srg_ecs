<?php
/**
 * Created by PhpStorm.
 * User: Lin
 * Date: 2015/11/8
 * Time: 16:42
 */
namespace System\Core\Router;
use System\Core\Router;
use System\Exception\CoraxException;
use System\Util\SEK;

/**
 * Class URLCreater URL地址生成器
 * @package System\Core\Router
 */
class URLCreater extends Router{

    /**
     * 创建URL
     * @param string|array $modulelist 模块序列
     * @param string $controller 控制器名称
     * @param string $action 操作（方法）名称
     * @param array $params 参数数组，默认为空数组
     * @param int $mode URL模式，为null或者空参数时默认为系统设置的URL模式
     * @param bool|true $withtail 是否带伪装的后缀
     * @return string 返回创建的URL地址
     * @throws CoraxException
     */
    public static function create($modulelist,$controller,$action,array $params=[],$mode=null,$withtail=true){
        $url = null;
        $mode = (null === $mode)?URL_MODE:intval($mode);
        switch($mode){
            case self::URLMODE_COMMON:
                $url = self::createInCommon($modulelist,$controller,$action,$params);
                break;
            case self::URLMODE_PATHINFO:
                $url = self::createInPathinfo($modulelist,$controller,$action,$params,$withtail);
                break;
            case self::URLMODE_COMPATIBLE:
                $url = self::createInCompatible($modulelist,$controller,$action,$params,$withtail);
                break;
            default:
                throw new CoraxException('Unknown url mode:'.URL_MODE);
        }
        return $url;
    }

    /**
     * 创建普通模式下的URL
     * @param string|array $modules
     * @param string $controller
     * @param string $action
     * @param array $params
     * @return string
     */
    public static function createInCommon($modules,$controller,$action,array $params=[]){
        $queries = [
            self::$convention['URL_CONTROLLER_VARIABLE'] => SEK::toCStyle($controller),
            self::$convention['URL_ACTION_VARIABLE'] => $action,
        ];
        if(isset($modules)){
            if(is_array($modules)){
                $modules = self::toModulesString($modules);
            }
            $queries[self::$convention['URL_MODULE_VARIABLE']] = $modules;
        }
        empty($params) or $queries = array_merge($params,$queries);
        return $_SERVER['SCRIPT_NAME'].'?'.http_build_query($queries);
    }
    /**
     * 创建PATHINFO模式的URL
     * 更具是否开启rewrite功能来决定是否省略入口文件
     * @param $modules
     * @param $controller
     * @param $action
     * @param $params
     * @param bool $withtail
     * @return string
     */
    public static function createInPathinfo($modules,$controller,$action,$params,$withtail=true){
        $url = $_SERVER['SCRIPT_NAME'].self::_createPathinfo($modules,$controller,$action,$params,$withtail);
        REWRITE_ENGINE_ON and self::rewrite($url);
        return $url;
    }

    /**
     * @param $modules
     * @param $controller
     * @param $action
     * @param $params
     * @param bool|true $withtail
     * @return string
     */
    public static function _createPathinfo($modules,$controller,$action,$params,$withtail=true){
        $conf = &self::$convention;
        //模块设置
        if($modules){
            if(is_array($modules)){
                $modules = self::toModulesString($modules);
            }
            $modules = "{$modules}{$conf['MC_BRIDGE']}";
        }else{
            $modules = '';
        }
        //控制器设置
        if($controller){
            $controller = SEK::toCStyle($controller);
        }
        //参数设置
        $params = self::toParametersString($params,self::$convention['PP_BRIDGE'],self::$convention['PKV_BRIDGE']);
        empty($params) or $params = "{$conf['AP_BRIDGE']}{$params}";

        //组装URL
        $url = "/{$modules}{$controller}{$conf['CA_BRIDGE']}{$action}{$params}";
        //伪装的尾巴设置
        if(isset($conf['MASQUERADE_TAIL']) && $withtail){
            $url .= $conf['MASQUERADE_TAIL'];
        }
        return $url;
    }

    /**
     * 创建compatible模式下的URL
     * @param $modules
     * @param $controller
     * @param $action
     * @param $params
     * @param bool $withtail
     * @return string
     */
    public static function createInCompatible($modules,$controller,$action,$params,$withtail=true){
        $var = self::$convention['URL_COMPATIBLE_VARIABLE'];
        $url = "{$_SERVER['SCRIPT_NAME']}?{$var}=". self::_createPathinfo($modules,$controller,$action,$params,$withtail);
        REWRITE_ENGINE_ON and self::rewrite($url);
        return $url;
    }

    /**
     * 对url进行url重写处理，需要借助以.htaccess或者虚拟主机配置才能正常解析URL
     * @param $url
     * @param $url
     */
    private static function rewrite(&$url){
        $pos = stripos($url,self::$convention['REWRITE_HIDDEN']);//获取第一个位置
        if(false !== $pos){
            $url = SEK::strReplaceJustOnce(self::$convention['REWRITE_HIDDEN'],'',$url);
        }
    }

}