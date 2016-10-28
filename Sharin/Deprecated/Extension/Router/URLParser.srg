<?php
/**
 * User: Lin
 * Email: linzhv@qq.com
 * Date: 2015/11/08
 * Finished:2015/12/05
 * Time: 16:41
 */
namespace System\Core\Router;
use System\Corax;
use System\Core\Router;
use System\Exception\CoraxException;
use System\Util\SEK;
use System\Util\UDK;

/**
 * Class URLParser URL解析类
 * @package System\Core\Router
 */
class URLParser extends Router {

    /**
     * 解析url
     * @param string|array $uri uri地址，未设置或者设置成null表示自动获取；如果是字符串类型则从该字符串中解析
     * @param null|int $mode URL模式，设置成null或者未设置将自动获取
     * @return array|null
     * @throws CoraxException
     */
    public static function parse($uri=null,$mode=null){
        isset($mode) or $mode = self::checkMode($uri);

//        UDK::dumpout($mode);

        switch($mode){
            case self::URLMODE_COMMON: return self::parseByCommon($uri);         break;
            case self::URLMODE_COMPATIBLE: return self::parseByCompatible($uri); break;
            case self::URLMODE_PATHINFO:  return self::parseByPathinfo($uri);    break;
            default: throw new CoraxException($mode);
        }
    }

    /**
     * 解析普通模式下URL参数 或者 极速模式下的URL参数解析
     * @param null|string|array $query_uri 可以是query字符串；未设置或者值为null时从指定的数据源中获取输入数组；如果是数组则直接认为其为Common模式uri解析结果
     * @return array 解析结果数组
     * @throws CoraxException
     */
    private static function parseByCommon($query_uri=null){
        Corax::status('parseurl_in_common_begin');

//        UDK::dump($params);
        //获取输入参数
        $params = self::parseCommonDataSource($query_uri);
//        UDK::dump($params);

        //初始化解析结果
        $parsed = [
            'm' => null,
            'c' => null,
            'a' => null,
            'p' => null,
        ];
        //组件变量名称
        $mName  = &self::$convention['URL_MODULE_VARIABLE'];
        $cName  = &self::$convention['URL_CONTROLLER_VARIABLE'];
        $aName  = &self::$convention['URL_ACTION_VARIABLE'];
        //获取模块名称
        if(isset($params[$mName])){
            if(false === stripos($params[$mName],self::$convention['MM_BRIDGE'])) {
                //不存在多个模块
                $parsed['m'] = SEK::toJavaStyle($params[$mName]);
            }else{
                $parsed['m'] = self::toModulesArray($params[$mName],self::$convention['MM_BRIDGE']);
            }
        }
        //获取控制器名称
        isset($params[$cName]) and $parsed['c'] = SEK::toJavaStyle($params[$cName]);
        //获取操作名称，类方法不区分大小写
        isset($params[$aName]) and $parsed['a'] = $params[$aName];
        //参数为剩余的变量
        unset($params[$mName],$params[$cName],$params[$aName]);
        $parsed['p'] = $params;

        Corax::status('parseurl_in_common_end');
        return $parsed;
    }

    /**
     * 解析compatible模式下的URL信息
     * 实现上仅仅是获取pathinfo变量（设置$_SERVER['PATH_INFO']）再调用self::parseByPathinfo()设置结果集
     * 传入字符串形式如下：index.php?_pathinfo=XXXXXXXXXXXXXXX中的query部分(_pathinfo=XXXXXXXXXXXXXXX)
     * @param string|null $pathinfo pathinfo字符串,未设置该参数或者该参数为null
     * @return array
     */
    private static function parseByCompatible($pathinfo=null){
        Corax::status('parseurl_in_Compatible');
        $pathinfoVar = &self::$convention['URL_COMPATIBLE_VARIABLE'];
        if(null === $pathinfo){
            $pathinfo = $_GET[$pathinfoVar];
            unset($_GET[$pathinfoVar]);
        }else{
            parse_str($pathinfo,$temp);
            $pathinfo = $temp[$pathinfoVar];
        }

        Corax::status('parseurl_in_compatible_trans_to_pathinfo');
        return self::parseByPathinfo($pathinfo);
    }

    /**
     * 解析pathinfo模式或者rewrite模式下的URL信息
     * 考虑多级模块的情况:
     *      ①获取操作及之前的部分 和 参数部分
     *      ②从后往前依次获取操作、控制器和模块列表
     *      ③参数从前往后解析
     * @param string|null $pathinfo
     * @return array
     */
    private static function parseByPathinfo($pathinfo=null){
        Corax::status('parseurl_in_pathinfo_begin');

        $parsed = [
            'm' => null,
            'c' => null,
            'a' => null,
            'p' => null,
        ];


//        UDK::dump($pathinfo);
        //获取pathinfo设置
        isset($pathinfo) or $pathinfo = self::getPathInfo();

//        UDK::dumpout($pathinfo);

        if($pathinfo){
            $pathinfo = self::stripMasqueradeTail($pathinfo);
            if(!empty($pathinfo) and is_string($pathinfo)){
                Corax::status('parseurl_in_pathinfo_getpathinfo_done');

                //-- 解析PATHINFO --//
                //截取参数段param与定位段local
                $papos          = strpos($pathinfo,parent::$convention['AP_BRIDGE']);
                $mcapart = null;
                $pparts = '';
                if(false === $papos){
                    $mcapart  = trim($pathinfo,'/');//不存在参数则认定PATH_INFO全部是MCA的部分，否则得到结果substr($pathinfo,0,0)即空字符串
                }else{
                    $mcapart  = trim(substr($pathinfo,0,$papos),'/');
                    $pparts   = substr($pathinfo,$papos + strlen(parent::$convention['AP_BRIDGE']));
                }

                //-- 解析MCA部分 --//
                //逆向检查CA是否存在衔接
                $mcaparsed = self::parseMCA($mcapart);
                $parsed = array_merge($parsed,$mcaparsed);
                Corax::status('parseurl_in_pathinfo_getmac_done');

                //-- 解析参数部分 --//
                $parsed['p'] = self::toParametersArray($pparts,parent::$convention['PP_BRIDGE'],parent::$convention['PKV_BRIDGE']);
                //URL中解析结果合并到$_GET中，$_GET的其他参数不能和之前的一样，否则会被解析结果覆盖
                SEK::merge($_GET,$parsed['p']);

                //注意到$_GET和$_REQUEST并不同步，当动态添加元素到$_GET中后，$_REQUEST中不会自动添加
//        SEK::dump($_REQUEST,$_GET,self::$_components['p']);
                Corax::status('parseurl_in_pathinfo_end');
            }
        }

        return $parsed;
    }

    /**
     * 解析"模块、控制器、操作"
     * @param string $mcapart
     * @return array
     */
    private static function parseMCA($mcapart){
        $parsed = ['m'=>null,'c'=>null,'a'=>null];
        $capos = strrpos($mcapart,self::$convention['CA_BRIDGE']);
//        SEK::dump($mcapart,$capos,self::$_convention['CA_BRIDGE']);
        if(false === $capos){
            //找不到控制器与操作之间分隔符（一定不存在控制器）
            //先判断位置部分是否为空字符串来决定是否有操作名称
            if(strlen($mcapart)){
                //位置字段全部是字符串的部分
                $parsed['a'] = $mcapart;
            }else{
                //没有操作部分，MCA全部使用默认的
            }
        }else{
            //apos+CA_BRIDGE 后面的部分全部算作action
            $parsed['a'] = substr($mcapart,$capos+strlen(self::$convention['CA_BRIDGE']));

            //CA存在衔接符 则说明一定存在控制器
            $mcalen = strlen($mcapart);
            $mcpart = substr($mcapart,0,$capos-$mcalen);//去除了action的部分

//            SEK::dump($mcpart);

            if(strlen($mcapart)){
                $mcpos = strrpos($mcpart,self::$convention['MC_BRIDGE']);
//                SEK::dump($mcpart,$mcpos);
                if(false === $mcpos){
                    //不存在模块
                    if(strlen($mcpart)){
                        //全部是控制器的部分
                        $parsed['c'] = SEK::toJavaStyle($mcpart);
                    }else{
                        //没有控制器部分，则使用默认的
                    }
                }else{
                    //截取控制器的部分
                    $parsed['c']   = SEK::toJavaStyle(substr($mcpart,$mcpos+strlen(self::$convention['MC_BRIDGE'])));

                    //既然存在MC衔接符 说明一定存在模块
                    $mpart = substr($mcpart,0,$mcpos-strlen($mcpart));//以下的全是模块部分的字符串
                    if(strlen($mpart)){
                        if(false === strpos($mpart,parent::$convention['MM_BRIDGE'])){
                            $parsed['m'] = SEK::toJavaStyle($mpart);
                        }else{
                            $parsed['m'] = self::toModulesArray($mpart,self::$convention['MM_BRIDGE']);
                        }
                    }else{
                        //一般存在衔接符的情况下不为空,但也考虑下特殊情况
                    }
                }
            }else{
                //一般存在衔接符的情况下不为空,但也考虑下特殊情况
            }
        }
        return $parsed;
    }


    /**
     * 确定当前访问的URL的URL模式
     * @param  string $uri
     * @return int|null
     */
    private static function checkMode($uri=null){
        Corax::status('parseurl_checkmode_begin');
        $mode = null;

        $query = self::parseCommonDataSource($uri);

        if(empty($query)) {
            //当输入为空时，默认理解为pathinfo模式，去pathinfo中获取信息
            $mode = self::URLMODE_PATHINFO;
        }elseif(isset($query[self::$convention['URL_COMPATIBLE_VARIABLE']]) ){
            //未设置普通模式下的变量 且 设置了pathinfo变量(唯一)时将被认定为compatible模式
            $mode = self::URLMODE_COMPATIBLE;
//                UDK::dump('is URLMODE_COMPATIBLE');
        }elseif(// 绑定的情况下不需要设置模块(反正用不上)，未绑定的情况下必须设置模块
        ((self::$moduleDomainBinded or isset($query[self::$convention['URL_MODULE_VARIABLE']]))
            //必须设置控制器变量
            and isset($query[self::$convention['URL_CONTROLLER_VARIABLE']])
            //必须设置操作变量
            and isset($query[self::$convention['URL_ACTION_VARIABLE']]))
            //pathinfo必须是空的(注：无法使用pathinfo的情况下条件不成立)
//            and empty($_SERVER['PATH_INFO'])
        ){
            //设置了普通模式变量将被认为是普通模式(必须三个全部被设置)
            //普通模式下不在乎URL有多么不友好，所以参数必须写全(非模块部署的情况下)
            $mode = self::URLMODE_COMMON;
        }else{
            //不是以上两种情况则被认定为pathinfo模式
            $mode = self::URLMODE_PATHINFO;
        }

        Corax::status('parseurl_checkmode_end');
        return $mode;
    }

    /**
     * 获取普通模式下的数据源
     * @param  null|string $query 可以是query字符串，此时将被解析并返回，如果未设置或者值为null将自动从系统指定的数据源中获取
     * @return null|array 获取失败市返回null
     * @throws  CoraxException
     */
    private static function parseCommonDataSource($query=null){
        static $cache = [];
        if(!isset($cache[$query])){
            if(null === $query){
                //空字符串或者等于false的值时
                switch(self::$convention['COMMONMODE_SOURCE']){
                    case Router::COMMONMODE_SOURCE_GET:     $cache[$query] = $_GET; break;
                    case Router::COMMONMODE_SOURCE_POST:    $cache[$query] = $_POST; break;
                    case Router::COMMONMODE_SOURCE_INPUT:   parse_str(file_get_contents('php://input'), $cache[$query]); break;
                    case Router::COMMONMODE_SOURCE_REQUEST: $cache[$query] = $_REQUEST; break;
                    default: throw new CoraxException(self::$convention['COMMONMODE_SOURCE']);
                }
            }elseif(is_string($query)){
                //去除伪装的尾巴，通常普通模式下不会创建伪装的后缀
                $query = self::stripMasqueradeTail($query);
                parse_str($query,$temp);
                $cache[$query] = $temp;
            }else {
                throw new CoraxException($query);
            }
        }
        return $cache[$query];
    }

}